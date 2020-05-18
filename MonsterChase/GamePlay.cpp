#include <iostream>
#include <assert.h>
#include "Gameplay.h"
#include "AIController.h"
#include "PlayerController.h"
#include "PhysicsInfo.h"
#include "RenderInfo.h"
#include "BoxCollision.h"


void* LoadFile(const char* i_pFilename, size_t& o_sizeFile)
{
	assert(i_pFilename != NULL);

	FILE* pFile = NULL;

	errno_t fopenError = fopen_s(&pFile, i_pFilename, "rb");
	if (fopenError != 0)
		return NULL;

	assert(pFile != NULL);

	int FileIOError = fseek(pFile, 0, SEEK_END);
	assert(FileIOError == 0);

	long FileSize = ftell(pFile);
	assert(FileSize >= 0);

	FileIOError = fseek(pFile, 0, SEEK_SET);
	assert(FileIOError == 0);

	uint8_t* pBuffer = new uint8_t[FileSize];
	assert(pBuffer);

	size_t FileRead = fread(pBuffer, 1, FileSize, pFile);
	assert(FileRead == FileSize);

	fclose(pFile);

	o_sizeFile = FileSize;

	return pBuffer;
}

GLib::Sprites::Sprite* CreateSprite(const char* i_pFilename, float& half_width, float& half_height)
{
	assert(i_pFilename);

	size_t sizeTextureFile = 0;

	// Load the source file (texture data)
	void* pTextureFile = LoadFile(i_pFilename, sizeTextureFile);

	// Ask GLib to create a texture out of the data (assuming it was loaded successfully)
	GLib::Texture* pTexture = pTextureFile ? GLib::CreateTexture(pTextureFile, sizeTextureFile) : nullptr;

	// exit if something didn't work
	// probably need some debug logging in here!!!!
	if (pTextureFile)
		delete[] pTextureFile;

	if (pTexture == nullptr)
		return NULL;

	unsigned int width = 0;
	unsigned int height = 0;
	unsigned int depth = 0;

	// Get the dimensions of the texture. We'll use this to determine how big it is on screen
	bool result = GLib::GetDimensions(pTexture, width, height, depth);
	assert(result == true);
	assert((width > 0) && (height > 0));
	half_width = width / 2.0f;
	half_height = height / 2.0f;

	// Define the sprite edges
	GLib::Sprites::SpriteEdges	Edges = { -float(width / 2.0f), float(height), float(width / 2.0f), 0.0f };
	GLib::Sprites::SpriteUVs	UVs = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f } };
	GLib::RGBA							Color = { 255, 255, 255, 255 };

	// Create the sprite
	GLib::Sprites::Sprite* pSprite = GLib::Sprites::CreateSprite(Edges, 0.1f, Color, UVs);
	if (pSprite == nullptr)
	{
		GLib::Release(pTexture);
		return nullptr;
	}

	// Bind the texture to sprite
	GLib::Sprites::SetTexture(*pSprite, *pTexture);

	return pSprite;
}

void RegisterControllerCreator(const std::string& i_ControllerName, std::function<void(SmartPointer<GameObject>&, nlohmann::json&)> i_ControllerCreator);
void RegisterComponentCreator(const std::string& i_ControllerName, std::function<void(SmartPointer<GameObject>&, nlohmann::json&)> i_ComponentCreator);

namespace Gameplay {


	bool bQuit = false;
	bool game_lose = false;

	// Level data
	int max_turtle = 6;
	int max_ghost = 4;
	int max_bomb = 6;

	PlayerController* player_info = nullptr;
}

void CreateTurtle(unsigned int round) {
	for (unsigned int i = 0; i < round + 1; i++) {
		float rand_row = -250.0f + 170.f * (i - 1.f);
		float rand_col = (i % 2) * 350.f - 1000.f;
		Engine::CreateActorWithPosition("..\\GameEngine\\Turtle.json", Point2D(rand_col, rand_row));
	}
}

void CreateGhost(unsigned int round) {
	Point2D base_vel(0.0f, - 40.0f);
	for (unsigned int i = 0; i < 2 * round; i++) {
		float cur_indi = (float)pow(-1, i % 2);
		float rand_col = cur_indi * 550.f - cur_indi * 200.f * i;
		float rand_row = cur_indi * 800.f;

		Engine::CreateActorWithPositionAndVelocity("..\\GameEngine\\Ghost.json", Point2D(rand_col, rand_row), cur_indi * base_vel);
	}
}

void CreateBomb(unsigned int round) {
	Point2D* player_position = Gameplay::player_info->GetPlayer()->position2D;
	float x = player_position->X();
	float y = player_position->Y();
	float spawn_x = 0.0f;
	float spawn_y = 0.0f;
	if (abs(x - spawn_x) < 150 && abs(y - spawn_y) < 150) {
		spawn_x = -x;
		spawn_y = -y;
	}

	Engine::CreateActorWithPosition("..\\GameEngine\\Bomb.json", Point2D(spawn_x, spawn_y));
}

void Gameplay::Initialize() {
	Engine::start();
	RegisterControllerCreator("AI", [](SmartPointer<GameObject>& i_GameObject, nlohmann::json& i_Initializer) {
		assert(i_Initializer["decay_rate"].is_number_float());

		float DecayRate = i_Initializer["decay_rate"];
		AIController* NewAIController = new AIController(i_GameObject, DecayRate);
		i_GameObject->Attach(NewAIController);
		}
	);

	RegisterControllerCreator("Player", [](SmartPointer<GameObject>& i_GameObject, nlohmann::json& i_Initializer) {
		PlayerController* NewPlayerController = new PlayerController(i_GameObject);
		i_GameObject->Attach(NewPlayerController);
		player_info = NewPlayerController;
	}
	);

	RegisterComponentCreator("physics_data", [](SmartPointer<GameObject>& i_GameObject, nlohmann::json& i_Initializer) {
		assert(i_Initializer["mass"].is_number_float());
		assert(i_Initializer["cur_vel"].is_array() && i_Initializer["cur_vel"].size() == 2);
		assert(i_Initializer["movable"].is_boolean());

		float Mass = i_Initializer["mass"];
		bool Movable = i_Initializer["movable"];
		Point2D cur_vel(i_Initializer["cur_vel"][0], i_Initializer["cur_vel"][1]);
		PhysicsInfo* NewPhysicsInfo = new PhysicsInfo(i_GameObject, Mass, cur_vel, Movable);
		i_GameObject->Attach(NewPhysicsInfo);
		}
	);

	RegisterComponentCreator("render_data", [](SmartPointer<GameObject>& i_GameObject, nlohmann::json& i_Initializer) {
		assert(i_Initializer["sprite"].is_string());
		string name = i_Initializer["sprite"];
		string channel = i_Initializer["channel"];
		float half_width;
		float half_height;
		GLib::Sprites::Sprite* player_pic = CreateSprite(name.c_str(), half_width, half_height);
		RenderInfo* NewRenderInfo = new RenderInfo(i_GameObject, player_pic);
		// Create a box collision according to it's sprite data
		BoxCollision* NewBoxCollision = new BoxCollision(i_GameObject, Point2D(0, half_height), Point2D(half_width, half_height), channel);
		i_GameObject->Attach(NewRenderInfo);
		i_GameObject->Attach(NewBoxCollision);
		}
	);

	Engine::CreateActor("..\\GameEngine\\Player.json");
	Engine::CreateActor("..\\GameEngine\\HorizontalBrick.json");
	Engine::CreateActorWithPosition("..\\GameEngine\\HorizontalBrick.json", Point2D(0.f, -1250.f));
	Engine::CreateActor("..\\GameEngine\\VerticalBrick.json");
	Engine::CreateActorWithPosition("..\\GameEngine\\VerticalBrick.json", Point2D(1100.f, -500.f));
}

void RegisterControllerCreator(const std::string& i_ControllerName, std::function<void(SmartPointer<GameObject>&, nlohmann::json&)> i_ControllerCreator)
{
	Engine::ControllerCreators->insert({ i_ControllerName, i_ControllerCreator });
}

void RegisterComponentCreator(const std::string& i_ComponentName, std::function<void(SmartPointer<GameObject>&, nlohmann::json&)> i_ComponentCreator)
{
	Engine::ComponentCreators->insert({ i_ComponentName, i_ComponentCreator });
}

void Gameplay::CreateMonsters(unsigned int round) {

	CreateTurtle(round);
	
	CreateGhost(round);
	
	CreateBomb(round);

}

void Gameplay::Update(int width, int height) {
	
	if (reinterpret_cast<BoxCollision*>(player_info->GetPlayer()->GetCollision())->GetCollided()) {
		game_lose = true;
	}
	Engine::RemoveActorIfOutOfBorder(width, height);
	Engine::tick();
}

void Gameplay::ShowEnding() {
	if (game_lose) {
		Engine::CreateActor("..\\GameEngine\\Lose.json");
	}
	else {
		Engine::CreateActor("..\\GameEngine\\Win.json");
	}
	Update(800, 450);
}

void Gameplay:: TestKeyCallback(unsigned int i_VKeyID, bool bWentDown)
{
#ifdef _DEBUG
	const size_t	lenBuffer = 65;
	char			Buffer[lenBuffer];

	// quit
	if (i_VKeyID == 0X0051) {
		bQuit = true;
	}
	else {

		player_info->ReceiveInput(i_VKeyID, bWentDown);

	}
	// move to the right

	sprintf_s(Buffer, lenBuffer, "VKey 0x%04x went %s\n", i_VKeyID, bWentDown ? "down" : "up");
	OutputDebugStringA(Buffer);
#endif // __DEBUG
}
