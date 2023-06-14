#include <iostream>
#include <cstdint>
#include <chrono>


struct entry {
	uint16_t sender;
	uint32_t amount;
	uint16_t receiver;
};	// 8 Bytes

void populate_array() {
	//entry Entries[17424];
	entry * Entries = new entry[8712];
	for(uint32_t i = 1; i < 8712; i++) {
		Entries[i].sender = i;
		Entries[i].amount = 512;
		Entries[i].receiver = i - 1;
	}

	delete[] Entries;
}

int main() {
	using std::chrono::high_resolution_clock;
	using std::chrono::duration_cast;
	using std::chrono::duration;
	using std::chrono::milliseconds;

	for(uint32_t i = 0; i < 100000; i++) {
	auto t1 = high_resolution_clock::now();
	populate_array();
	auto t2 = high_resolution_clock::now();
	auto ms_int = duration_cast<milliseconds>(t2 - t1);

	duration<double, std::milli> ms_double = t2 - t1;

	std::cout << ms_double.count() << "\n";
	}

	return 0;
}
