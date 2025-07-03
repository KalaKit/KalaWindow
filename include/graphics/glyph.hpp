//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <vector>
#include <memory>

#include "core/platform.hpp"
#include "graphics/window.hpp"

namespace KalaWindow::Graphics
{
    using std::string;
    using std::vector;
    using std::unique_ptr;

    enum class GlyphType
    {
        NUM_0, NUM_1, NUM_2, NUM_3,
        NUM_4, NUM_5, NUM_6, NUM_7,
        NUM_8, NUM_9,

        LET_A, LET_B, LET_C, LET_D,
        LET_E, LET_F, LET_G, LET_H,
        LET_I, LET_J, LET_K, LET_L,
        LET_M, LET_N, LET_O, LET_P,
        LET_Q, LET_R, LET_S, LET_T,
        LET_U, LET_V, LET_W, LET_X,
        LET_Y, LET_Z,

        LET_a, LET_b, LET_c, LET_d,
        LET_e, LET_f, LET_g, LET_h,
        LET_i, LET_j, LET_k, LET_l,
        LET_m, LET_n, LET_o, LET_p,
        LET_q, LET_r, LET_s, LET_t,
        LET_u, LET_v, LET_w, LET_x,
        LET_y, LET_z,

        SYM_APOS,   // '
        SYM_COMMA,  // ,
        SYM_DOT,    // .
        SYM_COLON,  // :
        SYM_DASH,   // -
        SYM_UND,    // _
        SYM_SLASH,  // /
        SYM_BSLASH, // '\'
        SYM_PLUS,   // +
        SYM_PERC,   // %
        SYM_LPAR,   // (
        SYM_RPAR,   // )

        CHAR_NONE
    };

    struct VulkanTex
    {
        void* image;
        void* imageView;
        void* descriptor;
    };
    struct Glyph
    {
        string glyphName{};
        string glyphPath{};   //Path to this glyph
        GlyphType glyphEnum{}; //Target enum

        int posX{};           //X position on screen (pixels)
        int posY{};           //Y position on screen (pixels)
        int scaleX{};         //Width of glyph quad (before padding)
        int scaleY{};         //Height of glyph quad (before padding)

        int padding_top{};    //Space to inset from top (content starts lower)
        int padding_bottom{}; //Space to inset from bottom (content ends earlier)
        int padding_left{};   //Space to inset from left (narrow glyph starts later)
        int padding_right{};  //Space to inset from right (narrow glyph ends sooner)

        unsigned int openGLTex{}; //OpenGL texture reference, unused in Vulkan
        VulkanTex vulkanTex{};  //Vulkan texture reference, unused in OpenGL
    };

    class KALAWINDOW_API GlyphSystem
    {
    public:
        //Add the pre-loaded set of glyphs you wish to use in this program at runtime.
        static bool AddGlyphChars(const vector<Glyph> chars);

        //Fills glyph maps and prepares the setup
        //for writing text on screen.
        static bool Initialize(Window* window);

        //Write text on the screen.
        static vector<unique_ptr<Glyph>> PlaceText(
            kvec2 pos,
            const string& text,
            Window* window);

        //Place a new image on the screen.
        static unique_ptr<Glyph> PlaceImage(
            Glyph glyphData,
            Window* window);

        //Displays all placed glyphs on the screen at runtime.
        static bool RenderGlyphs(Window* window);

        //Removes all displayed glyphs from the screen and memory.
        static void ClearGlyphs(Window* window);

    private:
        static inline bool isInitialized = false;
    };
}