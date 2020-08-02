// Monster_Chase.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <Windows.h>
#include <crtdbg.h>
#include <assert.h>

#include "Gameplay.h"
#include "GLib.h"

int WINAPI wWinMain(HINSTANCE i_hInstance, HINSTANCE i_hPrevInstance, LPWSTR i_lpCmdLine, int i_nCmdShow)
{
	// _CrtSetBreakAlloc(207);
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
	// GLib call
	// IMPORTANT: first we need to initialize GLib
	bool bSuccess = GLib::Initialize(i_hInstance, i_nCmdShow, "GLibTest", -1, 1600, 950);

	if (bSuccess)
	{
		// IMPORTANT (if we want keypress info from GLib): Set a callback for notification of key presses
		GLib::SetKeyStateChangeCallback(Gameplay::TestKeyCallback);
		
		// Game initialize
		Gameplay::Initialize();
		unsigned int round = 0;
		
		// Control spawning speed
		unsigned int max_delay = 30000;
		unsigned int delay = max_delay - 1;
		
		do
		{
			// IMPORTANT: We need to let GLib do it's thing. 
			GLib::Service(Gameplay:: bQuit);
			delay++;
			if (delay == max_delay) {
				round++;
				delay = 0;
			}
			if (Gameplay::game_lose || round > 4) {
				// lose or win
				Gameplay::ShowEnding();
				break;
			}
			if (!Gameplay::bQuit)
			{
				if (delay == 0) {
					Gameplay::CreateMonsters(round);
				}

				Gameplay::Update(800, 450);
			}
		} while (Gameplay::bQuit == false);
		
		while (!Gameplay::bQuit) {
			GLib::Service(Gameplay::bQuit);
		}

		Engine::Clean();
		// IMPORTANT:  Tell GLib to shutdown, releasing resources.
		GLib::Shutdown();

	}

#if defined _DEBUG
	_CrtDumpMemoryLeaks();
#endif // _DEBUG
}