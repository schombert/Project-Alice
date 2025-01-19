/**
C++ implementation of the SHA512, SHA384 and SHA256 hashing algorithms.

From: https://github.com/pr0f3ss/SHA

Copyright (c) 2022 Filip Dobrosavljevic

MIT license

@file
 * @brief Header file describing the function signatures and the constants of the SHA512 Algorithm
 */

#ifndef H_SHA512
#define H_SHA512

#include <string>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <iomanip>
#include <cstdint>
#include <sstream>

namespace network {

typedef unsigned long long uint64;

// Constants
unsigned int const SEQUENCE_LEN = (1024 / 64);
size_t const HASH_LEN = 8;
size_t const WORKING_VAR_LEN = 8;
size_t const MESSAGE_SCHEDULE_LEN = 80;
size_t const MESSAGE_BLOCK_SIZE = 1024;
size_t const CHAR_LEN_BITS = 8;
size_t const OUTPUT_LEN = 8;
size_t const WORD_LEN = 8;

class SHA512 {
private:
	typedef unsigned long long uint64;

	const uint64 hPrime[8] = { 0x6a09e667f3bcc908ULL,
								0xbb67ae8584caa73bULL,
								0x3c6ef372fe94f82bULL,
								0xa54ff53a5f1d36f1ULL,
								0x510e527fade682d1ULL,
								0x9b05688c2b3e6c1fULL,
								0x1f83d9abfb41bd6bULL,
								0x5be0cd19137e2179ULL
	};

	const uint64 m[80] = { 0x428a2f98d728ae22ULL, 0x7137449123ef65cdULL, 0xb5c0fbcfec4d3b2fULL, 0xe9b5dba58189dbbcULL, 0x3956c25bf348b538ULL,
			  0x59f111f1b605d019ULL, 0x923f82a4af194f9bULL, 0xab1c5ed5da6d8118ULL, 0xd807aa98a3030242ULL, 0x12835b0145706fbeULL,
			  0x243185be4ee4b28cULL, 0x550c7dc3d5ffb4e2ULL, 0x72be5d74f27b896fULL, 0x80deb1fe3b1696b1ULL, 0x9bdc06a725c71235ULL,
			  0xc19bf174cf692694ULL, 0xe49b69c19ef14ad2ULL, 0xefbe4786384f25e3ULL, 0x0fc19dc68b8cd5b5ULL, 0x240ca1cc77ac9c65ULL,
			  0x2de92c6f592b0275ULL, 0x4a7484aa6ea6e483ULL, 0x5cb0a9dcbd41fbd4ULL, 0x76f988da831153b5ULL, 0x983e5152ee66dfabULL,
			  0xa831c66d2db43210ULL, 0xb00327c898fb213fULL, 0xbf597fc7beef0ee4ULL, 0xc6e00bf33da88fc2ULL, 0xd5a79147930aa725ULL,
			  0x06ca6351e003826fULL, 0x142929670a0e6e70ULL, 0x27b70a8546d22ffcULL, 0x2e1b21385c26c926ULL, 0x4d2c6dfc5ac42aedULL,
			  0x53380d139d95b3dfULL, 0x650a73548baf63deULL, 0x766a0abb3c77b2a8ULL, 0x81c2c92e47edaee6ULL, 0x92722c851482353bULL,
			  0xa2bfe8a14cf10364ULL, 0xa81a664bbc423001ULL, 0xc24b8b70d0f89791ULL, 0xc76c51a30654be30ULL, 0xd192e819d6ef5218ULL,
			  0xd69906245565a910ULL, 0xf40e35855771202aULL, 0x106aa07032bbd1b8ULL, 0x19a4c116b8d2d0c8ULL, 0x1e376c085141ab53ULL,
			  0x2748774cdf8eeb99ULL, 0x34b0bcb5e19b48a8ULL, 0x391c0cb3c5c95a63ULL, 0x4ed8aa4ae3418acbULL, 0x5b9cca4f7763e373ULL,
			  0x682e6ff3d6b2b8a3ULL, 0x748f82ee5defb2fcULL, 0x78a5636f43172f60ULL, 0x84c87814a1f0ab72ULL, 0x8cc702081a6439ecULL,
			  0x90befffa23631e28ULL, 0xa4506cebde82bde9ULL, 0xbef9a3f7b2c67915ULL, 0xc67178f2e372532bULL, 0xca273eceea26619cULL,
			  0xd186b8c721c0c207ULL, 0xeada7dd6cde0eb1eULL, 0xf57d4f7fee6ed178ULL, 0x06f067aa72176fbaULL, 0x0a637dc5a2c898a6ULL,
			  0x113f9804bef90daeULL, 0x1b710b35131c471bULL, 0x28db77f523047d84ULL, 0x32caab7b40c72493ULL, 0x3c9ebe0a15c9bebcULL,
			  0x431d67c49c100d4cULL, 0x4cc5d4becb3e42b6ULL, 0x597f299cfc657e2aULL, 0x5fcb6fab3ad6faecULL, 0x6c44198c4a475817ULL };

	uint64** preprocess(const unsigned char* input, size_t& nBuffer);
	void appendLen(size_t l, uint64& lo, uint64& hi);
	void process(uint64** buffer, size_t nBuffer, uint64* h);
	std::string digest(uint64* h);
	void freeBuffer(uint64** buffer, size_t nBuffer);

	// Operations
#define Ch(x,y,z) ((x&y)^(~x&z))
#define Maj(x,y,z) ((x&y)^(x&z)^(y&z))
#define RotR(x, n) ((x>>n)|(x<<((sizeof(x)<<3)-n)))
#define Sig0(x) ((RotR(x, 28))^(RotR(x,34))^(RotR(x, 39)))
#define Sig1(x) ((RotR(x, 14))^(RotR(x,18))^(RotR(x, 41)))
#define sig0(x) (RotR(x, 1)^RotR(x,8)^(x>>7))
#define sig1(x) (RotR(x, 19)^RotR(x,61)^(x>>6))

public:
	std::string hash(const std::string input);

	SHA512();
	~SHA512();
};


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

	buffer = preprocess((unsigned const char*)input.c_str(), nBuffer);
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

#endif
