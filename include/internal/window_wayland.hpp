//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef KALAKIT_WAYLAND

#define KALAKIT_MODULE "WINDOW"

#include <chrono>

#include "window.hpp"
namespace KalaKit
{
	using std::chrono::steady_clock;
	using std::chrono::milliseconds;
	using std::to_string;

	struct SHMBuffer
	{
		void* pixels = nullptr;
		int width = 0;
		int height = 0;
		int stride = 0;
		int size = 0;
		wl_buffer* buffer = nullptr;
	};

    class Window_Wayland
    {
    public:
        static wl_display* CreateDisplay()
        {
            struct wl_display* newDisplay = wl_display_connect(nullptr);
            if (!newDisplay) return nullptr;
            
            uintptr_t rawDisplay = reinterpret_cast<uintptr_t>(newDisplay);
            KalaWindow::waylandDisplay = display_way(rawDisplay);

            return newDisplay;
        }

        static void SetupRegistry(wl_display* newDisplay)
        {
            struct wl_registry* registry = wl_display_get_registry(newDisplay);
            if (!registry)
            {
                LOG_ERROR("Failed to get Wayland registry!");
                wl_display_disconnect(newDisplay);
                return;
            }

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
                            version
                        );
                    }
				    else if (strcmp(interface, "wl_shm") == 0)
				    {
					    shm = static_cast<wl_shm*>(
						    wl_registry_bind(
							    registry,
							    name,
							    &wl_shm_interface,
							    version
						    )
					    );
				    }
				    else if (strcmp(interface, "xdg_wm_base") == 0)
				    {
					    xdgWmBase = static_cast<xdg_wm_base*>(
						    wl_registry_bind(
							    registry,
							    name,
							    &xdg_wm_base_interface,
							    version
						    )
					    );
				    }
                },
                .global_remove = [](void*, struct wl_registry*, uint32_t) {}
            };

            wl_registry_add_listener(registry, &registryListener, nullptr);
            wl_display_roundtrip(newDisplay);
        }
        static wl_compositor* GetCompositor() { return compositor; }
        static wl_shm* GetSHM() { return shm; }
        static xdg_wm_base* GetWMBase() { return xdgWmBase; }

        static wl_surface* CreateSurface()
        {
            wl_surface* newSurface = wl_compositor_create_surface(compositor);
            if (newSurface == nullptr) return nullptr;
            
		    uintptr_t rawSurface = reinterpret_cast<uintptr_t>(newSurface);
		    KalaWindow::waylandSurface = surface_way(rawSurface);

			return newSurface;
        }

        static void AddDecorations(const string& title, wl_display* newDisplay, wl_surface* newSurface)
        {
            struct xdg_surface* xdgSurface = xdg_wm_base_get_xdg_surface(xdgWmBase, newSurface);
		    struct xdg_toplevel* xdgTopLevel = xdg_surface_get_toplevel(xdgSurface);

		    //set window title and app id
		    xdg_toplevel_set_title(xdgTopLevel, title.c_str());
		    xdg_toplevel_set_app_id(xdgTopLevel, title.c_str());

		    //set window min and max size
		    xdg_toplevel_set_min_size(xdgTopLevel, 800, 600);
		    xdg_toplevel_set_max_size(xdgTopLevel, 7860, 4320);

		    //add a listener to handle configure events
		    static const struct xdg_surface_listener surface_listener =
		    {
			    .configure = [](
				    void* data,
				    struct xdg_surface* surface,
				    uint32_t serial)
			    {
					LOG_DEBUG("Received xdg_surface configure event! Serial: " + to_string(serial));
				    xdg_surface_ack_configure(surface, serial);
			    }
		    };
		    xdg_surface_add_listener(xdgSurface, &surface_listener, newSurface);

			//wait for the initial configure event from the compositor
			wl_display_roundtrip(newDisplay);

		    //commit surface to apply title and role decorations
		    wl_surface_commit(newSurface);
        }

        static bool CreateBuffer(
            int width, 
            int height, 
            wl_display* newDisplay, 
            wl_surface* newSurface)
        {
            int stride = width * 4;
		    int size = stride * height;
		
		    //create an in-memory file
		    int fd = memfd_create(
			    "wayland-shm", 
			    MFD_CLOEXEC
			    | MFD_ALLOW_SEALING);
		    if (fd >= 0)
		    {
			    if (ftruncate(fd, size) < 0)
			    {
				    LOG_ERROR("FTruncate failed!");
				    close(fd);
				    wl_display_disconnect(newDisplay);
				    return false;
			    }
		    }

		    void* data = mmap(
			    nullptr,
			    size,
			    PROT_READ
			    | PROT_WRITE,
			    MAP_SHARED,
			    fd,
			    0
		    );
		    if (data == MAP_FAILED)
		    {
			    LOG_ERROR("Failed to mmap shm file!");
			    close(fd);
			    wl_display_disconnect(newDisplay);
			    return false;
		    }

			//
			// DRAW CONTENT START
			//

		    uint32_t* pixels = static_cast<uint32_t*>(data);
			uint32_t green = 0xFF00AA00;
			uint32_t dark_gray = 0xFF202020;

			//fill entire background with green
			for (int i = 0; i < (width * height); ++i)
			{
				pixels[i] = green;
			}

			//draw title bar (top 32 pixels)
			int titlebar_height = 32;
			for (int y = 0; y < titlebar_height; ++y)
			{
				for (int x = 0; x < width; ++x)
				{
					pixels[y * width + x] = dark_gray;
				}
			}

			//
			// DRAW CONTENT END
			//

		    wl_shm_pool* pool = wl_shm_create_pool(shm, fd, size);
		    wl_buffer* buffer = wl_shm_pool_create_buffer(
			    pool,
			    0,
			    width,
			    height,
			    stride,
			    WL_SHM_FORMAT_XRGB8888
		    );
			wl_shm_pool_destroy(pool);

			shmBuffer =
			{
				.pixels = pixels,
				.width = width,
				.height = height,
				.stride = stride,
				.size = size,
				.buffer = buffer
			};

		    //attach the buffer to the surface 
		    wl_surface_attach(newSurface, shmBuffer.buffer, 0, 0);
			wl_surface_damage_buffer(newSurface, 0, 32, shmBuffer.width, shmBuffer.height - 32);
		    
		    //start the frame loop
		    wl_callback* frame_cb = wl_surface_frame(newSurface);
			wl_callback_add_listener(frame_cb, &FrameListener, newSurface);

			//commit after starting frame loop
			wl_surface_commit(newSurface);

			LOG_DEBUG("Initial frame callback listener added.");

			/*
            wl_egl_window* newRenderTarget = wl_egl_window_create(
                newSurface,
                width,
                height
            );
            if (!newRenderTarget)
            {
                LOG_ERROR("Failed to create egl window!");
                wl_display_disconnect(newDisplay);
                return false;
            }
		    uintptr_t rawRenderTarget = reinterpret_cast<uintptr_t>(newRenderTarget);
		    KalaWindow::waylandRenderTarget = target_way(rawRenderTarget);
			*/

			return true;
        }

		static void FrameDone(void* data, wl_callback* callback, uint32_t time)
		{
			wl_surface* surface = reinterpret_cast<wl_surface*>(data);

			//destroy the current frame callback
			wl_callback_destroy(callback);

			//re-attach the existing bufffer
			wl_surface_attach(surface, shmBuffer.buffer, 0, 0);

			FlipPixels();

			//call redraw, may be ignored if content is unchanged
			wl_surface_damage_buffer(surface, 0, 32, shmBuffer.width, shmBuffer.height - 32);

			//request the next frame
			wl_callback* next_frame = wl_surface_frame(surface);
			wl_callback_add_listener(next_frame, &FrameListener, data);

			//re-commit the surface
			wl_surface_commit(surface);

			if (KalaWindow::GetDebugType() == DebugType::DEBUG_ALL
				|| KalaWindow::GetDebugType() == DebugType::DEBUG_WAYLAND_CALLBACK_CHECK)
			{
				LOG_DEBUG("Frame callback received. Re-committing surface...");
			}
		}
		static inline const wl_callback_listener FrameListener = { .done = FrameDone };
    private:
        static inline struct wl_compositor* compositor;
        static inline struct wl_shm* shm;
        static inline struct xdg_wm_base* xdgWmBase;
		static inline SHMBuffer shmBuffer;

		//Flip 50x50 pixels below title bar red and blue
		static void FlipPixels()
		{
			static auto lastFlipTime = steady_clock::now();
			auto now = steady_clock::now();

			//flip every 500ms
			constexpr auto flipInterval = milliseconds(500);

			if (now - lastFlipTime >= flipInterval)
			{
				static bool flip = false;
				lastFlipTime = now;

				uint32_t* pixels = static_cast<uint32_t*>(shmBuffer.pixels);
				uint32_t color = flip ? 0xFFFF0000 : 0xFF0000FF; // RED : BLUE

				for (int y = 32; y < shmBuffer.height; ++y) //skip title bar
				{
					for (int x = 0; x < shmBuffer.width; ++x)
					{
						pixels[y * shmBuffer.width + x] = color;
					}
				}

				flip = !flip;
			}
		}
    };
}

#endif