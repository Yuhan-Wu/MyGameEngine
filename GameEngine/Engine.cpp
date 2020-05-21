#include "pch.h"

#include "Syncronization/Mutex.h"
#include "Syncronization/ScopeLock.h"
#include "Engine.h"
#include "Timer.h"
#include "ProcessFile.h"
#include "Point2D.h"
#include "PhysicsInfo.h"
#include "BoxCollision.h"
#include "FloatComparisonHelper.h"
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
	std::vector<SmartPointer<GameObject>>* New_GameObject = nullptr;

	namespace CollisionSystem {
		float GetSmaller(float op1, float op2) {
			return op1 > op2 ? op2 : op1;
		}

		float GetBigger(float op1, float op2) {
			return op1 > op2 ? op1 : op2;
		}

		bool CheckCollisionAlongAxis(Vector4 BBCenterInWorldA, Vector4 XWorldA, Vector4 YWorldA,
			Vector4 BBCenterInWorldB, Vector4 XWorldB, Vector4 YWorldB,
			Vector4 Axis, Vector4 VelARelB, float FrameLength,
			float& o_TClose, float& o_TOpen) {
			float CenterOnAxisA = BBCenterInWorldA * Axis;
			float CenterOnAxisB = BBCenterInWorldB * Axis;
			float ProjectedExtentsA = fabs(Axis * XWorldA) + fabs(Axis * YWorldA);
			float ProjectedExtentsB = fabs(Axis * XWorldB) + fabs(Axis * YWorldB) + ProjectedExtentsA;
			float ProjectedVelARelB = VelARelB * Axis;
			// Calculation
			float LeftB = CenterOnAxisB - ProjectedExtentsB;
			float RightB = CenterOnAxisB + ProjectedExtentsB;
			float DClose = LeftB - CenterOnAxisA;
			float DOpen = RightB - CenterOnAxisA;
			o_TOpen = 0.0f;
			o_TClose = FrameLength;
			
			if (CenterOnAxisA > LeftB && CenterOnAxisA < RightB) {
				o_TClose = -0.1f;
				o_TOpen = FrameLength;
				return true;
			}

			if (FloatEqual(CenterOnAxisA, LeftB) || FloatEqual (CenterOnAxisA, RightB)) {
				o_TClose = 0.0f;
				o_TOpen = FrameLength;
				return true;
			}

			// Special cases
			if (!FloatEqual(ProjectedVelARelB, 0.0f)) {
				o_TClose = DClose / ProjectedVelARelB;
				o_TOpen = DOpen / ProjectedVelARelB;
				if (o_TOpen < 0) {
					o_TClose = FrameLength;
					o_TOpen = 0.0f;
					return false;
				}
				if (o_TOpen < o_TClose)
				{
					float temp = o_TOpen;
					o_TOpen = o_TClose;
					o_TClose = temp;
				}
				o_TClose = o_TClose > FrameLength ? FrameLength : o_TClose;
			}
			else {
				o_TClose = FrameLength;
				o_TOpen = 0.0f;
			}
			return (o_TClose > 0.0f || FloatEqual(o_TClose, 0.0f)) && o_TClose < FrameLength;
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
			for (int i = 0; i < static_cast<int>(World_GameObject->size()) - 1; i++) {
				SmartPointer<GameObject> Object_A = (*World_GameObject)[i];
				BoxCollision* Box_A = reinterpret_cast<BoxCollision*>(Object_A->GetComponent(ComponentType::BoxCollision));
				if (Box_A->GetChannel() == "UI") {
					continue;
				}
				for (int j = i + 1; j < static_cast<int>(World_GameObject->size()); j++) {
					SmartPointer<GameObject> Object_B = (*World_GameObject)[j];
					BoxCollision* Box_B = reinterpret_cast<BoxCollision*>(Object_B->GetComponent(ComponentType::BoxCollision));
					if (Box_B->GetChannel() == "UI") {
						continue;
					}
					if (Box_A->GetChannel() == Box_B->GetChannel() && Box_A->GetChannel() == "Static") {
						continue;
					}
					if ((Box_A->GetChannel() != "Player" && Box_B->GetChannel() == "OverlapAll") || (Box_A->GetChannel() == "OverlapAll" && Box_B->GetChannel() != "Player")) {
						continue;
					}
					float collisiont_time = end;
					Vector4 cur_collision_normal;
					bool result_A_and_B = CheckCollisionAgainstOther(Box_A, Box_B, Object_A, Object_B, end, collisiont_time, cur_collision_normal);
					// Collided
					if (result_A_and_B) {
						if (collisiont_time < earliest_collision) {
							earliest_collision = collisiont_time < 0 ? (end - 0.1f) : collisiont_time;
							Object1 = Object_A;
							Object2 = Object_B;
							collision_normal = cur_collision_normal.Normalize();
							if (!(Box_A->GetChannel() == "Static") && !(Box_B->GetChannel() == "Static")) {
								Box_A->SetCollided(true);
								Box_B->SetCollided(true);
							}
							std::cout << "Collided" << std::endl;
						}
					}
				}
			}
			if (earliest_collision < end) {
				// Step forward
				Timer::getInstance()->tick(earliest_collision);

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
				Timer::getInstance()->tick(end);
				return;
			}

		}

	}
}

void Engine::CollisionSystem::CheckCollision(float delta_time) {
	// Only box collision right now
	RecursiveCheck(delta_time, delta_time + 1);
}

void Engine::start() {
	Engine::ControllerCreators = new std::map<std::string, std::function<void(SmartPointer<GameObject>&, nlohmann::json&)> >();
	Engine::ComponentCreators = new std::map<std::string, std::function<void(SmartPointer<GameObject>&, nlohmann::json&)> >();
	Engine::World_GameObject = new std::vector<SmartPointer<GameObject>>();
	Engine::New_GameObject = new std::vector<SmartPointer<GameObject>>();
	JobSystem::InitJobSystem();
}

void Engine::regist(GameObject* item) {
	Timer::getInstance()->regist(item);
}

void Engine::tick() {
	Timer::getInstance()->tick();
}

void Engine::clean() {
	while (New_GameObject->size() != 0)
	{
		New_GameObject->back()->ReleaseAll();
		New_GameObject->back().Clean();
		New_GameObject->back() = nullptr;
		New_GameObject->pop_back();
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
void AddNewGameObject(SmartPointer<GameObject> i_pNewGameObject);
void Engine::CreateGameObjects(std::vector<uint8_t> PlayerData) {
	using json = nlohmann::json;
	if (!PlayerData.empty())
	{
		json PlayerJSON = json::parse(PlayerData);
		std::string Name = PlayerJSON["name"];
		size_t length = Name.length();
		const char* NameCstr = Name.c_str();

		Point2D InitialPosition;
		InitialPosition << PlayerJSON["initial_position"];

		SmartPointer<GameObject> Player = GameObject::Create(NameCstr, length + 1, InitialPosition, 0);
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

		AddNewGameObject(Player);
	}
}

void Engine::CreateGameObjectsWithPosition(std::vector<uint8_t> PlayerData, Point2D initial) {
	using json = nlohmann::json;
	if (!PlayerData.empty())
	{
		json PlayerJSON = json::parse(PlayerData);
		std::string Name = PlayerJSON["name"];
		size_t length = Name.length();
		const char* NameCstr = Name.c_str();

		Point2D InitialPosition = initial;
	
		SmartPointer<GameObject> Player = GameObject::Create(NameCstr, length + 1, InitialPosition, 0);
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

		AddNewGameObject(Player);
	}
}

void Engine::CreateGameObjectsWithPositionAndVelocity(std::vector<uint8_t> PlayerData, Point2D initial, Point2D vel) {
	using json = nlohmann::json;
	if (!PlayerData.empty())
	{
		json PlayerJSON = json::parse(PlayerData);
		std::string Name = PlayerJSON["name"];
		size_t length = Name.length();
		const char* NameCstr = Name.c_str();

		Point2D InitialPosition = initial;

		SmartPointer<GameObject> Player = GameObject::Create(NameCstr, length + 1, InitialPosition, 0);
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
		reinterpret_cast<PhysicsInfo*>(Player->GetComponent(ComponentType::PhysicsInfo))->SetCurVel(vel);

		AddNewGameObject(Player);
	}
}

std::vector<uint8_t> Engine::LoadFile(const char* i_pScriptFilename) {
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

void Engine::ProcessFileContents(std::vector<uint8_t> PlayerData, std::function<void(std::vector<uint8_t>)> i_Processor, Engine::Event* i_pFinishEvent) {
	if (!PlayerData.empty())
	{
		if (!Engine::JobSystem::ShutdownRequested())
			i_Processor(PlayerData);

		//delete[] i_pFileContents;
	}

	std::cout << "ProcessFileContents finished processing file.\n";

	if (i_pFinishEvent)
		i_pFinishEvent->Signal();

}

void Engine::ProcessFileContentsWithPosition(std::vector<uint8_t> PlayerData, std::function<void(std::vector<uint8_t>, Point2D)> i_Processor, Point2D initial, Engine::Event* i_pFinishEvent) {
	if (!PlayerData.empty())
	{
		if (!Engine::JobSystem::ShutdownRequested())
			i_Processor(PlayerData, initial);

		//delete[] i_pFileContents;
	}

	std::cout << "ProcessFileContents finished processing file.\n";

	if (i_pFinishEvent)
		i_pFinishEvent->Signal();

}

void Engine::ProcessFileContentsWithPositionAndVelocity(std::vector<uint8_t> PlayerData, std::function<void(std::vector<uint8_t>, Point2D, Point2D)> i_Processor, Point2D initial,Point2D vel, Engine::Event* i_pFinishEvent) {
	if (!PlayerData.empty())
	{
		if (!Engine::JobSystem::ShutdownRequested())
			i_Processor(PlayerData, initial, vel);

		//delete[] i_pFileContents;
	}

	std::cout << "ProcessFileContents finished processing file.\n";

	if (i_pFinishEvent)
		i_pFinishEvent->Signal();

}

void operator<<(Point2D& vec, nlohmann::json& json_obj)
{
	assert(json_obj.is_array() && json_obj.size() == 2);

	vec.set_position(json_obj[0], json_obj[1]);
}

Engine::Mutex NewGameObjectMutex;
void AddNewGameObject(SmartPointer<GameObject> i_pNewGameObject)
{
	if (i_pNewGameObject)
	{
		// Acquire a scoped lock on the mutex
		Engine::ScopeLock Lock(NewGameObjectMutex);

		Engine::New_GameObject->push_back(i_pNewGameObject);
	}
}
void CheckForNewGameObjects()
{
	Engine::ScopeLock Lock(NewGameObjectMutex);

	if (!Engine::New_GameObject->empty())
	{
		for (SmartPointer<GameObject> p : *(Engine::New_GameObject))
		{
			if (p)
			{
				std::cout << "Moving " << p->name << " to AllObjects.\n";
				Engine::World_GameObject->push_back(p);
			}
		}

		Engine::New_GameObject->clear();
	}
}

void Engine::CreateActor(const char* i_pScriptFilename) {
	// using json = nlohmann::json;
	using namespace std::placeholders;
	// Engine::Event* pFinishEvent = m_pFinishEvent;
	Engine::JobSystem::CreateQueue("Default", 2);

	{
		Engine::JobSystem::RunJob(i_pScriptFilename, std::bind(ProcessFile(i_pScriptFilename, std::bind(CreateGameObjects, _1), nullptr)), "Default");

		do
		{
			CheckForNewGameObjects();
			Sleep(10);
		} while (Engine::JobSystem::HasJobs("Default"));
	}
	CheckForNewGameObjects();
	Engine::JobSystem::RequestShutdown();
}

void Engine::CreateActorWithPosition(const char* i_pScriptFilename, Point2D initial) {
	// using json = nlohmann::json;
	using namespace std::placeholders;
	// Engine::Event* pFinishEvent = m_pFinishEvent;
	Engine::JobSystem::CreateQueue("Default", 2);

	{
		Engine::JobSystem::RunJob(i_pScriptFilename, std::bind(ProcessFile(i_pScriptFilename, initial, std::bind(CreateGameObjectsWithPosition, _1, _2), nullptr)), "Default");

		do
		{
			CheckForNewGameObjects();
			Sleep(10);
		} while (Engine::JobSystem::HasJobs("Default"));
	}
	CheckForNewGameObjects();
	Engine::JobSystem::RequestShutdown();
}

void Engine::CreateActorWithPositionAndVelocity(const char* i_pScriptFilename, Point2D initial, Point2D vel) {
	// using json = nlohmann::json;
	using namespace std::placeholders;
	// Engine::Event* pFinishEvent = m_pFinishEvent;
	Engine::JobSystem::CreateQueue("Default", 2);

	{
		Engine::JobSystem::RunJob(i_pScriptFilename, std::bind(ProcessFile(i_pScriptFilename, initial, vel, std::bind(CreateGameObjectsWithPositionAndVelocity, _1, _2, _3), nullptr)), "Default");

		do
		{
			CheckForNewGameObjects();
			Sleep(10);
		} while (Engine::JobSystem::HasJobs("Default"));
	}
	CheckForNewGameObjects();
	Engine::JobSystem::RequestShutdown();
}

void Engine::RemoveActorIfOutOfBorder(int width, int height) {
	float left = width + 2000.0f;
	float top = height + 2000.0f;
	for (auto it = World_GameObject->begin(); it != World_GameObject->end(); )
	{
		Point2D* cur_position = (*it)->position2D;

		if (cur_position->X() > left || cur_position->X() < -left || cur_position->Y() > top || cur_position->Y() < -top)
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