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

namespace KalaHeaders
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
	
	//Not allowed to be past this position in X, Y or Z axis
	constexpr f32 MIN_POS = -10000.0f;
	//Not allowed to be past this position in X, Y or Z axis
	constexpr f32 MAX_POS = 10000.0f;
	
	//Not allowed to be below this size in X, Y or Z axis
	constexpr f32 MIN_SIZE = 0.01f;
	//Not allowed to be bigger than this size in X, Y or Z axis
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
		RESULT_INVALID_MODEL_POSITION      = 11, //model position must be within range
		RESULT_INVALID_MODEL_SIZE          = 12, //model size must be within range
		RESULT_INVALID_MODEL_TABLE_SIZE    = 13, //found a model table that wasnt the correct size
		RESULT_INVALID_MODEL_BLOCK_SIZE    = 14, //found a model block that was less or more than the allowed size
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
		case ImportResult::RESULT_INVALID_MODEL_COUNT:
			return "RESULT_INVALID_MODEL_COUNT";
		case ImportResult::RESULT_INVALID_MODEL_POSITION:
			return "RESULT_INVALID_MODEL_POSITION";
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
	
	//Takes in a path to the .kmd file and returns binary data with a result enum
	inline ImportResult ImportKMD(
		const path& inFile,
		ModelHeader& outHeader,
		vector<ModelTable>& outTables,
		vector<ModelBlock>& outBlocks)
	{
		//
		// PRE-READ CHECKS
		//
		
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
				
		try
		{
			//
			// TRY TO OPEN AND READ
			//
			
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
			size_t fileSize = static_cast<size_t>(in.tellg());
			
			if (fileSize == 0) return ImportResult::RESULT_FILE_EMPTY;
			if (fileSize < MIN_TOTAL_SIZE)
			{
				return ImportResult::RESULT_UNSUPPORTED_FILE_SIZE;
			}
			if (fileSize > MAX_TOTAL_SIZE)
			{
				return ImportResult::RESULT_UNSUPPORTED_FILE_SIZE;
			}
			
			in.seekg(static_cast<streamoff>(0), ios::beg);
			
			//
			// PARSE FOUND DATA
			//
			
			vector<u8> rawData(fileSize);
			
			in.read(
				reinterpret_cast<char*>(rawData.data()),
				static_cast<streamsize>(fileSize));
				
			in.close();	
				
			ModelHeader header{};
			
			//model header
			
			memcpy(&header.magic, rawData.data() + 0, sizeof(u32));
			if (header.magic != KMD_MAGIC) return ImportResult::RESULT_INVALID_MAGIC;
			
			memcpy(&header.version, rawData.data() + 4, sizeof(u8));
			if (header.version != KMD_VERSION) return ImportResult::RESULT_INVALID_VERSION;
			
			memcpy(&header.scaleFactor, rawData.data() + 5,  sizeof(u8));
			//clamp to 0 for out of range values
			if (header.scaleFactor > 8) header.scaleFactor = 0;
			
			memcpy(&header.modelCount, rawData.data() + 6,  sizeof(u32));
			if (header.modelCount > MAX_MODEL_COUNT) return ImportResult::RESULT_INVALID_MODEL_COUNT;

			memcpy(&header.modelTablesSize, rawData.data() + 10, sizeof(u32));
			if (header.modelTablesSize < CORRECT_MODEL_TABLE_SIZE
				|| header.modelTablesSize > MAX_MODEL_TABLE_SIZE)
			{
				return ImportResult::RESULT_INVALID_MODEL_TABLE_SIZE;
			}
			
			memcpy(&header.modelBlocksSize, rawData.data() + 14, sizeof(u32));
			if (header.modelBlocksSize < VERTICE_DATA_OFFSET
				|| header.modelBlocksSize > MAX_MODEL_BLOCK_SIZE)
			{
				return ImportResult::RESULT_INVALID_MODEL_BLOCK_SIZE;
			}
			
			//model table data
			
			vector<ModelTable> tables{};
			tables.reserve(header.modelCount);
			
			for (size_t i = CORRECT_MODEL_HEADER_SIZE; 
				i < CORRECT_MODEL_HEADER_SIZE + header.modelTablesSize; 
				i += CORRECT_MODEL_TABLE_SIZE)
			{
				ModelTable t{};
				
				memcpy(t.nodeName,    rawData.data() + i + 0, sizeof(t.nodeName));
				memcpy(&t.blockOffset, rawData.data() + i + 20, sizeof(u32));
				memcpy(&t.blockSize,   rawData.data() + i + 24, sizeof(u32));
				
				tables.push_back(t);
			}
			
			//model block data
			
			vector<ModelBlock> blocks{};
			blocks.reserve(header.modelCount);
			
			for (const auto& t : tables)
			{
				ModelBlock b{};
				size_t offset = t.blockOffset;
				
				if (t.blockOffset + t.blockSize > fileSize)
				{
					return ImportResult::RESULT_UNEXPECTED_EOF;
				}
				
				memcpy(b.nodeName,       rawData.data() + offset + 0, 20);
				memcpy(b.meshName,       rawData.data() + offset + 20, 20);
				memcpy(b.nodePath,       rawData.data() + offset + 40, 50);
				memcpy(&b.dataTypeFlags, rawData.data() + offset + 90, sizeof(u8));
				memcpy(&b.renderType,    rawData.data() + offset + 91, sizeof(u8));
				
				f32 newPos[3]{};
				memcpy(&newPos[0], rawData.data() + offset + 92, sizeof(f32));
				memcpy(&newPos[1], rawData.data() + offset + 96, sizeof(f32));
				memcpy(&newPos[2], rawData.data() + offset + 100, sizeof(f32));
				
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
				memcpy(&newRot[0], rawData.data() + offset + 104, sizeof(f32));
				memcpy(&newRot[1], rawData.data() + offset + 108, sizeof(f32));
				memcpy(&newRot[2], rawData.data() + offset + 112, sizeof(f32));
				memcpy(&newRot[3], rawData.data() + offset + 116, sizeof(f32));
				
				memcpy(b.rotation, newRot, sizeof(b.rotation));
				
				f32 newSize[3]{};
				memcpy(&newSize[0], rawData.data() + offset + 120, sizeof(f32));
				memcpy(&newSize[1], rawData.data() + offset + 124, sizeof(f32));
				memcpy(&newSize[2], rawData.data() + offset + 128, sizeof(f32));
				
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
				
				memcpy(&b.verticesOffset, rawData.data() + offset + 132, sizeof(u32));
				memcpy(&b.verticesSize,   rawData.data() + offset + 136, sizeof(u32));
				memcpy(&b.indicesOffset,  rawData.data() + offset + 140, sizeof(u32));
				memcpy(&b.indicesSize,    rawData.data() + offset + 144, sizeof(u32));
				
				if (offset + static_cast<u32>(VERTICE_DATA_OFFSET) + b.verticesSize > fileSize)
				{
					return ImportResult::RESULT_UNEXPECTED_EOF;
				}
				
				//vertices
				
				size_t vertexCount = b.verticesSize / sizeof(Vertex);
				
				b.vertices.resize(vertexCount);
				memcpy(b.vertices.data(), rawData.data() + offset + VERTICE_DATA_OFFSET, b.verticesSize);
				
				//indices
				
				size_t indexCount = b.indicesSize / sizeof(u32);
				
				b.indices.resize(indexCount);
				memcpy(b.indices.data(), rawData.data() + offset + VERTICE_DATA_OFFSET + b.verticesSize, b.indicesSize);
				
				blocks.push_back(move(b));
			}
			
			outHeader = move(header);
			outTables = move(tables);
			outBlocks = move(blocks);
		}
		catch (...)
		{
			return ImportResult::RESULT_UNKNOWN_READ_ERROR;
		}
		
		return ImportResult::RESULT_SUCCESS;
	}
}