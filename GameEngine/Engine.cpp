#include "pch.h"

#include "Syncronization/Mutex.h"
#include "Syncronization/ScopeLock.h"
#include "Engine.h"
#include "Timer.h"
#include "ProcessFile.h"
#include "GameData.h"
#include "PhysicsInfo.h"
#include "BoxCollision.h"
#include "HeapManagerProxy.h"

#include <thread>
#include <fstream>
#include <conio.h>
#include <functional>
#include <iostream>
#include <assert.h>

namespace Engine {
	std::map<std::string, std::function<void(SmartPointer<GameObject>&, nlohmann::json&)> >* ControllerCreators = nullptr;
	std::map<std::string, std::function<void(SmartPointer<GameObject>&, nlohmann::json&)> >* ComponentCreators = nullptr;
	std::vector<SmartPointer<GameObject>>* World_GameObject = nullptr;
	std::map<std::string, SmartPointer<GameObject>>* New_GameObject = nullptr;

	namespace CollisionSystem {
		float GetSmaller(float op1, float op2) {
			return op1 > op2 ? op2 : op1;
		}

		float GetBigger(float op1, float op2) {
			return op1 > op2 ? op1 : op2;
		}

		bool CheckCollisionAlongAxis(Vector4 p_BBCenterInWorldA, Vector4 p_XWorldA, Vector4 p_YWorldA,
			Vector4 p_BBCenterInWorldB, Vector4 p_XWorldB, Vector4 p_YWorldB,
			Vector4 p_Axis, Vector4 p_VelARelB, float p_FrameLength,
			float& o_TClose, float& o_TOpen) {
			float CenterOnAxisA = p_BBCenterInWorldA * p_Axis;
			float CenterOnAxisB = p_BBCenterInWorldB * p_Axis;
			float ProjectedExtentsA = fabs(p_Axis * p_XWorldA) + fabs(p_Axis * p_YWorldA);
			float ProjectedExtentsB = fabs(p_Axis * p_XWorldB) + fabs(p_Axis * p_YWorldB) + ProjectedExtentsA;
			float ProjectedVelARelB = p_VelARelB * p_Axis;
			// Calculation
			float LeftB = CenterOnAxisB - ProjectedExtentsB;
			float RightB = CenterOnAxisB + ProjectedExtentsB;
			float DClose = LeftB - CenterOnAxisA;
			float DOpen = RightB - CenterOnAxisA;
			o_TOpen = 0.0f;
			o_TClose = p_FrameLength;
			
			// Already collided
			if (CenterOnAxisA > LeftB && CenterOnAxisA < RightB) {
				o_TClose = -0.1f;
				o_TOpen = p_FrameLength;
				return true;
			}

			// On the edge
			if (FloatEqual(CenterOnAxisA, LeftB) || FloatEqual (CenterOnAxisA, RightB)) {
				o_TClose = 0.0f;
				o_TOpen = p_FrameLength;
				return true;
			}

			// Special cases
			// Not relatively static
			if (!FloatEqual(ProjectedVelARelB, 0.0f)) {
				o_TClose = DClose / ProjectedVelARelB;
				o_TOpen = DOpen / ProjectedVelARelB;
				// Moving in the opposite direction from B
				if (o_TOpen < 0) {
					o_TClose = p_FrameLength;
					o_TOpen = 0.0f;
					return false;
				}
				// A is on the other side of B
				if (o_TOpen < o_TClose)
				{
					float temp = o_TOpen;
					o_TOpen = o_TClose;
					o_TClose = temp;
				}
				o_TClose = o_TClose > p_FrameLength ? p_FrameLength : o_TClose;
			}
			else {
				o_TClose = p_FrameLength;
				o_TOpen = 0.0f;
			}
			return (o_TClose > 0.0f || FloatEqual(o_TClose, 0.0f)) && o_TClose < p_FrameLength;
		}

		bool CheckCollisionAgainstOther(BoxCollision* Box_A, BoxCollision* Box_B,
			SmartPointer<GameObject> Object_A, SmartPointer<GameObject> Object_B, float FrameLength, float& o_CollisionTime, Vector4& o_CollisionNormal) {
			// Considered as uniform motion
			// 2D
			// A
			Matrix44 AToWorld = Box_A->GetMatrixToWorld();
			Point2D VelA = reinterpret_cast<PhysicsInfo*>(Object_A->GetComponent(ComponentType::PhysicsInfo))->GetCurVel();
			Vector4 VelVectorA(VelA);
			Vector4 BBCenterVectorA(Box_A->GetCenter(), 1);
			Vector4 BBCenterInWorldA = BBCenterVectorA * AToWorld;
			Point2D ExtentsA = Box_A->GetExtents();
			Vector4 XWorldA = AToWorld[0].Normalize() * ExtentsA.X();
			Vector4 YWorldA = AToWorld[1].Normalize() * ExtentsA.Y();


			// B
			Matrix44 BToWorld = Box_B->GetMatrixToWorld();
			Point2D VelB = reinterpret_cast<PhysicsInfo*>(Object_B->GetComponent(ComponentType::PhysicsInfo))->GetCurVel();
			Vector4 VelVectorB(VelB);
			Vector4 BBCenterVectorB(Box_B->GetCenter(), 1);
			Vector4 BBCenterInWorldB = BBCenterVectorB * BToWorld;
			Vector4 BBExtentsVectorB(Box_B->GetExtents());
			Vector4 BBExtentsInWorldB = BBExtentsVectorB * BToWorld;
			Point2D ExtentsB = Box_B->GetExtents();
			Vector4 XWorldB = BToWorld[0].Normalize() * ExtentsB.X();
			Vector4 YWorldB = BToWorld[1].Normalize() * ExtentsB.Y();

			Vector4 VelARelB = VelVectorA - VelVectorB;

			float TClose_Latest = FrameLength;
			float TOpen_Earliest = 0.0f;
			bool collision = true;
			// A->B x axis
			Vector4 XAxisB = BToWorld[0].Normalize();
			float TOpen_AToB_X;
			float TClose_AToB_X;
			collision &= CheckCollisionAlongAxis(BBCenterInWorldA, XWorldA, YWorldA,
				BBCenterInWorldB, XWorldB, YWorldB,
				XAxisB, VelARelB, FrameLength, TClose_AToB_X, TOpen_AToB_X);
			if (!collision) {
				return false;
			}

			//A->B y axis
			Vector4 YAxisB = BToWorld[1].Normalize();
			float TOpen_AToB_Y;
			float TClose_AToB_Y;
			collision &= CheckCollisionAlongAxis(BBCenterInWorldA, XWorldA, YWorldA,
				BBCenterInWorldB, XWorldB, YWorldB,
				YAxisB, VelARelB, FrameLength, TClose_AToB_Y, TOpen_AToB_Y);
			if (!collision) {
				return false;
			}

			//B->A x axis
			Vector4 XAxisA = AToWorld[0].Normalize();
			float TOpen_BToA_X;
			float TClose_BToA_X;
			collision &= CheckCollisionAlongAxis(BBCenterInWorldB, XWorldB, YWorldB,
				BBCenterInWorldA, XWorldA, YWorldA,
				XAxisA, -VelARelB, FrameLength, TClose_BToA_X, TOpen_BToA_X);
			if (!collision) {
				return false;
			}

			//B->A y axis
			Vector4 YAxisA = AToWorld[1].Normalize();
			float TOpen_BToA_Y;
			float TClose_BToA_Y;
			collision &= CheckCollisionAlongAxis(BBCenterInWorldB, XWorldB, YWorldB,
				BBCenterInWorldA, XWorldA, YWorldA,
				YAxisA, -VelARelB, FrameLength, TClose_BToA_Y, TOpen_BToA_Y);
			if (!collision) {
				return false;
			}

			TClose_Latest = GetBigger(GetBigger(GetBigger(TClose_AToB_X, TClose_AToB_Y), TClose_BToA_X), TClose_BToA_Y);
			TOpen_Earliest = GetSmaller(GetSmaller(GetSmaller(TOpen_AToB_X, TOpen_AToB_Y), TOpen_BToA_X), TOpen_BToA_Y);

			if (TClose_Latest < TOpen_Earliest || FloatEqual(TClose_Latest, TOpen_Earliest) || TClose_Latest < 0) {
				vector<float> close_list;
				close_list.push_back(TClose_AToB_X);
				close_list.push_back(TClose_AToB_Y);
				close_list.push_back(TClose_BToA_X);
				close_list.push_back(TClose_BToA_Y);
				std::sort(close_list.begin(), close_list.begin() + 4);

				for (auto cur_close : close_list) {
					if (cur_close > TOpen_Earliest || cur_close < 0) {
						o_CollisionTime = cur_close;
						break;
					}
				}
				float x = 0;
				float y = 0;
				if (FloatEqual(o_CollisionTime, TClose_AToB_X)) {
					x = - BToWorld[1][0];
					y = - BToWorld[1][1];
				}
				else if (FloatEqual(o_CollisionTime, TClose_AToB_Y)) {
					x = - BToWorld[0][0];
					y = - BToWorld[0][1];
				}
				else if (FloatEqual(o_CollisionTime, TClose_BToA_X)) {
					x = - AToWorld[1][0];
					y = - AToWorld[1][1];
				}
				else if (FloatEqual(o_CollisionTime, TClose_BToA_Y)) {
					x = - AToWorld[0][0];
					y = - AToWorld[0][1];
				}
				o_CollisionNormal.X(x);
				o_CollisionNormal.Y(y);

				return true;
			}
			else {
				return false;
			}
		}

		void ReflectVel(SmartPointer<GameObject> CurObject, Vector4 Normal) {
			Point2D CurVel = reinterpret_cast<PhysicsInfo*>(CurObject->GetComponent(ComponentType::PhysicsInfo))->GetCurVel();
			Point2D Normal2D(Normal.X(), Normal.Y());
			if (CurVel * Normal2D > 0) {
				Normal2D = -Normal2D;
			}
			Point2D RefVel = CurVel - 2 * (CurVel * Normal2D) * Normal2D;
 			reinterpret_cast<PhysicsInfo*>(CurObject->GetComponent(ComponentType::PhysicsInfo))->SetCurVel(RefVel);
		}

		void ReflectBothVel(SmartPointer<GameObject> Object1, SmartPointer<GameObject> Object2) {
			Point2D CurVel1 = reinterpret_cast<PhysicsInfo*>(Object1->GetComponent(ComponentType::PhysicsInfo))->GetCurVel();
			float Mass1 = reinterpret_cast<PhysicsInfo*>(Object1->GetComponent(ComponentType::PhysicsInfo))->GetMass();

			Point2D CurVel2 = reinterpret_cast<PhysicsInfo*>(Object2->GetComponent(ComponentType::PhysicsInfo))->GetCurVel();
			float Mass2 = reinterpret_cast<PhysicsInfo*>(Object2->GetComponent(ComponentType::PhysicsInfo))->GetMass();

			Point2D RefVel1 = ((Mass1 - Mass2) / (Mass1 + Mass2)) * CurVel1 + ((2 * Mass2) / (Mass1 + Mass2)) * CurVel2;
			Point2D RefVel2 = ((Mass2 - Mass1) / (Mass1 + Mass2)) * CurVel2 + ((2 * Mass1) / (Mass1 + Mass2)) * CurVel1;

			reinterpret_cast<PhysicsInfo*>(Object1->GetComponent(ComponentType::PhysicsInfo))->SetCurVel(RefVel1);
			reinterpret_cast<PhysicsInfo*>(Object2->GetComponent(ComponentType::PhysicsInfo))->SetCurVel(RefVel2);
		}

		void RecursiveCheck(float end, float earliest) {
			SmartPointer<GameObject> Object1;
			SmartPointer<GameObject> Object2;
			Vector4 collision_normal;
			float earliest_collision = earliest;
			for (auto i = 0; i < World_GameObject->size() - 1; i++) {
				SmartPointer<GameObject> Object_A = (*World_GameObject)[i];
				BoxCollision* Box_A = reinterpret_cast<BoxCollision*>(Object_A->GetComponent(ComponentType::BoxCollision));
				unsigned int temp_mask = 1 << static_cast<int>(Box_A->GetChannel());
				for (auto j = i + 1; j < World_GameObject->size(); j++) {
					SmartPointer<GameObject> Object_B = (*World_GameObject)[j];
					BoxCollision* Box_B = reinterpret_cast<BoxCollision*>(Object_B->GetComponent(ComponentType::BoxCollision));
					//TODO: Debug
					if (!(temp_mask & static_cast<int>(Box_B->GetMask()))) {
						continue;
					}
					float collision_time = end;
					Vector4 cur_collision_normal;
					bool result_A_and_B = CheckCollisionAgainstOther(Box_A, Box_B, Object_A, Object_B, end, collision_time, cur_collision_normal);
					// Collided
					if (result_A_and_B) {
						if (collision_time < earliest_collision) {
							earliest_collision = collision_time < 0 ? (end - 0.1f) : collision_time;
							Object1 = Object_A;
							Object2 = Object_B;
							collision_normal = cur_collision_normal.Normalize();
							Box_A->SetCollided(Box_B);
							Box_B->SetCollided(Box_A);
							std::cout << "Collided" << std::endl;
						}
					}
				}
			}
			if (earliest_collision < end) {
				// Step forward
				Tick(earliest_collision);

				// Response
				// Response 1: one is movable, the other is not
				if (!reinterpret_cast<PhysicsInfo*>(Object1->GetComponent(ComponentType::PhysicsInfo))->GetMovability()) {
					ReflectVel(Object2, collision_normal);
				}else if (!reinterpret_cast<PhysicsInfo*>(Object2->GetComponent(ComponentType::PhysicsInfo))->GetMovability()) {
					ReflectVel(Object1, collision_normal);
				}
				else {
					// Response 2: both are movable
					ReflectBothVel(Object1, Object2);
				}

				// Recursive check
				RecursiveCheck(end - earliest_collision, end - earliest_collision + 1);
			}
			else {
				Tick(end);
				return;
			}
		}

	}
}

void Engine::CollisionSystem::CheckCollision(float p_DeltaTime) {
	// Only box collision right now
	for (int i = 0; i < static_cast<int>(World_GameObject->size()) - 1; i++) {
		SmartPointer<GameObject> Object = (*World_GameObject)[i];
		BoxCollision* Box = reinterpret_cast<BoxCollision*>(Object->GetComponent(ComponentType::BoxCollision));
		Box->CleanCollided();
	}
	RecursiveCheck(p_DeltaTime, p_DeltaTime + 1);
}

void Engine::Start() {
	Engine::ControllerCreators = new std::map<std::string, std::function<void(SmartPointer<GameObject>&, nlohmann::json&)> >();
	Engine::ComponentCreators = new std::map<std::string, std::function<void(SmartPointer<GameObject>&, nlohmann::json&)> >();
	Engine::World_GameObject = new std::vector<SmartPointer<GameObject>>();
	Engine::New_GameObject = new std::map<std::string, SmartPointer<GameObject>>();
	JobSystem::InitJobSystem();
}

void Engine::Tick() {
	float delta_time = Timer::getInstance()->CalcLastFrameTime_ms();
	// float delta_time = 1;
	Engine::CollisionSystem::CheckCollision(delta_time);
}

void Engine::Tick(float p_DeltaTime) {
	for (SmartPointer<GameObject> object : *(World_GameObject))
	{
		object->BeginUpdate(p_DeltaTime);
	}
	for (SmartPointer<GameObject> object : *(World_GameObject))
	{
		object->Update(p_DeltaTime);
	}
	for (SmartPointer<GameObject> object : *(World_GameObject))
	{
		object->EndUpdate(p_DeltaTime);
	}

	// IMPORTANT: Tell GLib that we want to start rendering
	GLib::BeginRendering();
	// Tell GLib that we want to render some sprites
	GLib::Sprites::BeginRendering();
	for (SmartPointer<GameObject> object : *(World_GameObject))
	{
		object->Render();
	}
	// Tell GLib we're done rendering sprites
	GLib::Sprites::EndRendering();
	// IMPORTANT: Tell GLib we're done rendering
	GLib::EndRendering();
}

void Engine::Clean() {
	std::map < std:: string, SmartPointer<GameObject> > ::iterator it;

	for (it = New_GameObject->begin(); it != New_GameObject->end(); it++)
	{
		it->second->ReleaseAll();
		it->second.Clean();
		it->second = nullptr;
	}
	while (World_GameObject->size() != 0)
	{
		World_GameObject->back()->ReleaseAll();
		World_GameObject->back().Clean();
		World_GameObject->back() = nullptr;
		World_GameObject->pop_back();
	}
	New_GameObject->clear();
	delete New_GameObject;
	World_GameObject->clear();
	delete World_GameObject;
	HeapManager::FreeInstance();
	HeapManagerProxy::Destroy(HeapManager::GetInstance());
	delete Timer::getInstance();
	ControllerCreators->clear();
	delete ControllerCreators;
	ComponentCreators->clear();
	delete ComponentCreators;

	JobSystem::CleanJobSystem();
}

void operator<<(Point2D& vec, nlohmann::json& json_obj);
void AddNewGameObject(SmartPointer<GameObject> i_pNewGameObject, std::string p_ID);
void Engine::FileProcess::CreateGameObjects(std::vector<uint8_t> PlayerData, std::string p_ID) {
	using json = nlohmann::json;
	if (!PlayerData.empty())
	{
		json PlayerJSON = json::parse(PlayerData);
		std::string name = PlayerJSON["name"];

		Point2D initialPosition;
		initialPosition << PlayerJSON["initial_position"];

		SmartPointer<GameObject> Player = GameObject::Create(name, initialPosition, 0);

		if (PlayerJSON.contains("controller"))
		{
			assert(PlayerJSON["controller"].is_object());
			assert(PlayerJSON["controller"]["type"].is_string());
			assert(PlayerJSON["controller"]["initializer"].is_object());


			auto ControllerCreator = ControllerCreators->find(PlayerJSON["controller"]["type"]);
			if (ControllerCreator != ControllerCreators->end())
			{
				ControllerCreator->second(Player, PlayerJSON["controller"]["initializer"]);
			}
		}

		if (PlayerJSON.contains("components"))
		{
			assert(PlayerJSON["components"].is_object());

			for (json::iterator it = PlayerJSON["components"].begin(); it != PlayerJSON["components"].end(); ++it)
			{
				const std::string& ComponentName = it.key();

				auto ComponentCreator = ComponentCreators->find(ComponentName);
				if (ComponentCreator != ComponentCreators->end())
				{
					ComponentCreator->second(Player, it.value());
				}
			}

		}

		AddNewGameObject(Player, p_ID);
	}
}

void Engine::FileProcess::CreateGameObjects(std::vector<uint8_t> p_PlayerData, std::string p_ID, Point2D p_Initial) {
	using json = nlohmann::json;
	if (!p_PlayerData.empty())
	{
		json PlayerJSON = json::parse(p_PlayerData);
		std::string name = PlayerJSON["name"];

		Point2D initialPosition = p_Initial;
	
		SmartPointer<GameObject> Player = GameObject::Create(name, initialPosition, 0);
		// GameObject Dummy;

		if (PlayerJSON.contains("controller"))
		{
			assert(PlayerJSON["controller"].is_object());
			assert(PlayerJSON["controller"]["type"].is_string());
			assert(PlayerJSON["controller"]["initializer"].is_object());


			auto ControllerCreator = ControllerCreators->find(PlayerJSON["controller"]["type"]);
			if (ControllerCreator != ControllerCreators->end())
			{
				ControllerCreator->second(Player, PlayerJSON["controller"]["initializer"]);
			}
		}

		if (PlayerJSON.contains("components"))
		{
			assert(PlayerJSON["components"].is_object());

			for (json::iterator it = PlayerJSON["components"].begin(); it != PlayerJSON["components"].end(); ++it)
			{
				const std::string& ComponentName = it.key();

				auto ComponentCreator = ComponentCreators->find(ComponentName);
				if (ComponentCreator != ComponentCreators->end())
				{
					ComponentCreator->second(Player, it.value());
				}
			}

		}

		AddNewGameObject(Player, p_ID);
	}
}

void Engine::FileProcess::CreateGameObjects(std::vector<uint8_t> p_PlayerData, std::string p_ID, Point2D p_Initial, Point2D p_Vel) {
	using json = nlohmann::json;
	if (!p_PlayerData.empty())
	{
		json PlayerJSON = json::parse(p_PlayerData);
		std::string name = PlayerJSON["name"];

		Point2D initialPosition = p_Initial;

		SmartPointer<GameObject> Player = GameObject::Create(name, initialPosition, 0);
		// GameObject Dummy;

		if (PlayerJSON.contains("controller"))
		{
			assert(PlayerJSON["controller"].is_object());
			assert(PlayerJSON["controller"]["type"].is_string());
			assert(PlayerJSON["controller"]["initializer"].is_object());


			auto ControllerCreator = ControllerCreators->find(PlayerJSON["controller"]["type"]);
			if (ControllerCreator != ControllerCreators->end())
			{
				ControllerCreator->second(Player, PlayerJSON["controller"]["initializer"]);
			}
		}

		if (PlayerJSON.contains("components"))
		{
			assert(PlayerJSON["components"].is_object());

			for (json::iterator it = PlayerJSON["components"].begin(); it != PlayerJSON["components"].end(); ++it)
			{
				const std::string& ComponentName = it.key();

				auto ComponentCreator = ComponentCreators->find(ComponentName);
				if (ComponentCreator != ComponentCreators->end())
				{
					ComponentCreator->second(Player, it.value());
				}
			}

		}
		reinterpret_cast<PhysicsInfo*>(Player->GetComponent(ComponentType::PhysicsInfo))->SetCurVel(p_Vel);

		AddNewGameObject(Player, p_ID);
	}
}

std::vector<uint8_t> Engine::FileProcess::LoadFile(const char* i_pScriptFilename) {
	assert(i_pScriptFilename != nullptr);

	std::vector<uint8_t> Buffer;

	FILE* pFile = nullptr;

	if (fopen_s(&pFile, i_pScriptFilename, "rb") == 0)
	{
		assert(pFile != nullptr);

		int FileIOError = fseek(pFile, 0, SEEK_END);
		assert(FileIOError == 0);

		long FileSize = ftell(pFile);
		assert(FileSize >= 0);

		FileIOError = fseek(pFile, 0, SEEK_SET);
		assert(FileIOError == 0);

		Buffer.reserve(FileSize);
		Buffer.resize(FileSize);

		size_t FileRead = fread(&Buffer[0], 1, FileSize, pFile);
		assert(FileRead == FileSize);

		fclose(pFile);
	}

	return Buffer;
}

void Engine::FileProcess::ProcessFileContents(std::vector<uint8_t> PlayerData, std::function<void(std::vector<uint8_t>, std::string)> i_Processor, std::string p_ID, Engine::Event* i_pFinishEvent) {
	if (!PlayerData.empty())
	{
		if (!Engine::JobSystem::ShutdownRequested())
			i_Processor(PlayerData, p_ID);

		//delete[] i_pFileContents;
	}

	std::cout << "ProcessFileContents finished processing file.\n";

	if (i_pFinishEvent)
		i_pFinishEvent->Signal();

}

void Engine::FileProcess::ProcessFileContents(std::vector<uint8_t> p_PlayerData, std::function<void(std::vector<uint8_t>, std::string, Point2D)> p_Processor, std::string p_ID, Point2D p_Initial, Engine::Event* p_FinishEvent) {
	if (!p_PlayerData.empty())
	{
		if (!Engine::JobSystem::ShutdownRequested())
			p_Processor(p_PlayerData, p_ID, p_Initial);

		//delete[] i_pFileContents;
	}

	std::cout << "ProcessFileContents finished processing file.\n";

	if (p_FinishEvent)
		p_FinishEvent->Signal();

}

void Engine::FileProcess::ProcessFileContents(std::vector<uint8_t> p_PlayerData, std::function<void(std::vector<uint8_t>, std::string, Point2D, Point2D)> p_Processor, std::string p_ID, Point2D p_Initial,Point2D p_Vel, Engine::Event* p_FinishEvent) {
	if (!p_PlayerData.empty())
	{
		if (!Engine::JobSystem::ShutdownRequested())
			p_Processor(p_PlayerData, p_ID, p_Initial, p_Vel);

		//delete[] i_pFileContents;
	}

	std::cout << "ProcessFileContents finished processing file.\n";

	if (p_FinishEvent)
		p_FinishEvent->Signal();

}

void operator<<(Point2D& vec, nlohmann::json& json_obj)
{
	assert(json_obj.is_array() && json_obj.size() == 2);

	vec.set_position(json_obj[0], json_obj[1]);
}

Engine::Mutex NewGameObjectMutex;
void AddNewGameObject(SmartPointer<GameObject> i_pNewGameObject, std::string p_ID)
{
	if (i_pNewGameObject)
	{
		// Acquire a scoped lock on the mutex
		Engine::ScopeLock Lock(NewGameObjectMutex);

		(*(Engine::New_GameObject))[p_ID] = i_pNewGameObject;
	}
}
bool CheckForNewGameObjects(std::string p_ID, SmartPointer<GameObject>& o_GameObject)
{
	Engine::ScopeLock Lock(NewGameObjectMutex);

	if (!Engine::New_GameObject->empty())
	{
		std::map < std::string , SmartPointer<GameObject> > ::iterator it;
		it = Engine::New_GameObject->find(p_ID);
		if (it != Engine::New_GameObject->end()) {
			o_GameObject = it->second;
			std::cout << "Moving " << o_GameObject->m_Name << " to AllObjects.\n";
			Engine::World_GameObject->push_back(o_GameObject);
			Engine::New_GameObject->erase(p_ID);
			return true;
		}
		return false;

	}
	return false;
}

std::string gen_random6() {
	static const char alphanum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";
	std::string s = "";
	for (int i = 0; i < 6; ++i) {
		s += alphanum[rand() % (sizeof(alphanum) - 1)];
	}
	return s;
}

SmartPointer<GameObject> Engine::FileProcess::CreateActor(const char* i_pScriptFilename) {
	// using json = nlohmann::json;
	using namespace std::placeholders;
	// Engine::Event* pFinishEvent = m_pFinishEvent;
	SmartPointer<GameObject> game_object;
	std::string rand_id = gen_random6();
	Engine::JobSystem::CreateQueue("Default", 2);

	{
		Engine::JobSystem::RunJob(i_pScriptFilename, std::bind(ProcessFile(i_pScriptFilename, rand_id, std::bind(static_cast<void (*) (std::vector<uint8_t>, std::string)>(&CreateGameObjects), _1, _2), nullptr)), "Default");

		do
		{
			CheckForNewGameObjects(rand_id, game_object);
			Sleep(10);
		} while (Engine::JobSystem::HasJobs("Default"));
	}
	CheckForNewGameObjects(rand_id, game_object);
	Engine::JobSystem::RequestShutdown();
	return game_object;
}

SmartPointer<GameObject> Engine::FileProcess::CreateActor(const char* p_ScriptFilename, Point2D p_Initial) {
	// using json = nlohmann::json;
	using namespace std::placeholders;
	// Engine::Event* pFinishEvent = m_pFinishEvent;
	Engine::JobSystem::CreateQueue("Default", 2);
	SmartPointer<GameObject> game_object;
	std::string rand_id = gen_random6();
	{
		Engine::JobSystem::RunJob(p_ScriptFilename, std::bind(ProcessFile(p_ScriptFilename, rand_id, p_Initial, std::bind(static_cast<void (*) (std::vector<uint8_t>, std::string, Point2D)>(&CreateGameObjects), _1, _2, _3), nullptr)), "Default");

		do
		{
			CheckForNewGameObjects(rand_id, game_object);
			Sleep(10);
		} while (Engine::JobSystem::HasJobs("Default"));
	}
	CheckForNewGameObjects(rand_id, game_object);
	Engine::JobSystem::RequestShutdown();
	return game_object;
}

SmartPointer<GameObject> Engine::FileProcess::CreateActor(const char* p_ScriptFilename, Point2D p_Initial, Point2D p_Vel) {
	// using json = nlohmann::json;
	using namespace std::placeholders;
	// Engine::Event* pFinishEvent = m_pFinishEvent;
	SmartPointer<GameObject> game_object;
	std::string rand_id = gen_random6();
	Engine::JobSystem::CreateQueue("Default", 2);

	{
		Engine::JobSystem::RunJob(p_ScriptFilename, std::bind(ProcessFile(p_ScriptFilename, rand_id, p_Initial, p_Vel, std::bind(static_cast<void (*) (std::vector<uint8_t>,std::string, Point2D, Point2D)>(&CreateGameObjects), _1, _2, _3, _4), nullptr)), "Default");

		do
		{
			CheckForNewGameObjects(rand_id, game_object);
			Sleep(10);
		} while (Engine::JobSystem::HasJobs("Default"));
	}
	CheckForNewGameObjects(rand_id, game_object);
	Engine::JobSystem::RequestShutdown();
	return game_object;
}

void Engine::Game::RemoveActorIfOutOfBorder(int width, int height) {
	float left = width + 2000.0f;
	float top = height + 2000.0f;
	for (auto it = World_GameObject->begin(); it != World_GameObject->end(); )
	{
		Point2D cur_position = (*it)->m_Position2D;

		if (cur_position.X() > left || cur_position.X() < -left || cur_position.Y() > top || cur_position.Y() < -top)
		{
			//remove from registry
			(*it)->ReleaseAll();
			//remove from list
			it = World_GameObject->erase(it);
		}
		else
		{
			++it;
		}
		
	}
}