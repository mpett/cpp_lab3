/// Description: Header file for GameEngine class.
///
/// Authors: Martin Pettersson, Christoffer Wiss
///	Version: 2013-11-27
#pragma once
#include <string>
#include <vector>
#include <map>
#include "Environment.h"

namespace GameLogic
{
	static const std::string startEventsLine       = "----------------EVENTS----------------\n";
	static const std::string startPlayerEventsLine = "-------------PLAYER EVENTS------------\n";
	static const std::string endSeperatorLine      = "--------------------------------------\n";
	static const std::string helpText			   = "Welcome to World of NoobCraft! These are your available commands:\n Move, Take, Help, Look, Drop, Inventory, Show, Stats, Equip, Unequip, Attack.\n Good luck!";
	
	class GameEngine
	{
	public:
		typedef bool (Character::*ACT_PTR) (std::string); // Declare a member function pointer type

		// Destructor
		~GameEngine();

		// Constructor 
		GameEngine();

		// Deletes all objects associated with current game instance.
		void cleanUpGame();

		// Parses input string from player console.
		void processPlayerTurn(Character&);

		// Returns true if this game has reached an end.
		bool gameOver() const;

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

		// -TODO-
		void gameLoop();

		// Goes through each room, runs action for each Character in room.
		bool updateRooms();

		// Returns a string containing menu options, such as ability to save, load, start a new game or to exit.
		std::string getMainMenu() const;
	
	private:
		std::vector<Environment*> environments_;
		std::map<std::string, ACT_PTR> actions_;
	};
}