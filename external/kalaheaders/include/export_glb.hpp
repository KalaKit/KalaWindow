//---------------------------------------------------------------------------
// export_glb.hpp
//
// Copyright (C) 2026 Lost Empire Entertainment
//
// This is free source code, and you are welcome to redistribute it under certain conditions.
// Read LICENSE.md for more information.
//
// Provides:
//   - export .glb files (full parity with KalaGraphics material system, but as fully standalone header)
//   - print json file of .glb to debug the output, can make it look pretty by setting makePretty true
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

namespace KalaHeaders::KalaExportGLB
{
	using std::string;
	using std::to_string;
	using std::filesystem::path;
	using std::filesystem::exists;
	using std::filesystem::absolute;
	using std::filesystem::perms;
	using std::filesystem::status;
	using std::vector;
	using std::ofstream;
	using std::ios;

	struct Transform
	{
		//XYZ position
		f32 position[3]{};
		//XYZW quaternion rotation
		f32 rotation[4]{ 0.0f, 0.0f, 0.0f, 1.0f }; 
		//XYZ size
		f32 size[3] = { 1.0f, 1.0f, 1.0f };
	};

	struct Vertex
	{
		f32 pos[3]{};
		f32 normal[3]{};
		f32 uv[2]{};
		//RGBA color, defaults to opaque white
		f32 color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	};

	enum class AlphaMode : u8
	{
		A_OPAQUE = 0,
		A_BLEND = 1,
		A_MASK = 2
	};

	struct ExportMeshData
	{
		vector<Vertex> vertices{};
		vector<u32> indices{};
	};

    struct ExportGLBTextureData
    {
		//should be already parsed png binary data, not raw pixel data
		vector<u8> pngImageData{};

		//TODO: add texture type like diffuse etc...
    };

	struct ExportMaterialData
	{
		string materialName = "unnamed_material";
		f32 baseColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

		AlphaMode alphaMode{};
		f32 alphaCutoff{};

		ExportGLBTextureData textureData{};
	};

	struct ExportNodeData
	{
		string nodeName = "unnamed_node";

		Transform transform{};
		ExportMeshData meshData{};
		ExportMaterialData matData{};
	};

	//Stores json data into a string that would otherwise be used during export,
	//set makePretty to true if you want to keep the nice json format,
	//otherwise all spaces outside of JSON strings are removed
	KNODISCARD
	inline string GetJsonDataFromNodeData(
		vector<ExportNodeData>&& nodeData,
		string& outJson,
		bool makePretty = false)
	{
		auto escape_json_string = [](const string& input) -> string
			{
				string output{};
				output.reserve(input.size());

				for (size_t i = 0; i < input.size();)
				{
					const unsigned char c = scast<unsigned char>(input[i]);

					switch (c)
					{
					case '"':
						output += "\\\"";
						i++;
						continue;
					case '\\':
						output += "\\\\";
						i++;
						continue;
					case '\b':
						output += "\\b";
						i++;
						continue;
					case '\f':
						output += "\\f";
						i++;
						continue;
					case '\n':
						output += "\\n";
						i++;
						continue;
					case '\r':
						output += "\\r";
						i++;
						continue;
					case '\t':
						output += "\\t";
						i++;
						continue;
					}

					if (c < 0x20)
					{
						constexpr char hex[] = "0123456789ABCDEF";

						output += "\\u00";
						output += hex[(c >> 4) & 0xF];
						output += hex[c & 0xF];

						i++;
						continue;
					}

					//ASCII
					if (c < 0x80)
					{
						output += scast<char>(c);
						i++;
						continue;
					}

					//UTF-8
					size_t length{};

					if      (c >= 0xC2 && c <= 0xDF) length = 2;
					else if (c >= 0xE0 && c <= 0xEF) length = 3;
					else if (c >= 0xF0 && c <= 0xF4) length = 4;
					else
					{
						output += '?';
						i++;
						continue;
					}

					if (i + length > input.size())
					{
						output += '?';
						break;
					}

					bool valid = true;

					for (size_t j = 1; j < length; j++)
					{
						const unsigned char continuation = 
							scast<unsigned char>(input[i + j]);

						if ((continuation & 0xC0) != 0x80)
						{
							valid = false;
							break;
						}
					}

					//reject overlong UTF-8, UTF-16 surrogates and > U+10FFFF
					if (valid && length == 3)
					{
						const unsigned char c1 = 
							scast<unsigned char>(input[i + 1]);

						if (c == 0xE0 && c1 < 0xA0)  valid = false;
						if (c == 0xED && c1 >= 0xA0) valid = false;
					}
					else if (valid && length == 4)
					{
						const unsigned char c1 = 
							scast<unsigned char>(input[i + 1]);

						if (c == 0xF0 && c1 < 0x90) valid = false;
						if (c == 0xF4 && c1 > 0x8F) valid = false;
					}

					if (!valid)
					{
						output += '?';
						i++;
						continue;
					}

					output.append(input, i, length);
					i += length;
				}

				return output;
			};

		if (nodeData.empty())
		{
			return "Failed to export GLB to path because no node data was passed!";
		}

		for (size_t i = 0; i < nodeData.size(); i++)
		{
			const ExportNodeData& node = nodeData[i];

			if (node.meshData.vertices.empty())
			{
				return "Failed to generate json because node '" + to_string(i) + "' had no vertices!";
			}
		}

		//write json data that will be stored
		outJson = 
R"(
{
  "asset":
  {
    "version": "2.0"
  },
  "scene": 0,
  "scenes":
  [
    {
      "nodes": [)";

		//write node indexes
		for (size_t i{}; i < nodeData.size(); i++)
		{
			outJson += to_string(i);
			if (i + 1 < nodeData.size()) outJson += ", ";
		}

		//start "nodes"
		outJson += 
R"(]
    }
  ],
  "nodes":
  [)";

		//write node data
		for (size_t i = 0; i < nodeData.size(); i++)
		{
			//name
			outJson += 
R"(
    {
      "name": ")";
			outJson += escape_json_string(nodeData[i].nodeName);

			//mesh
			outJson += 
R"(",
      "mesh": )";
			outJson += to_string(i);

			//translation
			outJson += 
R"(,
      "translation": [)";
			outJson += 
				to_string(nodeData[i].transform.position[0]) + ", "
				+ to_string(nodeData[i].transform.position[1]) + ", "
				+ to_string(nodeData[i].transform.position[2]);

			//rotation
			outJson += 
R"(],
      "rotation": [)";
			outJson += 
				to_string(nodeData[i].transform.rotation[0]) + ", "
				+ to_string(nodeData[i].transform.rotation[1]) + ", "
				+ to_string(nodeData[i].transform.rotation[2]) + ", "
				+ to_string(nodeData[i].transform.rotation[3]);

			//scale
			outJson += 
R"(],
      "scale": [)";
			outJson += 
				to_string(nodeData[i].transform.size[0]) + ", "
				+ to_string(nodeData[i].transform.size[1]) + ", "
				+ to_string(nodeData[i].transform.size[2]);
			outJson += 
R"(]
    })";

			//next transform
			if (i + 1 < nodeData.size()) outJson += ",";
		}

		//start "meshes"
		outJson +=
R"(
  ],
  "meshes":
  [)";

		//write mesh data
		size_t accessorIndex{};
		
		for (size_t i = 0; i < nodeData.size(); i++)
		{
			const bool hasIndices = !nodeData[i].meshData.indices.empty();

			outJson +=
R"(
    {
      "primitives":
      [
        {
          "attributes":
          {
            "POSITION": )";
			outJson += to_string(accessorIndex);

			outJson +=
R"(,
            "NORMAL": )";
			outJson += to_string(accessorIndex + 1);

			outJson +=
R"(,
            "TEXCOORD_0": )";
			outJson += to_string(accessorIndex + 2);

			outJson +=
R"(,
            "COLOR_0": )";
			outJson += to_string(accessorIndex + 3);

			outJson += 
R"(
          })";

			if (hasIndices)
			{
				outJson +=
R"(,
          "indices": )";
				outJson += to_string(accessorIndex + 4);
			}

			//material
			outJson +=
R"(,
          "material": )";
			outJson += to_string(i);

			outJson +=
R"(
        }
      ]
    })";

			//next mesh data
			if (i + 1 < nodeData.size()) outJson += ",";

			accessorIndex += hasIndices ? 5 : 4;
		}

		//start "materials"
		outJson += 
R"(
  ],
  "materials":
  [)";

		size_t textureIndex{};

		//write material data
		for (size_t i = 0; i < nodeData.size(); i++)
		{
			const ExportMaterialData& matData = nodeData[i].matData;

			outJson +=
R"(
    {
      "name": ")";
			outJson += escape_json_string(matData.materialName);

			outJson +=
R"(",
      "pbrMetallicRoughness":
      {
        "baseColorFactor": [)";
			outJson +=
				to_string(matData.baseColor[0]) + ", "
				+ to_string(matData.baseColor[1]) + ", "
				+ to_string(matData.baseColor[2]) + ", "
				+ to_string(matData.baseColor[3]);

			outJson += "]";

			if (!matData.textureData.pngImageData.empty())
			{
				outJson += 
R"(,
        "baseColorTexture":
        {
          "index": )";

				outJson += to_string(textureIndex);

				outJson +=
R"(
        })";
				textureIndex++;

			}

			string alphaModeString{};

			switch (matData.alphaMode)
			{
			default:
			case AlphaMode::A_OPAQUE:
				alphaModeString = "OPAQUE";
				break;
			case AlphaMode::A_BLEND:
				alphaModeString = "BLEND";
				break;
			case AlphaMode::A_MASK:
				alphaModeString = "MASK";
				break;
			}

			outJson +=
R"(
      },
	  "alphaMode": ")";
			outJson += alphaModeString;

			outJson +=
R"(",    
      "alphaCutoff": )";
			outJson += to_string(matData.alphaCutoff);

			outJson +=
R"(
    })";

			//next material
			if (i + 1 < nodeData.size()) outJson += ",";
		}

		//write textures
		if (textureIndex > 0)
		{
			outJson +=
R"(
  ],
  "textures":
  [)";

			size_t currentTextureIndex{};

			for (size_t i = 0; i < nodeData.size(); i++)
			{
				const ExportGLBTextureData& textureData = nodeData[i].matData.textureData;

				if (textureData.pngImageData.empty()) continue;

				outJson += 
R"(
    {
      "source": )";
				outJson += to_string(currentTextureIndex);

				outJson +=
R"(
    })";

				currentTextureIndex++;

				//next texture
				if (currentTextureIndex < textureIndex) outJson += ",";
			}
		}

		size_t imagebufferviewIndex{};

		for (size_t i = 0; i < nodeData.size(); i++)
		{
			imagebufferviewIndex += nodeData[i].meshData.indices.empty() ? 4 : 5;
		}

		//write images
		if (textureIndex > 0)
		{
			outJson +=
R"(
  ],
  "images":
  [)";

			size_t currentImageIndex{};

			for (size_t i = 0; i < nodeData.size(); i++)
			{
				const ExportGLBTextureData& textureData = nodeData[i].matData.textureData;

				if (textureData.pngImageData.empty()) continue;

				outJson +=
R"(
    {
      "bufferView": )";
				outJson += to_string(imagebufferviewIndex);

				outJson +=
R"(,
      "mimeType": "image/png"
    })";

				imagebufferviewIndex++;
				currentImageIndex++;

				//next image
				if (currentImageIndex < textureIndex) outJson += ",";
			}
		}

		//start "accessors"
		outJson +=
R"(
  ],
  "accessors":
  [)";

		//write accessors
		accessorIndex = 0;

		for (size_t i = 0; i < nodeData.size(); i++)
		{
			const bool hasIndices = !nodeData[i].meshData.indices.empty();

			f32 minPos[3]
			{
				nodeData[i].meshData.vertices[0].pos[0],
				nodeData[i].meshData.vertices[0].pos[1],
				nodeData[i].meshData.vertices[0].pos[2]
			};

			f32 maxPos[3]
			{
				nodeData[i].meshData.vertices[0].pos[0],
				nodeData[i].meshData.vertices[0].pos[1],
				nodeData[i].meshData.vertices[0].pos[2]
			};

			for (const Vertex& vertex : nodeData[i].meshData.vertices)
			{
				for (size_t j = 0; j < 3; j++)
				{
					if (vertex.pos[j] < minPos[j]) minPos[j] = vertex.pos[j];
					if (vertex.pos[j] > maxPos[j]) maxPos[j] = vertex.pos[j];
				}
			}

			//position
			outJson += 
R"(
    {
      "bufferView": )";
			outJson += to_string(accessorIndex);
			outJson +=
R"(,
      "componentType": 5126,
      "count": )";
			outJson += to_string(nodeData[i].meshData.vertices.size());
			outJson +=
R"(,
      "type": "VEC3",
      "min": [)";
			outJson += 
				to_string(minPos[0]) + ", "
				+ to_string(minPos[1]) + ", "
				+ to_string(minPos[2]);
			outJson +=
R"(],
      "max": [)";

			outJson += 
				to_string(maxPos[0]) + ", "
				+ to_string(maxPos[1]) + ", "
				+ to_string(maxPos[2]);
			outJson +=
R"(]
    },)";

			//normal
			outJson +=
R"(
    {
      "bufferView": )";
			outJson += to_string(accessorIndex + 1);
			outJson += 
R"(,
      "componentType": 5126,
      "count": )";
			outJson += to_string(nodeData[i].meshData.vertices.size());
			outJson += 
R"(,
      "type": "VEC3"
    },)";

			//uv
			outJson +=
R"(
    {
      "bufferView": )";
			outJson += to_string(accessorIndex + 2);
			outJson += 
R"(,
      "componentType": 5126,
      "count": )";
			outJson += to_string(nodeData[i].meshData.vertices.size());
			outJson += 
R"(,
      "type": "VEC2"
    },)";

			//color
			outJson +=
R"(
    {
      "bufferView": )";
			outJson += to_string(accessorIndex + 3);
			outJson += 
R"(,
      "componentType": 5126,
      "count": )";
			outJson += to_string(nodeData[i].meshData.vertices.size());
			outJson += 
R"(,
      "type": "VEC4"
    })";

			//indices
			if (hasIndices)
			{
				outJson +=
R"(,
    {
      "bufferView": )";
				outJson += to_string(accessorIndex + 4);
				outJson += 
R"(,
      "componentType": 5125,
      "count": )";
				outJson += to_string(nodeData[i].meshData.indices.size());
				outJson += 
R"(,
      "type": "SCALAR"
    })";
			}

			//next mesh accessors
			if (i + 1 < nodeData.size()) outJson += ",";

			accessorIndex += hasIndices ? 5 : 4;
		}

		//start "bufferViews"
		outJson +=
R"(
  ],
  "bufferViews":
  [)";

		//write buffer views
		size_t byteOffset{};

		for (size_t i = 0; i < nodeData.size(); i++)
		{
			const ExportMeshData& meshData = nodeData[i].meshData;
			const bool hasIndices = !meshData.indices.empty();

			const size_t positionByteLength = 
				meshData.vertices.size()
				* sizeof(f32)
				* 3;

			const size_t normalByteLength = 
				meshData.vertices.size()
				* sizeof(f32)
				* 3;

			const size_t uvByteLength = 
				meshData.vertices.size()
				* sizeof(f32)
				* 2;

			const size_t colorByteLength = 
				meshData.vertices.size()
				* sizeof(f32)
				* 4;

			const size_t indexByteLength = meshData.indices.size() * sizeof(u32);

			//position
			outJson += 
R"(
    {
      "buffer": 0,
      "byteOffset": )";
			outJson += to_string(byteOffset);
			outJson +=
R"(,
      "byteLength": )";
			outJson += to_string(positionByteLength);
			outJson += 
R"(
    },)";

			byteOffset += positionByteLength;

			//normal
			outJson += 
R"(
    {
      "buffer": 0,
      "byteOffset": )";
			outJson += to_string(byteOffset);
			outJson +=
R"(,
      "byteLength": )";
			outJson += to_string(normalByteLength);
			outJson += 
R"(
    },)";

			byteOffset += normalByteLength;

			//uv
			outJson += 
R"(
    {
      "buffer": 0,
      "byteOffset": )";
			outJson += to_string(byteOffset);
			outJson +=
R"(,
      "byteLength": )";
			outJson += to_string(uvByteLength);
			outJson += 
R"(
    },)";

			byteOffset += uvByteLength;

			//color
			outJson += 
R"(
    {
      "buffer": 0,
      "byteOffset": )";
			outJson += to_string(byteOffset);
			outJson +=
R"(,
      "byteLength": )";
			outJson += to_string(colorByteLength);
			outJson += 
R"(
    })";

			byteOffset += colorByteLength;

			//indices
			if (hasIndices)
			{
				outJson += 
R"(,
    {
      "buffer": 0,
      "byteOffset": )";
				outJson += to_string(byteOffset);
				outJson +=
R"(,
      "byteLength": )";
				outJson += to_string(indexByteLength);
				outJson += 
R"(
    })";

				byteOffset += indexByteLength;
			}
			
			//next mesh buffer views
			if (i + 1 < nodeData.size()) outJson += ",";
		}

		//write image buffer views
		for (size_t i = 0; i < nodeData.size(); i++)
		{
			const ExportGLBTextureData& textureData = nodeData[i].matData.textureData;

			if (textureData.pngImageData.empty()) continue;

			while (byteOffset % 4 != 0) byteOffset++;

			outJson +=
R"(,
    {
      "buffer": 0,
      "byteOffset": )";
			outJson += to_string(byteOffset);

			outJson +=
R"(,
      "byteLength": )";
			outJson += to_string(textureData.pngImageData.size());

			outJson +=
R"(
    })";

			byteOffset += textureData.pngImageData.size();
		}
   
		//write buffer
		outJson +=
R"(
  ],
  "buffers":
  [
    {
      "byteLength": )";
		outJson += to_string(byteOffset);
		outJson +=
R"(
    }
  ]
})";

		//remove spaces
		if (!makePretty)
		{
			string cleanedJson{};
			cleanedJson.reserve(outJson.size());

			bool insideString{};
			bool escaped{};

			for (char c : outJson)
			{
				if (insideString)
				{
					cleanedJson += c;

					if (escaped)        escaped = false;
					else if (c == '\\') escaped = true;
					else if (c == '"')  insideString = false;

					continue;
				}

				if (c == '"')
				{
					insideString = true;
					cleanedJson += c;
					continue;
				}

				//remove JSON formatting whitespace
				if (c == ' '
					|| c == '\t'
					|| c == '\n'
					|| c == '\r')
				{
					continue;
				}

				cleanedJson += c;
			}

			outJson = std::move(cleanedJson);
		}

		return "";
	}

	KNODISCARD
	inline string ExportMeshes(
		vector<ExportNodeData>&& nodeData,
		const path& exportPath,
		bool overwrite = false)
	{
		static constexpr u32 GLTF_MAGIC = 0x46546C67; // "glTF"
		static constexpr u32 GLTF_VERSION = 2;
		static constexpr u32 JSON_CHUNK_TYPE = 0x4E4F534A; // "JSON"
		static constexpr u32 BIN_CHUNK_TYPE = 0x004E4942; // "BIN"

		struct GLB_Header
		{
			u32 magic = GLTF_MAGIC;
			u32 version = GLTF_VERSION;
			u32 length{};
		};

		struct JSON_Header
		{
			u32 jsonChunkLength{};
			u32 jsonChunkType = JSON_CHUNK_TYPE;
		};

		struct BIN_Header
		{
			u32 binChunkLength{};
			u32 binChunkType = BIN_CHUNK_TYPE;
		};

		auto get_binary_data = [](const vector<ExportNodeData>& nodeData) -> vector<u8>
			{
				vector<u8> binaryData{};

				for (const ExportNodeData& node : nodeData)
				{
					const ExportMeshData& meshData = node.meshData;

					//positions
					for (const Vertex& vertex : meshData.vertices)
					{
						const u8* data = rcast<const u8*>(vertex.pos);

						binaryData.insert(
							binaryData.end(),
							data,
							data + sizeof(vertex.pos));
					}

					//normals
					for (const Vertex& vertex : meshData.vertices)
					{
						const u8* data = rcast<const u8*>(vertex.normal);

						binaryData.insert(
							binaryData.end(),
							data,
							data + sizeof(vertex.normal));
					}

					//uvs
					for (const Vertex& vertex : meshData.vertices)
					{
						const u8* data = rcast<const u8*>(vertex.uv);

						binaryData.insert(
							binaryData.end(),
							data,
							data + sizeof(vertex.uv));
					}

					//colors
					for (const Vertex& vertex : meshData.vertices)
					{
						const u8* data = rcast<const u8*>(vertex.color);

						binaryData.insert(
							binaryData.end(),
							data,
							data + sizeof(vertex.color));
					}

					//indices
					if (!meshData.indices.empty())
					{
						const u8* data = rcast<const u8*>(meshData.indices.data());

						binaryData.insert(
							binaryData.end(),
							data,
							data + meshData.indices.size() * sizeof(u32));
					}
				}

				//textures
				for (const ExportNodeData& node : nodeData)
				{
					const vector<u8>& pngImageData = node.matData.textureData.pngImageData;

					if (pngImageData.empty()) continue;

					while (binaryData.size() % 4 != 0) binaryData.push_back(0);

					binaryData.insert(
						binaryData.end(),
						pngImageData.begin(),
						pngImageData.end());
				}

				return binaryData;
			};

		if (nodeData.empty())
		{
			return "Failed to export GLB because its node data is empty!";
		}

		for (size_t i = 0; i < nodeData.size(); i++)
		{
			const ExportNodeData& node = nodeData[i];

			if (node.meshData.vertices.empty())
			{
				return "Failed to export GLB to path because node '" + to_string(i) + "' had no vertices!";
			}
		}

		if (exportPath.empty())
		{
			return "Failed to export GLB because its export path is empty!";
		}

		if (!exportPath.parent_path().empty()
			&& !exists(exportPath.parent_path()))
		{
			return "Failed to export GLB because export path '" + exportPath.string() + "' is invalid!";
		}

		if (is_directory(exportPath))
		{
			return "Failed to export GLB to export path '" + exportPath.string() + "' because it is a directory!";
		}

		if (!exportPath.has_extension())
		{
			return "Failed to export GLB to export path '" + exportPath.string() + "' because it is a directory!";
		}
		
		if (exportPath.extension() != ".glb")
		{
			return "Failed to export GLB to export path '" + exportPath.string() + "' because its extension is not supported!";
		}

		if (!overwrite
			&& exists(exportPath))
		{
			return "Failed to export GLB to export path '" + exportPath.string() + "' because it already exists!";
		}

        auto fileStatus = status(absolute(exportPath).parent_path());
        auto filePerms = fileStatus.permissions();

        bool canWrite = (filePerms & (
            perms::owner_write
            | perms::group_write
            | perms::others_write))
            != perms::none;

        if (!canWrite) return "Failed to export GLB to export path '" + exportPath.string() + "' because of insufficient write permissions!";

		//
		// GET DATA THAT WILL BE WRITTEN
		//

		string jsonData{};
		string _ = GetJsonDataFromNodeData(
			vector<ExportNodeData>(nodeData),
			jsonData);

		//pad the json file to be a multiple of four
		while (jsonData.size() % 4 != 0) jsonData += ' ';

		vector<u8> binData = get_binary_data(nodeData);

		//pad binary data to be a multiple of four
		while (binData.size() % 4 != 0) binData.push_back(0);

		GLB_Header glbHeader
		{ 
			.length = scast<u32>(
				sizeof(GLB_Header)
				+ sizeof(JSON_Header)
				+ jsonData.size()
				+ sizeof(BIN_Header)
				+ binData.size())
		};

		JSON_Header jsonHeader{ .jsonChunkLength = scast<u32>(jsonData.size()) };

		BIN_Header binHeader{ .binChunkLength = scast<u32>(binData.size()) };

		//
		// WRITE TO .GLB FILE
		//

		ofstream file(absolute(exportPath), ios::binary);
		if (!file.is_open()) return "Failed to open export path '" + exportPath.string() + "' for writing!";

		file.write(
			rcast<const char*>(&glbHeader),
			sizeof(GLB_Header));

		file.write(
			rcast<const char*>(&jsonHeader),
			sizeof(JSON_Header));

		file.write(
			jsonData.data(),
			jsonData.size());

		file.write(
			rcast<const char*>(&binHeader),
			sizeof(BIN_Header));

		file.write(
			rcast<const char*>(binData.data()),
			binData.size());

		if (!file) return "Failed to export GLB file '" + exportPath.string() + "'!";

		file.close();

		return "";
	}
}
