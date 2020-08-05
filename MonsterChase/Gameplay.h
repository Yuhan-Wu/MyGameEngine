#pragma once
#include "Engine.h"
#include "Point2D.h"
#include "PlayerController.h"

namespace Gameplay {

	extern bool Quit;
	extern PlayerController* PlayerInfo;

	void Initialize();

	void Update(int p_Width, int p_Height);

	void TestKeyCallback(unsigned int i_VKeyID, bool p_WentDown);

	namespace Customize {
		extern bool GameLose;

		void User_Initialize();
		void User_Update(int width, int height);

		void CreateMonsters(unsigned int round);
		void ShowEnding();
	}
}