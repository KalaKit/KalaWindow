//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifndef KALAKIT_MODULE
    #define KALAKIT_MODULE "UI"
#endif

#include <filesystem>
#include <string>

//external
#include "ft2build.h"
#include FT_FREETYPE_H

//kalawindow
#include "platform.hpp"

using std::filesystem::exists;
using std::string;

namespace KalaKit
{
    class FreeType
    {
    public:
        FreeType():
            lib(nullptr), 
            face(nullptr) 
        {
            if (FT_Init_FreeType(&lib))
            {
                LOG_ERROR("Couldn't initialize FreeType!");
            }
            LOG_DEBUG("Successfully initialized FreeType!");
            isInitialized = true;
        }

        ~FreeType() 
        {
            if (face) { FT_Done_Face(face); }
            if (lib) { FT_Done_FreeType(lib); }
        }

        bool LoadFont(const string& fontPath)
        {
            if (!isInitialized)
            {
                LOG_ERROR("Cannot load font because FreeType is not yet initialized!");
                return false;
            }

            if (!exists(fontPath))
            {
                LOG_ERROR("Couldn't find font path '" + fontPath + "'!");
                return false;
            }

            if (FT_New_Face(lib, fontPath.c_str(), 0, &face))
            {
                LOG_ERROR("Couldn't load font path '" + fontPath + "'!");
                return false;
            }

            if (face == nullptr)
            {
                LOG_ERROR("Font face is null after loading font: '" + fontPath + "'!");
                return false;
            }

            return true;
        }

	private:
        bool isInitialized;
        FT_Library lib;
        FT_Face face;
    };
}