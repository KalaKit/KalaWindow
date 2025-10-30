//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

// ===================================================================================
// This header originates from KalaFont. The purpose of this header is to allow
// importing .kfont files to your executable. This header requires file_utils.hpp,
// math_utils.hpp and log_utils.hpp from KalaHeaders to function, otherwise its
// completely standalone from the rest of the KalaFont ecosystem. 
// ===================================================================================

#include <filesystem>
#include <vector>
#include <fstream>
#include <sstream>
#include <cerrno>
#include <cstring>
#include <string>

#include "KalaHeaders/file_utils.hpp"
#include "KalaHeaders/math_utils.hpp"
#include "KalaHeaders/log_utils.hpp"

#pragma once

namespace KalaFont
{
	using std::filesystem::path;
	using std::filesystem::exists;
	using std::filesystem::is_regular_file;
	using std::vector;
	using std::ifstream;
	using std::ostringstream;
	using std::strerror;
	using std::ios;
	using std::string;
	using std::to_string;
	using std::move;
	
	using KalaHeaders::vec2;
	using KalaHeaders::mat2;
	using KalaHeaders::Log;
	using KalaHeaders::LogType;
	
	struct GlyphPoint
	{
		vec2 size{};
		bool onCurve{};
	};

	struct GlyphContours
	{
		vector<vector<GlyphPoint>> contours{};
	};

	struct GlyphResult
	{
		vector<f32> vertices{};
		vector<u32> indices{};

		GlyphContours contours{};
		vec2 anchor{};
		mat2 transform{};
		u32 glyphIndex{};
		f32 advanceWidth{};
		f32 leftSideBearing{};
	};
	
	inline bool CheckReadError(
		const ifstream& in,
		const string& message)
	{
		if (in.fail()
			&& errno != 0)
		{
			int err = errno;
			char buf[256]{};

			ostringstream oss{};
			oss << "Failed to " << message << " because it couldn't be opened! "
				<< "Reason: (errno " << err << "): ";

			if (strerror_s(buf, sizeof(buf), err) == 0) oss << buf;
			else oss << " Unknown error";
			
			Log::Print(
				oss.str(),
				"READ_KFONT",
				LogType::LOG_ERROR);
			
			errno = 0;
			return false;
		}
		
		errno = 0;
		return true;
	}
	
	inline bool ImportKalaFont(
		const path& fontPath,
		vector<GlyphResult>& outGlyphs)
	{
		if (!exists(fontPath))
		{
			Log::Print(
				"Did not find font from path '" + fontPath.string() + "'!",
				"READ_KFONT",
				LogType::LOG_ERROR);
			
			return false;
		}
		
		if (!is_regular_file(fontPath))
		{
			Log::Print(
				"Font '" + fontPath.string() + "' does not have a valid extension!",
				"READ_KFONT",
				LogType::LOG_ERROR);
			
			return false;
		}
		
		if (!fontPath.has_extension()
			|| fontPath.extension() != ".kfont")
		{
			Log::Print(
				"Font '" + fontPath.string() + "' does not have a valid extension!",
				"READ_KFONT",
				LogType::LOG_ERROR);
			
			return false;
		}
		
		ifstream file(fontPath, ios::binary);
		
		string readMsg = "read font '" + fontPath.string() + "'";
		if (!CheckReadError(file, readMsg)) return false;
		
		auto ReadU32 = [&](u32& v)
			{
				file.read(reinterpret_cast<char*>(&v), sizeof(u32));
			};
		auto ReadF32 = [&](f32& v)
			{
				file.read(reinterpret_cast<char*>(&v), sizeof(f32));
			};
		auto ReadStr = [&](char* buf, size_t len)
			{
				file.read(buf, len);
			};
			
		//
		// HEADER
		//
		
		char magic[4]{};
		ReadStr(magic, 4);
		
		if (memcmp(magic, "KFNT", 4) != 0)
		{
			Log::Print(
				"Did not find correct magic from font '" + fontPath.string() + "'!",
				"READ_KFONT",
				LogType::LOG_ERROR);
			
			return false;
		}
		
		u32 version{};
		u32 glyphCount{};
		
		ReadU32(version);
		ReadU32(glyphCount);
		
		if (version != 1)
		{
			Log::Print(
				"Font '" + fontPath.string() + "' has invalid version value!",
				"READ_KFONT",
				LogType::LOG_ERROR);
			
			return false;
		}
		if (glyphCount == 0)
		{
			Log::Print(
				"Font '" + fontPath.string() + "' has invalid glyph count value!",
				"READ_KFONT",
				LogType::LOG_ERROR);
			
			return false;
		}
		
		outGlyphs.clear();
		outGlyphs.reserve(glyphCount);
		
		//
		// GLYPH BLOCKS
		//
		
		for (u32 g = 0; g < glyphCount; ++g)
		{
			char tag[4]{};
			ReadStr(tag, 4);
			
			if (memcmp(tag, "GLYF", 4) != 0)
			{
				Log::Print(
					"Font '" + fontPath.string() + "' has invalid glyph tag for glyph '" + to_string(g) + "'!",
					"READ_KFONT",
					LogType::LOG_ERROR);
			
				return false;
			}
			
			GlyphResult glyph{};
			
			//core
			ReadU32(glyph.glyphIndex);
			ReadF32(glyph.advanceWidth);
			ReadF32(glyph.leftSideBearing);
			
			//anchor
			ReadF32(glyph.anchor.x);
			ReadF32(glyph.anchor.y);
			
			//transform
			ReadF32(glyph.transform.m00);
			ReadF32(glyph.transform.m01);
			ReadF32(glyph.transform.m10);
			ReadF32(glyph.transform.m11);
			
			//vertices
			ReadStr(tag, 4);
			if (memcmp(tag, "VERT", 4) != 0)
			{
				Log::Print(
					"Font '" + fontPath.string() + "' has invalid vertice tag for glyph '" + to_string(g) + "'!",
					"READ_KFONT",
					LogType::LOG_ERROR);
			
				return false;
			}
			
			u32 vertexCount{};
			ReadU32(vertexCount);
			glyph.vertices.resize(vertexCount * 2);
			for (u32 i = 0; i < vertexCount * 2; ++i)
			{
				ReadF32(glyph.vertices[i]);
			}
			
			//indices
			ReadStr(tag, 4);
			if (memcmp(tag, "INDI", 4) != 0)
			{
				Log::Print(
					"Font '" + fontPath.string() + "' has invalid indice tag for glyph '" + to_string(g) + "'!",
					"READ_KFONT",
					LogType::LOG_ERROR);
			
				return false;
			}
			
			u32 indexCount{};
			ReadU32(indexCount);
			glyph.indices.resize(indexCount);
			
			for (u32 i = 0; i < indexCount; ++i)
			{
				ReadU32(glyph.indices[i]);
			}
			
			outGlyphs.push_back(move(glyph));
		}
		
		return true;
	}
}