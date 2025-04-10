//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef KALAKIT_WAYLAND

#define KALAKIT_MODULE "WINDOW"

#include <cstring>
#include <wayland-egl.h>

//external
#include "crashHandler.hpp"

//kalawindow
#include "window.hpp"
#include "input.hpp"
#include "opengl.hpp"
#include "opengl_loader.hpp"

using std::strcmp;

namespace KalaKit
{
    bool KalaWindow::Initialize(
        const string& title,
        int width,
        int height)
    {
        if (isInitialized)
        {
            LOG_ERROR("Window is already initialized!");
            return false;
        }

        //initialize the crash handler first
        KalaCrashHandler::Initialize();

        //basic wayland connection setup
        struct wl_display* display = wl_display_connect(nullptr);
        if (!display)
        {
            LOG_ERROR("Failed to connect to Wayland display!");
            return false;
        }
        window.display = display;

        struct wl_registry* registry = wl_display_get_registry(window.display);
        if (!registry)
        {
            LOG_ERROR("Failed to get Wayland registry!");
            wl_display_disconnect(window.display);
            return false;
        }

        //global listener
        static struct wl_compositor* compositor = nullptr;
        static const struct wl_registry_listener registryListener =
        {
            .global = [](
                void* data, 
                struct wl_registry* registry, 
                uint32_t name,
                const char* interface,
                uint32_t version)
            {
                if (strcmp(interface, "wl_compositor") == 0)
                {
                    //get access to the display server
                    compositor = (wl_compositor*)wl_registry_bind(
                        registry,
                        name,
                        &wl_compositor_interface,
                        1
                    );
                }
            },
            .global_remove = [](void*, struct wl_registry*, uint32_t) {}
        };

        wl_registry_add_listener(registry, &registryListener, nullptr);
        wl_display_roundtrip(window.display);

        if (!compositor)
        {
            LOG_ERROR("Failed to get wl_compositor from Wayland registry!");
            wl_display_disconnect(window.display);
            return false;
        }

        //create the drawable area
        window.handle = wl_compositor_create_surface(compositor);
        if (!window.handle)
        {
            LOG_ERROR("Failed to create Wayland surface!");
            wl_display_disconnect(window.display);
            return false;
        }

        wl_egl_window* target = wl_egl_window_create(
            window.handle,
            width,
            height
        );
        if (!target)
        {
            LOG_ERROR("Failed to create egl window!");
            wl_display_disconnect(window.display);
            return false;
        }
        window.renderTarget = target;

        //initialize input
        KalaInput::Initialize();

        //initialize opengl
        bool openglInitialized = OpenGL::Initialize();
        if (!openglInitialized) return false;

        //and finally set opengl viewport size
        OpenGLLoader::glViewportPtr(0, 0, width, height);

        isInitialized = true;

        LOG_SUCCESS("Window successfully initialized");
        return true;
    }
}

#endif // KALAKIT_WAYLAND