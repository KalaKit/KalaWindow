//------------------------------------------------------------------------------
// import_kfd.hpp
//
// Copyright (C) 2026 Lost Empire Entertainment
//
// This is free source code, and you are welcome to redistribute it under certain conditions.
// Read LICENSE.md for more information.
//
// Provides:
//   - Helpers for streaming individual font glyphs or loading the full kalafontdata binary into memory
//------------------------------------------------------------------------------

/*------------------------------------------------------------------------------

# KFD binary top header

Offset | Size | Field
-------|------|--------------------------------------------
0      | 4    | KFD magic word, always 'K', 'F', 'D', '\0'
4      | 1    | kfd binary version
5      | 1    | type, '1' for bitmap, '2' for glyph
6      | 2    | height of glyphs passed during export
8      | 4    | max number of allowed glyphs
12     | 1    | first indice, always '0'
13     | 1    | second indice, always '1'
14     | 1    | third indice, always '2'
15     | 1    | fourth indice, always '2'
16     | 1    | fifth indice, always '3'
17     | 1    | sixth indice, always '0'
18     | 2    | top-left uv position (x, y)
20     | 2    | top-right uv position (x, y)
22     | 2    | bottom-right uv position (x, y)
24     | 2    | bottom-left uv position (x, y)
26     | 4    | glyph table size in bytes
30     | 4    | glyph block size in bytes

# KFD binary glyph table

Offset | Size | Field
-------|------|--------------------------------------------
??     | 4    | character code in unicode
??+4   | 4    | absolute offset from start of file relative to its glyph block start
??+8   | 4    | size of the glyph block (info + payload)

# KFD binary glyph block

Note: bearings and vertices can be negative

Offset | Size | Field
-------|------|--------------------------------------------
??     | 4    | character code in unicode
??+4   | 2    | width
??+6   | 2    | height
??+8   | 2    | left bearing (X)
??+10  | 2    | top bearing (Y)
??+12  | 2    | advance

??+14  | 4    | top-left vertice position (x, y)
??+18  | 4    | top-right vertice position (x, y)
??+22  | 4    | bottom-right vertice position (x, y)
??+26  | 4    | bottom-left vertice position (x, y)

??+30  | 4    | raw pixels size
??+34  | 1    | each raw pixel value
...

------------------------------------------------------------------------------*/

#pragma once

#include <vector>
#include <array>
#include <string>
#include <fstream>
#include <filesystem>

//reinterpret_cast
#ifndef rcast
	#define rcast reinterpret_cast
#endif

//static_cast
#ifndef scast
	#define scast static_cast
#endif

namespace KalaHeaders::KalaFontData
{	
	using std::vector;
	using std::array;
	using std::string;
	using std::ifstream;
	using std::filesystem::path;
	using std::filesystem::current_path;
	using std::filesystem::weakly_canonical;
	using std::filesystem::exists;
	using std::filesystem::is_regular_file;
	using std::filesystem::perms;
	using std::filesystem::status;
	using std::streamoff;
	using std::streamsize;
	using std::ios;
	using std::move;
	
	using u8 = uint8_t;
	using u16 = uint16_t;
	using u32 = uint32_t;
	using i8 = int8_t;
	using i16 = int16_t;
	
	//The magic that must exist in all kfd files at the first four bytes
	constexpr u32 KFD_MAGIC = 0x0044464B;
	
	//The version that must exist in all kfd files as the fifth byte
	constexpr u8 KFD_VERSION = 1;
	
	//The true top header size that is always required
	constexpr u8 CORRECT_GLYPH_HEADER_SIZE = 34u;
	
	//The true per-glyph table size that is always required
	constexpr u8 CORRECT_GLYPH_TABLE_SIZE = 12u;
	
	//The offset where pixel data must always start relative to each glyph block
	constexpr u8 RAW_PIXEL_DATA_OFFSET = 34u;
	
	//Max allowed glyphs
	constexpr u16 MAX_GLYPH_COUNT = 1024u;
	
	//Max allowed total glyph table size in bytes (12 KB)
	constexpr u32 MAX_GLYPH_TABLE_SIZE = 12288u;
	
	//Max allowed total glyph blocks size in bytes (1024 KB)
	constexpr u32 MAX_GLYPH_BLOCK_SIZE = 1048576u;
	
	constexpr u32 MIN_TOTAL_SIZE = 
		CORRECT_GLYPH_HEADER_SIZE
		+ CORRECT_GLYPH_TABLE_SIZE
		+ RAW_PIXEL_DATA_OFFSET;
	
	//Max allowed size for kfd files
	constexpr u32 MAX_TOTAL_SIZE = 
		CORRECT_GLYPH_HEADER_SIZE 
		+ MAX_GLYPH_TABLE_SIZE 
		+ MAX_GLYPH_BLOCK_SIZE;
	
	//Min allowed glyph height
	constexpr u8 MIN_GLYPH_HEIGHT = 10;
	//Max allowed glyph height
	constexpr u8 MAX_GLYPH_HEIGHT = 100;
	
	//The main header at the top of each kfd file
	struct GlyphHeader
	{
		u32 magic = KFD_MAGIC;    //kfd magic word
		u8 version = KFD_VERSION; //kfd binary version
		u8 type{};                //1 = bitmap, 2 = glyph
		u16 glyphHeight{};        //height of all glyphs in pixels
		u32 glyphCount{};         //number of glyphs
		array<u8, 6> indices = { 0, 1, 2, 2, 3, 0 };
		//uv of this glyph
		array<array<u8, 2>, 4> uvs = 
		{{
		    {0, 0},  //top-left
			{1, 0},  //top-right
			{1, 1},  //bottom-right
			{0, 1}   //bottom-left
		}};       
		u32 glyphTableSize{};     //glyph search table size in bytes
		u32 glyphBlockSize{};     //glyph payload block size in bytes
	};

	//The table that helps look up glyphs individually
	struct GlyphTable
	{
		u32 charCode{};    //glyph character code in unicode
		u32 blockOffset{}; //absolute offset from start of file
		u32 blockSize{};   //size of the glyph block (info + payload)
	};
		
	//The block containing data of each glyph
	struct GlyphBlock
	{
		u32 charCode{};                     //glyph character code in unicode
		u16 width{};                        //glyph width
		u16 height{};                       //glyph height
		i16 bearingX{};                     //glyph left bearing
		i16 bearingY{};                     //glyph top bearing
		u16 advance{};                      //glyph advance
		array<array<i16, 2>, 4> vertices{}; //vertices of this glyph, can be negative
		u32 rawPixelSize{};                 //size of this glyph's pixels
		vector<u8> rawPixels{};             //8-bit raw pixels of this glyph (0 - 255, 0 is transparent, 255 is white)
	};
	
	enum class ImportResult : u8
	{
		RESULT_SUCCESS                     = 0, //No errors, succeeded with import
		
		//
		// FILE OPERATIONS
		//
		
		RESULT_FILE_NOT_FOUND              = 1,  //File does not exist
		RESULT_INVALID_EXTENSION           = 2,  //File is not '.kfd'
		RESULT_UNAUTHORIZED_READ           = 3,  //Not authorized to read this file
		RESULT_FILE_LOCKED                 = 4,  //Cannot read this file, file is in use
		RESULT_UNKNOWN_READ_ERROR          = 5,  //Unknown file error when reading file
		RESULT_FILE_EMPTY                  = 6,  //There is no content inside this file
		
		//
		// IMPORT ERRORS
		//
		
		RESULT_UNSUPPORTED_FILE_SIZE       = 7,  //Always assume total size is atleast 52 bytes
		
		RESULT_INVALID_MAGIC               = 8,  //magic must be 'KFD\0'
		RESULT_INVALID_VERSION             = 9,  //version must match
		RESULT_INVALID_TYPE                = 10, //type must be '1' or '2'
		RESULT_INVALID_GLYPH_HEIGHT        = 11, //glyph height must be within range
		RESULT_INVALID_GLYPH_TABLE_SIZE    = 12, //found a glyph table that wasnt the correct size
		RESULT_INVALID_GLYPH_BLOCK_SIZE    = 13, //found a glyph block that was less or more than the allowed size
		RESULT_INVALID_GLYPH_COUNT         = 14, //total glyph count was above allowed max glyph count
		RESULT_UNEXPECTED_EOF              = 15  //file reached end sooner than expected
	};
	
	inline string ResultToString(ImportResult result)
	{
		switch (result)
		{
		default: return "RESULT_UNKNOWN";
			
		case ImportResult::RESULT_SUCCESS:
			return "RESULT_SUCCESS";
		
		case ImportResult::RESULT_FILE_NOT_FOUND:
			return "RESULT_FILE_NOT_FOUND";
		case ImportResult::RESULT_INVALID_EXTENSION:
			return "RESULT_INVALID_EXTENSION";
		case ImportResult::RESULT_UNAUTHORIZED_READ:
			return "RESULT_UNAUTHORIZED_READ";
		case ImportResult::RESULT_FILE_LOCKED:
			return "RESULT_FILE_LOCKED";
		case ImportResult::RESULT_UNKNOWN_READ_ERROR:
			return "RESULT_UNKNOWN_READ_ERROR";
		case ImportResult::RESULT_FILE_EMPTY:
			return "RESULT_FILE_EMPTY";
			
		case ImportResult::RESULT_UNSUPPORTED_FILE_SIZE:
			return "RESULT_UNSUPPORTED_FILE_SIZE";
			
		case ImportResult::RESULT_INVALID_MAGIC:
			return "RESULT_INVALID_MAGIC";
		case ImportResult::RESULT_INVALID_VERSION:
			return "RESULT_INVALID_VERSION";
		case ImportResult::RESULT_INVALID_TYPE:
			return "RESULT_INVALID_TYPE";
		case ImportResult::RESULT_INVALID_GLYPH_TABLE_SIZE:
			return "RESULT_INVALID_GLYPH_TABLE_SIZE";
		case ImportResult::RESULT_INVALID_GLYPH_BLOCK_SIZE:
			return "RESULT_INVALID_GLYPH_BLOCK_SIZE";
		case ImportResult::RESULT_INVALID_GLYPH_COUNT:
			return "RESULT_INVALID_GLYPH_COUNT";
		case ImportResult::RESULT_UNEXPECTED_EOF:
			return "RESULT_UNEXPECTED_EOF";
		}
		
		return "RESULT_UNKNOWN";
	}
	
	inline ImportResult PreReadCheck(const path& inFile)
	{
		if (!exists(inFile)) return ImportResult::RESULT_FILE_NOT_FOUND;
		if (!is_regular_file(inFile)
			|| !inFile.has_extension()
			|| inFile.extension() != ".kfd")
		{
			return ImportResult::RESULT_INVALID_EXTENSION;
		}
		
		auto fileStatus = status(inFile);
		auto filePerms = fileStatus.permissions();
		
		bool canRead = (filePerms & (
			perms::owner_read
			| perms::group_read
			| perms::others_read))  
			!= perms::none;
		
		if (!canRead) return ImportResult::RESULT_UNAUTHORIZED_READ;
		
		return ImportResult::RESULT_SUCCESS;
	}
	inline ImportResult TryOpenCheck(const path& inFile)
	{
		try
		{
			errno = 0;
			ifstream in(inFile, ios::in | ios::binary);
			if (in.fail()
				&& errno != 0)
			{
				if (errno == EBUSY
					|| errno == ETXTBSY)
				{
					return ImportResult::RESULT_FILE_LOCKED;
				}
				else return ImportResult::RESULT_UNKNOWN_READ_ERROR;
			}
			
			in.seekg(0, ios::end);
			size_t fileSize = scast<size_t>(in.tellg());
			
			if (fileSize == 0) return ImportResult::RESULT_FILE_EMPTY;
			if (fileSize < MIN_TOTAL_SIZE)
			{
				return ImportResult::RESULT_UNSUPPORTED_FILE_SIZE;
			}
			if (fileSize > MAX_TOTAL_SIZE)
			{
				return ImportResult::RESULT_UNSUPPORTED_FILE_SIZE;
			}
			
			in.close();
			
			return ImportResult::RESULT_SUCCESS;
		}
		catch (...)
		{
			return ImportResult::RESULT_UNKNOWN_READ_ERROR;
		}
	}
	
	//Returns header data of the file,
	//set skipChecks to true if the file has already been checked
	inline ImportResult GetHeaderData(
		const path& inFile,
		GlyphHeader& outHeader,
		bool skipChecks = false)
	{
		if (!skipChecks)
		{
			ImportResult preReadResult = PreReadCheck(inFile);
			if (preReadResult != ImportResult::RESULT_SUCCESS) return preReadResult;
		
			ImportResult tryOpenResult = TryOpenCheck(inFile);
			if (tryOpenResult != ImportResult::RESULT_SUCCESS) return tryOpenResult;
		}
		
		ifstream in(inFile, ios::in | ios::binary);
		
		try
		{
			vector<u8> headerData(CORRECT_GLYPH_HEADER_SIZE);
			
			in.read(
				rcast<char*>(headerData.data()),
				scast<streamsize>(CORRECT_GLYPH_HEADER_SIZE));
				
			in.close();	
			
			GlyphHeader header{};
			
			//glyph header
				
			memcpy(&header.magic, headerData.data() + 0, sizeof(u32));
			if (header.magic != KFD_MAGIC) return ImportResult::RESULT_INVALID_MAGIC;
				
			memcpy(&header.version, headerData.data() + 4, sizeof(u8));
			if (header.version != KFD_VERSION) return ImportResult::RESULT_INVALID_VERSION;
				
			memcpy(&header.type, headerData.data() + 5,  sizeof(u8));
			if (header.type != 1
				&& header.type != 2)
			{
				return ImportResult::RESULT_INVALID_TYPE;
			}
				
			memcpy(&header.glyphHeight, headerData.data() + 6,  sizeof(u16));
			if (header.glyphHeight < MIN_GLYPH_HEIGHT
				|| header.glyphHeight > MAX_GLYPH_HEIGHT)
			{
				return ImportResult::RESULT_INVALID_GLYPH_HEIGHT;
			}
				
			memcpy(&header.glyphCount, headerData.data() + 8,  sizeof(u32));
			if (header.glyphCount < 1
				|| header.glyphCount > MAX_GLYPH_COUNT)
			{
				return ImportResult::RESULT_INVALID_GLYPH_COUNT;
			}
				
			memcpy(&header.indices[0], headerData.data() + 12, sizeof(u8) * 6);
			memcpy(&header.uvs[0][0],  headerData.data() + 18, sizeof(u8) * 8);

			memcpy(&header.glyphTableSize, headerData.data() + 26, sizeof(u32));
			if (header.glyphTableSize < CORRECT_GLYPH_TABLE_SIZE
				|| header.glyphTableSize > MAX_GLYPH_TABLE_SIZE)
			{
				return ImportResult::RESULT_INVALID_GLYPH_TABLE_SIZE;
			}
				
			memcpy(&header.glyphBlockSize, headerData.data() + 30, sizeof(u32));
			if (header.glyphBlockSize < RAW_PIXEL_DATA_OFFSET
				|| header.glyphBlockSize > MAX_GLYPH_BLOCK_SIZE)
			{
				return ImportResult::RESULT_INVALID_GLYPH_BLOCK_SIZE;
			}
			
			outHeader = header;
			
			return ImportResult::RESULT_SUCCESS;
		}
		catch (...)
		{
			return ImportResult::RESULT_UNKNOWN_READ_ERROR;
		}
	}
	
	//Loads the kfd tables for streaming font glyphs at runtime,
	//set skipChecks to true if the file has already been checked
	inline ImportResult GetTableData(
		const path& inFile,
		vector<GlyphTable>& outTables,
		bool skipChecks = false)
	{
		if (!skipChecks)
		{
			ImportResult preReadResult = PreReadCheck(inFile);
			if (preReadResult != ImportResult::RESULT_SUCCESS) return preReadResult;
			
			ImportResult tryOpenResult = TryOpenCheck(inFile);
			if (tryOpenResult != ImportResult::RESULT_SUCCESS) return tryOpenResult;
		}
		
		GlyphHeader header{};
			
		ImportResult headerResult = GetHeaderData(
			inFile,
			header,
			true);
				
		if (headerResult != ImportResult::RESULT_SUCCESS) return headerResult;
		
		ifstream in(inFile, ios::in | ios::binary);
		
		try
		{
			vector<u8> tablesData(header.glyphTableSize);
			
			//start at the end of the top header
			in.seekg(CORRECT_GLYPH_HEADER_SIZE);
			
			//read in the size of all tables
			in.read(
				rcast<char*>(tablesData.data()),
				scast<streamsize>(header.glyphTableSize));
				
			in.close();	
			
			vector<GlyphTable> tables{};
			tables.reserve(header.glyphCount);
			
			for (size_t i = 0; 
				i < header.glyphTableSize; 
				i += CORRECT_GLYPH_TABLE_SIZE)
			{
				GlyphTable t{};
				
				memcpy(&t.charCode,    tablesData.data() + i + 0, sizeof(u32));
				memcpy(&t.blockOffset, tablesData.data() + i + 4, sizeof(u32));
				memcpy(&t.blockSize,   tablesData.data() + i + 8, sizeof(u32));
				
				tables.push_back(t);
			}
			
			outTables = move(tables);
			
			return ImportResult::RESULT_SUCCESS;
		}
		catch (...)
		{
			return ImportResult::RESULT_UNKNOWN_READ_ERROR;
		}
	}
	
	//Returns glyph blocks for the inserted tables, set skipChecks to true if the file has already been checked
	inline ImportResult StreamGlyphs(
		const path& inFile,
		const vector<GlyphTable>& inTables,
		vector<GlyphBlock>& outBlocks,
		bool skipChecks = false)
	{
		if (!skipChecks)
		{
			ImportResult preReadResult = PreReadCheck(inFile);
			if (preReadResult != ImportResult::RESULT_SUCCESS) return preReadResult;
			
			ImportResult tryOpenResult = TryOpenCheck(inFile);
			if (tryOpenResult != ImportResult::RESULT_SUCCESS) return tryOpenResult;
		}
		
		try
		{
			ifstream in(inFile, ios::in | ios::binary);
			
			in.seekg(0, ios::end);
			size_t fileSize = scast<size_t>(in.tellg());
			
			//glyph block data
			
			vector<GlyphBlock> blocks{};
			blocks.reserve(inTables.size());
			
			for (const auto& t : inTables)
			{
				GlyphBlock b{};
				
				size_t offset = t.blockOffset;
				
				//verify that block size is not OOB
				if (offset + t.blockSize > fileSize)
				{
					return ImportResult::RESULT_UNEXPECTED_EOF;
				}
				
				in.seekg(offset);
				
				in.read(rcast<char*>(&b.charCode), sizeof(u32));
				in.read(rcast<char*>(&b.width),    sizeof(u16));
				in.read(rcast<char*>(&b.height),   sizeof(u16));
				in.read(rcast<char*>(&b.bearingX), sizeof(i16));
				in.read(rcast<char*>(&b.bearingY), sizeof(i16));
				in.read(rcast<char*>(&b.advance),  sizeof(u16));
				
				//vertices
				in.read(rcast<char*>(&b.vertices), sizeof(b.vertices));
				
				//raw pixel size
				in.read(rcast<char*>(&b.rawPixelSize), sizeof(u32));
				
				//verify that pixel data is not OOB
				if (offset + RAW_PIXEL_DATA_OFFSET + b.rawPixelSize > fileSize)
				{
					return ImportResult::RESULT_UNEXPECTED_EOF;
				}
				
				//raw pixel data
				b.rawPixels.resize(b.rawPixelSize);
				in.read(rcast<char*>(b.rawPixels.data()), b.rawPixelSize);
				
				blocks.push_back(move(b));
			}
			
			in.close();
			
			outBlocks = move(blocks);
			
			return ImportResult::RESULT_SUCCESS;
		}
		catch (...)
		{
			return ImportResult::RESULT_UNKNOWN_READ_ERROR;
		}
	}
	
	//Returns the entire kfd file binary content in structs
	inline ImportResult ImportKFD(
		const path& inFile,
		GlyphHeader& outHeader,
		vector<GlyphTable>& outTables,
		vector<GlyphBlock>& outBlocks)
	{
		ImportResult preReadResult = PreReadCheck(inFile);
		if (preReadResult != ImportResult::RESULT_SUCCESS) return preReadResult;
		
		ImportResult tryOpenResult = TryOpenCheck(inFile);
		if (tryOpenResult != ImportResult::RESULT_SUCCESS) return tryOpenResult;
		
		//header data
			
		GlyphHeader header{};
			
		ImportResult headerResult = GetHeaderData(
			inFile,
			header,
			false);
			
		if (headerResult != ImportResult::RESULT_SUCCESS) return headerResult;
			
		//glyph table data
			
		vector<GlyphTable> tables{};
		tables.reserve(header.glyphCount);
			
		ImportResult tableResult = GetTableData(
			inFile,
			tables,
			true);
			
		if (tableResult != ImportResult::RESULT_SUCCESS) return tableResult;
				
		try
		{
			ifstream in(inFile, ios::in | ios::binary);
			
			vector<u8> blockData(header.glyphBlockSize);
			
			//start at the end of the tables
			
			size_t blockRegionStart = CORRECT_GLYPH_HEADER_SIZE + header.glyphTableSize;
			in.seekg(blockRegionStart);
			
			//read in the size of all models
			in.read(
				rcast<char*>(blockData.data()),
				scast<streamsize>(header.glyphBlockSize));
				
			in.close();
			
			//glyph block data
			
			vector<GlyphBlock> blocks{};
			blocks.reserve(header.glyphCount);
			
			for (const auto& t : tables)
			{
				GlyphBlock b{};
				size_t relativeOffset = t.blockOffset - blockRegionStart;
				
				//verify that block size is not OOB
				if (relativeOffset + t.blockSize > blockData.size())
				{
					return ImportResult::RESULT_UNEXPECTED_EOF;
				}
				
				memcpy(&b.charCode, blockData.data() + relativeOffset + 0, sizeof(u32));
				memcpy(&b.width,    blockData.data() + relativeOffset + 4, sizeof(u16));
				memcpy(&b.height,   blockData.data() + relativeOffset + 6, sizeof(u16));
				memcpy(&b.bearingX, blockData.data() + relativeOffset + 8, sizeof(i16));
				memcpy(&b.bearingY, blockData.data() + relativeOffset + 10, sizeof(i16));
				memcpy(&b.advance,  blockData.data() + relativeOffset + 12, sizeof(u16));
				
				//vertices
				memcpy(&b.vertices, blockData.data() + relativeOffset + 14, sizeof(b.vertices));
				
				//raw pixel size
				memcpy(&b.rawPixelSize, blockData.data() + relativeOffset + 30, sizeof(u32));
				
				//verify that pixel data is not OOB
				if (relativeOffset + scast<u32>(RAW_PIXEL_DATA_OFFSET) + b.rawPixelSize > blockData.size())
				{
					return ImportResult::RESULT_UNEXPECTED_EOF;
				}
				
				//raw pixel data
				b.rawPixels.resize(b.rawPixelSize);
				memcpy(b.rawPixels.data(), blockData.data() + relativeOffset + RAW_PIXEL_DATA_OFFSET, b.rawPixelSize);
				
				blocks.push_back(move(b));
			}
			
			outHeader = header;
			outTables = move(tables);
			outBlocks = move(blocks);
			
			return ImportResult::RESULT_SUCCESS;
		}
		catch (...)
		{
			return ImportResult::RESULT_UNKNOWN_READ_ERROR;
		}
	}
}