/// Description: Entry point for running program.
///
/// Authors: Martin Pettersson, Christoffer Wiss
///	Version: 2013-12-09

#include "GameEngine.h"
using namespace GameLogic;

#ifdef _WIN32 
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

int main()
{
	#ifdef _CRTDBG_MAP_ALLOC
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	#endif

	GameEngine *game = new GameEngine();
	game->mainLoop();
	delete game;

	return 0;
}