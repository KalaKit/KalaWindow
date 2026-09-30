//---------------------------------------------------------------------------
// export_png.hpp
//
// Copyright (C) 2026 Lost Empire Entertainment
//
// This is free source code, and you are welcome to redistribute it under certain conditions.
// Read LICENSE.md for more information.
//
// Provides:
//   - exports .png files with full parity with the KalaGraphics material system while remaining as fully standalone header
//   - uses dynamic Huffman deflate compression (if compression is enabled)
//   - uses adaptive png scanline filtering
//   - converts raw pixel data directly into .png binary data
//   - exports already-converted .png binary data directly to disk
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
#include <filesystem>
#include <vector>
#include <fstream>
#include <algorithm>

namespace KalaHeaders::KalaExportPNG
{
	using std::string;
	using std::to_string;
	using std::filesystem::path;
	using std::filesystem::is_directory;
	using std::filesystem::exists;
	using std::filesystem::absolute;
	using std::filesystem::perms;
	using std::filesystem::status;
	using std::vector;
	using std::ofstream;
	using std::ios;
	using std::min;
	using std::sort;

	//Controls how hard the LZ77 matcher searches for a better previous match,
	//higher values improve compression at the cost of more CPU time,
	//should not be set below 1 or above 32768
	static constexpr size_t LZ77_CHAIN_DEPTH = 128;

    enum class TexturePixelFormat : u8
    {
		//1 channel, 8-bit UNORM, color type 0
        FORMAT_BASIC_R8       = 0,
		//2 channels, 8-bit UNORM, color type 4 
        FORMAT_BASIC_R8G8     = 1,
		//4 channels, 8-bit UNORM, color type 6
        FORMAT_BASIC_R8G8B8A8 = 2,

		//4 channels, 8-bit sRGB-encoded, color type 6
        FORMAT_SRGB_R8G8B8A8  = 3
    };

	struct ExportPNGTextureData
	{
		vector<u8> pixelData{};
		f32 size[2]{};
		TexturePixelFormat format{};

		bool useCompression = true;
	};

	//Returns .png binary data from raw pixel data
	KNODISCARD
	inline string GetPNGData(
		ExportPNGTextureData&& textureData,
		vector<u8>& outPngData)
	{
		//biggest allowed png texture width or height
		static constexpr u32 MAX_WIDTH_HEIGHT = 8192;

		static constexpr u8 PNG_SIGNATURE[8] =
		{
			0x89,
			0x50, 0x4E, 0x47, //PNG
			0x0D, 0x0A,
			0x1A,
			0x0A
		};

		static constexpr u8 IHDR_CHUNK_TYPE[4] =
		{
			0x49, 0x48, 0x44, 0x52
		};

		static constexpr u32 IHDR_DATA_LENGTH = 13;

		static constexpr u8 IDAT_CHUNK_TYPE[4] =
		{
			0x49, 0x44, 0x41, 0x54
		};

		static constexpr u8 IEND_CHUNK[12] = 
		{
			//length
			0x00, 0x00, 0x00, 0x00,
			//IEND
			0x49, 0x45, 0x4E, 0x44,
			//CRC
			0xAE, 0x42, 0x60, 0x82
		};

		static constexpr u16 LENGTH_BASE[29] =
		{
			3, 4, 5, 6, 7, 8, 9, 10,
			11, 13, 15, 17,
			19, 23, 27, 31,
			35, 43, 51, 59,
			67, 83, 99, 115,
			131, 163, 195, 227,
			258
		};

		static constexpr u8 LENGTH_EXTRA_BITS[29] =
		{
			0, 0, 0, 0, 0, 0, 0, 0,
			1, 1, 1, 1,
			2, 2, 2, 2,
			3, 3, 3, 3,
			4, 4, 4, 4,
			5, 5, 5, 5,
			0
		};

		static constexpr u16 DISTANCE_BASE[30] =
		{
			1, 2, 3, 4,
			5, 7,
			9, 13,
			17, 25,
			33, 49,
			65, 97,
			129, 193,
			257, 385,
			513, 769,
			1025, 1537,
			2049, 3073,
			4097, 6145,
			8193, 12289,
			16385, 24577
		};

		static constexpr u8 DISTANCE_EXTRA_BITS[30] =
		{
			0, 0, 0, 0,
			1, 1,
			2, 2,
			3, 3,
			4, 4,
			5, 5,
			6, 6,
			7, 7,
			8, 8,
			9, 9,
			10, 10,
			11, 11,
			12, 12,
			13, 13
		};

		struct IHDR_Chunk
		{
			u32 width{};
			u32 height{};

			u8 bitDepth = 8;
			u8 colorType{};

			u8 compressionMethod = 0;
			u8 filterMethod = 0;
			u8 interlaceMethod = 0;
		};

		struct ZLIB_Header
		{
			//compression method and compression info (CMF)
			u8 compressionInfo = 0x78;
			//FLG
			u8 flags = 0x01;
		};

		struct Deflate_Block
		{
			//BFINAL + BTYPE
			//0x01 = final block, uncompressed
			u8 blockHeader = 0x01;
			//LEN - amount of uncompressed data in this block (little endian)
			u16 dataLength{};
			//NLEN - one's complement of LEN
			u16 inverseDataLength{};

			//filtered PNG scanline data
			vector<u8> imageData{};
		};

		//writes a u32 in big-endian byte order
		auto write_u32_big_endian = [](
			vector<u8>& data,
			const u32 value) -> void
			{
				data.push_back(scast<u8>((value >> 24) & 0xFF));
				data.push_back(scast<u8>((value >> 16) & 0xFF));
				data.push_back(scast<u8>((value >> 8) & 0xFF));
				data.push_back(scast<u8>(value & 0xFF));
			};

		//writes a u16 in little-endian byte order
		auto write_u16_little_endian = [](
			vector<u8>& data,
			const u16 value) -> void
			{
				data.push_back(scast<u8>(value & 0xFF));
				data.push_back(scast<u8>((value >> 8) & 0xFF));
			};

		//calculates png crc-32
		auto calculate_crc_32 = [](
			const u8* data,
			const size_t size) -> u32
			{
				u32 crc = 0xFFFFFFFF;

				for (size_t i = 0; i < size; i++)
				{
					crc ^= data[i];

					for (u8 j = 0; j < 8; j++)
					{
						if (crc & 1) crc = (crc >> 1) ^ 0xEDB88320;
						else crc >>= 1;
					}
				}

				return crc ^ 0xFFFFFFFF;
			};

		//calculates zlib adler-32
		auto calculate_adler_32 = [](
			const u8* data,
			const size_t size) -> u32
			{
				//largest prime number smaller than 65536
				static constexpr u32 ADLER_32_MODULUS = 65521;

				u32 a = 1;
				u32 b = 0;

				for (size_t i = 0; i < size; i++)
				{
					a = (a + data[i]) % ADLER_32_MODULUS;
					b = (b + a) % ADLER_32_MODULUS;
				}

				return (b << 16) | a;
			};

		auto paeth_predictor = [](
			const u8 left,
			const u8 above,
			const u8 upperLeft) -> u8
			{
				const i32 prediction = 
					scast<i32>(left)
					+ scast<i32>(above)
					- scast<i32>(upperLeft);

				const i32 leftDistance = abs(prediction - scast<i32>(left));
				const i32 aboveDistance = abs(prediction - scast<i32>(above));
				const i32 upperLeftDistance = abs(prediction - scast<i32>(upperLeft));

				if (leftDistance <= aboveDistance
					&& leftDistance <= upperLeftDistance)
				{
					return left;
				}

				if (aboveDistance <= upperLeftDistance)
				{
					return above;
				}

				return upperLeft;
			};

		auto score_filter = [](const vector<u8>& filteredData) -> u64
			{
				u64 score{};

				for (const u8 value : filteredData)
				{
					const i16 signedValue = value < 128
						? scast<i16>(value)
						: scast<i16>(value) - 256;

					score += scast<u64>(abs(signedValue));
				}

				return score;
			};

		if (textureData.pixelData.empty())
		{
			return "Failed to generate PNG data because its pixel data is empty!";
		}

		if (textureData.size[0] <= 0
			|| textureData.size[1] <= 0)
		{
			return "Failed to generate PNG data because texture width or height is 0 or below!";
		}

		if (textureData.size[0] > MAX_WIDTH_HEIGHT
			|| textureData.size[1] > MAX_WIDTH_HEIGHT)
		{
			return "Failed to generate PNG data because texture width or height is too big!";
		}

		const u32 width = scast<u32>(textureData.size[0]);
		const u32 height = scast<u32>(textureData.size[1]);

		IHDR_Chunk ihdr
		{
			.width = width,
			.height = height
		};

		u8 channelCount{};

		switch (textureData.format)
		{
		default:
		case TexturePixelFormat::FORMAT_BASIC_R8:
			ihdr.colorType = 0;
			channelCount = 1;
			break;
		case TexturePixelFormat::FORMAT_BASIC_R8G8:
			ihdr.colorType = 4;
			channelCount = 2;
			break;
		case TexturePixelFormat::FORMAT_BASIC_R8G8B8A8:
		case TexturePixelFormat::FORMAT_SRGB_R8G8B8A8:
			ihdr.colorType = 6;
			channelCount = 4;
			break;
		}

		const size_t rowSize = scast<size_t>(width) * channelCount;
		const size_t expectedPixelDataSize = rowSize * height;

		if (textureData.pixelData.size() != expectedPixelDataSize)
		{
			return "Failed to generate PNG data because pixel data size did not match texture size and format!";
		}

		outPngData.clear();

		//PNG signature
		outPngData.insert(
			outPngData.end(),
			PNG_SIGNATURE,
			PNG_SIGNATURE + 8);

		//IHDR data length
		write_u32_big_endian(
			outPngData,
			IHDR_DATA_LENGTH);

		//IHDR chunk type
		const size_t crcStart = outPngData.size();

		outPngData.insert(
			outPngData.end(),
			IHDR_CHUNK_TYPE,
			IHDR_CHUNK_TYPE + 4);

		//IHDR data
		write_u32_big_endian(
			outPngData,
			ihdr.width);
		write_u32_big_endian(
			outPngData,
			ihdr.height);

		outPngData.push_back(ihdr.bitDepth);
		outPngData.push_back(ihdr.colorType);
		outPngData.push_back(ihdr.compressionMethod);
		outPngData.push_back(ihdr.filterMethod);
		outPngData.push_back(ihdr.interlaceMethod);

		//IHDR crc
		const u32 crc = calculate_crc_32(
			outPngData.data() + crcStart,
			4 + IHDR_DATA_LENGTH);

		write_u32_big_endian(
			outPngData,
			crc);

		if (textureData.format == TexturePixelFormat::FORMAT_SRGB_R8G8B8A8)
		{
			static constexpr u8 SRGB_CHUNK_TYPE[4]
			{
				's', 'R', 'G', 'B'
			};

			//sRGB data length
			write_u32_big_endian(
				outPngData,
				1);

			const size_t srgbCrcStart = outPngData.size();

			outPngData.insert(
				outPngData.end(),
				SRGB_CHUNK_TYPE,
				SRGB_CHUNK_TYPE + 4);

			//rendering intent 0 - perceptual
			outPngData.push_back(0);

			//sRGB crc covers chunk type + data
			const u32 srgbCrc = calculate_crc_32(
				outPngData.data() + srgbCrcStart,
				5);

			write_u32_big_endian(
				outPngData,
				srgbCrc);
		}

		//construct filtered scanline data
		vector<u8> scanlineData{};

		const size_t bytesPerPixel = channelCount;

		scanlineData.reserve(height * (rowSize + 1));

		vector<u8> candidate(rowSize);
		vector<u8> bestFilteredRow(rowSize);

		for (size_t y = 0; y < height; y++)
		{
			const size_t rowOffset = y * rowSize;

			u64 bestScore = UINT64_MAX;
			u8 bestFilter{};

			for (u8 filter = 0; filter <= 4; filter++)
			{
				for (size_t x = 0; x < rowSize; x++)
				{
					const u8 current = textureData.pixelData[rowOffset + x];

					const u8 left = x >= bytesPerPixel
						? textureData.pixelData[rowOffset + x - bytesPerPixel]
						: 0;

					const u8 above = y > 0
						? textureData.pixelData[rowOffset - rowSize + x]
						: 0;

					const u8 upperLeft = y > 0 && x >= bytesPerPixel
						? textureData.pixelData[rowOffset - rowSize + x - bytesPerPixel]
						: 0;

					u8 predictor{};

					switch (filter)
					{
					case 0: //none
						predictor = 0;
						break;
					case 1: //sub
						predictor = left;
						break;
					case 2: //up
						predictor = above;
						break;
					case 3: //average
						predictor = scast<u8>(
							(scast<u16>(left) 
							+ scast<u16>(above)) / 2);
						break;
					case 4: //paeth
						predictor = paeth_predictor(
							left,
							above,
							upperLeft);
						break;
					}

					candidate[x] = scast<u8>(current - predictor);
				}

				const u64 score = score_filter(candidate);

				if (score < bestScore)
				{
					bestScore = score;
					bestFilter = filter;

					bestFilteredRow = candidate;
				}
			}

			//each png scanline begins with its filter type
			scanlineData.push_back(bestFilter);

			scanlineData.insert(
				scanlineData.end(),
				bestFilteredRow.begin(),
				bestFilteredRow.end());
		}

		vector<u8> deflateData{};

		//
		// UNCOMPRESSED DEFLATE
		//

		if (!textureData.useCompression)
		{
			//pass data to defalate block
			vector<Deflate_Block> deflateBlocks{};

			size_t offset{};

			while(offset < scanlineData.size())
			{
				const size_t remainingSize = scanlineData.size() - offset;

				const u16 blockSize = scast<u16>(
					min<size_t>(remainingSize, 65535));

				Deflate_Block block{};

				block.dataLength = blockSize;
				block.inverseDataLength = scast<u16>(~block.dataLength);

				block.imageData.insert(
					block.imageData.end(),
					scanlineData.begin() + offset,
					scanlineData.begin() + offset + blockSize);

				offset += blockSize;

				//only the final deflate block has BFINAL set
				block.blockHeader = offset == scanlineData.size()
					? 0x01
					: 0x00;

				deflateBlocks.push_back(std::move(block));
			}

			//serialize stored deflate blocks
			for (const Deflate_Block& block : deflateBlocks)
			{
				deflateData.push_back(block.blockHeader);

				write_u16_little_endian(
					deflateData,
					block.dataLength);

				write_u16_little_endian(
					deflateData,
					block.inverseDataLength);

				deflateData.insert(
					deflateData.end(),
					block.imageData.begin(),
					block.imageData.end());
			}
		}

		//
		// COMPRESSED DEFLATE
		//

		else
		{
			struct LZ77_Token
			{
				//true = literal,
				//false = length/distance pair
				bool isLiteral{};

				u8 literal{};

				u16 length{};
				u16 distance{};
			};

			auto compress_lz77 = [](
				const vector<u8>& data,
				vector<LZ77_Token>& outTokens) -> void
				{
					static constexpr size_t WINDOW_SIZE = 32768;
					static constexpr size_t MIN_MATCH_LENGTH = 3;
					static constexpr size_t MAX_MATCH_LENGTH = 258;

					static constexpr size_t HASH_BITS = 15;
					static constexpr size_t HASH_SIZE = 1 << HASH_BITS;

					struct LZ77_Match
					{
						u16 length{};
						u16 distance{};
					};

					//most recent position for each 3-byte hash
					vector<i32> hashHeads(HASH_SIZE, -1);

					//previous position with the same hash
					vector<i32> hashPrevious(data.size(), -1);

					auto get_hash = [&](const size_t position) -> size_t
						{
							const u32 value = 
								(scast<u32>(data[position]) << 16)
								| (scast<u32>(data[position + 1]) << 8)
								| scast<u32>(data[position + 2]);

							return (value * 2654435761u) >> (32 - HASH_BITS);
						};

					auto insert_position = [&](const size_t position) -> void
						{
							if (position + MIN_MATCH_LENGTH > data.size()) return;

							const size_t hash = get_hash(position);

							hashPrevious[position] = hashHeads[hash];
							hashHeads[hash] = scast<i32>(position);
						};

					auto find_best_match = [&](const size_t position) -> LZ77_Match
						{
							LZ77_Match bestMatch{};

							if (position + MIN_MATCH_LENGTH > data.size()) return bestMatch;

							const size_t hash = get_hash(position);

							i32 candidate = hashHeads[hash];
							size_t searchedCandidates{};

							const size_t maxLength = min<size_t>(
								MAX_MATCH_LENGTH,
								data.size() - position);

							while (candidate >= 0
								&& searchedCandidates < LZ77_CHAIN_DEPTH)
							{
								const size_t candidatePosition = scast<size_t>(candidate);
								const size_t distance = position - candidatePosition;

								if (distance > WINDOW_SIZE) break;

								size_t matchLength{};

								while (matchLength < maxLength
									&& data[candidatePosition + matchLength]
									== data[position + matchLength])
								{
									matchLength++;
								}

								if (matchLength >= MIN_MATCH_LENGTH
									&& matchLength > bestMatch.length)
								{
									bestMatch.length = scast<u16>(matchLength);
									bestMatch.distance = scast<u16>(distance);

									if (matchLength == maxLength) break;
								}

								candidate = hashPrevious[candidatePosition];
								searchedCandidates++;
							}

							return bestMatch;
						};

					size_t position{};

					while (position < data.size())
					{
						const LZ77_Match currentMatch = find_best_match(position);

						if (currentMatch.length >= MIN_MATCH_LENGTH)
						{
							//make the current position available before
							//checking the next position
							insert_position(position);

							LZ77_Match nextMatch{};

							if (position + 1 < data.size())
							{
								nextMatch = find_best_match(position + 1);
							}

							//prefer a literal if delaying produces
							//a strictly longer match
							if (nextMatch.length > currentMatch.length)
							{
								outTokens.push_back(
								{
									.isLiteral = true,
									.literal = data[position]
								});

								position++;
								continue;
							}

							outTokens.push_back(
							{
								.isLiteral = false,
								.length = currentMatch.length,
								.distance = currentMatch.distance
							});

							//insert positions covered by the match so
							//future searches can reference them
							for (size_t i = 1; i < currentMatch.length; i++)
							{
								insert_position(position + i);
							}

							position += currentMatch.length;
						}
						else
						{
							outTokens.push_back(
							{
								.isLiteral = true,
								.literal = data[position]
							});

							insert_position(position);

							position++;
						}
					}
				};

			auto encode_dynamic_huffman = [](
				vector<LZ77_Token>&& tokens,
				vector<u8>& outData) -> void
				{
					//max allowed code length for building huffman code lengths
					static constexpr u8 MAX_CODE_LENGTH = 15;

					static constexpr u8 CODE_LENGTH_ORDER[19] =
					{
						16, 17, 18, 0, 8, 7, 9, 6, 10,
						5, 11, 4, 12, 3, 13, 2, 14, 1, 15
					};

					struct CodeLengthToken
					{
						u8 symbol{};

						u8 extraBits{};
						u8 extraBitCount{};
					};

					u8 currentByte{};
					u8 bitCount{};

					auto write_bits = [&](u32 value, u8 count) -> void
						{
							for (u8 i = 0; i < count; i++)
							{
								//take the next least-significant bit
								const u8 bit = scast<u8>((value >> i) & 1);

								currentByte |= bit << bitCount;
								bitCount++;

								//flush completed byte
								if (bitCount == 8)
								{
									outData.push_back(currentByte);

									currentByte = 0;
									bitCount = 0;
								}
							}
						};

					auto get_length_symbol = [](const u16 length) -> u16
						{
							//258 has its own dedicated symbol
							if (length == 258) return 285;

							for (u16 i = 0; i < 28; i++)
							{
								const u16 base = LENGTH_BASE[i];
								const u8 extraBits = LENGTH_EXTRA_BITS[i];

								const u16 maxLength = base + ((1u << extraBits) - 1);

								if (length >= base
									&& length <= maxLength)
								{
									return 257 + i;
								}
							}

							//valid LZ77 tokens guarantee length is 3-258
							return 285;
						};

					auto get_distance_symbol = [](const u16 distance) -> u8
						{
							for (u8 i = 0; i < 30; i++)
							{
								const u16 base = DISTANCE_BASE[i];
								const u8 extraBits = DISTANCE_EXTRA_BITS[i];

								const u32 maxDistance = 
									scast<u32>(base)
									+ ((1u << extraBits) - 1);

								if (distance >= base
									&& distance <= maxDistance)
								{
									return i;
								}
							}

							//valid LZ77 tokens guarantee distance is 1-32768
							return 29;
						};

					auto build_huffman_code_lengths = [](
						const u32* frequencies,
						const size_t symbolCount,
						const u8 maxCodeLength,
						u8* outCodeLengths) -> void
						{
							struct PackageMergeNode
							{
								u64 weight{};

								i32 symbol = -1;

								i32 left = -1;
								i32 right = -1;
							};

							//clear output
							for (size_t i = 0; i < symbolCount; i++)
							{
								outCodeLengths[i] = 0;
							}

							vector<PackageMergeNode> nodes{};
							vector<i32> leaves{};

							nodes.reserve(symbolCount * (maxCodeLength + 1));

							leaves.reserve(symbolCount);

							//create leaves

							for (size_t i = 0; i < symbolCount; i++)
							{
								if (frequencies[i] == 0) continue;

								const i32 nodeIndex = scast<i32>(nodes.size());

								nodes.push_back(
								{
									.weight = frequencies[i],
									.symbol = scast<i32>(i)
								});

								leaves.push_back(nodeIndex);
							}

							//deflate needs at least one usable code
							if (leaves.empty())
							{
								outCodeLengths[0] = 1;
								return;
							}

							//a single symbol still needs a one-bit code
							if (leaves.size() == 1)
							{
								outCodeLengths[nodes[leaves[0]].symbol] = 1;
								return;
							}

							//sort leaves by increasing frequency
							sort(
								leaves.begin(),
								leaves.end(),
								[&](const i32 a, const i32 b)
								{
									if (nodes[a].weight != nodes[b].weight)
									{
										return nodes[a].weight < nodes[b].weight;
									}

									return nodes[a].symbol < nodes[b].symbol;
								});

							//build package-merge lists

							vector<i32> currentList = leaves;

							for (u8 level = 1; level < maxCodeLength; level++)
							{
								vector<i32> packages{};

								packages.reserve(currentList.size() / 2);

								//pair adjacent lowest-weight items
								for (size_t i = 0; i + 1 < currentList.size(); i += 2)
								{
									const i32 left = currentList[i];
									const i32 right = currentList[i + 1];

									const i32 packageIndex = scast<i32>(nodes.size());

									nodes.push_back(
									{
										.weight = nodes[left].weight + nodes[right].weight,
										.left = left,
										.right = right
									});

									packages.push_back(packageIndex);
								}

								//packages are already ordered because they were
								//formed from adjacent items in an ordered list

								//merge original leaves and new packages
								vector<i32> nextList{};

								nextList.reserve(leaves.size() + packages.size());

								size_t leafIndex{};
								size_t packageIndex{};

								while(leafIndex < leaves.size()
									|| packageIndex < packages.size())
								{
									if (packageIndex >= packages.size())
									{
										nextList.push_back(leaves[leafIndex++]);
										continue;
									}

									if (leafIndex >= leaves.size())
									{
										nextList.push_back(packages[packageIndex++]);
										continue;
									}

									const i32 leaf = leaves[leafIndex];
									const i32 package = packages[packageIndex];

									if (nodes[leaf].weight <= nodes[package].weight)
									{
										nextList.push_back(leaf);
										leafIndex++;
									}
									else
									{
										nextList.push_back(package);
										packageIndex++;
									}
								}

								currentList = std::move(nextList);
							}

							//select package-merge solution

							//for a binary prefix tree with N leaves,
							//package-merge selects 2N - 2 items
							const size_t selectionCount = (leaves.size() * 2) - 2;

							//expand selected packages

							auto add_code_length = [&](
								auto&& self,
								const i32 nodeIndex) -> void
								{
									const PackageMergeNode& node = nodes[nodeIndex];

									if (node.symbol >= 0)
									{
										outCodeLengths[node.symbol]++;
										return;
									}

									self(self, node.left);
									self(self, node.right);
								};

							for (size_t i = 0; i < selectionCount; i++)
							{
								add_code_length(
									add_code_length,
									currentList[i]);
							}
						};

					auto build_huffman_codes = [](
						const u8* codeLengths,
						const size_t symbolCount,
						const u8 maxCodeLength,
						u16* outCodes) -> void
						{
							u16 lengthCounts[MAX_CODE_LENGTH + 1]{};
							u16 nextCodes[MAX_CODE_LENGTH + 1]{};

							//count how many symbols use each code length
							for (size_t i = 0; i < symbolCount; i++)
							{
								if (codeLengths[i] != 0)
								{
									lengthCounts[codeLengths[i]]++;
								}
							}

							//calculate the first canonical code for each length
							u16 code{};

							for (u8 length = 1; length <= maxCodeLength; length++)
							{
								code = scast<u16>((code + lengthCounts[length - 1]) << 1);
								nextCodes[length] = code;
							}

							//assign canonical codes in symbol order
							for (size_t i = 0; i < symbolCount; i++)
							{
								const u8 length = codeLengths[i];
								if (length == 0) continue;
								outCodes[i] = nextCodes[length]++;
							}
						};

					auto encode_code_lengths = [](
						const vector<u8>& codeLengths,
						vector<CodeLengthToken>& outTokens) -> void
						{
							size_t i{};

							while (i < codeLengths.size())
							{
								const u8 length = codeLengths[i];

								//count consecutive occurrences of this code length
								size_t runLength = 1;

								while (i + runLength < codeLengths.size()
									&& codeLengths[i + runLength] == length)
								{
									runLength++;
								}

								size_t remaining = runLength;

								//zero code lengths

								if (length == 0)
								{
									while (remaining >= 11)
									{
										const u8 repeatCount = scast<u8>(min<size_t>(remaining, 138));

										//symbol 18 repeats zero 11-138 times
										outTokens.push_back(
										{
											.symbol = 18,
											.extraBits = scast<u8>(repeatCount - 11),
											.extraBitCount = 7
										});

										remaining -= repeatCount;
									}

									if (remaining >= 3)
									{
										const u8 repeatCount = scast<u8>(min<size_t>(remaining, 10));

										//symbol 17 repeats zero 3-10 times
										outTokens.push_back(
										{
											.symbol = 17,
											.extraBits = scast<u8>(repeatCount - 3),
											.extraBitCount = 3
										});

										remaining -= repeatCount;
									}

									//remaining one or two zeros are written literally
									while (remaining > 0)
									{
										outTokens.push_back(
										{
											.symbol = 0
										});

										remaining--;
									}
								}

								//non-zero code lengths

								else
								{
									//the first occurrence must be written literally
									outTokens.push_back(
									{
										.symbol = length
									});

									remaining--;

									while (remaining >= 3)
									{
										const u8 repeatCount = scast<u8>(min<size_t>(remaining, 6));

										//symbol 16 repeats previous code length 3-6 times
										outTokens.push_back(
										{
											.symbol = 16,
											.extraBits = scast<u8>(repeatCount - 3),
											.extraBitCount = 2
										});

										remaining -= repeatCount;
									}

									//remaining one or two lengths are written literally
									while (remaining > 0)
									{
										outTokens.push_back(
										{
											.symbol = length
										});

										remaining--;
									}
								}

								i += runLength;
							}
						};

					auto reverse_bits = [](
						u16 value,
						const u8 bitCount) -> u16
						{
							u16 reversed{};

							for (u8 i = 0; i < bitCount; i++)
							{
								reversed = scast<u16>((reversed << 1) | (value & 1));

								value >>= 1;
							}

							return reversed;
						};

					//BFINAL = 1
					write_bits(1, 1);

					//BTYPE = 10 - dynamic Huffman
					write_bits(2, 2);

					//dynamic huffman header

					//literal/length symbol frequencies
					u32 literalLengthFrequencies[286]{};
					//distance symbol frequencies
					u32 distanceFrequencies[30]{};

					for (const LZ77_Token& token : tokens)
					{
						if (token.isLiteral)
						{
							//literal values map directly to symbols 0-255
							literalLengthFrequencies[token.literal]++;
						}
						else
						{
							const u16 lengthSymbol = get_length_symbol(token.length);
							const u8 distanceSymbol = get_distance_symbol(token.distance);

							literalLengthFrequencies[lengthSymbol]++;
							distanceFrequencies[distanceSymbol]++;
						}
					}

					//256 is the mandatory end-of-block symbol
					literalLengthFrequencies[256]++;

					u8 literalLengthCodeLengths[286]{};
					u8 distanceCodeLengths[30]{};

					build_huffman_code_lengths(
						literalLengthFrequencies,
						286,
						MAX_CODE_LENGTH,
						literalLengthCodeLengths);

					build_huffman_code_lengths(
						distanceFrequencies,
						30,
						MAX_CODE_LENGTH,
						distanceCodeLengths);

					u16 literalLengthCodes[286]{};
					u16 distanceCodes[30]{};

					build_huffman_codes(
						literalLengthCodeLengths,
						286,
						MAX_CODE_LENGTH,
						literalLengthCodes);

					build_huffman_codes(
						distanceCodeLengths,
						30,
						MAX_CODE_LENGTH,
						distanceCodes);

					//determine amount of literal/length codes
					u16 literalLengthCodeCount = 286;

					while (literalLengthCodeCount > 257
						&& literalLengthCodeLengths[literalLengthCodeCount - 1] == 0)
					{
						literalLengthCodeCount--;
					}

					//determine amount of distance codes
					u8 distanceCodeCount = 30;

					while (distanceCodeCount > 1
						&& distanceCodeLengths[distanceCodeCount - 1] == 0)
					{
						distanceCodeCount--;
					}

					//build code-length alphabet
					vector<u8> combinedCodeLengths{};

					combinedCodeLengths.reserve(literalLengthCodeCount + distanceCodeCount);

					combinedCodeLengths.insert(
						combinedCodeLengths.end(),
						literalLengthCodeLengths,
						literalLengthCodeLengths + literalLengthCodeCount);

					combinedCodeLengths.insert(
						combinedCodeLengths.end(),
						distanceCodeLengths,
						distanceCodeLengths + distanceCodeCount);

					vector<CodeLengthToken> codeLengthTokens{};

					encode_code_lengths(
						combinedCodeLengths,
						codeLengthTokens);

					u32 codeLengthFrequencies[19]{};

					for (const CodeLengthToken& token : codeLengthTokens)
					{
						codeLengthFrequencies[token.symbol]++;
					}

					u8 codeLengthCodeLengths[19]{};

					build_huffman_code_lengths(
						codeLengthFrequencies,
						19,
						7,
						codeLengthCodeLengths);

					u16 codeLengthCodes[19]{};

					build_huffman_codes(
						codeLengthCodeLengths,
						19,
						7,
						codeLengthCodes);

					//determine amount of code-length codes
					u8 codeLengthCodeCount = 19;

					while (codeLengthCodeCount > 4
						&& codeLengthCodeLengths[CODE_LENGTH_ORDER[codeLengthCodeCount - 1]] == 0)
					{
						codeLengthCodeCount--;
					}

					const u8 hlit = scast<u8>(literalLengthCodeCount - 257);
					const u8 hdist = scast<u8>(distanceCodeCount - 1);
					const u8 hclen = codeLengthCodeCount - 4;

					//serialize dynamic huffman header

					//number of literal/length codes minus 257
					write_bits(hlit, 5);

					//number of distance codes minus 1
					write_bits(hdist, 5);

					//number of code-length codes minus 4
					write_bits(hclen, 4);

					//code-length code lengths are transmitted
					//in deflate's special ordering
					for (u8 i = 0; i < codeLengthCodeCount; i++)
					{
						write_bits(
							codeLengthCodeLengths[CODE_LENGTH_ORDER[i]],
							3);
					}

					//serialize literal/length and distance
					//code-length descriptions
					for (const CodeLengthToken& token : codeLengthTokens)
					{
						const u8 codeLength = codeLengthCodeLengths[token.symbol];

						const u16 code = reverse_bits(
							codeLengthCodes[token.symbol],
							codeLength);

						write_bits(code, codeLength);

						if (token.extraBitCount > 0)
						{
							write_bits(token.extraBits, token.extraBitCount);
						}
					}

					//serialize lz77 tokens
					for (const LZ77_Token& token : tokens)
					{
						if (token.isLiteral)
						{
							const u8 codeLength = literalLengthCodeLengths[token.literal];

							const u16 code = reverse_bits(
								literalLengthCodes[token.literal],
								codeLength);

							write_bits(code, codeLength);
						}
						else
						{
							//length

							const u16 lengthSymbol = get_length_symbol(token.length);
							
							const u8 lengthCodeLength = literalLengthCodeLengths[lengthSymbol];

							const u16 lengthCode = reverse_bits(
								literalLengthCodes[lengthSymbol],
								lengthCodeLength);

							write_bits(lengthCode, lengthCodeLength);

							const u8 lengthIndex = scast<u8>(lengthSymbol - 257);
							const u8 lengthExtraBitCount = LENGTH_EXTRA_BITS[lengthIndex];

							if (lengthExtraBitCount > 0)
							{
								const u16 lengthExtraBits = token.length - LENGTH_BASE[lengthIndex];

								write_bits(lengthExtraBits, lengthExtraBitCount);
							}

							//distance

							const u8 distanceSymbol = get_distance_symbol(token.distance);

							const u8 distanceCodeLength = distanceCodeLengths[distanceSymbol];

							const u16 distanceCode = reverse_bits(
								distanceCodes[distanceSymbol],
								distanceCodeLength);

							write_bits(distanceCode, distanceCodeLength);

							const u8 distanceExtraBitCount = DISTANCE_EXTRA_BITS[distanceSymbol];

							if (distanceExtraBitCount > 0)
							{
								const u16 distanceExtraBits = token.distance - DISTANCE_BASE[distanceSymbol];

								write_bits(distanceExtraBits, distanceExtraBitCount);
							}
						}
					}

					//end-of-block symbol
					const u8 endCodeLength = literalLengthCodeLengths[256];

					const u16 endCode = reverse_bits(
						literalLengthCodes[256],
						endCodeLength);

					write_bits(endCode, endCodeLength);

					//flush final partial byte
					if (bitCount > 0) outData.push_back(currentByte);
				};

			vector<LZ77_Token> tokens{};

			compress_lz77(
				scanlineData,
				tokens);

			//dynamic Huffman takes the LZ77 tokens and produces
			//the actual compressed deflate stream
			encode_dynamic_huffman(
				std::move(tokens),
				deflateData);
		}

		//calculate adler-32 over the entire original scanline data
		const u32 adler32 = calculate_adler_32(
			scanlineData.data(),
			scanlineData.size());

		//serialize zlib stream
		vector<u8> idatData{};

		ZLIB_Header zlib
		{ 
			.flags = scast<u8>(textureData.useCompression ? (2 << 6) : 0)
		};

		//FCHECK makes CMF/FLG divisible by 31
		const u16 header =
			(scast<u16>(zlib.compressionInfo) << 8)
			| zlib.flags;

		zlib.flags |= scast<u8>((31 - (header % 31)) % 31);

		idatData.push_back(zlib.compressionInfo);
		idatData.push_back(zlib.flags);

		//serialized deflate stream
		idatData.insert(
			idatData.end(),
			deflateData.begin(),
			deflateData.end());

		//adler32 is big-endian
		write_u32_big_endian(
			idatData,
			adler32);

		//IDAT data length
		write_u32_big_endian(
			outPngData,
			scast<u32>(idatData.size()));

		//IDAT chunk type
		const size_t idatCrcStart = outPngData.size();

		outPngData.insert(
			outPngData.end(),
			IDAT_CHUNK_TYPE,
			IDAT_CHUNK_TYPE + 4);

		//IDAT data
		outPngData.insert(
			outPngData.end(),
			idatData.begin(),
			idatData.end());

		//IDAT crc
		const u32 idatCrc = calculate_crc_32(
			outPngData.data() + idatCrcStart,
			4 + idatData.size());

		write_u32_big_endian(
			outPngData,
			idatCrc);

		//IEND
		outPngData.insert(
			outPngData.end(),
			IEND_CHUNK,
			IEND_CHUNK + 12);

		return "";
	}

	//Takes in already converted .png data and exports a .png to target path
	KNODISCARD
	inline string ExportPNGFromBinaryData(
		vector<u8>&& pngData,
		const path& exportPath,
		bool overwrite = false)
	{
		if (pngData.empty())
		{
			return "Failed to export PNG because png data is empty!";
		}

		if (exportPath.empty())
		{
			return "Failed to export PNG because its export path is empty!";
		}

		if (!exportPath.parent_path().empty()
			&& !exists(exportPath.parent_path()))
		{
			return "Failed to export PNG because export path '" + exportPath.string() + "' is invalid!";
		}

		if (is_directory(exportPath))
		{
			return "Failed to export PNG to export path '" + exportPath.string() + "' because it is a directory!";
		}
		
		if (exportPath.extension() != ".png")
		{
			return "Failed to export PNG to export path '" + exportPath.string() + "' because its extension is not supported!";
		}

		if (!overwrite
			&& exists(exportPath))
		{
			return "Failed to export PNG to export path '" + exportPath.string() + "' because it already exists!";
		}

        auto fileStatus = status(absolute(exportPath).parent_path());
        auto filePerms = fileStatus.permissions();

        bool canWrite = (filePerms & (
            perms::owner_write
            | perms::group_write
            | perms::others_write))
            != perms::none;

        if (!canWrite) return "Failed to export PNG to export path '" + exportPath.string() + "' because of insufficient write permissions!";


		//
		// WRITE TO .PNG FILE
		//

		ofstream file(absolute(exportPath), ios::binary);
		if (!file.is_open()) return "Failed to open export path '" + exportPath.string() + "' for writing!";

		file.write(
			rcast<const char*>(pngData.data()),
			pngData.size());

		if (!file) return "Failed to export PNG file '" + exportPath.string() + "'!";

		file.close();

		return "";
	}

	//Takes in pixel data, exports a .png to target path
	KNODISCARD
	inline string ExportPNG(
		ExportPNGTextureData&& textureData,
		const path& exportPath,
		bool overwrite = false)
	{
		vector<u8> pngData{};
		string err = GetPNGData(
			std::move(textureData),
			pngData);

		if (!err.empty()) return err;

		return ExportPNGFromBinaryData(
			std::move(pngData),
			exportPath,
			overwrite);
	}
}
