//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>

//external
#include "ft2build.h"
#include FT_FREETYPE_H

namespace KalaKit
{
    using std::string;

    class FreeType
    {
    public:
        FreeType();

        ~FreeType();

        bool LoadFont(const string& fontPath, int size, bool glyphTest = false);

	private:
        bool isInitialized;
        FT_Library lib;
        FT_Face face;
    };
}