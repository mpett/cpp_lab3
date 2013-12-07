/// Description: The GameEngine maintains the current game, allowing user to provide input, 
///              diplays text to screen and updates world accordingly.
///
/// Authors: Martin Pettersson, Christoffer Wiss
///	Version: 2013-12-07

#include "GameEngine.h"
#include "DungeonRoom.h"
#include "AppleRoom.h"
#include "Human.h"
#include "Goblin.h"
#include "NormalSword.h"
#include "Potion.h"
#include "Food.h"
#include "SpikedBall.h"
#include "NormalArmor.h"
#include <iostream>
#include <fstream>
#include <algorithm>

#ifdef _WIN32 
#include <conio.h>
#define NOMINMAX
#include <Windows.h>
#define USE_SOUND
#else
 #include <unistd.h>   //_getch*/
 #include <termios.h>  //_getch*/
#endif

using std::string;
using std::cout;
using std::cerr;
using std::endl;
using std::cin;
using std::ifstream;
using std::ofstream;

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

#ifdef _WIN32 
#ifdef USE_SOUND
			Beep(200,200);
			Beep(300,200);
			Beep(200,200);
			Beep(300,200);
#endif
#endif

			cout << endl;

			ss >> inputVerb >> std::ws;
			getline(ss,rest);
			std::transform(inputVerb.begin(), inputVerb.end(),inputVerb.begin(), ::toupper);

			auto it = actions_.find(inputVerb);

			// Could not find action
			if(it == actions_.end())
			{
				// ???
				if(inputVerb == "BONUS")
				{
					bonus(); // It is a secret to everybody
				}
				else
				{
					cout << "Action is not available." << endl;
				}
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

	// What might this function do?
	void GameEngine::bonus()
	{
		#ifdef _WIN32 
		cout << "Initiating Song of Storms Melody Protocol.." << endl;
		Sleep(80);
		Beep(698,100);
		Sleep(80);
		Beep(1175,200);
		Sleep(250);
		
		Beep(587,100);
		Sleep(80);
		Beep(698,100);
		Sleep(80);
		Beep(1175,200);
		Sleep(250);

		Beep(1319, 200);
		Sleep(200);
		Beep(1397, 100);
		Sleep(100);
		Beep(1319, 100);
		Sleep(75);
		Beep(1397, 100);
		Sleep(75);
		Beep(1319, 100);
		Sleep(75);
		Beep(1047, 100);
		Sleep(75);
		Beep(880, 100);
		Sleep(300);

		Beep(880, 200);
		Sleep(100);
		Beep(587, 200);
		Sleep(100);
		Beep(698, 100);
		Sleep(100);
		Beep(784, 100);
		Sleep(100);
		Beep(880, 100);
		Sleep(500);

		Beep(880, 200);
		Sleep(100);
		Beep(587, 200);
		Sleep(100);
		Beep(698, 100);
		Sleep(100);
		Beep(784, 100);
		Sleep(100);
		Beep(659, 100);
		Sleep(500);

		Beep(587, 100);
		Sleep(80);
		Beep(698, 100);
		Sleep(80);
		Beep(1175, 200);
		Sleep(250);

		Beep(1319, 200);
		Sleep(200);
		Beep(1397, 100);
		Sleep(100);
		Beep(1319, 100);
		Sleep(80);
		Beep(1397, 100);
		Sleep(80);
		Beep(1319, 100);
		Sleep(80);
		Beep(1047, 100);
		Sleep(80);
		Beep(880, 100);
		Sleep(300);

		Beep(880, 200);
		Sleep(100);
		Beep(587, 200);
		Sleep(100);
		Beep(698, 100);
		Sleep(100);
		Beep(784, 100);
		Sleep(100);
		Beep(880, 300);
		Sleep(100);
		Beep(880, 200);
		Sleep(100);
		Beep(587,300);
		Sleep(2000);
		#else
		cout << "I am sorry, to hear this you need to be on a windows system =(." << endl;
		#endif
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
		int index = 0;
		int nrToProcess;
		string line;
		ifstream saveFile(file);
		
		if(saveFile.is_open())
		{
			std::map<int, Item*> miscItems;
			std::map<std::string, Equipable*> equipables;
			std::map<int, Consumable*> consumables;
			std::map<std::string, Character*> characters;
			while(std::getline(saveFile, line))
			{
				string readLine = line.substr(0, line.find("#", 0));
				if(readLine == "")continue;

				readLine.erase(remove(readLine.begin(),readLine.end(),' '),readLine.end());  // Trim whitespaces
				readLine.erase(remove(readLine.begin(),readLine.end(),'\t'),readLine.end());  // Trim whitespaces
				try
				{
					nrToProcess = std::stoi(readLine);
				} catch(std::exception&) 
				{
					std::cerr << "Not a number." << endl;
				}

				switch(index)
				{
					//Consumables
					case(0):
					{
						while(nrToProcess > 0)
						{
							// READ NAME
							std::getline(saveFile, line);
							string readLine = line.substr(0, line.find("#", 0));
						
							if(readLine == "") continue;

							readLine.erase(remove(readLine.begin(),readLine.end(),'\t'),readLine.end());  // Trim tabs
							string consName = readLine;
							
							// READ ID
							std::getline(saveFile, line);
							readLine = line.substr(0, line.find("#", 0));
							readLine.erase(remove(readLine.begin(),readLine.end(),'\t'),readLine.end());  // Trim tabs
							int consId = std::atoi(readLine.c_str());
							
							// READ TYPE
							std::getline(saveFile, line);
							readLine = line.substr(0, line.find("#", 0));
							readLine.erase(remove(readLine.begin(),readLine.end(),'\t'),readLine.end());  // Trim tabs
							string consType = readLine;
							
							// READ PRICE
							std::getline(saveFile, line);
							readLine = line.substr(0, line.find("#", 0));
							readLine.erase(remove(readLine.begin(),readLine.end(),'\t'),readLine.end());  // Trim tabs
							int consPrice = std::atoi(readLine.c_str());
							
							// READ WEIGHT
							std::getline(saveFile, line);
							readLine = line.substr(0, line.find("#", 0));
							readLine.erase(remove(readLine.begin(),readLine.end(),'\t'),readLine.end());  // Trim tabs
							double consWeight = std::atof(readLine.c_str());
							
							// READ NR_USES
							std::getline(saveFile, line);
							readLine = line.substr(0, line.find("#", 0));
							readLine.erase(remove(readLine.begin(),readLine.end(),'\t'),readLine.end());  // Trim tabs
							int consNrUses = std::atoi(readLine.c_str());
							
							// READ CONS ON PICKUP
							std::getline(saveFile, line);
							readLine = line.substr(0, line.find("#", 0));
							readLine.erase(remove(readLine.begin(),readLine.end(),'\t'),readLine.end());  // Trim tabs
							bool consumedOnPickup = static_cast <bool> (std::atoi(readLine.c_str()));
						
							// Food
							if(consType == "Food")
							{
								// READ HEALTH AFFECTED
								std::getline(saveFile, line);
								readLine = line.substr(0, line.find("#", 0));
								readLine.erase(remove(readLine.begin(),readLine.end(),'\t'),readLine.end());  // Trim tabs
								int healthAffected = std::atoi(readLine.c_str());
								Consumable * food = new Food(consName, consType, consPrice, consWeight, consNrUses, consumedOnPickup, healthAffected,consId);
								consumables.insert(std::pair<int,Consumable*>(consId, food));
							}
							// Potion
							else if(consType == "Potion")
							{
								// READ HEALTH AFFECTED
								std::getline(saveFile, line);
								readLine = line.substr(0, line.find("#", 0));
								readLine.erase(remove(readLine.begin(),readLine.end(),'\t'),readLine.end());  // Trim tabs
								int healthAffected = std::atoi(readLine.c_str());
								Consumable * potion = new Potion(consName, consType, consPrice, consWeight, consNrUses, consumedOnPickup, healthAffected,consId);
								consumables.insert(std::pair<int,Consumable*>(consId, potion));
							}
							// Spiked Ball
							else if(consType == "Spiked Ball of Pain and Death")
							{
								// READ HEALTH AFFECTED
								std::getline(saveFile, line);
								readLine = line.substr(0, line.find("#", 0));
								readLine.erase(remove(readLine.begin(),readLine.end(),'\t'),readLine.end());  // Trim tabs
								int healthAffected = std::atoi(readLine.c_str());
								Consumable * ball = new SpikedBall(consName, consType, consPrice, consWeight, consNrUses, consumedOnPickup, healthAffected,consId);
								consumables.insert(std::pair<int,Consumable*>(consId, ball));
							}
							nrToProcess--;
							cerr << "CONSUMABLE " << consName << " ADDED" << endl;
							ITEM_ID++;
						}
						break;
					}
					// MISC ITEMS
					case(1):
					{
						while(nrToProcess > 0)
						{
							// READ NAME
							std::getline(saveFile, line);
							string readLine = line.substr(0, line.find("#", 0));
						
							if(readLine == "") continue;
							readLine.erase(remove(readLine.begin(),readLine.end(),'\t'),readLine.end());  // Trim tabs
							string consName = readLine;
							
							// READ ID
							std::getline(saveFile, line);
							readLine = line.substr(0, line.find("#", 0));
							readLine.erase(remove(readLine.begin(),readLine.end(),'\t'),readLine.end());  // Trim tabs
							int consId = std::atoi(readLine.c_str());
							
							// READ TYPE
							std::getline(saveFile, line);
							readLine = line.substr(0, line.find("#", 0));
							readLine.erase(remove(readLine.begin(),readLine.end(),'\t'),readLine.end());  // Trim tabs
							string consType = readLine;
							
							// READ PRICE
							std::getline(saveFile, line);
							readLine = line.substr(0, line.find("#", 0));
							readLine.erase(remove(readLine.begin(),readLine.end(),'\t'),readLine.end());  // Trim tabs
							int consPrice = std::atoi(readLine.c_str());
							
							// READ WEIGHT
							std::getline(saveFile, line);
							readLine = line.substr(0, line.find("#", 0));
							readLine.erase(remove(readLine.begin(),readLine.end(),'\t'),readLine.end());  // Trim tabs
							double consWeight = std::atof(readLine.c_str());
						
							Item * ball = new Item(consName, consType, consPrice, consWeight,consId);
							miscItems.insert(std::pair<int,Item*>(consId, ball));
							
							nrToProcess--;
							cerr << "MISC ITEM " << consName << " ADDED" << endl;
							ITEM_ID++;
						}
						break;
					}
					// EQUIPABLE ITEMS
					case(2):
					{
						while(nrToProcess > 0)
						{
							// READ NAME
							std::getline(saveFile, line);
							string readLine = line.substr(0, line.find("#", 0));
						
							if(readLine == "") continue;

							readLine.erase(remove(readLine.begin(),readLine.end(),'\t'),readLine.end());  // Trim tabs
							string consName = readLine;
							
							// READ ID
							std::getline(saveFile, line);
							readLine = line.substr(0, line.find("#", 0));
							readLine.erase(remove(readLine.begin(),readLine.end(),'\t'),readLine.end());  // Trim tabs
							int consId = std::atoi(readLine.c_str());
							
							// READ TYPE
							std::getline(saveFile, line);
							readLine = line.substr(0, line.find("#", 0));
							readLine.erase(remove(readLine.begin(),readLine.end(),'\t'),readLine.end());  // Trim tabs
							string consType = readLine;
							
							// READ PRICE
							std::getline(saveFile, line);
							readLine = line.substr(0, line.find("#", 0));
							readLine.erase(remove(readLine.begin(),readLine.end(),'\t'),readLine.end());  // Trim tabs
							int consPrice = std::atoi(readLine.c_str());
							
							// READ WEIGHT
							std::getline(saveFile, line);
							readLine = line.substr(0, line.find("#", 0));
							readLine.erase(remove(readLine.begin(),readLine.end(),'\t'),readLine.end());  // Trim tabs
							double consWeight = std::atof(readLine.c_str());
							
							// EQUIPPED
							std::getline(saveFile, line);
							readLine = line.substr(0, line.find("#", 0));
							readLine.erase(remove(readLine.begin(),readLine.end(),'\t'),readLine.end());  // Trim tabs
							bool equipped = static_cast <bool> (std::atoi(readLine.c_str()));
							
							// READ REQ. DESC
							std::getline(saveFile, line);
							readLine = line.substr(0, line.find("#", 0));
							readLine.erase(remove(readLine.begin(),readLine.end(),'\t'),readLine.end());  // Trim tabs
							string reqDesc = readLine;

							// Normal Armor
							if(consType == "Normal Armor")
							{
								// READ HEALTH INCR
								std::getline(saveFile, line);
								readLine = line.substr(0, line.find("#", 0));
								readLine.erase(remove(readLine.begin(),readLine.end(),'\t'),readLine.end());  // Trim tabs
								int healthIncreased = std::atoi(readLine.c_str());
								
								// READ STRENGTH REQ
								std::getline(saveFile, line);
								readLine = line.substr(0, line.find("#", 0));
								readLine.erase(remove(readLine.begin(),readLine.end(),'\t'),readLine.end());  // Trim tabs
								int strengthReq = std::atoi(readLine.c_str());
								Equipable * armor = new NormalArmor(consName, consType, consPrice, consWeight, equipped, healthIncreased, strengthReq, consId);
								armor->setRequirementDesc(reqDesc);
								
								equipables.insert(std::pair<string,Equipable*>(consName, armor));
							}
							// Normal Weapon
							else if(consType == "Normal Sword")
							{
								// MIN DMG
								std::getline(saveFile, line);
								readLine = line.substr(0, line.find("#", 0));
								readLine.erase(remove(readLine.begin(),readLine.end(),'\t'),readLine.end());  // Trim tabs
								int minDMG = std::atoi(readLine.c_str());
								
								// MAX DMG
								std::getline(saveFile, line);
								readLine = line.substr(0, line.find("#", 0));
								readLine.erase(remove(readLine.begin(),readLine.end(),'\t'),readLine.end());  // Trim tabs
								int maxDMG = std::atoi(readLine.c_str());
								
								// STRENGTH REQ
								std::getline(saveFile, line);
								readLine = line.substr(0, line.find("#", 0));
								readLine.erase(remove(readLine.begin(),readLine.end(),'\t'),readLine.end());  // Trim tabs
								int strengthReq = std::atoi(readLine.c_str());
								Equipable * weapon = new NormalSword(consName, consType, consPrice, consWeight, equipped, minDMG, maxDMG, strengthReq,consId);
								weapon->setRequirementDesc(reqDesc);
								
								equipables.insert(std::pair<string,Equipable*>(consName, weapon));
							}
							cerr << "EQUIPABLE " << consName << " ADDED" << endl;
							nrToProcess--;
							ITEM_ID++;
						}
						break;
					}
					// CREATE ROOMS
					case(3):
					{
						while(nrToProcess > 0)
						{
							// READ TYPE
							std::getline(saveFile, line);
							string readLine = line.substr(0, line.find("#", 0));
						
							if(readLine == "") continue;

							readLine.erase(remove(readLine.begin(),readLine.end(),'\t'),readLine.end());  // Trim tabs
							string type = readLine;
							
							// READ DESC
							std::getline(saveFile, line);
							readLine = line.substr(0, line.find("#", 0));
							readLine.erase(remove(readLine.begin(),readLine.end(),'\t'),readLine.end());  // Trim tabs
							string desc = readLine;
							
							// READ EQUIPABLES
							std::getline(saveFile, line);
							readLine = line.substr(0, line.find("#", 0));
							readLine.erase(remove(readLine.begin(),readLine.end(),'\t'),readLine.end());  // Trim tabs
							std::vector<string> equipableString;

							string delimiter = ",";
							size_t pos = 0;

							pos = 0;
							// Find all equipables
							while ((pos = readLine.find(delimiter)) != string::npos) {
								equipableString.push_back(readLine.substr(0, pos));
								readLine.erase(0, pos + delimiter.length());
							}

							if(readLine.find(delimiter) == string::npos)
							{
								if(readLine != "")
								{
									equipableString.push_back(readLine);
								}
							}

							// READ MISC ITEMS
							std::getline(saveFile, line);
							readLine = line.substr(0, line.find("#", 0));
							readLine.erase(remove(readLine.begin(),readLine.end(),'\t'),readLine.end());  // Trim tabs
							std::vector<int> miscItemsInts;

							pos = 0;
							// Find all misc items
							while ((pos = readLine.find(delimiter)) != std::string::npos) {
								miscItemsInts.push_back(std::atoi(readLine.substr(0, pos).c_str()));
								readLine.erase(0, pos + delimiter.length());
							}

							if(readLine.find(delimiter) == string::npos)
							{
								if(readLine != "")
								{
									miscItemsInts.push_back(std::atoi(readLine.c_str()));
								}
							}
							

							// READ CONSUMABLES
							std::getline(saveFile, line);
							readLine = line.substr(0, line.find("#", 0));
							readLine.erase(remove(readLine.begin(),readLine.end(),'\t'),readLine.end());  // Trim tabs
							std::vector<int> consumableInts;

							pos = 0;
							// Find all CONSUMABLES
							while ((pos = readLine.find(delimiter)) != std::string::npos) {
								consumableInts.push_back(std::atoi(readLine.substr(0, pos).c_str()));
							}
							
							if(readLine.find(delimiter) == string::npos)
							{
								if(readLine != "")
								{
									consumableInts.push_back(std::atoi(readLine.c_str()));
								}
							}
							
							// READ ROOM REQUIREMENT STRING
							std::getline(saveFile, line);
							readLine = line.substr(0, line.find("#", 0));
							readLine.erase(remove(readLine.begin(),readLine.end(),'\t'),readLine.end());  // Trim tabs
							string roomReq = readLine;

							// Apple Room
							if(type == "Apple Room")
							{
								Environment * appleRoom = new AppleRoom(desc);
								appleRoom->setRoomRequirement(roomReq);
								
								// Add consumables
								for(int item : consumableInts)
								{	
									appleRoom->addConsumable(consumables.find(item)->second);
								}
								// Add misc items
								for(int item : miscItemsInts)
								{	
									appleRoom->addMiscItem(miscItems.find(item)->second);
								}
								// Add equipables
								for(string item : equipableString)
								{	
									appleRoom->addEquipable(equipables.find(item)->second);
								}
								environments_.push_back(appleRoom);
							}
							// Normal Weapon
							else if(type == "Dungeon Room")
							{
								Environment * dungeonRoom = new DungeonRoom(desc);
								dungeonRoom->setRoomRequirement(roomReq);
								
								// Add consumables
								for(int item : consumableInts)
								{	
									dungeonRoom->addConsumable(consumables.find(item)->second);
								}
								// Add misc items
								for(int item : miscItemsInts)
								{	
									dungeonRoom->addMiscItem(miscItems.find(item)->second);
								}
								// Add equipables
								for(string item : equipableString)
								{	
									dungeonRoom->addEquipable(equipables.find(item)->second);
								}
								environments_.push_back(dungeonRoom);
							}
							cerr << "ROOM " << type << " ADDED" << endl;
							nrToProcess--;
						}
						break;
					}
					// ADD EXITS
					case(4):
					{
						int roomI = 0;
						while(nrToProcess > 0)
						{
							// READ EXITS
							std::getline(saveFile, line);
							string readLine = line.substr(0, line.find("#", 0));
						
							if(readLine == "") continue;

							readLine.erase(remove(readLine.begin(),readLine.end(),'\t'),readLine.end());  // Trim tabs
							
							string delimiter = ";";
							std::vector<string> exits;
							size_t pos = 0;
							// Find all exits
							while ((pos = readLine.find(delimiter)) != std::string::npos) 
							{
								exits.push_back(readLine.substr(0, pos));
								readLine.erase(0, pos + delimiter.length());
							}
							if(readLine.find(delimiter) == string::npos)
							{
								if(readLine != "")
								{
									exits.push_back(readLine);
								}
							}

							delimiter = ",";
							// Process and add each exit to room
							for(string exit : exits)
							{
								string dir   = readLine.substr(0, readLine.find(delimiter)); // String direction
								int neighbor = std::atoi(readLine.substr(readLine.find(delimiter)+1, string::npos).c_str()); // String room
								environments_[roomI]->addNeigbor(environments_[neighbor],dir);
							}
							nrToProcess--;
							cerr << "ROOM " << roomI << " EXITS ADDED" << endl;
							roomI++;
						}
						break;
					}
					// Character
					case(5):
					{
						while(nrToProcess > 0)
						{
							// CURRENT ROOM INDEX
							std::getline(saveFile, line);
							readLine = line.substr(0, line.find("#", 0));
							
							if(readLine == "") continue;
							
							readLine.erase(remove(readLine.begin(),readLine.end(),'\t'),readLine.end());  // Trim tabs
							int currRoom = std::atoi(readLine.c_str());
							
							// READ NAME
							std::getline(saveFile, line);
							string readLine = line.substr(0, line.find("#", 0));
							readLine.erase(remove(readLine.begin(),readLine.end(),'\t'),readLine.end());  // Trim tabs
							string name = readLine;
							
							// READ TYPE
							std::getline(saveFile, line);
							readLine = line.substr(0, line.find("#", 0));
							readLine.erase(remove(readLine.begin(),readLine.end(),'\t'),readLine.end());  // Trim tabs
							string type = readLine;
							
							// READ STRENGTH
							std::getline(saveFile, line);
							readLine = line.substr(0, line.find("#", 0));
							readLine.erase(remove(readLine.begin(),readLine.end(),'\t'),readLine.end());  // Trim tabs
							int strength = std::atoi(readLine.c_str());
							
							// READ WEIGHT
							std::getline(saveFile, line);
							readLine = line.substr(0, line.find("#", 0));
							readLine.erase(remove(readLine.begin(),readLine.end(),'\t'),readLine.end());  // Trim tabs
							double weight = std::atof(readLine.c_str());
							
							// READ CURRENT HP
							std::getline(saveFile, line);
							readLine = line.substr(0, line.find("#", 0));
							readLine.erase(remove(readLine.begin(),readLine.end(),'\t'),readLine.end());  // Trim tabs
							int currHP = std::atoi(readLine.c_str());
							
							// READ MAX HP
							std::getline(saveFile, line);
							readLine = line.substr(0, line.find("#", 0));
							readLine.erase(remove(readLine.begin(),readLine.end(),'\t'),readLine.end());  // Trim tabs
							int maxHP = std::atoi(readLine.c_str());
							
							// READ MIN DMG
							std::getline(saveFile, line);
							readLine = line.substr(0, line.find("#", 0));
							readLine.erase(remove(readLine.begin(),readLine.end(),'\t'),readLine.end());  // Trim tabs
							int minDMG = std::atoi(readLine.c_str());
							
							// READ MAX DMG
							std::getline(saveFile, line);
							readLine = line.substr(0, line.find("#", 0));
							readLine.erase(remove(readLine.begin(),readLine.end(),'\t'),readLine.end());  // Trim tabs
							int maxDMG = std::atoi(readLine.c_str());
							
							// READ CURRENT CARRIED
							std::getline(saveFile, line);
							readLine = line.substr(0, line.find("#", 0));
							readLine.erase(remove(readLine.begin(),readLine.end(),'\t'),readLine.end());  // Trim tabs
							int currCarried = std::atoi(readLine.c_str());
							
							// READ MAX CARRIED
							std::getline(saveFile, line);
							readLine = line.substr(0, line.find("#", 0));
							readLine.erase(remove(readLine.begin(),readLine.end(),'\t'),readLine.end());  // Trim tabs
							int maxCarried = std::atoi(readLine.c_str());
							
							// CURRENT EQUIPPED ARMOR
							std::getline(saveFile, line);
							readLine = line.substr(0, line.find("#", 0));
							readLine.erase(remove(readLine.begin(),readLine.end(),'\t'),readLine.end());  // Trim tabs
							string equippedArmor = readLine;
							
							// CURRENT EQUIPPED WEAPON
							std::getline(saveFile, line);
							readLine = line.substr(0, line.find("#", 0));
							readLine.erase(remove(readLine.begin(),readLine.end(),'\t'),readLine.end());  // Trim tabs
							string equippedWeapon = readLine;
							
							// THINGS TO SAY
							std::getline(saveFile, line);
							readLine = line.substr(0, line.find("#", 0));
							readLine.erase(remove(readLine.begin(),readLine.end(),'\t'),readLine.end());  // Trim tabsTrim tabs
							std::vector<string> thingsToSay;

							string delimiter = ";";
							size_t pos = 0;

							// Find all things to say
							while ((pos = readLine.find(delimiter)) != string::npos) {
								thingsToSay.push_back(readLine.substr(0, pos));
								readLine.erase(0, pos + delimiter.length());
							}

							if(readLine.find(delimiter) == string::npos)
							{
								if(readLine != "")
								{
									thingsToSay.push_back(readLine);
								}
							}
							
							// CAN PERFORM ACTION
							std::getline(saveFile, line);
							readLine = line.substr(0, line.find("#", 0));
							readLine.erase(remove(readLine.begin(),readLine.end(),'\t'),readLine.end());  // Trim tabs
							bool canPerformAction = static_cast <bool> (std::atoi(readLine.c_str()));
							
							// CONTROLLABLE
							std::getline(saveFile, line);
							readLine = line.substr(0, line.find("#", 0));
							readLine.erase(remove(readLine.begin(),readLine.end(),'\t'),readLine.end());  // Trim tabs
							bool controllable = static_cast <bool> (std::atoi(readLine.c_str()));
							
							// AT NEW ROOM
							std::getline(saveFile, line);
							readLine = line.substr(0, line.find("#", 0));
							readLine.erase(remove(readLine.begin(),readLine.end(),'\t'),readLine.end());  // Trim tabs
							bool atNewRoom = static_cast <bool> (std::atoi(readLine.c_str()));
							
							// AI BEHAVIOR (IF NPC)
							std::getline(saveFile, line);
							readLine = line.substr(0, line.find("#", 0));
							readLine.erase(remove(readLine.begin(),readLine.end(),'\t'),readLine.end());  // Trim tabs
							int aiBehavior = std::atoi(readLine.c_str());
							
							// INV MISC ITEMS
							std::getline(saveFile, line);
							readLine = line.substr(0, line.find("#", 0));
							readLine.erase(remove(readLine.begin(),readLine.end(),'\t'),readLine.end());  // Trim tabs
							std::vector<int> miscItemsInts;

							delimiter = ",";
							
							pos = 0;
							// Find all misc items
							while ((pos = readLine.find(delimiter)) != std::string::npos) {
								miscItemsInts.push_back(std::atoi(readLine.substr(0, pos).c_str()));
								readLine.erase(0, pos + delimiter.length());
							}

							if(readLine.find(delimiter) == string::npos)
							{
								if(readLine != "")
								{
									miscItemsInts.push_back(std::atoi(readLine.c_str()));
								}
							}
							
							// CONSUMABLES
							std::getline(saveFile, line);
							readLine = line.substr(0, line.find("#", 0));
							readLine.erase(remove(readLine.begin(),readLine.end(),'\t'),readLine.end());  // Trim tabs
							std::vector<int> consumableInts;

							pos = 0;
							// Find all misc items
							while ((pos = readLine.find(delimiter)) != std::string::npos) {
								consumableInts.push_back(std::atoi(readLine.substr(0, pos).c_str()));
							}
							
							if(readLine.find(delimiter) == string::npos)
							{
								if(readLine != "")
								{
									consumableInts.push_back(std::atoi(readLine.c_str()));
								}
							}
							
							// EQUIPABLES
							std::getline(saveFile, line);
							readLine = line.substr(0, line.find("#", 0));
							readLine.erase(remove(readLine.begin(),readLine.end(),'\t'),readLine.end());  // Trim tabs
							std::vector<string> equipableString;

							delimiter = ",";
							pos = 0;

							// Find all equipables
							while ((pos = readLine.find(delimiter)) != string::npos) {
								equipableString.push_back(readLine.substr(0, pos));
								readLine.erase(0, pos + delimiter.length());
							}

							if(readLine.find(delimiter) == string::npos)
							{
								if(readLine != "")
								{
									equipableString.push_back(readLine);
								}
							}
							
							// HUMAN
							if(type == "Human")
							{
								// Armor
								Equipable * currArmor;
								auto it = equipables.find(equippedArmor);
								if(it == equipables.end()) 
								{
									currArmor = nullptr; 
								}
								else
								{
									currArmor = it->second;
								}
								
								// Weapon
								Equipable * currWeapon;
								it = equipables.find(equippedWeapon);
								if(it == equipables.end()) 
								{
									currWeapon = nullptr; 
								}
								else
								{
									currWeapon = it->second;
								}

								Character * human = new Human(controllable, name, type, weight, strength, currHP, maxHP, minDMG, maxDMG, currCarried, maxCarried, currArmor, currWeapon, canPerformAction, atNewRoom, environments_[currRoom]);
								
								// Add consumables
								for(int item : consumableInts)
								{	
									human->addInvConsumable(consumables.find(item)->second);
								}
								// Add misc items
								for(int item : miscItemsInts)
								{	
									human->addInvMiscItem(miscItems.find(item)->second);
								}
								// Add equipables
								for(string item : equipableString)
								{	
									human->addInvEquipable(equipables.find(item)->second);
								}
								// Add things to say
								for(string sentence : thingsToSay)
								{	
									human->addThingToSay(sentence);
								}
								
								human->setCurrentCarried(currCarried); //Make sure to preserve currCarried (might have been altered by some "spell"
								human->setMaxCarried(maxCarried); //Make sure to preserve maxCarried (might have been altered by some "spell"
								human->setAIBehavior(aiBehavior);
								characters.insert(std::pair<string,Character*>(name, human));
							}
							// GOBLIN
							else if(type == "Goblin")
							{
								// Armor
								Equipable * currArmor;
								auto it = equipables.find(equippedArmor);
								if(it == equipables.end()) 
								{
									currArmor = nullptr; 
								}
								else
								{
									currArmor = it->second;
								}
								
								// Weapon
								Equipable * currWeapon;
								it = equipables.find(equippedWeapon);
								if(it == equipables.end()) 
								{
									currWeapon = nullptr; 
								}
								else
								{
									currWeapon = it->second;
								}

								Character * goblin = new Goblin(controllable, name, type, weight, strength, currHP, maxHP, minDMG, maxDMG, currCarried, maxCarried, currArmor, currWeapon, canPerformAction, atNewRoom, environments_[currRoom]);

								
								// Add consumables
								for(int item : consumableInts)
								{	
									goblin->addInvConsumable(consumables.find(item)->second);
								}
								// Add misc items
								for(int item : miscItemsInts)
								{	
									goblin->addInvMiscItem(miscItems.find(item)->second);
								}
								// Add equipables
								for(string item : equipableString)
								{	
									goblin->addInvEquipable(equipables.find(item)->second);
								}
								// Add things to say
								for(string sentence : thingsToSay)
								{	
									goblin->addThingToSay(sentence);
								}
								goblin->setCurrentCarried(currCarried); //Make sure to preserve currCarried (might have been altered by some "spell"
								goblin->setMaxCarried(maxCarried); //Make sure to preserve maxCarried (might have been altered by some "spell"
								goblin->setAIBehavior(aiBehavior);
								characters.insert(std::pair<string,Character*>(name, goblin));
							}
							std::cerr << "CHARACTER " << name << " ADDED " << endl;
							nrToProcess--;
						}
						break;
					}
					// ADD INVENTORY CHARACTERS
					case(6):
					{
						while(nrToProcess > 0)
						{
							// READ NAME
							std::getline(saveFile, line);
							string readLine = line.substr(0, line.find("#", 0));
							readLine.erase(remove(readLine.begin(),readLine.end(),'\t'),readLine.end());  // Trim tabs
						
							if(readLine == "") continue;

							string name = readLine;
							
							// INV CHARACTERS
							std::getline(saveFile, line);
							readLine = line.substr(0, line.find("#", 0));
							readLine.erase(remove(readLine.begin(),readLine.end(),'\t'),readLine.end());  // Trim tabs
							
							string delimiter = ";";
							std::vector<string> chars;
							size_t pos = 0;
							// Find all characters in inventory
							while ((pos = readLine.find(delimiter)) != std::string::npos) 
							{
								chars.push_back(readLine.substr(0, pos));
								readLine.erase(0, pos + delimiter.length());
							}
							if(readLine.find(delimiter) == string::npos)
							{
								if(readLine != "")
								{
									chars.push_back(readLine);
								}
							}

							auto meIT = characters.find(name);
							if(meIT == characters.end()) throw 1; // Error!
							
							if(!chars.empty())
							{
								double currCarried = meIT->second->getCurrentCarried();
								// Add each character to inventory
								for(string character : chars)
								{
									auto invIT = characters.find(character);
									cerr << character << endl;
									if(invIT == characters.end()) throw 1; // Error!
									
									invIT->second->getCurrentRoom()->removeCharacter(character);
									invIT->second->setCanPerformAction(false); 					
									meIT->second->addInvCharacter(invIT->second);
									cerr << "INV CHARACTER " << character << " FOR " << name << " ADDED" << endl;
								}
								meIT->second->setCurrentCarried(currCarried); // Preserve currCarried (might have been affected by a "spell")
							}
							nrToProcess--;
						}
						break;
					}
					// FIND CHARACTER WHOSE TURN GAME WAS SAVED AT
					case(7):
					{
						// Should be one - but for sake of continuity have same format as the others
						while(nrToProcess > 0)
						{
							// READ NAME
							std::getline(saveFile, line);
							string readLine = line.substr(0, line.find("#", 0));
							readLine.erase(remove(readLine.begin(),readLine.end(),'\t'),readLine.end());  // Trim tabs
						
							if(readLine == "") continue;

							string name = readLine;
							
							auto it = characters.find(name);
							if(it == characters.end()) throw 1; // ERROR
							
							charAt = it->second;
							roomAt = it->second->getCurrentRoom();
							
							nrToProcess--;
						}
						break;
					}
				}
				index++;
			}
			saveFile.close();
			
			miscItems.clear();
			equipables.clear();
			consumables.clear();
			characters.clear();
			
			return true;
		} else
		{
			std::cerr << "Could not open file.";
		}
		return false;
	}

	// Saves a game state to file.
	bool GameEngine::saveGame(std::string file) const
	{
		ofstream saveFile;
		std::stringstream ss;
		ss.str("");
		ss.clear();
		saveFile.open(file);
		int totalCons = 0;
		// WRITE CONSUMABLES
		saveFile << "# READ CONSUMABLES" << endl;
		for(Environment * room : environments_)
		{
			auto consumables = room->getConsumables();
			auto c = room->getCharacters();
			for(auto it = consumables.begin(); it != consumables.end(); it++)
			{
				totalCons++;
				ss << it->second->printItem() << endl;
				//ss << it->second->getName() << endl;
				//ss << it->second->getId() << endl;
				//ss << it->second->getType() << endl;
				//ss << it->second->getPrice() << endl;
				//ss << it->second->getWeight() << endl;
				//ss << it->second->getNrUses() << endl;
				//if(it->second->isConsumedOnPickup())
				//	ss << "1" << endl;
				//else
				//	ss << "0" << endl;
				//string type = it->second->getType();
				//if(type == "Potion" || type == "SpikedBall" || type == "Apple")
				//{
				//	ss << it->second->getHealthAffected() << endl;
				//}
			}
			for(auto itC = c.begin(); itC != c.end(); itC++) 
			{
				auto charConsumables = itC->second->getConsumables();
				for(auto it = charConsumables.begin(); it != charConsumables.end(); it++)
				{
					totalCons++;
					ss << it->second->printItem() << endl;
					//ss << it->second->getName() << endl;
					//ss << it->second->getId() << endl;
					//ss << it->second->getType() << endl;
					//ss << it->second->getPrice() << endl;
					//ss << it->second->getWeight() << endl;
					//ss << it->second->getNrUses() << endl;
					//if(it->second->isConsumedOnPickup())
					//	ss << "1" << endl;
					//else
					//	ss << "0" << endl;
					//string type = it->second->getType();
					//if(type == "Potion" || type == "SpikedBall" || type == "Apple")
					//{
					//	ss << it->second->getHealthAffected() << endl;
					//}
					//ss << endl;
				}
				auto invChars = itC->second->getCharacters();
				for(auto itinv  = invChars.begin(); itinv != invChars.end(); itinv++)
				{
					auto invCharCons = itinv->second->getConsumables();
					for(auto itinvc = invCharCons.begin(); itinvc != invCharCons.end(); itinvc++)
					{
						totalCons++;
						ss << itinvc->second->printItem() << endl;
						//ss << itinvc->second->getName() << endl;
						//ss << itinvc->second->getId() << endl;
						//ss << itinvc->second->getType() << endl;
						//ss << itinvc->second->getPrice() << endl;
						//ss << itinvc->second->getWeight() << endl;
						//ss << itinvc->second->getNrUses() << endl;
						//if(itinvc->second->isConsumedOnPickup())
						//	ss << "1" << endl;
						//else
						//	ss << "0" << endl;
						//string type = itinvc->second->getType();
						//if(type == "Potion" || type == "SpikedBall" || type == "Apple")
						//{
						//	ss << itinvc->second->getHealthAffected() << endl;
						//}
						//ss << endl;
					}
				}
			}
			
		}

		saveFile << totalCons << endl << endl;
		saveFile << ss.str();
		ss.str("");
		ss.clear();
		
		// WRITE MISC ITEMS
		saveFile << "# READ MISCITEMS" << endl;
		int totalMiscItems = 0;

		for(Environment * room : environments_)
		{
			auto items = room->getMiscItems();
			auto c = room->getCharacters();
			for(auto it = items.begin(); it != items.end(); it++)
			{
				totalMiscItems++;
				ss << it->second->printItem() << endl;
				//ss << it->second->getName() << endl;
				//ss << it->second->getId() << endl;
				//ss << it->second->getType() << endl;
				//ss << it->second->getPrice() << endl;
				//ss << it->second->getWeight() << endl;
				//ss << endl;
			}
			
			for(auto itC = c.begin(); itC != c.end(); itC++) 
			{
				auto charItems = itC->second->getMiscItems();
				for(auto it = charItems.begin(); it != charItems.end(); it++)
				{
					totalMiscItems++;
					ss << it->second->printItem() << endl;
					//ss << it->second->getName() << endl;
					//ss << it->second->getId() << endl;
					//ss << it->second->getType() << endl;
					//ss << it->second->getPrice() << endl;
					//ss << it->second->getWeight() << endl;
					//ss << endl;
				}

				auto invChars = itC->second->getCharacters();
				for(auto itinv  = invChars.begin(); itinv != invChars.end(); itinv++)
				{
					auto invCharMisc = itinv->second->getMiscItems();
					for(auto itinvc = invCharMisc.begin(); itinvc != invCharMisc.end(); itinvc++)
					{
						totalMiscItems++;
						ss << itinvc->second->printItem() << endl;
						//ss << itinvc->second->getName() << endl;
						//ss << itinvc->second->getId() << endl;
						//ss << itinvc->second->getType() << endl;
						//ss << itinvc->second->getPrice() << endl;
						//ss << itinvc->second->getWeight() << endl;
						//ss << endl;
					}
				}
			}
		}

		saveFile << totalMiscItems << endl << endl;
		saveFile << ss.str() << endl;
		ss.str("");
		ss.clear();

		// WRITE EQUIPABLES
		saveFile << "# READ EQUIPABLES" << endl;
		int totalEqs = 0;

		for(Environment * room : environments_)
		{
			auto equipables = room->getEquipables();
			auto c = room->getCharacters();
			for(auto it = equipables.begin(); it != equipables.end(); it++)
			{
				totalEqs++;
				ss << it->second->printItem() << endl;
				//ss << it->second->getName() << endl;
				//ss << it->second->getId() << endl;
				//ss << it->second->getType() << endl;
				//ss << it->second->getPrice() << endl;
				//ss << it->second->getWeight() << endl;
				//if(it->second->isEquipped())
				//	ss << "1" << endl;
				//else
				//	ss << "0" << endl;
				//ss << endl;

			}
			
			for(auto itC = c.begin(); itC != c.end(); itC++) 
			{
				//cout << itC->second->getName();
				auto charEquipables = itC->second->getEquipables();
				for(auto it = charEquipables.begin(); it != charEquipables.end(); it++)
				{
					totalEqs++;
					ss << it->second->printItem() << endl;
					//ss << it->second->getName() << endl;
					//ss << it->second->getId() << endl;
					//ss << it->second->getType() << endl;
					//ss << it->second->getPrice() << endl;
					//ss << it->second->getWeight() << endl;
					//if(it->second->isEquipped())
					//	ss << "1" << endl;
					//else
					//	ss << "0" << endl;
					//ss << endl;
				}

				auto invChars = itC->second->getCharacters();
				for(auto itinv  = invChars.begin(); itinv != invChars.end(); itinv++)
				{
					auto invCharMisc = itinv->second->getEquipables();
					for(auto itinvc = invCharMisc.begin(); itinvc != invCharMisc.end(); itinvc++)
					{
						totalEqs++;
						ss << itinvc->second->printItem() << endl;
						//ss << itinvc->second->getName() << endl;
						//ss << itinvc->second->getId() << endl;
						//ss << itinvc->second->getType() << endl;
						//ss << itinvc->second->getPrice() << endl;
						//ss << itinvc->second->getWeight() << endl;
						//if(itinvc->second->isEquipped())
						//	ss << "1" << endl;
						//else
						//	ss << "0" << endl;
						//ss << endl;
					}
				}
			}
		}

		saveFile << totalEqs << endl << endl;
		saveFile << ss.str() << endl;
		ss.str("");
		ss.clear();

		// WRITE ROOMS
		saveFile << "# READ ROOMS" << endl;
		int totalRooms = 0;

		for(Environment * room : environments_)
		{
			totalRooms++;
			ss << room->printEnvironment() << endl;
			//ss << room->getType() << endl;
			//ss << room->getShortDescription() << endl;
			//auto roomEqs = room->getEquipables();
			
			//string line = "";
			//for(auto it = roomEqs.begin(); it != roomEqs.end(); it++)
			//{
			//	line = it->second->getName() + ",";
			//}
			//line = line.substr(0, line.size()-1);
			//ss << line.c_str();
			//ss << endl;

			//auto roomMisc = room->getMiscItems();
			//line = "";
			//for(auto it = roomMisc.begin(); it != roomMisc.end(); it++)
			//{
			//	int id = it->second->getId();
			//	line += std::to_string(id) + ",";
			//}
			//line = line.substr(0, line.size()-1);
			//ss << line.c_str();
			//ss << endl;

			//line = "";
			//auto roomCons = room->getConsumables();
			//for(auto it = roomCons.begin(); it != roomCons.end(); it++)
			//{
			//	int id = it->second->getId();
			//	line += std::to_string(id) + ",";
			//}
			//line = line.substr(0, line.size()-1);
			//ss << line.c_str();
			//ss << endl << endl;

		}

		saveFile << totalRooms << endl << endl;
		saveFile << ss.str() << endl;
		ss.str("");
		ss.clear();

		// WRITE ROOM EXITS
		saveFile << "# READ ROOMS EXITS" << endl;
		int totalRoomsExits = 0;
		
		for(Environment * room : environments_)
		{
			totalRoomsExits++;
			auto exits = room->getExits();
			for(auto it = exits.begin(); it != exits.end(); it++)
			{
				ss << it->first << ",";
				string i = std::to_string(totalRoomsExits);
				ss << i.c_str() << endl;
			}
			ss << endl;
		}

		saveFile << totalRoomsExits << endl << endl;
		saveFile << ss.str() << endl;
		ss.str("");
		ss.clear();
		
		saveFile << "# READ CHARACTERS" << endl;
		int totalCharacters = 0;

		// Go through each room
		for(Environment * room : environments_)
		{
			auto c = room->getCharacters();
			// Each character
			for(auto it = c.begin(); it != c.end(); it++)
			{
				totalCharacters++;
				ss << it->second->printCharacter();
				auto invChars = it->second->getCharacters()
				
				// Each inventory character
				for(auto itinvc = invChars.begin(); itinvc != invChars.end(); itinvc++)
				{
					totalCharacters++;
					ss << it->second->printCharacter();
					// <- RECURSIVE CALL HERE
				}
				
			}
		}


		saveFile.close();
		return true;
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
		character->addThingToSay("Blargh blargh blurb!");
		character->setAIBehavior(AI_BEHAVIOR::PASSIVE_LOOTER);
		character = new Goblin(false,"Bonk", "Goblin", 5, 10, 10, 1, 1, 0, 500, env1);
		character->addThingToSay("Bonk!");
		character->setAIBehavior(AI_BEHAVIOR::DO_NOT_HIT_ME);
		character = new Goblin(false,"Goblin Scholar", "Goblin", 5, 1, 1, 1, 1, 0, 10, env2);
		character->addThingToSay("Ah..ehm, blargh blurgh blab. Quite blargh indeed!");
		character->addThingToSay("Mumble, mumble..");
		character->addThingToSay("Fascinating..");
		character->setAIBehavior(AI_BEHAVIOR::PACIFIST);

		env1->addNeigbor(env2, "UP");
		env2->addNeigbor(env1, "DOWN");
		env1->addNeigbor(env3, "STAIRS");
		env3->addNeigbor(env1, "STAIRS");
		env3->setRoomRequirement("Golden Key;Silver Key");

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
		ITEM_ID = 0;
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
					cleanUpGame(); // We might have a game going on
					clearScreen();
					cout << endSeperatorLine;
					startNewGame("file");
					gameLoop();
					break;
				// Load game
				case(2):
					cleanUpGame(); // We might have a game going on
					if(loadGame("save_file_template.file"))
					{
						isPaused = true;
						cout << "Game loaded." << endl;
						cerr << "Player is: " << charAt << endl;
						cerr << "Room in: " << roomAt << endl;
						
						cout << "\nPress enter to continue . . . ";
						cin.sync();
						cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
					}
					else
					{
						cout << "Error: There is no saved game by that name." << endl;
						
						cout << "\nPress enter to continue . . . ";
						cin.sync();
						cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
					}
					break;
				// Save game
				case(3):
					if(saveGame("save_test.file"))
					{
						cout << "Save successful!";
					}
					else
					{
						cout << "There was an error when saving the game.";
					}

					break;
				// Exit game
				case(4):
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
					if(!useStartValues)
					{
						room->turnEvent(*it->second);
						cerr << it->second->getName() << " C: " << it->second << " P: " << it->second->getCanPerformAction() << " R: " << it->second->getCurrentRoom() << endl;
					}
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
								it->second->look(""); // Output room information
							} else 
							{
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
				// Remove Character from room and delete from memory
				else
				{
					it->second->getCurrentRoom()->removeCharacter(it->second->getName());
					delete it->second;
				}
			}
		}

		if(!thereIsAPlayer)
		{
			cout << whipeSkull << endl;
			cout << "     The return of whipeskull!\nAll of your character were incapacitated.\n\t    GAME OVER" << endl;
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
		return gameTitle + "\n\nMain Menu\n--------\n" + ((isPaused)?"0. Resume Game\n":"") + "1. Play Game \n2. Load Game \n3. Save Game \n4. Exit Game";
	}
}