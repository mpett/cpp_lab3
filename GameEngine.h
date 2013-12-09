/// Description: Header file for GameEngine class.
///
/// Authors: Martin Pettersson, Christoffer Wiss
///	Version: 2013-12-09
#pragma once
#include <string>
#include <vector>
#include <map>
#include <limits>
#include <iostream>
#include "Environment.h"
#include "Character.h"

namespace GameLogic
{
	static const std::string version = "0.95";
	static const std::string gameTitle             = "**************************************\n"
		                                             "****** ________________________ ******\n"
		                                             "*****//                        \\\\*****\n"
		                                             "|/|/|| The Legend of Whipeskull ||\\|\\|\n"
		                                             "*****\\\\________________________//*****\n"
	                                                 "******       Version "+ version +"       ******\n"
													 "**************************************\n";

	static const std::string whipeSkull          = 
													"0000000000000000000000000000000000000000\n"
													"000000000000 10000000000 000000000000000\n"
													"000000000000          1  000000000000000\n"
													"000000000000            0000000000000000\n"
													"00000000000              000000000000000\n"
													"00000000000              000000000000000\n"
													"0000000000                00000000000000\n"
													"000000000                  0000000000000\n"
													"000000000                  0000000000000\n"
													"000000000                  0000000000000\n"
													"000000000 000          000 0000000000000\n"
													"000000000  0000      0000  0000000000000\n"
													"000000000  00000     0000 00000000000000\n"
													"000000000  000        000  0000000000000\n"
													"000000001  00     0    00  0000000000000\n"
													"00000000         00         000000000000\n"
													"00000000         00         000000000000\n"
													"000000000       0000       1000000000000\n"
													"0000000000                00000000000000\n"
													"0000000000000          00000000000000000\n"
													"0000000000000          00000000000000000\n"
													"00000000000001        100000000000000000\n"
													"0000000000000   0000   00000000000000000\n"
													"0000000000000 000000 0 00000000000000000\n"
													"0000000000000000000000000000000000000000\n";

	static const std::string startEventsLine       = "----------------EVENTS----------------\n";
	static const std::string startPlayerEventsLine = "-------------PLAYER EVENTS------------\n";
	static const std::string endSeperatorLine      = "--------------------------------------\n";
	static const std::string helpText			   = "Welcome to the Legend of Whipeskull! These are your available commands:\n Move, Take, Help, Look, Drop, Inventory, Show, Stats, Equip, Unequip, Attack.\n Good luck!";
	
	class GameEngine
	{
	public:
		typedef bool (Character::*ACT_PTR) (std::string); // Declare a member function pointer type
		
		// Shows a message to user and holds continuation of program until enter is pressed.
		static void pressEnterToContinue();
		
		// Returns the nr of characters (width) of console.
		static int GetConsoleBufferWidth();
		
		// You have won the game!
		static void winGame();
		
		// Outputs formatted string (with newlines inserted after or before words) to std::cout
		static void FormattedOutput(std::string str);

		// Destructor
		~GameEngine();

		// Constructor 
		GameEngine();

		// Deletes all objects associated with current game instance.
		void cleanUpGame();

		// Parses input string from player console.
		void processPlayerTurn(Character& character);

		// What might this function do?
		void bonus();
		
		// "Clears" the terminal window.
		void clearScreen();

		// Loads a saved game state from file.
		bool loadGame(std::string file);

		// Saves a game state to file.
		bool saveGame(std::string file) const;
		
		// Start a new game, create all items and inhabit environments.
		void startNewGame(std::string file);

		// Run each time a new turn has occurred.
		bool newTurn();

		// Updates current state of the game. This is the main method.
		void mainLoop();

		// Always call a new turn until game is over.
		// The game is played out here.
		void gameLoop();

		// Goes through each room, runs action for each Character in room.
		bool updateRooms();

		// Returns a string containing menu options, such as ability to save, load, start a new game or to exit.
		std::string getMainMenu() const;
	
	private:
		// Prints character along with each character in its inventory
		void printInvCharacters(Character& character, std::stringstream& ss, int& nrCharacters) const;
		
		// Prints relations between characters (i.e. characters in inventory)
		void printInvCharRel(Character& character, std::stringstream& ss, int& nrCharacters) const;
		
		// Returns the index of room in environments_.
		int getIndexRoom(Environment* env) const;
		
		// Updates the non-player characters (their turn)
		void updateNPC();
		
		// Updates the player characters (their turn)
		bool updatePC(bool useStartValues);
	
		std::vector<Environment*> environments_;
		std::map<std::string, ACT_PTR> actions_;
	};
}