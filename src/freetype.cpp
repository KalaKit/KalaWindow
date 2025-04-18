//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#define KALAKIT_MODULE "UI"

#include <filesystem>

//kalawindow
#include "freetype.hpp"
#include "window.hpp"

using std::filesystem::exists;
using std::to_string;
using std::cout;

namespace KalaKit
{
    FreeType::FreeType() :
        lib(nullptr), 
        face(nullptr) 
    {
        if (FT_Init_FreeType(&lib))
        {
            LOG_ERROR("Couldn't initialize FreeType!");
        }
        LOG_DEBUG("Successfully initialized FreeType!");
    }

    FreeType::~FreeType() 
    {
        if (face) { FT_Done_Face(face); }
        if (lib) { FT_Done_FreeType(lib); }
    }

    bool FreeType::LoadFont(const string& fontPath, int size, bool glyphTest)
    {
        if (KalaWindow::freeType == nullptr)
        {
            LOG_ERROR("Cannot load font because FreeType is not yet initialized!");
            return false;
        }

        if (!exists(fontPath))
        {
            LOG_ERROR("Couldn't find font path '" + fontPath + "'!");
            return false;
        }

        if (face)
        {
            FT_Done_Face(face);
            face = nullptr;
        }
        if (FT_New_Face(lib, fontPath.c_str(), 0, &face))
        {
            LOG_ERROR("Couldn't load font path '" + fontPath + "'!");
            return false;
        }

        FT_UInt ftSize = static_cast<FT_UInt>(size);
        if (FT_Set_Pixel_Sizes(face, 0, ftSize))
        {
            LOG_ERROR("Failed to set pixel sizes!");
            return false;
        }

        if (glyphTest)
        {
            if (FT_Load_Char(face, 'A', FT_LOAD_RENDER))
            {
                LOG_ERROR("Failed to load glyph!");
                return false;
            }
    
            FT_Bitmap& bmp = face->glyph->bitmap;
    
            LOG_DEBUG("Glyph size: " 
                + to_string(bmp.width) 
                + "x" 
                + to_string(bmp.rows));
            LOG_DEBUG("Pitch: " + to_string(bmp.pitch));
            LOG_DEBUG("Grayscale buffer:");
            for (int y = 0; y < bmp.rows; ++y)
            {
                string line;
                for (int x = 0; x < bmp.width; ++x) {
                    unsigned char pixel = bmp.buffer[y * bmp.pitch + x];
                    line += (pixel > 128) ? '#' : ' ';
                }
                cout << line << '\n';
            }
        }

        return true;
    }
}