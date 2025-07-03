//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#define KALAKIT_MODULE "GLYPH"

#include <unordered_map>
#include <vector>
#include <string>
#include <filesystem>
#include <memory>

#include "graphics/glyph.hpp"
#include "graphics/window.hpp"

using KalaWindow::Graphics::Window;
using KalaWindow::Graphics::GlyphType;
using KalaWindow::Graphics::Glyph;

using std::unordered_map;
using std::vector;
using std::unique_ptr;
using std::make_unique;
using std::string;
using std::filesystem::path;
using std::filesystem::exists;
using std::filesystem::current_path;
using std::filesystem::directory_iterator;

unordered_map<GlyphType, Glyph> templateGlyphs = {};
vector<unique_ptr<Glyph>> placedGlyphs = {};

#ifdef KALAWINDOW_SUPPORT_OPENGL
static bool InitializeGlyphs_OpenGL(Window* window);
static unique_ptr<Glyph> PlaceGlyph_OpenGL(Window* window);
static bool RenderGlyphs_OpenGL(Window* window);
static void ClearGlyphs_OpenGL(Window* window);
#elif KALAWINDOW_SUPPORT_VULKAN
static bool InitializeGlyphs_Vulkan(Window* window);
static unique_ptr<Glyph> PlaceGlyph_Vulkan(Window* window);
static bool RenderGlyphs_Vulkan(Window* window);
static void ClearGlyphs_Vulkan(Window* window);
#endif

static GlyphType CharToGlyphEnum(const char& c);

namespace KalaWindow::Graphics
{
    bool GlyphSystem::Initialize(Window* window)
    {
        int foundCount = 0;
        unsigned int charSize = templateGlyphs.size();
        path texDir = path(current_path() / "textures");
        for (const auto& file : directory_iterator(texDir))
        {
            if (!is_regular_file(file)) continue;
            
            const path filePath = file.path();
            if (filePath.extension() != ".png") continue;

            //normalize to relative path
            string relPath = "/" + filePath.lexically_relative(current_path()).generic_string();
            
            for (const auto& [key, value] : templateGlyphs)
            {
                const string& expectedPath = value.glyphPath;
                if (relPath == expectedPath)
                {
                    ++foundCount;
                    break;
                }
            }
        }
        if (foundCount < charSize)
        {
            LOG_ERROR("Cannot initialize glyph renderer because one or more glyph textures are missing!");
            return false;
        }

        bool initSuccess = false;
        string failReason{};
#ifdef KALAWINDOW_SUPPORT_OPENGL
        failReason = "OpenGL";
        initSuccess = InitializeGlyphs_OpenGL(window);
#elif KALAWINDOW_SUPPORT_VULKAN
        failReason = "Vulkan";
        initSuccess = InitializeGlyphs_Vulkan(window);
#endif
        if (!initSuccess)
        {
            LOG_ERROR("Failed to initialize glyph renderer because of " << failReason << " error!");
            return false;
        }

        LOG_SUCCESS("Initialized glyph renderer!");
        isInitialized = true;
        return true;
    }

    vector<unique_ptr<Glyph>> GlyphSystem::PlaceText(
        kvec2 pos,
        const string& text,
        Window* window)
    {
        vector<unique_ptr<Glyph>> validGlyphs{};

        if (!isInitialized)
        {
            LOG_ERROR("Cannot place any glyphs because glyph renderer is not initialized!");
            return validGlyphs;
        }

        for (const char c : text)
        {
            GlyphType type = CharToGlyphEnum(c);

            if (templateGlyphs.find(type) == templateGlyphs.end())
            {
                LOG_ERROR("Did not find char '" << c << "' in template glyphs!");
                continue;
            }

            const Glyph& glyphTemplate = templateGlyphs[type];

            unique_ptr<Glyph> newGlyph{};
            string failReason{};
#ifdef KALAWINDOW_SUPPORT_OPENGL
            failReason = "OpenGL";
            newGlyph = PlaceGlyph_OpenGL(window);
#elif KALAWINDOW_SUPPORT_VULKAN
            failReason = "Vulkan";
            newGlyph = PlaceGlyph_Vulkan(window);
#endif

            if (newGlyph == nullptr)
            {
                LOG_ERROR("Failed to place glyph because of " << failReason << " error!");
                continue;
            }

            validGlyphs.push_back(move(newGlyph));
        }

        return validGlyphs;
    }

    unique_ptr<Glyph> GlyphSystem::PlaceImage(
        Glyph glyphData,
        Window* window)
    {
        unique_ptr<Glyph> newGlyph{};

        if (!isInitialized)
        {
            LOG_ERROR("Cannot place any glyphs because glyph renderer is not initialized!");
            return nullptr;
        }

        bool placeSuccess = false;
        string failReason{};
#ifdef KALAWINDOW_SUPPORT_OPENGL
        failReason = "OpenGL";
        newGlyph = PlaceGlyph_OpenGL(window);
#elif KALAWINDOW_SUPPORT_VULKAN
        failReason = "Vulkan";
        newGlyph = PlaceGlyph_Vulkan(window);
#endif

        if (newGlyph == nullptr)
        {
            LOG_ERROR("Failed to place glyph because of " << failReason << " error!");
            return nullptr;
        }

        return newGlyph;
    }

    bool GlyphSystem::RenderGlyphs(Window* window)
    {
        if (!isInitialized)
        {
            LOG_ERROR("Cannot render any glyphs because glyph renderer is not initialized!");
            return false;
        }

        bool renderSuccess = false;
        string failReason{};
#ifdef KALAWINDOW_SUPPORT_OPENGL
        failReason = "OpenGL";
        renderSuccess = RenderGlyphs_OpenGL(window);
#elif KALAWINDOW_SUPPORT_VULKAN
        failReason = "Vulkan";
        renderSuccess = RenderGlyphs_Vulkan(window);
#endif

        if (!renderSuccess)
        {
            LOG_ERROR("Failed to render glyph because of " << failReason << " error!");
            return false;
        }
        return true;
    }

    void GlyphSystem::ClearGlyphs(Window* window)
    {
#ifdef KALAWINDOW_SUPPORT_OPENGL
        ClearGlyphs_OpenGL(window);
#elif KALAWINDOW_SUPPORT_VULKAN
        ClearGlyphs_Vulkan(window);
#endif
        placedGlyphs.clear();

        LOG_SUCCESS("All glyphs were cleared!");
    }
}

#ifdef KALAWINDOW_SUPPORT_OPENGL
bool InitializeGlyphs_OpenGL(Window* window)
{
    return true;
}

unique_ptr<Glyph> PlaceGlyph_OpenGL(Window* window)
{
    return nullptr;
}

bool RenderGlyphs_OpenGL(Window* window)
{
    return true;
}

void ClearGlyphs_OpenGL(Window* window)
{

}
#elif KALAWINDOW_SUPPORT_VULKAN
bool InitializeGlyphs_Vulkan(Window* window)
{
    return true;
}

unique_ptr<Glyph> PlaceGlyph_Vulkan(Window* window)
{
    return nullptr;
}

bool RenderGlyphs_Vulkan(Window* window)
{
    return true;
}

void ClearGlyphs_Vulkan(Window* window)
{

}
#endif //KALAWINDOW_SUPPORT_VULKAN

GlyphType CharToGlyphEnum(const char& c)
{
    return GlyphType::CHAR_NONE;
}