#pragma once
#include <vector>
#include "Point2D.h"
#include "JobSystem/JobSystem.h"
#include "JobSystem/IJob.h"
#include "Syncronization/Events.h"

enum ProcessType {
	PureActor, WithPosition, WithPosAndVel
};

class ProcessFile
{
public:
	ProcessFile();
	ProcessFile(const char* i_pFilename, std::function<void(std::vector<uint8_t>)> i_Processor, Engine::Event* i_pFinishEvent = nullptr);
	ProcessFile(const char* i_pFilename, Point2D initial, std::function<void(std::vector<uint8_t>, Point2D)> i_Processor, Engine::Event* i_pFinishEvent = nullptr);
	ProcessFile(const char* i_pFilename, Point2D initial, Point2D vel, std::function<void(std::vector<uint8_t>, Point2D, Point2D)> i_Processor, Engine::Event* i_pFinishEvent = nullptr);
	~ProcessFile();

	void operator() ();

private:
	const char* m_pFilename;
	std::function<void(std::vector<uint8_t>)>  m_Processor;
	std::function<void(std::vector<uint8_t>, Point2D)> m_ProcessorWithPosition;
	std::function<void(std::vector<uint8_t>, Point2D, Point2D)> m_ProcessorWithPositionAndVel;
	Engine::Event* m_pFinishEvent;
	Point2D m_initial;
	Point2D m_vel;
	
	ProcessType type = ProcessType::PureActor;
};