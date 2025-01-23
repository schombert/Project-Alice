/**
C++ implementation of the SHA512, SHA384 and SHA256 hashing algorithms.

From: https://github.com/pr0f3ss/SHA

Copyright (c) 2022 Filip Dobrosavljevic

MIT license

@file
 * @brief Header file describing the function signatures and the constants of the SHA512 Algorithm
 */

#include <string>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <iomanip>
#include <cstdint>
#include <sstream>

#include "SHA512.hpp"

namespace network {

/**
 * SHA512 class constructor
 */
SHA512::SHA512() { }

/**
 * SHA512 class destructor
 */
SHA512::~SHA512() { }

/**
 * Returns a message digest using the SHA512 algorithm
 * @param input message string used as an input to the SHA512 algorithm, must be < size_t bits
 */
std::string SHA512::hash(const std::string input) {
	size_t nBuffer; // amount of message blocks
	uint64** buffer; // message block buffers (each 1024-bit = 16 64-bit words)
	uint64* h = new uint64[HASH_LEN]; // buffer holding the message digest (512-bit = 8 64-bit words)

	buffer = preprocess((unsigned char*)input.c_str(), nBuffer);
	process(buffer, nBuffer, h);

	freeBuffer(buffer, nBuffer);
	return digest(h);
}

/**
 * Preprocessing of the SHA512 algorithm
 * @param input message in byte representation
 * @param nBuffer amount of message blocks
 */
uint64** SHA512::preprocess(const unsigned char* input, size_t& nBuffer) {
	// Padding: input || 1 || 0*k || l (in 128-bit representation)
	size_t mLen = strlen((const char*)input);
	size_t l = mLen * CHAR_LEN_BITS; // length of input in bits
	size_t kp = (896 - 1 - l) % MESSAGE_BLOCK_SIZE; // length of zero bit padding (l + 1 + k = 896 mod 1024) 
	nBuffer = (l + 1 + kp + 128) / MESSAGE_BLOCK_SIZE;

	uint64** buffer = new uint64 * [nBuffer];

	for(size_t i = 0; i < nBuffer; i++) {
		buffer[i] = new uint64[SEQUENCE_LEN];
	}

	uint64 in;
	size_t index;

	// Either copy existing message, add 1 bit or add 0 bit
	for(size_t i = 0; i < nBuffer; i++) {
		for(size_t j = 0; j < SEQUENCE_LEN; j++) {
			in = 0x0ULL;
			for(size_t k = 0; k < WORD_LEN; k++) {
				index = i * 128 + j * 8 + k;
				if(index < mLen) {
					in = in << 8 | (uint64)input[index];
				} else if(index == mLen) {
					in = in << 8 | 0x80ULL;
				} else {
					in = in << 8 | 0x0ULL;
				}
			}
			buffer[i][j] = in;
		}
	}

	// Append the length to the last two 64-bit blocks
	appendLen(l, buffer[nBuffer - 1][SEQUENCE_LEN - 1], buffer[nBuffer - 1][SEQUENCE_LEN - 2]);
	return buffer;
}

/**
 * Processing of the SHA512 algorithm
 * @param buffer array holding the preprocessed
 * @param nBuffer amount of message blocks
 * @param h array of output message digest
 */
void SHA512::process(uint64** buffer, size_t nBuffer, uint64* h) {
	uint64 s[WORKING_VAR_LEN];
	uint64 w[MESSAGE_SCHEDULE_LEN];

	memcpy(h, hPrime, WORKING_VAR_LEN * sizeof(uint64));

	for(size_t i = 0; i < nBuffer; i++) {
		// copy over to message schedule
		memcpy(w, buffer[i], SEQUENCE_LEN * sizeof(uint64));

		// Prepare the message schedule
		for(size_t j = 16; j < MESSAGE_SCHEDULE_LEN; j++) {
			w[j] = w[j - 16] + sig0(w[j - 15]) + w[j - 7] + sig1(w[j - 2]);
		}
		// Initialize the working variables
		memcpy(s, h, WORKING_VAR_LEN * sizeof(uint64));

		// Compression
		for(size_t j = 0; j < MESSAGE_SCHEDULE_LEN; j++) {
			uint64 temp1 = s[7] + Sig1(s[4]) + Ch(s[4], s[5], s[6]) + m[j] + w[j];
			uint64 temp2 = Sig0(s[0]) + Maj(s[0], s[1], s[2]);

			s[7] = s[6];
			s[6] = s[5];
			s[5] = s[4];
			s[4] = s[3] + temp1;
			s[3] = s[2];
			s[2] = s[1];
			s[1] = s[0];
			s[0] = temp1 + temp2;
		}

		// Compute the intermediate hash values
		for(size_t j = 0; j < WORKING_VAR_LEN; j++) {
			h[j] += s[j];
		}
	}

}

/**
 * Appends the length of the message in the last two message blocks
 * @param l message size in bits
 * @param lo pointer to second last message block
 * @param hi pointer to last message block
 */
void SHA512::appendLen(size_t l, uint64& lo, uint64& hi) {
	lo = l;
	hi = 0x00ULL;
}

/**
 * Outputs the final message digest in hex representation
 * @param h array of output message digest
 */
std::string SHA512::digest(uint64* h) {
	std::stringstream ss;
	for(size_t i = 0; i < OUTPUT_LEN; i++) {
		ss << std::hex << std::setw(16) << std::setfill('0') << h[i];
	}
	delete[] h;
	return ss.str();
}

/**
 * Free the buffer correctly
 * @param buffer array holding the preprocessed
 * @param nBuffer amount of message blocks
 */
void SHA512::freeBuffer(uint64** buffer, size_t nBuffer) {
	for(size_t i = 0; i < nBuffer; i++) {
		delete[] buffer[i];
	}

	delete[] buffer;
}

}

