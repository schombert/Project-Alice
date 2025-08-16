#include "network.hpp"
#include <iostream>
#include <string>
#include <sstream>
using namespace std;

namespace sys {

void process_console_command(sys::state& state, string command) {
	if(command == "help") {
		cout << "List of available commands:\n";
		cout << "pause - pauses the game\n";
		cout <<	"unpause - unpauses the game\n";
		cout << "speed [0-5] - changes the game speed\n";
	}
	else if(command == "pause" || command == "speed 0") {
		network::pause_game(state);
		cout << "Game speed set to " + std::to_string(state.actual_game_speed) + "\n";
	} else if(command == "unpause") {
		network::unpause_game(state);
		cout << "Game speed set to " + std::to_string(state.actual_game_speed) + "\n";
	} else if(command == "speed 1") {
		state.actual_game_speed = 1;
		cout << "Game speed set to " + std::to_string(state.actual_game_speed) + "\n";
	}
	else if(command == "speed 2") {
		state.actual_game_speed = 2;
		cout << "Game speed set to " + std::to_string(state.actual_game_speed) + "\n";
	}
	else if(command == "speed 3") {
		state.actual_game_speed = 3;
		cout << "Game speed set to " + std::to_string(state.actual_game_speed) + "\n";
	}
	else if(command == "speed 4") {
		state.actual_game_speed = 4;
		cout << "Game speed set to " + std::to_string(state.actual_game_speed) + "\n";
	}
	else if(command == "speed 5") {
		state.actual_game_speed = 5;
		cout << "Game speed set to " + std::to_string(state.actual_game_speed) + "\n";
	}
}

#ifdef _WIN32

void read_console_input(sys::state& state) {
	string command;
	getline(cin, command);

	process_console_command(state, command);
}

#else

void read_console_input(sys::state& state) {
	// TODO: for someone to implement reading input in Linux	
}

#endif

}
