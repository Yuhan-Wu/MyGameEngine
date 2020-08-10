#include "pch.h"
#include "ProcessFile.h"
#include "Engine.h"

ProcessFile::ProcessFile() {
	m_pFilename = nullptr;
	m_pFinishEvent = nullptr;
}

ProcessFile::ProcessFile(const char* i_pFilename, std::function<void(std::vector<uint8_t>)> i_Processor, Engine::Event* i_pFinishEvent) :
	m_pFilename(i_pFilename), m_pFinishEvent(i_pFinishEvent), m_Processor(i_Processor) {
	assert(m_pFilename);
}

ProcessFile::ProcessFile(const char* i_pFilename, Point2D initial, std::function<void(std::vector<uint8_t>, Point2D)> i_Processor, Engine::Event* i_pFinishEvent) :
	m_pFilename(i_pFilename), m_pFinishEvent(i_pFinishEvent), m_initial(initial), m_ProcessorWithPosition(i_Processor) {
	assert(m_pFilename);
	type = ProcessType::WithPosition;
}

ProcessFile::ProcessFile(const char* i_pFilename, Point2D initial, Point2D vel, std::function<void(std::vector<uint8_t>, Point2D, Point2D)> i_Processor, Engine::Event* i_pFinishEvent) :
	m_pFilename(i_pFilename), m_pFinishEvent(i_pFinishEvent), m_initial(initial), m_vel(vel), m_ProcessorWithPositionAndVel(i_Processor) {
	assert(m_pFilename);
	type = ProcessType::WithPosAndVel;
}



ProcessFile::~ProcessFile() {
	// delete m_pFilename;
	delete m_pFinishEvent;
}

void ProcessFile::operator()() {
	if (m_pFilename)
	{
		std::vector<uint8_t> pFileContents = Engine::FileProcess::LoadFile(m_pFilename);

		if (!pFileContents.empty())
		{
			if (Engine::JobSystem::ShutdownRequested())
			{
				return;
			}
			else
			{
				std::cout << "ProcessFile finished loading file.\n";

				switch (type)
				{
				case ProcessType::PureActor:
				{
					// this works around C++11 issue with capturing member variable by value
					std::function<void(std::vector<uint8_t>)> Processor = m_Processor;
					Engine::Event* pFinishEvent = m_pFinishEvent;

					Engine::JobSystem::RunJob("ProcessFileContents",
						[pFileContents, Processor, pFinishEvent]()
						{
							Engine::FileProcess::ProcessFileContents(pFileContents, Processor, pFinishEvent);
						},
						"Default"
							);

				}
					break;
				case ProcessType::WithPosition:
				{
					std::function<void(std::vector<uint8_t>, Point2D)> Processor = m_ProcessorWithPosition;
					Engine::Event* pFinishEvent = m_pFinishEvent;
					Point2D ini_loc = m_initial;

					Engine::JobSystem::RunJob("ProcessFileContents",
						[pFileContents, Processor, ini_loc, pFinishEvent]()
						{
							Engine::FileProcess::ProcessFileContents(pFileContents, Processor, ini_loc, pFinishEvent);
						},
						"Default"
							);

				}
					break;
				case ProcessType::WithPosAndVel:
				{
					std::function<void(std::vector<uint8_t>, Point2D, Point2D)> Processor = m_ProcessorWithPositionAndVel;
					Engine::Event* pFinishEvent = m_pFinishEvent;
					Point2D ini_loc = m_initial;
					Point2D ini_vel = m_vel;

					Engine::JobSystem::RunJob("ProcessFileContents",
						[pFileContents, Processor, ini_loc, ini_vel, pFinishEvent]()
						{
							Engine::FileProcess::ProcessFileContents(pFileContents, Processor, ini_loc, ini_vel, pFinishEvent);
						},
						"Default"
							);

				}
					break;
				default:
					break;
				}
				
			}
		}
	}
}