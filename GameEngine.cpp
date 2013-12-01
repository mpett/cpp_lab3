/// Description: The GameEngine maintains the current game, allowing user to provide input, 
///              diplays text to screen and updates world accordingly.
///
/// Authors: Martin Pettersson, Christoffer Wiss
///	Version: 2013-11-27

#include "GameEngine.h"
#include "DungeonRoom.h"
#include "AppleRoom.h"
#include "Human.h"
#include "Goblin.h"
#include "NormalSword.h"
#include "Potion.h"
#include "SpikedBall.h"
#include "NormalArmor.h"
#include <iostream>
#include <algorithm>

#ifdef _WIN32 
#include <conio.h>
#else
 #include <unistd.h>   //_getch*/
 #include <termios.h>  //_getch*/
#endif

using std::string;
using std::cout;
using std::endl;
using std::cin;

namespace GameLogic
{
#ifndef _WIN32
	// This function mimicks getch() on windows systems.
	// Orignal author: _mf on StackOverflow
	char _getch()
	{
		char buf=0;
		struct termios old={0};
		fflush(stdout);
		if(tcgetattr(0, &old)<0)
			perror("tcsetattr()");
		old.c_lflag&=~ICANON;
		old.c_lflag&=~ECHO;
		old.c_cc[VMIN]=1;
		old.c_cc[VTIME]=0;
		if(tcsetattr(0, TCSANOW, &old)<0)
			perror("tcsetattr ICANON");
		if(read(0,&buf,1)<0)
			perror("read()");
		old.c_lflag|=ICANON;
		old.c_lflag|=ECHO;
		if(tcsetattr(0, TCSADRAIN, &old)<0)
			perror ("tcsetattr ~ICANON");
		return buf;
	}
#endif

	// Used for when pausing / resuming the game
	Environment * roomAt;
	Character * charAt;
	bool isPaused;

	// Destructor - Free memory here
	GameEngine::~GameEngine()
	{
		cleanUpGame();
	}

	// Constructor 
	GameEngine::GameEngine()
	{
		actions_.insert(std::pair<string, ACT_PTR>("GO", &Character::go));
		actions_.insert(std::pair<string, ACT_PTR>("MOVE", &Character::go));
		actions_.insert(std::pair<string, ACT_PTR>("TAKE", &Character::pickup));
		actions_.insert(std::pair<string, ACT_PTR>("PICKUP", &Character::pickup));
		actions_.insert(std::pair<string, ACT_PTR>("HELP", &Character::help));
		actions_.insert(std::pair<string, ACT_PTR>("LOOK", &Character::look));
		actions_.insert(std::pair<string, ACT_PTR>("SEE", &Character::look));
		actions_.insert(std::pair<string, ACT_PTR>("DROP", &Character::drop));
		actions_.insert(std::pair<string, ACT_PTR>("PUT", &Character::drop));
		actions_.insert(std::pair<string, ACT_PTR>("INVENTORY", &Character::showInventory));
		actions_.insert(std::pair<string, ACT_PTR>("INV", &Character::showInventory));
		actions_.insert(std::pair<string, ACT_PTR>("SHOW", &Character::showInventory));
		actions_.insert(std::pair<string, ACT_PTR>("STATS", &Character::showStats));
		actions_.insert(std::pair<string, ACT_PTR>("ME", &Character::showStats));
		actions_.insert(std::pair<string, ACT_PTR>("EQUIP", &Character::equip));
		actions_.insert(std::pair<string, ACT_PTR>("UNEQUIP", &Character::unequip));
		actions_.insert(std::pair<string, ACT_PTR>("ATTACK", &Character::attack));
		actions_.insert(std::pair<string, ACT_PTR>("HIT", &Character::attack));
		actions_.insert(std::pair<string, ACT_PTR>("TALK", &Character::talkTo));
		actions_.insert(std::pair<string, ACT_PTR>("SPEAK", &Character::talkTo));
		actions_.insert(std::pair<string, ACT_PTR>("CONSUME", &Character::consume));
		actions_.insert(std::pair<string, ACT_PTR>("CONS", &Character::consume));
		actions_.insert(std::pair<string, ACT_PTR>("USE", &Character::consume));
	}

	// Parses input string from player console.
	void GameEngine::processPlayerTurn(Character& character)
	{
		std::stringstream ss;
		string inputVerb;
		string rest;
		bool turnIsOver = false;
		while(!turnIsOver)
		{
			ss.str("");
			ss.clear();
			cout << "\n>";

			char nextChar = -1;
			// Wait until enter or esc is pressed
			while(nextChar != '\r' && nextChar != '\n')
			{
				if(nextChar > 0)
				{
					// Pressed backspace, delete character
					if(nextChar == 8 || nextChar == 127)
					{
						int size = ss.str().size();

						if(size > 0)
						{
							string bla = ss.str().substr(0,size-1);
							ss.str("");
							ss.clear();
							ss << bla;
							bla = ss.str();
							cout << "\b" << " " << "\b";
						}
					}
					// User pressed esc
					else if(nextChar == 0x1B)
					{
						isPaused = true;
						return;
					}
					else if(nextChar < 127 && nextChar > 31)
					{
						ss << nextChar;
						cout << nextChar;
					}
				}
				nextChar = _getch();
			}

			cout << endl;

			ss >> inputVerb >> std::ws;
			getline(ss,rest);
			std::transform(inputVerb.begin(), inputVerb.end(),inputVerb.begin(), ::toupper);

			auto it = actions_.find(inputVerb);

			// Could not find action
			if(it == actions_.end())
			{
				cout << "Action is not available." << endl;
			}
			// Found action
			else
			{
				clearScreen();
				cout << endl << startPlayerEventsLine;
				turnIsOver = (character.*(it->second))(rest); // Turn is only over if action returns true (i.e. turn is consumed) 
				cout << endSeperatorLine;
			}
		}
	}

	// "Clears" the terminal window.
	void GameEngine::clearScreen()
	{
		cout << string( 100, '\n' );
	}

	// Returns true if this game has reached an end.
	bool GameEngine::gameOver() const
	{
		// TODO: GameOver
		return false;
	}

	// Loads a saved game state from file.
	bool GameEngine::loadGame(std::string file)
	{
		// TODO : Load
		return false;
	}

	// Saves a game state to file.
	bool GameEngine::saveGame(std::string file) const
	{
		// TODO : Save
		return false;
	}
		
	// Start a new game, create all items and inhabit environments.
	void GameEngine::startNewGame(std::string file)
	{
		// TODO : Start new game
		cout << "This is not a game!" << endl;

		Environment *env1 = new DungeonRoom("A very (very) dark room.");
		Environment *env2 = new DungeonRoom("Another very dark room.");
		Environment *env3 = new AppleRoom("A very predictable room.");
		
		Character *character = new Human(true,"Joe", "Human", 5, 1, 1, 1, 1, 0, 100, env2);
		character = new Goblin(false,"Goe1", "Goblin", 5, 1, 1, 1, 1, 0, 10, env1);
		character ->addThingToSay("Blargh blargh blurb!");

		env1->addNeigbor(env2, "UP");
		env2->addNeigbor(env1, "DOWN");
		env1->addNeigbor(env3, "STAIRS");
		env3->addNeigbor(env1, "STAIRS");

		Item *miscItem1 = new Item("Golden Key", "Key", 100, 1);
		env1->addMiscItem(miscItem1);
		Item *miscItem2 = new Item("Silver Key", "Key", 100, 1);
		env2->addMiscItem(miscItem2);
		Consumable *potion = new Potion("Magic Potion", 100, 0.5, 3, false, 5);
		env2-> addConsumable(potion);
		Consumable *ball = new SpikedBall("Spiked Ball", 10, 10.0, 1);
		env1-> addConsumable(ball);
		Equipable *eqItem1 = new NormalSword("Bastard Sword", 50, 5.25, 2, 5, 1);
		Equipable *eqItem2 = new NormalSword("Heavy Sword", 50, 5.25, 2, 5, 10);
		Equipable *eqItem3 = new NormalArmor("Good Amror", 75, 6.25, 25, 1);
		Equipable *eqItem4 = new NormalArmor("Armor of Pain", 1, 10.25, -10, 1);
		env1->addEquipable(eqItem1);
		env1->addEquipable(eqItem2);
		env2->addEquipable(eqItem3);
		env2->addEquipable(eqItem4);
		environments_.push_back(env1);
		environments_.push_back(env2);
		environments_.push_back(env3);
	}

	// Deletes all objects associated with current game instance.
	void GameEngine::cleanUpGame()
	{
		for (Environment* room : environments_)
		{
			if(room != nullptr)
			{
				delete room;
			}
		}
		environments_.clear();
	}

	// Updates current state of the game. This is the main method.
	void GameEngine::mainLoop()
	{
		bool gameOver = false;
		string input;
		int menuChoice;

		while(!gameOver) 
		{
			clearScreen();
			cout << endl << getMainMenu() << endl;
			cout << "\n>";
			
			std::getline(cin, input);
			
			// Try to convert input to an integer
			try
			{
				menuChoice = std::stoi(input);
			}
			catch (std::exception&)
			{
				menuChoice = -1;
			}
			

			switch(menuChoice)
			{
				// Resume game (only if game is active)
				case(0):
					if(isPaused)
					{
						clearScreen();
						cout << endSeperatorLine;
						gameLoop();
					}
					else
					{
						cout << "This is not a menu option." << endl;
					}
					break;
				// Start new game
				case(1):
					isPaused = false;
					cleanUpGame();
					clearScreen();
					cout << endSeperatorLine;
					startNewGame("file");
					gameLoop();
					break;
				// Exit game
				case(2):
					cout << "Good Bye! :)" << endl;
					gameOver = true;
					break;
				default:
					cout << "This is not a menu option." << endl;
					break;
			}
		}
	}

	// Always call a new turn until game is over.
	// The game is played out here.
	void GameEngine::gameLoop()
	{
		bool doGameLoop = true;

		while(doGameLoop)
		{
			doGameLoop = newTurn();
		}
		if(!isPaused)
		{
			cleanUpGame();
		}
	}

	// Run each time a new turn has occurred.
	bool GameEngine::newTurn()
	{
		for(Environment * room : environments_)
		{
			room->enableCharacterActions();
		}
		return updateRooms();
	}

	// Goes through each room, runs action for each Character in room.
	bool GameEngine::updateRooms()
	{
		bool thereIsAPlayer = false;
		bool useStartValues = isPaused;
		isPaused = false;
		//cout << endl << startEventsLine;
		for(Environment * room : environments_)
		{
			if(useStartValues && room != roomAt) continue; // Resume at correct room

			auto characters = room->getCharacters(); // Not the most effective but it works...
			for(auto it = characters.begin(); it != characters.end(); it++)
			{
				if(useStartValues && charAt != it->second) continue; // Resume at correct character
				bool controllable = it->second->isControllable();
				// Check if character is alive
				if(it->second->isAlive())
				{
					auto lambdaFunc = [it]() 
					{
						cout << endSeperatorLine;
						cout << "\tIt is now " << it->second->getName() << "'s turn." << endl;
						cout << endSeperatorLine;
					};

					// Do not apply turn event when resuming game
					if(!useStartValues)room->turnEvent(*it->second);
					if(controllable) thereIsAPlayer = true;

					if(it->second->getCanPerformAction())
					{
						bool inNewRoom = it->second->getInNewRoom();
						it->second->setInNewRoom(false);

						// Controllable by user - parse input time
						if(controllable)
						{
							if(!useStartValues)lambdaFunc();
							// If at new room for the first time write 
							if(inNewRoom || useStartValues)
							{
								//cout << endl;
								it->second->look(""); // Output room information
							} else 
							{
								//cout << endl;
								cout << room->getRoomContent(it->second);
							}
							processPlayerTurn(*it->second);
							if(isPaused) 
							{
								roomAt = room;
								charAt = it->second;
								return false;
							}
						}
						// NPC - Generate random action
						else
						{
							it->second->action();
						}
						it->second->setCanPerformAction(false);
					}
				}
				// Generate loot and remove character
				else
				{
					if(controllable)
					{
						cout << it->second->getName() << " was killed! " << endl;
						
						cout << "\nPress enter to continue . . . ";
						cin.sync();
						cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
						cout << endSeperatorLine;
					}
					it->second->killCharacter(true);
				}
			}
		}

		if(!thereIsAPlayer)
		{
			cout << whipeSkull << endl;
			cout << "     The return of whipeskull!\n  All of your character were killed.\n\t    GAME OVER" << endl;
			cout << "\nPress enter to continue . . . ";
			cin.sync();
			cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			cout << endSeperatorLine;
		}

		return thereIsAPlayer;
	}

	// Returns a string containing menu options, such as ability to save, load, start a new game or to exit.
	string GameEngine::getMainMenu() const
	{
		return gameTitle + "\n\nMain Menu\n--------\n" + ((isPaused)?"0. Resume Game\n":"") + "1. Play Game \n2. Exit game";
	}
}