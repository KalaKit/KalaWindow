//------------------------------------------------------------------------------
// import_kmd.hpp
//
// Copyright (C) 2025 Lost Empire Entertainment
//
// This is free source code, and you are welcome to redistribute it under certain conditions.
// Read LICENSE.md for more information.
//
// Provides:
//   - Helpers for streaming individual models or loading the full kalamodeldata binary into memory
//------------------------------------------------------------------------------

/*------------------------------------------------------------------------------

# KMD binary top header

Offset | Size | Field
-------|------|--------------------------------------------
0      | 4    | KMD magic word, always 'K', 'M', 'D', '\0'
4      | 1    | kmd binary version
5      | 1    | global model-space scale factor
6      | 4    | total model count (table and block per model)
10     | 4    | combined size of all model tables
14     | 4    | combined size of all model data blocks

Downscale multiplier values:
	0 - 1.0f
	1 - 10.0f
	2 - 100.0f
	3 - 1000.0f
	4 - 10000.0f
	5 - 0.1f
	6 - 0.01f
	7 - 0.001f
	8 - 0.0001f
	9-255 - unused, defaults to 0

# KMD binary model table

Offset | Size | Field
-------|------|--------------------------------------------
??     | 20   | fixed-length name of the model (19 characters + null terminator)
??+20  | 4    | model block offset from start
??+24  | 4    | size of the model payload (block data size)

# KMD binary model block

Offset | Size | Field
-------|------|--------------------------------------------
??     | 20   | fixed-length name of the model node (19 characters + null terminator)
??+20  | 20   | fixed-length name of the model mesh (19 characters + null terminator)
??+40  | 50   | fixed-length path of the model (49 characters + null terminator)
??+90  | 1    | data types (bit flags)
??+91  | 1    | render type (0-2)
??+92  | 12   | model position in floats in XYZ axis
??+104 | 16   | model rotation in floats in quaternion
??+120 | 12   | model size in floats in XYZ axis
??+132 | 4    | vertices offset
??+136 | 4    | vertices size
??+140 | 4    | indices offset
??+144 | 4    | indices size
??+148 | ???  | vertices data
??+??  | ???  | indices data

Data types:
	0 - has material data,
	1 - has texture data,
	2 - has camera data,
	3 - has light data,
	4 - has animation data (animations, bones, curves)
	5-7 - unused
	
Render type:
	0 - opaque
	1 - transparent (assigned if material is enabled, material has transparent texture or color)
	2 - masked (assigned if material is enabled, material has transparent texture or color but alpha/transparency is 100% or 0%)
	3-255 - unused, defaults to 0

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

namespace KalaHeaders::KalaModelData
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
	using f32 = float;
	
	//The magic that must exist in all kmd files at the first four bytes
	constexpr u32 KMD_MAGIC = 0x00444D4B;
	
	//The version that must exist in all kmd files as the fifth byte
	constexpr u8 KMD_VERSION = 1;
	
	//The true top header size that is always required
	constexpr u8 CORRECT_MODEL_HEADER_SIZE = 18u;
	
	//The true per-model table size that is always required
	constexpr u8 CORRECT_MODEL_TABLE_SIZE = 28u;
	
	//The offset where vertice data must always start relative to each model block
	constexpr u8 VERTICE_DATA_OFFSET = 148u;
	
	//Max allowed models
	constexpr u16 MAX_MODEL_COUNT = 1024u;
	
	//Max allowed total model table size in bytes (28 KB)
	constexpr u32 MAX_MODEL_TABLE_SIZE = 28672u;
	
	//Max allowed total model blocks size in bytes (1 GB)
	constexpr u32 MAX_MODEL_BLOCK_SIZE = 1073741824u;
	
	//Not allowed to be less than this position in X, Y or Z axis
	constexpr f32 MIN_POS = -10000.0f;
	//Not allowed to be more than this position in X, Y or Z axis
	constexpr f32 MAX_POS = 10000.0f;
	
	//Not allowed to be less than this rotation in W, X, Y or Z
	constexpr f32 MIN_ROT = -1.0f;
	//Not allowed to be more than this rotation in W, X, Y or Z
	constexpr f32 MAX_ROT = 1.0f;
	
	//Not allowed to be less than this size in X, Y or Z axis
	constexpr f32 MIN_SIZE = 1e-6f;
	//Not allowed to be more than than this size in X, Y or Z axis
	constexpr f32 MAX_SIZE = 10000.0f;
	
	constexpr u32 MIN_TOTAL_SIZE = 
		CORRECT_MODEL_HEADER_SIZE
		+ CORRECT_MODEL_TABLE_SIZE
		+ VERTICE_DATA_OFFSET;
	
	//Max allowed size for kmd files
	constexpr u32 MAX_TOTAL_SIZE = 
		CORRECT_MODEL_HEADER_SIZE 
		+ MAX_MODEL_TABLE_SIZE 
		+ MAX_MODEL_BLOCK_SIZE;
	
	//The main header at the top of each kmd file
	struct ModelHeader
	{
		u32 magic = KMD_MAGIC;    //kmd magic word
		u8 version = KMD_VERSION; //kmd binary version
		u8 scaleFactor{};         //global model-space scale factor
		u32 modelCount{};         //count of models
		u32 modelTablesSize{};    //total size of model tables
		u32 modelBlocksSize{};    //total size of model data blocks
	};

	//The table that helps look up models individually
	struct ModelTable
	{
		char nodeName[20]{}; //19 chars + null terminator
		u32 blockOffset{};   //absolute offset from start of file
		u32 blockSize{};     //size of the model data block
	};
	
	//Render-ready data relative to each vertex
	struct Vertex
	{
		f32 position[3]{}; //x, y, z
		f32 normal[3]{};   //nx, ny, nz
		f32 texCoord[2]{}; //u, v
		f32 tangent[4]{};  //tx, ty, tz, tw
	};
		
	//The block containing data of each model
	struct ModelBlock
	{
		char nodeName[20]{}; //19 chars + null terminator
		char meshName[20]{}; //19 chars + null terminator
		char nodePath[50]{}; //49 chars + null terminator
		u8 dataTypeFlags{};  //defines what kind of data was stored to this model data block
		u8 renderType{};     //defines if this model is opaque, transparent or masked
		
		f32 position[3]{}; //x, y, z (vector3)
		f32 rotation[4]{}; //w, x, y, z (quaternion)
		f32 size[3]{};     //x, y, z (vector3)
		
		u32 verticesOffset{};
		u32 verticesSize{};
		u32 indicesOffset{};
		u32 indicesSize{};
		
		vector<Vertex> vertices{};
		vector<u32> indices{};
	};
	
	enum class ImportResult : u8
	{
		RESULT_SUCCESS                     = 0, //No errors, succeeded with import
		
		//
		// FILE OPERATIONS
		//
		
		RESULT_FILE_NOT_FOUND              = 1,  //File does not exist
		RESULT_INVALID_EXTENSION           = 2,  //File is not '.kmd'
		RESULT_UNAUTHORIZED_READ           = 3,  //Not authorized to read this file
		RESULT_FILE_LOCKED                 = 4,  //Cannot read this file, file is in use
		RESULT_UNKNOWN_READ_ERROR          = 5,  //Unknown file error when reading file
		RESULT_FILE_EMPTY                  = 6,  //There is no content inside this file
		
		//
		// IMPORT ERRORS
		//
		
		RESULT_UNSUPPORTED_FILE_SIZE       = 7,  //Always assume total size is atleast 52 bytes
		
		RESULT_INVALID_MAGIC               = 8,  //magic must be 'KMD\0'
		RESULT_INVALID_VERSION             = 9,  //version must match
		RESULT_INVALID_MODEL_COUNT         = 10, //model count must be within range
		RESULT_INVALID_DATA_FLAGS          = 11, //data flags must be within range
		RESULT_INVALID_RENDER_TYPE         = 12, //render type must be within range
		RESULT_INVALID_MODEL_POSITION      = 13, //model position must be within range
		RESULT_INVALID_MODEL_ROTATION      = 14, //model rotation must be within range
		RESULT_INVALID_MODEL_SIZE          = 15, //model size must be within range
		RESULT_INVALID_MODEL_TABLE_SIZE    = 16, //found a model table that wasnt the correct size
		RESULT_INVALID_MODEL_BLOCK_SIZE    = 17, //found a model block that was less or more than the allowed size
		RESULT_UNEXPECTED_EOF              = 18  //file reached end sooner than expected
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
		case ImportResult::RESULT_INVALID_MODEL_COUNT:
			return "RESULT_INVALID_MODEL_COUNT";
		case ImportResult::RESULT_INVALID_DATA_FLAGS:
			return "RESULT_INVALID_DATA_FLAGS";
		case ImportResult::RESULT_INVALID_RENDER_TYPE:
			return "RESULT_INVALID_RENDER_TYPE";
		case ImportResult::RESULT_INVALID_MODEL_POSITION:
			return "RESULT_INVALID_MODEL_POSITION";
		case ImportResult::RESULT_INVALID_MODEL_ROTATION:
			return "RESULT_INVALID_MODEL_ROTATION";
		case ImportResult::RESULT_INVALID_MODEL_SIZE:
			return "RESULT_INVALID_MODEL_SIZE";
		case ImportResult::RESULT_INVALID_MODEL_TABLE_SIZE:
			return "RESULT_INVALID_MODEL_TABLE_SIZE";
		case ImportResult::RESULT_INVALID_MODEL_BLOCK_SIZE:
			return "RESULT_INVALID_MODEL_BLOCK_SIZE";
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
			|| inFile.extension() != ".kmd")
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
		ModelHeader& outHeader,
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
			vector<u8> headerData(CORRECT_MODEL_HEADER_SIZE);
			
			in.read(
				rcast<char*>(headerData.data()),
				scast<streamsize>(CORRECT_MODEL_HEADER_SIZE));
				
			in.close();	
			
			ModelHeader header{};
			
			//model header
			
			memcpy(&header.magic, headerData.data() + 0, sizeof(u32));
			if (header.magic != KMD_MAGIC) return ImportResult::RESULT_INVALID_MAGIC;
			
			memcpy(&header.version, headerData.data() + 4, sizeof(u8));
			if (header.version != KMD_VERSION) return ImportResult::RESULT_INVALID_VERSION;
			
			memcpy(&header.scaleFactor, headerData.data() + 5,  sizeof(u8));
			//clamp to 0 for out of range values
			if (header.scaleFactor > 8) header.scaleFactor = 0;
			
			memcpy(&header.modelCount, headerData.data() + 6,  sizeof(u32));
			if (header.modelCount > MAX_MODEL_COUNT) return ImportResult::RESULT_INVALID_MODEL_COUNT;

			memcpy(&header.modelTablesSize, headerData.data() + 10, sizeof(u32));
			if (header.modelTablesSize < CORRECT_MODEL_TABLE_SIZE
				|| header.modelTablesSize > MAX_MODEL_TABLE_SIZE)
			{
				return ImportResult::RESULT_INVALID_MODEL_TABLE_SIZE;
			}
			
			memcpy(&header.modelBlocksSize, headerData.data() + 14, sizeof(u32));
			if (header.modelBlocksSize < VERTICE_DATA_OFFSET
				|| header.modelBlocksSize > MAX_MODEL_BLOCK_SIZE)
			{
				return ImportResult::RESULT_INVALID_MODEL_BLOCK_SIZE;
			}
			
			outHeader = header;
			
			return ImportResult::RESULT_SUCCESS;
		}
		catch (...)
		{
			return ImportResult::RESULT_UNKNOWN_READ_ERROR;
		}
	}
	
	//Loads the kmd tables for streaming models at runtime,
	//set skipChecks to true if the file has already been checked
	inline ImportResult GetTableData(
		const path& inFile,
		vector<ModelTable>& outTables,
		bool skipChecks = false)
	{
		if (!skipChecks)
		{
			ImportResult preReadResult = PreReadCheck(inFile);
			if (preReadResult != ImportResult::RESULT_SUCCESS) return preReadResult;
			
			ImportResult tryOpenResult = TryOpenCheck(inFile);
			if (tryOpenResult != ImportResult::RESULT_SUCCESS) return tryOpenResult;
		}
		
		ModelHeader header{};
			
		ImportResult headerResult = GetHeaderData(
			inFile,
			header,
			true);
				
		if (headerResult != ImportResult::RESULT_SUCCESS) return headerResult;
		
		ifstream in(inFile, ios::in | ios::binary);
		
		try
		{
			vector<u8> tablesData(header.modelTablesSize);
			
			//start at the end of the top header
			in.seekg(CORRECT_MODEL_HEADER_SIZE);
			
			//read in the size of all tables
			in.read(
				rcast<char*>(tablesData.data()),
				scast<streamsize>(header.modelTablesSize));
				
			in.close();	
		
			vector<ModelTable> tables{};
			tables.reserve(header.modelCount);
			
			for (size_t i = 0; 
				i < header.modelTablesSize; 
				i += CORRECT_MODEL_TABLE_SIZE)
			{
				ModelTable t{};
				
				memcpy(t.nodeName,     tablesData.data() + i + 0, sizeof(t.nodeName));
				memcpy(&t.blockOffset, tablesData.data() + i + 20, sizeof(u32));
				memcpy(&t.blockSize,   tablesData.data() + i + 24, sizeof(u32));
				
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
	
	//Returns model blocks for the inserted tables, set skipChecks to true if the file has already been checked
	inline ImportResult StreamModels(
		const path& inFile,
		const vector<ModelTable>& inTables,
		vector<ModelBlock>& outBlocks,
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
			
			vector<ModelBlock> blocks{};
			blocks.reserve(inTables.size());
			
			for (const auto& t : inTables)
			{
				ModelBlock b{};
				
				size_t offset = t.blockOffset;
				
				//verify that block size is not OOB
				if (offset + t.blockSize > fileSize)
				{
					return ImportResult::RESULT_UNEXPECTED_EOF;
				}
				
				in.seekg(offset);
				
				in.read(rcast<char*>(b.nodeName),       20);
				in.read(rcast<char*>(b.meshName),       20);
				in.read(rcast<char*>(b.nodePath),       50);
				
				//data flags go from 0 to 4
				in.read(rcast<char*>(&b.dataTypeFlags), sizeof(u8));
				if (b.dataTypeFlags & ~0b00011111) return ImportResult::RESULT_INVALID_DATA_FLAGS;
				
				//render type goes from 0 to 2
				in.read(rcast<char*>(&b.renderType),    sizeof(u8));
				if (b.renderType > 2) return ImportResult::RESULT_INVALID_RENDER_TYPE;
				
				f32 newPos[3]{};
				in.read(rcast<char*>(&newPos[0]), sizeof(f32));
				in.read(rcast<char*>(&newPos[1]), sizeof(f32));
				in.read(rcast<char*>(&newPos[2]), sizeof(f32));
				
				if (newPos[0] < MIN_POS
					|| newPos[0] > MAX_POS
					|| newPos[1] < MIN_POS
					|| newPos[1] > MAX_POS
					|| newPos[2] < MIN_POS
					|| newPos[2] > MAX_POS)
				{
					return ImportResult::RESULT_INVALID_MODEL_POSITION;
				}
				
				memcpy(b.position, newPos, sizeof(b.position));
				
				f32 newRot[4]{};
				in.read(rcast<char*>(&newRot[0]), sizeof(f32));
				in.read(rcast<char*>(&newRot[1]), sizeof(f32));
				in.read(rcast<char*>(&newRot[2]), sizeof(f32));
				in.read(rcast<char*>(&newRot[3]), sizeof(f32));
				
				if (newRot[0] < MIN_ROT
					|| newRot[0] > MAX_ROT
					|| newRot[1] < MIN_ROT
					|| newRot[1] > MAX_ROT
					|| newRot[2] < MIN_ROT
					|| newRot[2] > MAX_ROT
					|| newRot[3] < MIN_ROT
					|| newRot[3] > MAX_ROT)
				{
					return ImportResult::RESULT_INVALID_MODEL_ROTATION;
				}
				
				memcpy(b.rotation, newRot, sizeof(b.rotation));
				
				f32 newSize[3]{};
				in.read(rcast<char*>(&newSize[0]), sizeof(f32));
				in.read(rcast<char*>(&newSize[1]), sizeof(f32));
				in.read(rcast<char*>(&newSize[2]), sizeof(f32));
				
				if (newSize[0] < MIN_SIZE
					|| newSize[0] > MAX_SIZE
					|| newSize[1] < MIN_SIZE
					|| newSize[1] > MAX_SIZE
					|| newSize[2] < MIN_SIZE
					|| newSize[2] > MAX_SIZE)
				{
					return ImportResult::RESULT_INVALID_MODEL_SIZE;
				}
				
				memcpy(b.size, newSize, sizeof(b.size));
				
				in.read(rcast<char*>(&b.verticesOffset), sizeof(u32));
				in.read(rcast<char*>(&b.verticesSize),   sizeof(u32));
				in.read(rcast<char*>(&b.indicesOffset),  sizeof(u32));
				in.read(rcast<char*>(&b.indicesSize),    sizeof(u32));
				
				//verify that vertices are not OOB
				if (offset + VERTICE_DATA_OFFSET + b.verticesSize > fileSize)
				{
					return ImportResult::RESULT_UNEXPECTED_EOF;
				}
				
				//vertices
				
				size_t vertexCount = b.verticesSize / sizeof(Vertex);
				
				b.vertices.resize(vertexCount);
				in.read(rcast<char*>(b.vertices.data()), b.verticesSize);
				
				//verify that indices are not OOB
				if (offset + VERTICE_DATA_OFFSET + b.verticesSize + b.indicesSize > fileSize)
				{
					return ImportResult::RESULT_UNEXPECTED_EOF;
				}
				
				//indices
				
				size_t indexCount = b.indicesSize / sizeof(u32);
				
				b.indices.resize(indexCount);
				in.read(rcast<char*>(b.indices.data()), b.indicesSize);
				
				blocks.push_back(move(b));
			}
			
			in.close();
			
			outBlocks = move(blocks);
			
			return ImportResult::RESULT_SUCCESS;
		}
		catch(...)
		{
			return ImportResult::RESULT_UNEXPECTED_EOF;
		}
	}
	
	//Returns the entire kmd file binary content in structs
	inline ImportResult ImportKMD(
		const path& inFile,
		ModelHeader& outHeader,
		vector<ModelTable>& outTables,
		vector<ModelBlock>& outBlocks)
	{
		ImportResult preReadResult = PreReadCheck(inFile);
		if (preReadResult != ImportResult::RESULT_SUCCESS) return preReadResult;
		
		ImportResult tryOpenResult = TryOpenCheck(inFile);
		if (tryOpenResult != ImportResult::RESULT_SUCCESS) return tryOpenResult;
		
		//header data
			
		ModelHeader header{};
			
		ImportResult headerResult = GetHeaderData(
			inFile,
			header,
			false);
			
		if (headerResult != ImportResult::RESULT_SUCCESS) return headerResult;
			
		//model table data
			
		vector<ModelTable> tables{};
		tables.reserve(header.modelCount);
			
		ImportResult tableResult = GetTableData(
			inFile,
			tables,
			true);
			
		if (tableResult != ImportResult::RESULT_SUCCESS) return tableResult;
		
		try
		{
			ifstream in(inFile, ios::in | ios::binary);
			
			vector<u8> blockData(header.modelBlocksSize);
			
			//start at the end of the tables
			
			size_t blockRegionStart = CORRECT_MODEL_HEADER_SIZE + header.modelTablesSize;
			in.seekg(blockRegionStart);
			
			//read in the size of all models
			in.read(
				rcast<char*>(blockData.data()),
				scast<streamsize>(header.modelBlocksSize));
				
			in.close();
			
			//model block data
			
			vector<ModelBlock> blocks{};
			blocks.reserve(header.modelCount);
			
			for (const auto& t : tables)
			{
				ModelBlock b{};
				size_t relativeOffset = t.blockOffset - blockRegionStart;
				
				//verify that block size is not OOB
				if (relativeOffset + t.blockSize > blockData.size())
				{
					return ImportResult::RESULT_UNEXPECTED_EOF;
				}
				
				memcpy(b.nodeName, blockData.data() + relativeOffset + 0, 20);
				memcpy(b.meshName, blockData.data() + relativeOffset + 20, 20);
				memcpy(b.nodePath, blockData.data() + relativeOffset + 40, 50);
				
				//data flags go from 0 to 4
				memcpy(&b.dataTypeFlags, blockData.data() + relativeOffset + 90, sizeof(u8));
				if (b.dataTypeFlags & ~0b00011111) return ImportResult::RESULT_INVALID_DATA_FLAGS;
				
				//render type goes from 0 to 2
				memcpy(&b.renderType, blockData.data() + relativeOffset + 91, sizeof(u8));
				if (b.renderType > 2) return ImportResult::RESULT_INVALID_RENDER_TYPE;
				
				f32 newPos[3]{};
				memcpy(&newPos[0], blockData.data() + relativeOffset + 92, sizeof(f32));
				memcpy(&newPos[1], blockData.data() + relativeOffset + 96, sizeof(f32));
				memcpy(&newPos[2], blockData.data() + relativeOffset + 100, sizeof(f32));
				
				if (newPos[0] < MIN_POS
					|| newPos[0] > MAX_POS
					|| newPos[1] < MIN_POS
					|| newPos[1] > MAX_POS
					|| newPos[2] < MIN_POS
					|| newPos[2] > MAX_POS)
				{
					return ImportResult::RESULT_INVALID_MODEL_POSITION;
				}
				
				memcpy(b.position, newPos, sizeof(b.position));
				
				f32 newRot[4]{};
				memcpy(&newRot[0], blockData.data() + relativeOffset + 104, sizeof(f32));
				memcpy(&newRot[1], blockData.data() + relativeOffset + 108, sizeof(f32));
				memcpy(&newRot[2], blockData.data() + relativeOffset + 112, sizeof(f32));
				memcpy(&newRot[3], blockData.data() + relativeOffset + 116, sizeof(f32));
				
				if (newRot[0] < MIN_ROT
					|| newRot[0] > MAX_ROT
					|| newRot[1] < MIN_ROT
					|| newRot[1] > MAX_ROT
					|| newRot[2] < MIN_ROT
					|| newRot[2] > MAX_ROT
					|| newRot[3] < MIN_ROT
					|| newRot[3] > MAX_ROT)
				{
					return ImportResult::RESULT_INVALID_MODEL_ROTATION;
				}
				
				memcpy(b.rotation, newRot, sizeof(b.rotation));
				
				f32 newSize[3]{};
				memcpy(&newSize[0], blockData.data() + relativeOffset + 120, sizeof(f32));
				memcpy(&newSize[1], blockData.data() + relativeOffset + 124, sizeof(f32));
				memcpy(&newSize[2], blockData.data() + relativeOffset + 128, sizeof(f32));
				
				if (newSize[0] < MIN_SIZE
					|| newSize[0] > MAX_SIZE
					|| newSize[1] < MIN_SIZE
					|| newSize[1] > MAX_SIZE
					|| newSize[2] < MIN_SIZE
					|| newSize[2] > MAX_SIZE)
				{
					return ImportResult::RESULT_INVALID_MODEL_SIZE;
				}
				
				memcpy(b.size, newSize, sizeof(b.size));
				
				memcpy(&b.verticesOffset, blockData.data() + relativeOffset + 132, sizeof(u32));
				memcpy(&b.verticesSize,   blockData.data() + relativeOffset + 136, sizeof(u32));
				memcpy(&b.indicesOffset,  blockData.data() + relativeOffset + 140, sizeof(u32));
				memcpy(&b.indicesSize,    blockData.data() + relativeOffset + 144, sizeof(u32));
				
				//verify that vertices are not OOB
				if (relativeOffset + scast<u32>(VERTICE_DATA_OFFSET) + b.verticesSize > blockData.size())
				{
					return ImportResult::RESULT_UNEXPECTED_EOF;
				}
				
				//vertices
				
				size_t vertexCount = b.verticesSize / sizeof(Vertex);
				
				b.vertices.resize(vertexCount);
				memcpy(b.vertices.data(), blockData.data() + relativeOffset + VERTICE_DATA_OFFSET, b.verticesSize);
				
				//verify that indices are not OOB
				if (relativeOffset + scast<u32>(VERTICE_DATA_OFFSET) + b.verticesSize + b.indicesSize > blockData.size())
				{
					return ImportResult::RESULT_UNEXPECTED_EOF;
				}
				
				//indices
				
				size_t indexCount = b.indicesSize / sizeof(u32);
				
				b.indices.resize(indexCount);
				memcpy(b.indices.data(), blockData.data() + relativeOffset + VERTICE_DATA_OFFSET + b.verticesSize, b.indicesSize);
				
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