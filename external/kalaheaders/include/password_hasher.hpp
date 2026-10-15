//---------------------------------------------------------------------------
// password_hasher.hpp
//
// Copyright (C) 2026 Lost Empire Entertainment
//
// This is free source code, and you are welcome to redistribute it under certain conditions.
// Read LICENSE.md for more information.
//
// Provides:
//   - Argon2id v1.3 (RFC 9106) and BLAKE2b (RFC 7693) compatible password hash implementation
//   - HashPassword function to convert any string into a hashed password and salt
//   - VerifyPassword function to confirm if a raw password is correct compared to a hashed password and its salt
//   - StringToBytes function to safely convert hexadecimal string to binary bytes
//   - BytesToString function to safely convert binary bytes to hexadecimal string 
//---------------------------------------------------------------------------

#pragma once

//
// SKIP UNSUPPORTED C++ VERSION
//

#if __cplusplus < 202002L
	#error "UNSUPPORTED C++ VERSION! SUPPORTED: C++20 AND ABOVE"
#endif

//
// SKIP UNSUPPORTED PLATFORMS AND ARCHITECTURES
//

#if !defined(K_REDEFINE_GUARD_PLAT_ARCH)
	#define K_REDEFINE_GUARD_PLAT_ARCH

	#if defined(__APPLE__) || \
		defined(__FreeBSD__) || \
		defined(__OpenBSD__) || \
		defined(__NetBSD__) || \
		defined(__DragonFly__) || \
		defined(__CYGWIN__) || \
		defined(__ANDROID__)
		#error "UNSUPPORTED TARGET! SUPPORTED: _WIN32, __linux__"
	#elif !defined(_WIN32) && \
		!defined(__linux__)
		#error "UNSUPPORTED TARGET! SUPPORTED: _WIN32, __linux__"
	#elif !defined(_M_X64) && \
		!defined(__x86_64__)
		#error "UNSUPPORTED ARCHITECTURE! SUPPORTED: x64"
	#endif
#endif

//
// WINDOWS/LINUX MACROS
//

#if !defined(K_REDEFINE_GUARD_WIN_LIN)
	#define K_REDEFINE_GUARD_WIN_LIN

	#if defined(_WIN32)
		//any targeting windows
		#define KWIN_ANY

		//any msvc targeting windows
		#if defined(_MSC_VER)
			#define KWIN_MSVC
		//any posix targeting Windows
		#elif defined(__GNUC__)
			#define KWIN_GNU
		#else
			#error "UNKNOWN COMPILER DETECTED"
		#endif
	#endif

	#if defined(__linux__)
		//any targeting linux
		#define KLIN_ANY

		//any libc targeting linux
		#if defined(__GLIBC__)
			#define KLIN_GNU
		//any musl targeting linux
		#else
			#define KLIN_MUSL
		#endif
	#endif
#endif

//
// DEBUG MACRO
//

#if !defined(K_REDEFINE_GUARD_REL_DEB)
	#define K_REDEFINE_GUARD_REL_DEB

	#if !defined(KDEBUG)
		#if (defined(_MSC_VER) || \
			defined(__MINGW64__)) && \
			defined(_DEBUG)
			#define KDEBUG
		#elif defined(__linux__) && \
			!defined(NDEBUG)
			#define KDEBUG
		#endif
	#endif
#endif

//
// CAST SHORTHANDS
//

#if !defined(K_REDEFINE_GUARD_CASTS)
	#define K_REDEFINE_GUARD_CASTS

	#define rcast reinterpret_cast
	#define scast static_cast
	#define ccast const_cast
#endif

//
// COMPILER MACROS
//

#if !defined(KNORETURN)
	#define KNORETURN [[noreturn]]
#endif

#if !defined(KNODISCARD)
	#define KNODISCARD [[nodiscard]]
#endif

#include <cstdint>

//
// NUMERIC TYPE SHORTHANDS
//

#if !defined(KNUM)
	#define KNUM
	//8-bit unsigned int
	//Min: 0
	//Max: 255
	using u8 = uint8_t;

	//16-bit unsigned int
	//Min: 0
	//Max: 65,535
	using u16 = uint16_t;

	//32-bit unsigned int
	//Min: 0
	//Max: 4,294,967,295
	using u32 = uint32_t;

	//64-bit unsigned int
	//Replaces handles and pointers (uintptr_t)
	//Min: 0
	//Max: 18 quintillion
	using u64 = uint64_t;

	//8-bit int
	//Min: -128
	//Max: 127
	using i8 = int8_t;

	//16-bit int
	//Min: -32,768
	//Max: 32,767
	using i16 = int16_t;

	//32-bit int
	//Min: -2,147,483,648
	//Max: 2,147,483,647
	using i32 = int32_t;

	//64-bit int
	//Min: -9 quintillion
	//Max: 9 quintillion
	using i64 = int64_t;

	//32-bit float
	//6 decimal precision
	using f32 = float;

	//64-bit float
	//15 decimal precision
	using f64 = double;
#endif

#include <string>
#include <vector>
#include <array>
#include <random>

namespace KalaHeaders::KalaPasswordHasher
{
	using std::string;
	using std::string_view;
	using std::to_string;
	using std::pair;
	using std::vector;
	using std::array;
	using std::random_device;

	static constexpr u8 MAX_PASSWORD_LENGTH_BYTES = 32;
	static constexpr u8 MIN_PASSWORD_LENGTH_BYTES = 8;

	//256-bit hashed password
	static constexpr u8 HASH_SIZE_BYTES = 32;
	//128-bit password salt
	static constexpr u8 SALT_SIZE_BYTES = 16;

	struct Argon2idConfig
	{
		//Value in KiB, more memory makes massively parallel password cracking more expensive
		//because every concurrent guess needs substantial memory, rather than just cheap compute
		u32 memoryCost = 19456;
		//Controls the number of passes over memory, increasing means more CPU/time per password attempt
		u32 timeCost = 2;
		//Number of independent lanes Argon2 uses, higher values allow to exploit more parallel execution
		u32 parallelism = 1;
	};

	KNODISCARD
	inline string _GenerateHash(
		string_view rawPassword,
		const array<u8, SALT_SIZE_BYTES>& salt,
		const Argon2idConfig& config,
		array<u8, HASH_SIZE_BYTES>& outHash);

	KNODISCARD
	inline string _VerifyRawPassword(string_view rawPassword);
	KNODISCARD
	inline string _VerifyArgon2idConfig(const Argon2idConfig& config);

	//Converts hashed password or password salt bytes to hexadecimal string
	template<size_t SIZE>
		requires (SIZE == HASH_SIZE_BYTES || SIZE == SALT_SIZE_BYTES)
	inline void BytesToString(
		const array<u8, SIZE>& value,
		string& outValue)
	{
		static constexpr char HEX[] = "0123456789abcdef";

		string result{};
		result.reserve(value.size() * 2);

		for (u8 byte : value)
		{
			result += HEX[byte >> 4];
			result += HEX[byte & 0x0F];
		}

		outValue = std::move(result);
	}

	//Converts hexadecimal string to hashed password or password salt bytes,
	//returns error string on failure
	template<size_t SIZE>
		requires (SIZE == HASH_SIZE_BYTES || SIZE == SALT_SIZE_BYTES)
	KNODISCARD
	inline string StringToBytes(
		string_view bytesString,
		array<u8, SIZE>& outValue)
	{
		if (bytesString.size() != SIZE * 2)
		{
			return "Bytes string size was invalid!";
		}

		auto hex_to_value = [](
			char value,
			u8& outValue) -> bool
			{
				if (value >= '0' && value <= '9')
				{
					outValue = scast<u8>(value - '0');
					return true;
				}

				if (value >= 'a' && value <= 'f')
				{
					outValue = scast<u8>((value - 'a') + 10);
					return true;
				}

				return false;
			};

		array<u8, SIZE> result{};

		for (size_t i = 0; i < result.size(); i++)
		{
			u8 high{};
			u8 low{};

			if (!hex_to_value(bytesString[i * 2], high)
				|| !hex_to_value(bytesString[(i * 2) + 1], low))
			{
				return "Bytes string contained invalid hexadecimal characters!";
			}

			result[i] = scast<u8>((high << 4) | low);
		}

		outValue = std::move(result);
		return "";
	}

	//Generates a random 128-bit salt
	KNODISCARD
	inline array<u8, SALT_SIZE_BYTES> GenerateSalt()
	{
		random_device rd{};

		array<u8, SALT_SIZE_BYTES> generatedSalt{};

		for (size_t i = 0; i < generatedSalt.size();)
		{
			u32 value = rd();

			for (size_t j = 0; j < sizeof(value) && i < generatedSalt.size(); j++, i++)
			{
				generatedSalt[i] = scast<u8>(value & 0xFF);
				value >>= 8;
			}
		}

		return generatedSalt;
	}

	//Takes in a raw password string and optional Argon2id config,
	//returns a string for error, 256-bit hash password and 128-bit salt
	KNODISCARD
	inline string HashPassword(
		string_view rawPassword,
		pair<array<u8, HASH_SIZE_BYTES>, array<u8, SALT_SIZE_BYTES>>& outResult,
		Argon2idConfig config = {})
	{
		string err = _VerifyRawPassword(rawPassword);
		if (!err.empty()) return err;

		err = _VerifyArgon2idConfig(config);
		if (!err.empty()) return err;

		pair<array<u8, HASH_SIZE_BYTES>, array<u8, SALT_SIZE_BYTES>> result = { {}, GenerateSalt() };

		err = _GenerateHash(
			rawPassword,
			result.second,
			config,
			result.first);

		if (!err.empty()) return err;

		outResult = std::move(result);
		return "";
	}

	//Takes in a raw password string, optional Argon2id config, hash password and salt,
	//returns filled string on errors and if raw password does not match hashed password
	KNODISCARD
	inline string VerifyPassword(
		string_view rawPassword,
		const pair<array<u8, HASH_SIZE_BYTES>, array<u8, SALT_SIZE_BYTES>>& hashAndSalt,
		Argon2idConfig config = {})
	{
		string err = _VerifyRawPassword(rawPassword);
		if (!err.empty()) return err;

		err = _VerifyArgon2idConfig(config);
		if (!err.empty()) return err;

		array<u8, HASH_SIZE_BYTES> outHash{};

		err = _GenerateHash(
			rawPassword,
			hashAndSalt.second,
			config,
			outHash);

		if (!err.empty()) return err;

		//Constant-time comparison
		u8 difference{};

		for (size_t i = 0; i < outHash.size(); i++)
		{
			difference |= scast<u8>(outHash[i] ^ hashAndSalt.first[i]);
		}

		return difference == 0 
			? "" 
			: "Raw password does not match hashed password!";
	}

	KNODISCARD
	inline string _GenerateHash(
		string_view rawPassword,
		const array<u8, SALT_SIZE_BYTES>& salt,
		const Argon2idConfig& config,
		array<u8, HASH_SIZE_BYTES>& outHash)
	{
		//Argon2id
		static constexpr u8 ARGON2_TYPE_ID = 2;

		static constexpr u64 BLAKE2B_IV[8] =
		{
			0x6A09E667F3BCC908ULL,
			0xBB67AE8584CAA73BULL,
			0x3C6EF372FE94F82BULL,
			0xA54FF53A5F1D36F1ULL,
			0x510E527FADE682D1ULL,
			0x9B05688C2B3E6C1FULL,
			0x1F83D9ABFB41BD6BULL,
			0x5BE0CD19137E2179ULL
		};

		auto store_u64 = [](
			u64 value,
			u8* data) -> void
			{
				for (u32 i = 0; i < 8; i++)
				{
					data[i] = scast<u8>(value & 0xFF);
					value >>= 8;
				}
			};

		auto store_u32 = [](
			u32 value,
			u8* data) -> void
			{
				for (u32 i = 0; i < 4; i++)
				{
					data[i] = scast<u8>(value & 0xFF);
					value >>= 8;
				}
			};

		auto load_u64 = [](const u8* data) -> u64
			{
				u64 value{};

				for (u32 i = 0; i < 8; i++)
				{
					value |= scast<u64>(data[i]) << (i * 8);
				}

				return value;
			};

		auto rotate_right_64 = [](
			u64 value,
			u32 amount) -> u64
			{
				return (value >> amount) | (value << (64 - amount));
			};

		auto blake2b_mix = [&rotate_right_64](
			u64& a,
			u64& b,
			u64& c,
			u64& d,
			u64 x,
			u64 y) -> void
			{
				a = a + b + x;
				d = rotate_right_64(d ^ a, 32);

				c += d;
				b = rotate_right_64(b ^ c, 24);

				a = a + b + y;
				d = rotate_right_64(d ^ a, 16);

				c += d;
				b = rotate_right_64(b ^ c, 63);
			};

		auto blake2b_compress = [
			&blake2b_mix,
			&load_u64](
			u64 state[8],
			const u8 block[128],
			u64 counterLow,
			u64 counterHigh,
			bool isFinal) -> void
			{
				static constexpr u8 BLAKE2B_SIGMA[12][16] =
				{
					{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 },
					{ 14, 10, 4, 8, 9, 15, 13, 6, 1, 12, 0, 2, 11, 7, 5, 3 },
					{ 11, 8, 12, 0, 5, 2, 15, 13, 10, 14, 3, 6, 7, 1, 9, 4 },
					{ 7, 9, 3, 1, 13, 12, 11, 14, 2, 6, 5, 10, 4, 0, 15, 8 },
					{ 9, 0, 5, 7, 2, 4, 10, 15, 14, 1, 11, 12, 6, 8, 3, 13 },
					{ 2, 12, 6, 10, 0, 11, 8, 3, 4, 13, 7, 5, 15, 14, 1, 9 },
					{ 12, 5, 1, 15, 14, 13, 4, 10, 0, 7, 6, 3, 9, 2, 8, 11 },
					{ 13, 11, 7, 14, 12, 1, 3, 9, 5, 0, 15, 4, 8, 6, 2, 10 },
					{ 6, 15, 14, 9, 11, 3, 0, 8, 12, 2, 13, 7, 1, 4, 10, 5 },
					{ 10, 2, 8, 4, 7, 6, 1, 5, 15, 11, 9, 14, 3, 12, 13, 0 },
					{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 },
					{ 14, 10, 4, 8, 9, 15, 13, 6, 1, 12, 0, 2, 11, 7, 5, 3 }
				};

				u64 message[16]{};

				for (u32 i = 0; i < 16; i++)
				{
					message[i] = load_u64(block + (i * 8));
				}

				u64 work[16]{};

				for (u32 i = 0; i < 8; i++)
				{
					work[i] = state[i];
					work[i + 8] = BLAKE2B_IV[i];
				}

				//128-bit byte counter
				work[12] ^= counterLow;
				work[13] ^= counterHigh;

				if (isFinal) work[14] = ~work[14];

				for (u32 round = 0; round < 12; round++)
				{
					const u8* sigma = BLAKE2B_SIGMA[round];

					//columns

					blake2b_mix(
						work[0], 
						work[4], 
						work[8], 
						work[12], 
						message[sigma[0]], 
						message[sigma[1]]);

					blake2b_mix(
						work[1], 
						work[5], 
						work[9], 
						work[13], 
						message[sigma[2]], 
						message[sigma[3]]);

					blake2b_mix(
						work[2], 
						work[6], 
						work[10], 
						work[14], 
						message[sigma[4]], 
						message[sigma[5]]);

					blake2b_mix(
						work[3], 
						work[7], 
						work[11], 
						work[15], 
						message[sigma[6]], 
						message[sigma[7]]);

					//diagonals

					blake2b_mix(
						work[0], 
						work[5], 
						work[10], 
						work[15], 
						message[sigma[8]], 
						message[sigma[9]]);

					blake2b_mix(
						work[1], 
						work[6], 
						work[11], 
						work[12], 
						message[sigma[10]], 
						message[sigma[11]]);

					blake2b_mix(
						work[2], 
						work[7], 
						work[8], 
						work[13], 
						message[sigma[12]], 
						message[sigma[13]]);

					blake2b_mix(
						work[3], 
						work[4], 
						work[9], 
						work[14], 
						message[sigma[14]], 
						message[sigma[15]]);
				}

				for (u32 i = 0; i < 8; i++)
				{
					state[i] ^= work[i] ^ work[i + 8];
				}
			};

		auto blake2b = [
			&store_u64,
			&blake2b_compress](
			const u8* input,
			size_t inputSize,
			u8* output,
			size_t outputSize) -> void
			{
				u64 state[8]{};

				for (u32 i = 0; i < 8; i++)
				{
					state[i] = BLAKE2B_IV[i];
				}

				//fanout = 1,
				//depth = 1,
				//key length = 0,
				//digest length = outputSize
				state[0] ^= 0x01010000ULL ^ scast<u64>(outputSize);

				u64 counterLow{};
				u64 counterHigh{};

				auto add_to_counter = [
					&counterLow,
					&counterHigh](u64 amount) -> void
					{
						u64 previous = counterLow;
						counterLow += amount;

						if (counterLow < previous) counterHigh++;
					};

				const u8* current = input;
				size_t remaining = inputSize;

				//leave the final block for the final compression call
				while (remaining > 128)
				{
					add_to_counter(128);

					blake2b_compress(
						state,
						current,
						counterLow,
						counterHigh,
						false);

					current += 128;
					remaining -= 128;
				}

				u8 finalBlock[128]{};

				for (size_t i = 0; i < remaining; i++)
				{
					finalBlock[i] = current[i];
				}

				add_to_counter(scast<u64>(remaining));

				blake2b_compress(
					state,
					finalBlock,
					counterLow,
					counterHigh,
					true);

				u8 fullDigest[64]{};

				for (u32 i = 0; i < 8; i++)
				{
					store_u64(state[i], fullDigest + (i * 8));
				}

				for (size_t i = 0; i < outputSize; i++)
				{
					output[i] = fullDigest[i];
				}
			};

		auto argon2_hash = [
			&blake2b,
			&store_u32](
			const u8* input,
			size_t inputSize,
			u32 outputSize) -> vector<u8>
			{
				vector<u8> initialInput(4 + inputSize);

				store_u32(outputSize, initialInput.data());

				for (size_t i = 0; i < inputSize; i++)
				{
					initialInput[4 + i] = input[i];
				}

				vector<u8> output(outputSize);

				//for outputs up to 64 bytes, one blake2b invocation is enough
				if (outputSize <= 64)
				{
					blake2b(
						initialInput.data(),
						initialInput.size(),
						output.data(),
						outputSize);

					return output;
				}

				u8 currentHash[64]{};

				blake2b(
					initialInput.data(),
					initialInput.size(),
					currentHash,
					64);

				size_t outputOffset{};

				//first intermediate hash contributes its first 32 bytes
				for (u32 i = 0; i < 32; i++)
				{
					output[i] = currentHash[i];
				}

				outputOffset += 32;

				//each intermediate hash contributes its first 32 bytes
				while (outputSize - outputOffset > 64)
				{
					u8 nextHash[64]{};

					blake2b(
						currentHash,
						64,
						nextHash,
						64);

					for (u32 i = 0; i < 64; i++)
					{
						currentHash[i] = nextHash[i];
					}

					for (u32 i = 0; i < 32; i++)
					{
						output[outputOffset + i] = currentHash[i];
					}

					outputOffset += 32;
				}

				//the final blake2b invocation produces exactly the remaining bytes
				blake2b(
					currentHash,
					64,
					output.data() + outputOffset,
					outputSize - outputOffset);

				return output;
			};

		auto generate_initial_hash = [
			&store_u32,
			&blake2b,
			&rawPassword,
			&salt,
			&config]() -> array<u8, 64>
			{
				//Argon2 version 1.3
				static constexpr u8 ARGON2_VERSION = 0x13;

				vector<u8> input{};

				auto append_u32 = [
					&input,
					&store_u32](u32 value) -> void
					{
						size_t offset = input.size();
						input.resize(offset + 4);

						store_u32(value, input.data() + offset);
					};

				auto append_bytes = [&input](
					const u8* data,
					size_t size) -> void
					{
						input.insert(
							input.end(),
							data,
							data + size);
					};

				append_u32(config.parallelism);
				append_u32(HASH_SIZE_BYTES);
				append_u32(config.memoryCost);
				append_u32(config.timeCost);
				append_u32(ARGON2_VERSION);
				append_u32(ARGON2_TYPE_ID);

				append_u32(scast<u32>(rawPassword.size()));

				append_bytes(
					rcast<const u8*>(rawPassword.data()),
					rawPassword.size());

				append_u32(scast<u32>(salt.size()));

				append_bytes(
					rcast<const u8*>(salt.data()),
					salt.size());

				//secret length
				append_u32(0);
				//associated data length
				append_u32(0);

				array<u8, 64> initialHash{};

				blake2b(
					input.data(),
					input.size(),
					initialHash.data(),
					initialHash.size());

				return initialHash;
			};

		array<u8, 64> initialHash = generate_initial_hash();

		auto calculate_memory_geometry = [&config]() -> array<u32, 3>
			{
				u32 memoryBlocks = config.memoryCost;

				//each lane consists of 4 equal segments
				u32 segmentLength = memoryBlocks / (config.parallelism * 4);

				u32 laneLength = segmentLength * 4;

				//actual memory block count after alignment
				memoryBlocks = laneLength * config.parallelism;

				return
				{
					memoryBlocks,
					laneLength,
					segmentLength	
				};
			};

		array<u32, 3> memoryGeometry = calculate_memory_geometry();

		u32 memoryBlocks = memoryGeometry[0];
		u32 laneLength = memoryGeometry[1];
		u32 segmentLength = memoryGeometry[2];

		vector<array<u64, 128>> memory(memoryBlocks);

		auto initialize_memory = [
			&memory,
			&initialHash,
			&store_u32,
			&load_u64,
			&argon2_hash,
			&config,
			laneLength]() -> void
			{
				auto initialize_block = [
					&memory,
					&initialHash,
					&store_u32,
					&load_u64,
					&argon2_hash,
					laneLength](
					u32 lane,
					u32 blockIndex) -> void
					{
						array<u8, 72> input{};

						for (u32 i = 0; i < initialHash.size(); i++)
						{
							input[i] = initialHash[i];
						}

						store_u32(blockIndex, input.data() + 64);
						store_u32(lane, input.data() + 68);

						vector<u8> blockBytes = argon2_hash(
							input.data(),
							input.size(),
							1024);

						array<u64, 128>& block = memory[(lane * laneLength) + blockIndex];

						for (u32 i = 0; i < block.size(); i++)
						{
							block[i] = load_u64(blockBytes.data() + (i * 8));
						}
					};

				for (u32 lane = 0; lane < config.parallelism; lane++)
				{
					initialize_block(lane, 0);
					initialize_block(lane, 1);
				}
			};

		initialize_memory();

		auto argon2_mix = [&rotate_right_64](
			u64& a,
			u64& b,
			u64& c,
			u64& d) -> void
			{
				auto blamka = [](
					u64 x,
					u64 y) -> u64
					{
						u64 xLow = scast<u32>(x);
						u64 yLow = scast<u32>(y);

						return x + y + (2 * xLow * yLow);
					};

				a = blamka(a, b);
				d = rotate_right_64(d ^ a, 32);

				c = blamka(c, d);
				b = rotate_right_64(b ^ c, 24);

				a = blamka(a, b);
				d = rotate_right_64(d ^ a, 16);

				c = blamka(c, d);
				b = rotate_right_64(b ^ c, 63);
			};

		auto argon2_permute = [&argon2_mix](array<u64, 16>& values) -> void
			{
				//columns

				argon2_mix(
					values[0], 
					values[4], 
					values[8], 
					values[12]);
				argon2_mix(
					values[1], 
					values[5], 
					values[9], 
					values[13]);
				argon2_mix(
					values[2], 
					values[6], 
					values[10], 
					values[14]);
				argon2_mix(
					values[3], 
					values[7], 
					values[11], 
					values[15]);

				//diagonals

				argon2_mix(
					values[0], 
					values[5], 
					values[10], 
					values[15]);
				argon2_mix(
					values[1], 
					values[6], 
					values[11], 
					values[12]);
				argon2_mix(
					values[2], 
					values[7], 
					values[8], 
					values[13]);
				argon2_mix(
					values[3], 
					values[4], 
					values[9], 
					values[14]);
			};

		auto argon2_compress = [&argon2_permute](
			const array<u64, 128>& x,
			const array<u64, 128>& y) -> array<u64, 128>
			{
				array<u64, 128> result{};

				for (u32 i = 0; i < result.size(); i++)
				{
					result[i] = x[i] ^ y[i];
				}

				array<u64, 128> original = result;

				for (u32 row = 0; row < 8; row++)
				{
					array<u64, 16> values{};

					for (u32 i = 0; i < 16; i++)
					{
						values[i] = result[(row * 16) + i];
					}

					argon2_permute(values);

					for (u32 i = 0; i < 16; i++)
					{
						result[(row * 16) + i] = values[i];
					}
				}

				for (u32 column = 0; column < 8; column++)
				{
					array<u64, 16> values{};

					for (u32 i = 0; i < 8; i++)
					{
						values[i * 2] = result[(i * 16) + (column * 2)];
						values[(i * 2) + 1] = result[(i * 16) + (column * 2) + 1];
					}

					argon2_permute(values);

					for (u32 i = 0; i < 8; i++)
					{
						result[(i * 16) + (column * 2)] = values[(i * 2)];
						result[(i * 16) + (column * 2) + 1] = values[(i * 2) + 1];
					}
				}

				for (u32 i = 0; i < result.size(); i++)
				{
					result[i] ^= original[i];
				}

				return result;
			};

		auto generate_address_block = [
			&argon2_compress,
			&config,
			memoryBlocks](
			u32 pass,
			u32 lane,
			u32 slice,
			u64 counter) -> array<u64, 128>
			{
				array<u64, 128> zeroBlock{};
				array<u64, 128> inputBlock{};

				inputBlock[0] = pass;
				inputBlock[1] = lane;
				inputBlock[2] = slice;
				inputBlock[3] = memoryBlocks;
				inputBlock[4] = config.timeCost;
				inputBlock[5] = ARGON2_TYPE_ID;
				inputBlock[6] = counter;

				array<u64, 128> temporary = argon2_compress(
					zeroBlock,
					inputBlock);

				return argon2_compress(
					zeroBlock,
					temporary);
			};

		auto get_reference_block = [
			&config,
			laneLength,
			segmentLength](
			u32 j1,
			u32 j2,
			u32 pass,
			u32 lane,
			u32 slice,
			u32 index) -> array<u32, 2>
			{
				u32 referenceLane = j2 % config.parallelism;

				//first slice of the first pass can only reference the current lane
				if (pass == 0 && slice == 0) referenceLane = lane;

				bool sameLane = referenceLane == lane;

				u32 referenceAreaSize{};

				if (pass == 0)
				{
					if (slice == 0) referenceAreaSize = index - 1;
					else if (sameLane)
					{
						referenceAreaSize = 
							(slice * segmentLength)
							+ index
							- 1;
					}
					else
					{
						referenceAreaSize = slice * segmentLength;

						if (index == 0) referenceAreaSize--;
					}
				}
				else
				{
					if (sameLane)
					{
						referenceAreaSize = 
							laneLength
							- segmentLength
							+ index
							- 1;
					}
					else
					{
						referenceAreaSize = laneLength - segmentLength;
						if (index == 0) referenceAreaSize--;
					}
				}

				u64 relativePosition = scast<u64>(j1) * j1;

				relativePosition >>= 32;

				relativePosition = 
					scast<u64>(referenceAreaSize)
					* relativePosition;

				relativePosition >>= 32;

				relativePosition = 
					referenceAreaSize
					- 1
					- relativePosition;

				u32 startPosition{};

				if (pass != 0)
				{
					startPosition =
						((slice + 1) * segmentLength)
						% laneLength;
				}

				u32 referenceIndex = 
					(startPosition + scast<u32>(relativePosition))
					% laneLength;

				return 
				{
					referenceLane,
					referenceIndex
				};
			};

		auto fill_memory = [
			&memory,
			&config,
			&generate_address_block,
			&get_reference_block,
			&argon2_compress,
			laneLength,
			segmentLength]() -> void
			{
				for (u32 pass = 0; pass < config.timeCost; pass++)
				{
					for (u32 slice = 0; slice < 4; slice++)
					{
						for (u32 lane = 0; lane < config.parallelism; lane++)
						{
							bool dataIndependent = 
								pass == 0
								&& slice < 2;

							u32 startIndex{};

							if (pass == 0
								&& slice == 0)
							{
								startIndex = 2;
							}

							array<u64, 128> addressBlock{};
							u64 addressCounter{};

							if (dataIndependent
								&& pass == 0
								&& slice == 0)
							{
								addressCounter++;

								addressBlock = generate_address_block(
									pass,
									lane,
									slice,
									addressCounter);
							}

							for (u32 index = startIndex; index < segmentLength; index++)
							{
								u32 currentIndex = 
									(slice * segmentLength)
									+ index;

								u32 previousIndex = currentIndex == 0
									? laneLength - 1
									: currentIndex - 1;

								array<u64, 128>& previousBlock = memory[
									(lane * laneLength)
									+ previousIndex];

								u64 pseudoRandom{};

								if (dataIndependent)
								{
									if (index % 128 == 0)
									{
										addressCounter++;

										addressBlock = generate_address_block(
											pass,
											lane,
											slice,
											addressCounter);
									}

									pseudoRandom = addressBlock[index % 128];
								}
								else pseudoRandom = previousBlock[0];

								u32 j1 = scast<u32>(pseudoRandom & 0xFFFFFFFFULL);
								u32 j2 = scast<u32>(pseudoRandom >> 32);

								array<u32, 2> reference = get_reference_block(
									j1,
									j2,
									pass,
									lane,
									slice,
									index);

								array<u64, 128>& referenceBlock = memory[
									(reference[0] * laneLength)
									+ reference[1]];

								array<u64, 128> newBlock = argon2_compress(
									previousBlock,
									referenceBlock);

								array<u64, 128>& currentBlock = memory[
									(lane * laneLength)
									+ currentIndex];

								if (pass == 0) currentBlock = newBlock;
								else
								{
									for (u32 i = 0; i < currentBlock.size(); i++)
									{
										currentBlock[i] ^= newBlock[i];
									}
								}
							}
						}
					}
				}
			};

		fill_memory();

		auto finalize_hash = [
			&store_u64,
			&memory,
			&argon2_hash,
			&outHash,
			&config,
			laneLength]() -> void
			{
				array<u64, 128> finalBlock = memory[laneLength - 1];

				for (u32 lane = 1; lane < config.parallelism; lane++)
				{
					array<u64, 128>& lastBlock = memory[
						(lane * laneLength)
						+ laneLength
						- 1];

					for (u32 i = 0; i < finalBlock.size(); i++)
					{
						finalBlock[i] ^= lastBlock[i];
					}
				}

				array<u8, 1024> finalBytes{};

				for (u32 i = 0; i < finalBlock.size(); i++)
				{
					store_u64(
						finalBlock[i],
						finalBytes.data() + (i * 8));
				}

				vector<u8> hash = argon2_hash(
					finalBytes.data(),
					finalBytes.size(),
					HASH_SIZE_BYTES);

				std::move(
					hash.begin(),
					hash.end(),
					outHash.begin());
			};

		finalize_hash();

		return "";
	}

	KNODISCARD
	inline string _VerifyRawPassword(string_view rawPassword)
	{
		if (rawPassword.empty())
		{
			return "Raw password was empty!";
		}

		if (rawPassword.size() < MIN_PASSWORD_LENGTH_BYTES
			|| rawPassword.size() > MAX_PASSWORD_LENGTH_BYTES)
		{
			return "Raw password size was out of range!";
		}

		return "";
	}

	KNODISCARD
	inline string _VerifyArgon2idConfig(const Argon2idConfig& config)
	{
		static constexpr u32 MIN_MEMORY_COST_KIBIBYTES = 8 * 1024;    //8 MiB
		static constexpr u32 MAX_MEMORY_COST_KIBIBYTES = 1024 * 1024; //1 GiB

		static constexpr u8 MIN_TIME_COST = 1;
		static constexpr u8 MAX_TIME_COST = 10;

		static constexpr u8 MIN_PARALLELISM = 1;
		static constexpr u8 MAX_PARALLELISM = 16;

		if (config.memoryCost < MIN_MEMORY_COST_KIBIBYTES
			|| config.memoryCost > MAX_MEMORY_COST_KIBIBYTES)
		{
			return "Argon2id config memory cost was out of range!";
		}

		if (config.timeCost < MIN_TIME_COST
			|| config.timeCost > MAX_TIME_COST)
		{
			return "Argon2id config time cost was out of range!";
		}

		if (config.parallelism < MIN_PARALLELISM
			|| config.parallelism > MAX_PARALLELISM)
		{
			return "Argon2id config parallelism was out of range!";
		}

		return "";
	}
}
