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
	ProcessFile(const char* p_Filename, std::string p_ID, std::function<void(std::vector<uint8_t>, std::string)> p_Processor, Engine::Event* p_FinishEvent = nullptr);
	ProcessFile(const char* p_Filename, std::string p_ID, Point2D p_Initial, std::function<void(std::vector<uint8_t>, std::string, Point2D)> p_Processor, Engine::Event* p_FinishEvent = nullptr);
	ProcessFile(const char* p_Filename, std::string p_ID, Point2D p_Initial, Point2D p_Vel, std::function<void(std::vector<uint8_t>, std::string, Point2D, Point2D)> p_Processor, Engine::Event* p_FinishEvent = nullptr);
	~ProcessFile();

	void operator() ();

private:
	const char* m_pFilename;
	std::string m_ID;
	std::function<void(std::vector<uint8_t>, std::string)>  m_Processor;
	std::function<void(std::vector<uint8_t>, std::string, Point2D)> m_ProcessorWithPosition;
	std::function<void(std::vector<uint8_t>, std::string, Point2D, Point2D)> m_ProcessorWithPositionAndVel;
	Engine::Event* m_pFinishEvent;
	Point2D m_initial;
	Point2D m_vel;
	
	ProcessType type = ProcessType::PureActor;
};