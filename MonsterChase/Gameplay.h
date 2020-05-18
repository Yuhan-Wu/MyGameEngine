#pragma once
#include "Engine.h"
#include "Point2D.h"
#include "PlayerController.h"

namespace Gameplay {

	extern bool bQuit;
	extern bool game_lose;
	extern PlayerController* player_info;

	void CreateMonsters(unsigned int round);

	void Initialize();

	void Update(int width, int height);

	void ShowEnding();

	void TestKeyCallback(unsigned int i_VKeyID, bool bWentDown);

}