/// Description: The GameEngine maintains the current game, allowing user to provide input, 
///              diplays text to screen and updates world accordingly.
///
/// Authors: Martin Pettersson, Christoffer Wiss
///	Version: 2013-12-09

#include "GameEngine.h"
#include "DungeonRoom.h"
#include "AppleRoom.h"
#include "GoalRoom.h"
#include "Human.h"
#include "Scholar.h"
#include "Goblin.h"
#include "Golem.h"
#include "Dragon.h"
#include "NormalSword.h"
#include "Potion.h"
#include "StrengthPotion.h"
#include "Food.h"
#include "SpikedBall.h"
#include "NormalArmor.h"
#include "NormalBow.h"
#include <fstream>
#include <algorithm>

#ifdef _WIN32 
#include <conio.h>
#define NOMINMAX
#include <Windows.h>
//#define USE_SOUND
#else
 #include <sys/ioctl.h>
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

	// Game scenario relevant variables
	string intro; // Shown on beginning of a new game
	string outro; // Shown on the end of a game (i.e when winning)
	bool newGame;
	bool wonGame;

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

	// Shows a message to user and holds continuation of program until enter is pressed.
	void GameEngine::pressEnterToContinue()
	{
		cout << "\nPress enter to continue . . . ";
		cin.sync();
		cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
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
			if(wonGame) return;
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
							bool consumedOnPickup = std::atoi(readLine.c_str()) != 0;
						
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
							// Strength Potion
							else if(consType == "Strength Potion")
							{
								// READ STRENGTH AFFECTED
								std::getline(saveFile, line);
								readLine = line.substr(0, line.find("#", 0));
								readLine.erase(remove(readLine.begin(),readLine.end(),'\t'),readLine.end());  // Trim tabs
								int strengthAffected = std::atoi(readLine.c_str());
								StrengthPotion * potion = new StrengthPotion(consName, consType, consPrice, consWeight, consNrUses, consumedOnPickup, strengthAffected,consId);
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
							bool equipped = std::atoi(readLine.c_str()) != 0;
							
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
							// Normal Bow
							else if(consType == "Normal Bow")
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
								Equipable * weapon = new NormalBow(consName, consType, consPrice, consWeight, equipped, minDMG, maxDMG, strengthReq,consId);
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
							cerr << "Going to find room equipables" << endl;
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
							cerr << "Going to find room misc items" << endl;
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
							cerr << "Going to find room consumables" << endl;
							// Find all CONSUMABLES
							while ((pos = readLine.find(delimiter)) != std::string::npos) {
								consumableInts.push_back(std::atoi(readLine.substr(0, pos).c_str()));
								readLine.erase(0, pos + delimiter.length());
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
							// Dungeon Room
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
							// Goal Room
							else if(type == "Goal Room")
							{
								Environment * goalRoom = new GoalRoom(desc);
								goalRoom->setRoomRequirement(roomReq);
								
								// Add consumables
								for(int item : consumableInts)
								{	
									goalRoom->addConsumable(consumables.find(item)->second);
								}
								// Add misc items
								for(int item : miscItemsInts)
								{	
									goalRoom->addMiscItem(miscItems.find(item)->second);
								}
								// Add equipables
								for(string item : equipableString)
								{	
									goalRoom->addEquipable(equipables.find(item)->second);
								}
								environments_.push_back(goalRoom);
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
								string dir   = exit.substr(0, exit.find(delimiter)); // String direction
								int neighbor = std::atoi(exit.substr(exit.find(delimiter)+1, string::npos).c_str()); // String room
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
							bool canPerformAction = std::atoi(readLine.c_str()) != 0;
							
							// CONTROLLABLE
							std::getline(saveFile, line);
							readLine = line.substr(0, line.find("#", 0));
							readLine.erase(remove(readLine.begin(),readLine.end(),'\t'),readLine.end());  // Trim tabs
							bool controllable = std::atoi(readLine.c_str()) != 0;
							
							// AT NEW ROOM
							std::getline(saveFile, line);
							readLine = line.substr(0, line.find("#", 0));
							readLine.erase(remove(readLine.begin(),readLine.end(),'\t'),readLine.end());  // Trim tabs
							bool atNewRoom = std::atoi(readLine.c_str()) != 0;
							
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
							// Find all consumables
							while ((pos = readLine.find(delimiter)) != std::string::npos) {
								consumableInts.push_back(std::atoi(readLine.substr(0, pos).c_str()));
								readLine.erase(0, pos + delimiter.length());
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
								
								human->setAtNewRoom(atNewRoom); // Make sure to preserve setAtNewRoom (can be altered when adding character to a room)
								human->setCurrentCarried(currCarried); //Make sure to preserve currCarried (might have been altered by some "spell"
								human->setMaxCarried(maxCarried); //Make sure to preserve maxCarried (might have been altered by some "spell"
								human->setAIBehavior(aiBehavior);
								characters.insert(std::pair<string,Character*>(name, human));
							}
							// SCHOLAR
							else if(type == "Scholar")
							{
								// SEARCHED OBJECT
								std::getline(saveFile, line);
								readLine = line.substr(0, line.find("#", 0));
								readLine.erase(remove(readLine.begin(),readLine.end(),'\t'),readLine.end());  // Trim tabs
								string searchedObj = readLine;
								
								// REWARD OBJECT
								std::getline(saveFile, line);
								readLine = line.substr(0, line.find("#", 0));
								readLine.erase(remove(readLine.begin(),readLine.end(),'\t'),readLine.end());  // Trim tabs
								string rewardObj = readLine;
								
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

								Scholar * scholar = new Scholar(controllable, name, type, weight, strength, currHP, maxHP, minDMG, maxDMG, currCarried, maxCarried, currArmor, currWeapon, canPerformAction, atNewRoom, environments_[currRoom]);

								// Add consumables
								for(int item : consumableInts)
								{	
									scholar->addInvConsumable(consumables.find(item)->second);
								}
								// Add misc items
								for(int item : miscItemsInts)
								{	
									scholar->addInvMiscItem(miscItems.find(item)->second);
								}
								// Add equipables
								for(string item : equipableString)
								{	
									scholar->addInvEquipable(equipables.find(item)->second);
								}
								// Add things to say
								for(string sentence : thingsToSay)
								{	
									scholar->addThingToSay(sentence);
								}
								
								scholar->setSearchedObject(searchedObj);
								scholar->setRewardObject(rewardObj);
								
								scholar->setAtNewRoom(atNewRoom); // Make sure to preserve setAtNewRoom (can be altered when adding character to a room)
								scholar->setCurrentCarried(currCarried); //Make sure to preserve currCarried (might have been altered by some "spell"
								scholar->setMaxCarried(maxCarried); //Make sure to preserve maxCarried (might have been altered by some "spell"
								scholar->setAIBehavior(aiBehavior);
								characters.insert(std::pair<string,Character*>(name, scholar));
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
								goblin->setAtNewRoom(atNewRoom); // Make sure to preserve setAtNewRoom (can be altered when adding character to a room)
								goblin->setCurrentCarried(currCarried); //Make sure to preserve currCarried (might have been altered by some "spell"
								goblin->setMaxCarried(maxCarried); //Make sure to preserve maxCarried (might have been altered by some "spell"
								goblin->setAIBehavior(aiBehavior);
								characters.insert(std::pair<string,Character*>(name, goblin));
							}
							// GOLEM
							else if(type == "Golem")
							{
								// TARGET
								std::getline(saveFile, line);
								readLine = line.substr(0, line.find("#", 0));
								readLine.erase(remove(readLine.begin(),readLine.end(),'\t'),readLine.end());  // Trim tabs
								string target = readLine;
								
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

								Golem * golem = new Golem(controllable, name, type, weight, strength, currHP, maxHP, minDMG, maxDMG, currCarried, maxCarried, currArmor, currWeapon, canPerformAction, atNewRoom, environments_[currRoom]);

								// Add consumables
								for(int item : consumableInts)
								{	
									golem->addInvConsumable(consumables.find(item)->second);
								}
								// Add misc items
								for(int item : miscItemsInts)
								{	
									golem->addInvMiscItem(miscItems.find(item)->second);
								}
								// Add equipables
								for(string item : equipableString)
								{	
									golem->addInvEquipable(equipables.find(item)->second);
								}
								// Add things to say
								for(string sentence : thingsToSay)
								{	
									golem->addThingToSay(sentence);
								}
								
								golem->setTarget(target);
								
								golem->setAtNewRoom(atNewRoom); // Make sure to preserve setAtNewRoom (can be altered when adding character to a room)
								golem->setCurrentCarried(currCarried); //Make sure to preserve currCarried (might have been altered by some "spell"
								golem->setMaxCarried(maxCarried); //Make sure to preserve maxCarried (might have been altered by some "spell"
								golem->setAIBehavior(aiBehavior);
								characters.insert(std::pair<string,Character*>(name, golem));
							}
							// DRAGON
							else if(type == "Dragon")
							{
								// CURR FIRE
								std::getline(saveFile, line);
								readLine = line.substr(0, line.find("#", 0));
								readLine.erase(remove(readLine.begin(),readLine.end(),'\t'),readLine.end());  // Trim tabs
								int currFire = std::atoi(readLine.c_str());

								// FIRE CAP.
								std::getline(saveFile, line);
								readLine = line.substr(0, line.find("#", 0));
								readLine.erase(remove(readLine.begin(),readLine.end(),'\t'),readLine.end());  // Trim tabs
								int fireCap = std::atoi(readLine.c_str());
								
								// FIRE GEN.
								std::getline(saveFile, line);
								readLine = line.substr(0, line.find("#", 0));
								readLine.erase(remove(readLine.begin(),readLine.end(),'\t'),readLine.end());  // Trim tabs
								int fireGen = std::atoi(readLine.c_str());

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

								Dragon * dragon = new Dragon(controllable, name, type, weight, strength, currHP, maxHP, minDMG, maxDMG, currCarried, maxCarried, currArmor, currWeapon, canPerformAction, atNewRoom, environments_[currRoom]);

								// Add consumables
								for(int item : consumableInts)
								{	
									dragon->addInvConsumable(consumables.find(item)->second);
								}
								// Add misc items
								for(int item : miscItemsInts)
								{	
									dragon->addInvMiscItem(miscItems.find(item)->second);
								}
								// Add equipables
								for(string item : equipableString)
								{	
									dragon->addInvEquipable(equipables.find(item)->second);
								}
								// Add things to say
								for(string sentence : thingsToSay)
								{	
									dragon->addThingToSay(sentence);
								}
								
								dragon->setCurrFire(currFire);
								dragon->setFireCapacity(fireCap);
								dragon->setFireGen(fireGen);
								
								dragon->setAtNewRoom(atNewRoom); // Make sure to preserve setAtNewRoom (can be altered when adding character to a room)
								dragon->setCurrentCarried(currCarried); //Make sure to preserve currCarried (might have been altered by some "spell"
								dragon->setMaxCarried(maxCarried); //Make sure to preserve maxCarried (might have been altered by some "spell"
								dragon->setAIBehavior(aiBehavior);
								characters.insert(std::pair<string,Character*>(name, dragon));
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
							
							cerr << "CHAR AT " << charAt->getName() << (charAt) << endl;
							cerr << "ROOM AT " << roomAt->getType() << (roomAt) << endl;
							nrToProcess--;
						}
						break;
					}
					// FIND INTRO TEXT (ONLY SHOWN ON NEW GAME)
					case(8):
					{
						// Should be one - but for sake of continuity have same format as the others
						while(nrToProcess > 0)
						{
							// READ INTRO
							std::getline(saveFile, line);
							string readLine = line.substr(0, line.find("#", 0));
							readLine.erase(remove(readLine.begin(),readLine.end(),'\t'),readLine.end());  // Trim tabs
						
							if(readLine == "") continue;

							intro = readLine;
							
							cerr << "INTRO TEXT LOADED " << endl;
							nrToProcess--;
						}
						break;
					}
					// FIND OUTRO TEXT (ONLY SHOWN ON WINNING GAME)
					case(9):
					{
						// Should be one - but for sake of continuity have same format as the others
						while(nrToProcess > 0)
						{
							// READ OUTRO
							std::getline(saveFile, line);
							string readLine = line.substr(0, line.find("#", 0));
							readLine.erase(remove(readLine.begin(),readLine.end(),'\t'),readLine.end());  // Trim tabs
						
							if(readLine == "") continue;

							outro = readLine;
							
							cerr << "OUTRO TEXT LOADED " << endl;
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
			std::cerr << "Could not open file." << endl;;
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
		
		// Make sure file is open
		if(!saveFile.is_open())
		{
			return false;
		}
		
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
			}
			for(auto itC = c.begin(); itC != c.end(); itC++) 
			{
				auto charConsumables = itC->second->getConsumables();
				for(auto it = charConsumables.begin(); it != charConsumables.end(); it++)
				{
					totalCons++;
					ss << it->second->printItem() << endl;
				}
				auto invChars = itC->second->getCharacters();
				for(auto itinv  = invChars.begin(); itinv != invChars.end(); itinv++)
				{
					auto invCharCons = itinv->second->getConsumables();
					for(auto itinvc = invCharCons.begin(); itinvc != invCharCons.end(); itinvc++)
					{
						totalCons++;
						ss << itinvc->second->printItem() << endl;
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
			}
			
			for(auto itC = c.begin(); itC != c.end(); itC++) 
			{
				auto charItems = itC->second->getMiscItems();
				for(auto it = charItems.begin(); it != charItems.end(); it++)
				{
					totalMiscItems++;
					ss << it->second->printItem() << endl;
				}

				auto invChars = itC->second->getCharacters();
				for(auto itinv  = invChars.begin(); itinv != invChars.end(); itinv++)
				{
					auto invCharMisc = itinv->second->getMiscItems();
					for(auto itinvc = invCharMisc.begin(); itinvc != invCharMisc.end(); itinvc++)
					{
						totalMiscItems++;
						ss << itinvc->second->printItem() << endl;
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
			}
			
			for(auto itC = c.begin(); itC != c.end(); itC++) 
			{
				auto charEquipables = itC->second->getEquipables();
				for(auto it = charEquipables.begin(); it != charEquipables.end(); it++)
				{
					totalEqs++;
					ss << it->second->printItem() << endl;
				}

				auto invChars = itC->second->getCharacters();
				for(auto itinv  = invChars.begin(); itinv != invChars.end(); itinv++)
				{
					auto invCharMisc = itinv->second->getEquipables();
					for(auto itinvc = invCharMisc.begin(); itinvc != invCharMisc.end(); itinvc++)
					{
						totalEqs++;
						ss << itinvc->second->printItem() << endl;
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
			bool first = true;
			for(auto it = exits.begin(); it != exits.end(); it++)
			{
				if(!first) ss << ";";
				ss << it->first << ",";
				ss << getIndexRoom(it->second);
				first = false;
			}
			ss << endl << endl;
		}

		saveFile << totalRoomsExits << endl << endl;
		saveFile << ss.str() << endl;
		ss.str("");
		ss.clear();
		
		// WRITE CHARACTERS
		saveFile << "# READ CHARACTERS" << endl;
		int totalCharacters = 0;

		// Go through each room
		for(Environment * room : environments_)
		{
			auto c = room->getCharacters();
			// Each character
			for(auto it = c.begin(); it != c.end(); it++)
			{
				printInvCharacters(*(it->second), ss, totalCharacters);
			}
		}

		saveFile << totalCharacters << endl << endl;
		saveFile << ss.str() << endl;
		ss.str("");
		ss.clear();
		
		// WRITE CHARACTERS IN INV
		saveFile << "# READ CHARACTERS IN INVENTORY" << endl;
		totalCharacters = 0;
		
		// Go through each room
		for(Environment * room : environments_)
		{
			auto c = room->getCharacters();
			// Each character
			for(auto it = c.begin(); it != c.end(); it++)
			{
				printInvCharRel(*it->second, ss, totalCharacters);
			}
		}
		
		saveFile << totalCharacters << endl << endl;
		saveFile << ss.str() << endl;
		ss.str("");
		ss.clear();
		
		// WRITE SAVED CAHRACTER AT
		saveFile << "# READ WHICH CHARACTER WE SAVED AT" << endl;
		saveFile << 1 << endl << endl; // For now this will be hardcoded (nr of characters saved at; makes sense to only save at one at a time)
		
		if(charAt == nullptr) 
		{
			cerr << "ERROR: Character at was not set when trying to save." << endl;
			throw 1;
		}
		else
		{
			saveFile << charAt->getName() << endl;
		}
		saveFile << endl;
		
		// WRITE INTRO
		saveFile << "# READ INTRO TEXT (ONLY SHOWN ON NEW GAME)" << endl;
		saveFile << 1 << endl << endl; // For now this will be hardcoded (nr of intro; makes sense to be only one)
		
		ss << intro << endl;
		saveFile << ss.str() << endl;
		
		ss.str("");
		ss.clear();
		
		// WRITE OUTRO
		saveFile << "# READ OUTRO TEXT (ONLY SHOW UPON WINNING THE GAME)" << endl;
		saveFile << 1 << endl << endl; // For now this will be hardcoded (nr of outro; makes sense to be only one)
		
		ss << outro << endl;
		saveFile << ss.str() << endl;
		
		ss.str("");
		ss.clear();

		saveFile.close();
		return true;
	}
	
	// Prints relations between characters (i.e. characters in inventory)
	void GameEngine::printInvCharRel(Character& character, std::stringstream& ss, int& nrCharacters) const
	{
		nrCharacters++;
		ss << character.getName() << endl;
		
		bool first = true;
		auto invChars = character.getCharacters();
		// Each inventory character
		for(auto itinvc = invChars.begin(); itinvc != invChars.end(); itinvc++)
		{
			if(!first) ss << ";";
			ss << itinvc->second->getName();
			first = false;
		}
		ss << endl;
		ss << endl;
		
		// Do recursive calls
		for(auto itinvc = invChars.begin(); itinvc != invChars.end(); itinvc++)
		{
			printInvCharRel(*itinvc->second, ss, nrCharacters);
		}
	}
	
	// Prints character along with each character in its inventory
	void GameEngine::printInvCharacters(Character& character, std::stringstream& ss, int& nrCharacters) const
	{
		nrCharacters++;
		ss << getIndexRoom(character.getCurrentRoom()) << "\n" << character.printCharacter() << endl;
		
		auto invChars = character.getCharacters();
		// Each inventory character
		for(auto itinvc = invChars.begin(); itinvc != invChars.end(); itinvc++)
		{
			printInvCharacters(*itinvc->second, ss, nrCharacters);
		}
	}
	
	// Returns the index of room in environments_.
	int GameEngine::getIndexRoom(Environment* env) const
	{
		auto it = std::find(environments_.begin(), environments_.end(), env);
		if (it == environments_.end())
		{
			cerr << "ERROR: Character room does no long exist" << endl;
		  throw -1;
		  return -1;
		} 
		else
		{
		  return std::distance(environments_.begin(), it);
		}
	}
	
	// Returns the nr of characters (width) of console.
	int GameEngine::GetConsoleBufferWidth()
	{
	#ifdef _WIN32
		CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
		int bufferWidth, result;
	 
		result = GetConsoleScreenBufferInfo(GetStdHandle( STD_OUTPUT_HANDLE ),&bufferInfo);
		if(result)
		{
			bufferWidth = bufferInfo.dwSize.X;
		}
		return bufferWidth;
	#else
		struct winsize w;
		ioctl(0, TIOCGWINSZ, &w);
		return w.ws_col;
	#endif
	}
	
	// Outputs formatted string (with newlines inserted after or before words) to std::cout
	// Originally by: Alex Rodgers 
	void GameEngine::FormattedOutput(string str)
	{
		int consoleWidth = GetConsoleBufferWidth();
 
		for (unsigned int i = 1; i <= str.length() ; i++)
		{
			char c = str[i-1];
	 
			int spaceCount = 0;
	 
			// Add whitespace if newline detected.
			if (c == '\n')
			{
				int charNumOnLine = ((i) % consoleWidth);
				spaceCount = consoleWidth - charNumOnLine;
				str.insert((i-1), (spaceCount), ' ');
				i+=(spaceCount);
				continue;
			}
	 
			if ((i % consoleWidth) == 0)
			{
				if (c != ' ')
				{
					for (int j = (i-1); j > -1 ; j--)
					{
						if (str[j] == ' ')
						{
							str.insert(j, spaceCount, ' ');
							break;
						}
						else spaceCount++;
					}
				}
			}
		}
		cout << str;
	}
	
	// You have won the game!
	void GameEngine::winGame()
	{
		wonGame = true;
	}
		
	// Start a new game, create all items and inhabit environments.
	void GameEngine::startNewGame(std::string file)
	{
		loadGame(file);
		newGame = true;
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
					startNewGame("map01.map");
					gameLoop();
					break;
				// Load game
				case(2):
					cleanUpGame(); // We might have a game going on
					if(loadGame("save_test.file"))
					{
						isPaused = true;
						cout << "Game loaded." << endl;
						cerr << "Player is: " << charAt << endl;
						cerr << "Room in: " << roomAt << endl;
						pressEnterToContinue();
					}
					else
					{
						cout << "Error: There is no saved game by that name." << endl;					
						pressEnterToContinue();
					}
					break;
				// Save game
				case(3):
					if(isPaused)
					{
						if(saveGame("save_test.file"))
						{
							cout << "Save successful!";
							pressEnterToContinue();
						}
						else
						{
							cout << "There was an error when saving the game.";
							pressEnterToContinue();
						}
					}
					else
					{
						cout << "This is not a menu option." << endl;
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
			if(newGame) 
			{
				clearScreen();
				cout << endSeperatorLine;
				FormattedOutput(intro);
				cout << endl;
				pressEnterToContinue();
				newGame = false;
			}
			else if(wonGame)
			{
				clearScreen();
				cout << endSeperatorLine;
				FormattedOutput(outro);
				cout << endl;
				pressEnterToContinue();
				wonGame = false;
				doGameLoop = false;
			}
			else
			{
				doGameLoop = newTurn();
			}
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

	// Updates the non-player characters (their turn)
	void GameEngine::updateNPC()
	{
		for(Environment * room : environments_)
		{
			auto characters = room->getCharacters(); // Not the most effective but it works...
			for(auto it = characters.begin(); it != characters.end(); it++)
			{
				bool controllable = it->second->isControllable();
				
				if(!controllable)
				{
					// Check if character is alive
					if(it->second->isAlive())
					{
						// Do not apply turn event when resuming game
						room->turnEvent(*it->second);

						if(it->second->getCanPerformAction())
						{
							it->second->setInNewRoom(false);
							it->second->action();
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
		}
	}
	
	// Updates the player characters (their turn)
	bool GameEngine::updatePC(bool useStartValues)
	{
		bool thereIsAPlayer = false;
		for(Environment * room : environments_)
		{
			if(useStartValues && room != roomAt) continue; // Resume at correct room

			auto characters = room->getCharacters(); // Not the most effective but it works...
			for(auto it = characters.begin(); it != characters.end(); it++)
			{
				if(useStartValues && charAt != it->second) continue; // Resume at correct character
				bool controllable = it->second->isControllable();
				if(controllable)
				{
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
						}
						thereIsAPlayer = true;

						if(it->second->getCanPerformAction())
						{
							bool inNewRoom = it->second->getInNewRoom();
							it->second->setInNewRoom(false);

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
		}

		if(!thereIsAPlayer)
		{
			cout << whipeSkull << endl;
			cout << "     The return of Whipeskull!\nAll of your character were incapacitated.\n\t    GAME OVER" << endl;
			#ifdef USE_SOUND
						Beep(1000,300);
						Beep(800, 300);
						Beep(200, 1000);
			#endif
			pressEnterToContinue();
			cout << endSeperatorLine;
		}
		
		return thereIsAPlayer;
	}
	
	// Goes through each room, runs action for each Character in room.
	bool GameEngine::updateRooms()
	{
		bool useStartValues = isPaused;
		isPaused = false;
		
		bool res = updatePC(useStartValues);  // Update player characters
		if(res) updateNPC(); // Update NPC (only if there are players that are alive)
		return res;
	}

	// Returns a string containing menu options, such as ability to save, load, start a new game or to exit.
	string GameEngine::getMainMenu() const
	{
		return gameTitle + "\n\nMain Menu\n--------\n" + ((isPaused)?"0. Resume Game\n":"") + "1. Play Game \n2. Load Game \n" + ((isPaused)?"3. Save Game ":"") + "\n4. Exit Game";
	}
}