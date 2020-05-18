#include <Windows.h>

#if defined _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif // _DEBUG

#include <conio.h>
#include <iostream>
#include <functional>
#include <string>
#include <vector>

#include "JobSystem/JobSystem.h"
#include "JobSystem/IJob.h"
#include "Syncronization/Events.h"
/*
void BasicSample();
void GameObjectSample();

int main(int i_argc, char** i_argl)
{
//	BasicSample();
	GameObjectSample();

#if defined _DEBUG
	_CrtDumpMemoryLeaks();
#endif // _DEBUG
	return 0;

}

void PrintOnInterval(std::string i_String, unsigned int i_IntervalMilliseconds, unsigned int i_Count = 0)
{
	bool bCounted = i_Count > 0;

	do
	{
		std::cout << i_String << "\n";
		Sleep(i_IntervalMilliseconds);
	} while ((!bCounted || (bCounted && --i_Count)) && !Engine::JobSystem::ShutdownRequested());
}

void ProcessFileContents(uint8_t* i_pFileContents, size_t i_sizeFileContents, std::function<void(uint8_t*, size_t)> i_Processor, Engine::Event* i_pFinishEvent = nullptr)
{
	if (i_pFileContents)
	{
		if (i_sizeFileContents  && !Engine::JobSystem::ShutdownRequested())
			i_Processor(i_pFileContents, i_sizeFileContents);

		delete[] i_pFileContents;
	}

	std::cout << "ProcessFileContents finished processing file.\n";

	if (i_pFinishEvent)
		i_pFinishEvent->Signal();
}

class ProcessFile
{
public:
	ProcessFile(const char* i_pFilename, std::function<void(uint8_t*, size_t)> i_Processor, Engine::Event* i_pFinishEvent = nullptr) :
		m_pFilename(i_pFilename),
		m_Processor(i_Processor),
		m_pFinishEvent(i_pFinishEvent)
	{
		assert(m_pFilename);
	}

	void operator() ()
	{
		if (m_pFilename)
		{
			size_t sizeFileContents = 0;
			uint8_t* pFileContents = LoadFile(m_pFilename, sizeFileContents);

			if (pFileContents && sizeFileContents)
			{
				if (Engine::JobSystem::ShutdownRequested())
				{
					delete[] pFileContents;
				}
				else
				{
					std::cout << "ProcessFile finished loading file.\n";

					// this works around C++11 issue with capturing member variable by value
					std::function<void(uint8_t*, size_t)> Processor = m_Processor;
					Engine::Event* pFinishEvent = m_pFinishEvent;

					Engine::JobSystem::RunJob("ProcessFileContents",
						[pFileContents, sizeFileContents, Processor, pFinishEvent]()
						{
							ProcessFileContents(pFileContents, sizeFileContents, Processor, pFinishEvent);
						},
						"Default"
					);

				}
			}
		}
	}

	static uint8_t* LoadFile(const char* i_pFilename, size_t& o_sizeFile)
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
private:
	const char* m_pFilename;
	std::function<void(uint8_t*, size_t)>  m_Processor;
	Engine::Event* m_pFinishEvent;
};

void PrintFileContents(uint8_t* i_pFileContents, size_t i_sizeFileContents)
{
	assert(i_pFileContents && i_sizeFileContents);

	std::cout << "File Contents:\n";
	while (i_sizeFileContents--)
		std::cout << *(i_pFileContents++);

	std::cout << "\n";
}

void BasicSample()
{
	using namespace std::placeholders;

	// use a lambda function to run a job
	Engine::JobSystem::RunJob("Wahoo", 
		[]()
		{
			PrintOnInterval("Wahoo!!", 500, 1);
		},
		"Default"
	);

	{
		Engine::AutoResetEvent FinishEvent;

		// use std::bind to bind a function to a std::function
		// _1 and _2 are placeholders that get replaced when actually executing the function
		ProcessFile* pProcessText = new ProcessFile("File.txt", std::bind(PrintFileContents, _1, _2), &FinishEvent);
		assert(pProcessText);
		// use std::bind to bind an instance of a class that has operator() () (a functor)
		Engine::JobSystem::RunJob("File.txt", std::bind(*pProcessText), "Default");

		// wait for FinishEvent to be signaled
		FinishEvent.Wait();

		std::cout << "ProcessFile finished running.\n";
	}

	// Sleep for 10 seconds. Let some of the print jobs run for a bit
	Sleep(10 * 1000);

	Engine::JobSystem::RequestShutdown();
}

#include "Syncronization/Mutex.h"
#include "Syncronization/ScopeLock.h"

// a super basic GameObject
class GameObject
{
public:
	GameObject(const std::string& i_Name) :
		m_Name(i_Name)
	{
	}
	
	std::string GetName() const { return m_Name;  }

private:
	std::string	m_Name;
};

// this is our master list of GameObjects
std::vector<GameObject*> AllGameObjects;

// this is the list of new GameObjects created via our JobSystem (on other threads)
std::vector<GameObject*> NewGameObjects;
// This is the mutex we use to guard against accessing NewGameObjects simultaneously on multiple threads
Engine::Mutex NewGameObjectMutex;

// This adds a new GameObject to NewGameObjects
void AddNewGameObject(GameObject* i_pNewGameObject)
{
	if (i_pNewGameObject)
	{
		// Acquire a scoped lock on the mutex
		Engine::ScopeLock Lock(NewGameObjectMutex);
		
		NewGameObjects.push_back(i_pNewGameObject);
	}
}

// This will move GameObjects from NewGameObjects -> AllGameObjects
void CheckForNewGameObjects()
{
	Engine::ScopeLock Lock(NewGameObjectMutex);

	if (!NewGameObjects.empty())
	{
		for (GameObject* p : NewGameObjects)
		{
			if (p)
			{
				std::cout << "Moving " << p->GetName() << " to AllObjects.\n";
				AllGameObjects.push_back(p);
			}
		}

		NewGameObjects.clear();
	}
}

// Helper function to create new GameObjects from a list of names in a text file
void CreateGameObjects(uint8_t* i_pFileContents, size_t i_sizeFileContents)
{
	assert(i_pFileContents && i_sizeFileContents);

	uint8_t* pEndFileContents = i_pFileContents + i_sizeFileContents;

	while (i_pFileContents < pEndFileContents)
	{
		size_t NameLength = 0;
		const char* i_pNextName = reinterpret_cast<const char*>(i_pFileContents);
		
		while ((i_pFileContents < pEndFileContents) && *i_pFileContents++ != '\r')
			NameLength++;

		if (NameLength)
		{
			std::string Name(i_pNextName, NameLength);
			AddNewGameObject(new GameObject(Name));
		}

		i_pFileContents++;
	}
}

void GameObjectSample()
{
	using namespace std::placeholders;

	Engine::JobSystem::CreateQueue("Default", 2);

	{
		Engine::JobSystem::RunJob("GameObjects.txt", std::bind(ProcessFile("GameObjects.txt", std::bind(CreateGameObjects, _1, _2), nullptr)), "Default");

		do
		{
			CheckForNewGameObjects();
			Sleep(10);
		} while (Engine::JobSystem::HasJobs("Default"));
	}
	CheckForNewGameObjects();
	Engine::JobSystem::RequestShutdown();
}
*/
