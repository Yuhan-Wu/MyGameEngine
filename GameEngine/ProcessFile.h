#pragma once
#include <vector>
#include "Point2D.h"
#include "JobSystem/JobSystem.h"
#include "JobSystem/IJob.h"
#include "Syncronization/Events.h"

enum class ProcessType {
	PureActor = 0, WithPosition = 1, WithPosAndVel = 2
};

class ProcessFile
{
public:
	ProcessFile();
	ProcessFile(const char* p_Filename, std::function<void(std::vector<uint8_t>)> p_Processor, Engine::Event* p_FinishEvent = nullptr);
	ProcessFile(const char* p_Filename, Point2D p_Initial, std::function<void(std::vector<uint8_t>, Point2D)> p_Processor, Engine::Event* p_FinishEvent = nullptr);
	ProcessFile(const char* p_Filename, Point2D p_Initial, Point2D p_Vel, std::function<void(std::vector<uint8_t>, Point2D, Point2D)> p_Processor, Engine::Event* p_FinishEvent = nullptr);
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