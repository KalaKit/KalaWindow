//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef KALAKIT_WAYLAND

#pragma once

#include <chrono>
#include <fstream>
#include <poll.h>
#include <thread>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/eventfd.h>
#include <unistd.h>
#include <stdio.h>

//external
#include "xdg-shell-client-protocol.h"

//kalawindow
#include "window.hpp"

namespace KalaKit
{
	using std::chrono::steady_clock;
	using std::chrono::microseconds;
	using std::chrono::duration_cast;
	using std::to_string;
	using std::ostringstream;
	using std::hex;
	using std::uppercase;
	using std::setw;
	using std::setfill;
	using std::abs;
	using std::this_thread::sleep_for;

	struct SHMBuffer
	{
		void* pixels = nullptr;
		int width = 0;
		int height = 0;
		int stride = 0;
		int size = 0;
		wl_buffer* buffer = nullptr;
		bool busy = false;
	};
	struct ButtonRect
	{
		int x;
		int y;
		int width;
		int height;
	};

    class Window_Wayland
    {
    public:
		static inline wl_surface* newSurface = nullptr;
		static inline wl_display* newDisplay = nullptr;

        static wl_display* CreateDisplay()
        {
            newDisplay = wl_display_connect(nullptr);
            if (!newDisplay) return nullptr;

            return newDisplay;
        }

        static void SetupRegistry()
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

						static const xdg_wm_base_listener WmBaseListener =
						{
							.ping = [](
								void* data,
								xdg_wm_base* base,
								uint32_t serial)
							{
								if (KalaWindow::GetDebugType() == DebugType::DEBUG_ALL
									|| KalaWindow::GetDebugType() == DebugType::DEBUG_WAYLAND_CALLBACK_CHECK)
								{
									LOG_DEBUG("Received xdg_wm_base ping, sending pong!");
								}
								xdg_wm_base_pong(base, serial);
							}
						};
						xdg_wm_base_add_listener(xdgWmBase, &WmBaseListener, nullptr);
				    }
					/*
					else if (strcmp(interface, "wp_linux_drm_syncobj_manager_v1") == 0
							 && OpenGL::isInitialized)
					{
						LOG_DEBUG("Skipping syncobj manager");
					}
					*/
					
                },
                .global_remove = [](void*, struct wl_registry*, uint32_t) {}
            };

            wl_registry_add_listener(registry, &registryListener, nullptr);
            wl_display_roundtrip(newDisplay);
        }
        static wl_compositor* GetCompositor() { return compositor; }
        static wl_shm* GetSHM() { return shm; }
        static xdg_wm_base* GetWMBase() { return xdgWmBase; }

		static bool CreateSHMBuffers(
			const string& title,
			int width,
			int height)
		{
			if (!OpenGL::isInitialized)
			{
				for (int i = 0; i < 2; ++i)
				{
					int stride = width * 4;
					int size = stride * height;
			
					//create an in-memory file
					char shm_path[] = "/dev/shm/wayland-shmXXXXXX";
					int fd = mkstemp(shm_path);
					if (fd < 0)
					{
						LOG_ERROR("mkstemp failed for SHM buffer!");
						return false;
					}
					shm_unlink(shm_path);
			
					if (ftruncate(fd, size) < 0)
					{
						LOG_ERROR("FTruncate failed on SHM buffer.");
						close(fd);
						return false;
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
						LOG_ERROR("Failed to mmap SHM file for SHM buffer.");
						close(fd);
						return false;
					}
			
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
					close(fd); // can safely close after pool creation
			
					//store buffer in the double buffer array
					uint32_t* pixels = static_cast<uint32_t*>(data);
					shmBuffers[i] =
					{
						.pixels = pixels,
						.width = width,
						.height = height,
						.stride = stride,
						.size = size,
						.buffer = buffer,
						.busy = false
					};
	
					wl_buffer_add_listener(buffer, &BufferReleaseListener, &shmBuffers[i]);
	
					DrawWindowContent(
						title,
						shmBuffers[i],
						width,
						height);
				}
			}
		
			return true;
		}
		static inline const wl_buffer_listener BufferReleaseListener =
		{
			.release = [](void* data, wl_buffer* buffer)
			{
				auto* buf  = reinterpret_cast<SHMBuffer*>(data);
				buf->busy = false;

				if (KalaWindow::GetDebugType() == DebugType::DEBUG_ALL
					|| KalaWindow::GetDebugType() == DebugType::DEBUG_WAYLAND_CALLBACK_CHECK)
				{
					LOG_DEBUG("Buffer released back by compositor.");
				}
			}
		};

        static wl_surface* CreateSurface()
        {
            newSurface = wl_compositor_create_surface(compositor);
            if (newSurface == nullptr) return nullptr;

			return newSurface;
        }

        static void AddDecorations(const string& title)
        {
            struct xdg_surface* xdgSurface = xdg_wm_base_get_xdg_surface(xdgWmBase, newSurface);
		    struct xdg_toplevel* xdgTopLevel = xdg_surface_get_toplevel(xdgSurface);

		    //set window title and app id
		    xdg_toplevel_set_title(xdgTopLevel, title.c_str());
		    xdg_toplevel_set_app_id(xdgTopLevel, title.c_str());

		    //set window min and max size
		    xdg_toplevel_set_min_size(xdgTopLevel, 800, 632);   //600 + 32
		    xdg_toplevel_set_max_size(xdgTopLevel, 7860, 4352); //4320 + 32

		    //add a listener to handle configure events
		    static const struct xdg_surface_listener surface_listener =
		    {
			    .configure = [](
				    void* data,
				    struct xdg_surface* surface,
				    uint32_t serial)
			    {
					
					if (KalaWindow::GetDebugType() == DebugType::DEBUG_ALL
						|| KalaWindow::GetDebugType() == DebugType::DEBUG_WAYLAND_DISPLAY_CHECK)
					{
						LOG_DEBUG("Received xdg_surface configure event! Serial: " + to_string(serial));
					}
				    xdg_surface_ack_configure(surface, serial);

					wl_surface* targetSurface = static_cast<wl_surface*>(data);

    				Window_Wayland::currentBufferIndex = 0;
    				SHMBuffer& initialBuffer = Window_Wayland::shmBuffers[0];

    				wl_surface_attach(targetSurface, initialBuffer.buffer, 0, 0);
    				wl_surface_damage(
						targetSurface, 
						0, 
						0, 
						initialBuffer.width, 
						initialBuffer.height);
    				wl_surface_damage_buffer(targetSurface, 
						0, 
						0, 
						initialBuffer.width, 
						initialBuffer.height);
    				wl_surface_commit(targetSurface);
    				initialBuffer.busy = true;

					wl_display_flush(newDisplay);
			    }
		    };
		    xdg_surface_add_listener(xdgSurface, &surface_listener, newSurface);

			//wait for the initial configure event from the compositor
			wl_display_roundtrip(newDisplay);

		    //commit surface to apply title and role decorations
		    wl_surface_commit(newSurface);
        }

		static void DrawWindowContent(
			const string& title,
			SHMBuffer& buffer,
			int width, 
			int height)
		{
			if (!OpenGL::isInitialized)
			{
				uint32_t* pixels = static_cast<uint32_t*>(buffer.pixels);

				//
				// DRAW TITLE BAR
				//
	
				int titlebarHeight = 32;
			
				//dark gray
				uint32_t color_titleBar = 0xFF202020; 
				//draw title bar (top 32 pixels)
				for (int y = 0; y < titlebarHeight; ++y)
				{
					for (int x = 0; x < width; ++x)
					{
						pixels[y * width + x] = color_titleBar;
					}
				}
	
				//
				// DRAW TITLEBAR BUTTONS
				//
	
				int buttonSize = 24;
				int buttonPadding = 4;
	
				closeButton = 
				{
					width - buttonPadding - buttonSize,
					4,
					buttonSize,
					buttonSize
				};
				maxButton = 
				{
					closeButton.x - buttonPadding - buttonSize,
					4,
					buttonSize,
					buttonSize
				};
				minButton = 
				{
					maxButton.x - buttonPadding - buttonSize,
					4,
					buttonSize,
					buttonSize
				};
	
				DrawButtonRect(buffer, closeButton, 0xFFCC6666);  //red
				DrawButtonRect(buffer, maxButton,   0xFFCCCC66);  //yellow
				DrawButtonRect(buffer, minButton,   0xFF66CC66);  //green
	
				//
				// DRAW SCREEN CONTENT
				//
	
				//light green
				uint32_t color_window = 0xFF66CC99; 
				//draw window content  
				for (int y = titlebarHeight; y < height; ++y)
				{
					for (int x = 0; x < width; ++x)
					{
						pixels[y * width + x] = color_window;
					}
				}
			}
		}
		static void DrawButtonRect(
			SHMBuffer& buffer,
			const ButtonRect& rect,
			uint32_t color)
		{
			uint32_t* pixels = static_cast<uint32_t*>(buffer.pixels);
			for (int y = 0; y < rect.height; ++y)
			{
				for (int x = 0; x < rect.width; ++x)
				{
					int px = rect.x + x;
					int py = rect.y + y;
					if (px < buffer.width
						&& py < buffer.height)
					{
						pixels[py * buffer.width + px] = color;
					}
				}
			}
		}
		
		//Enables drawing only if a buffer is available
		static bool TryRender()
		{
			if (!OpenGL::isInitialized)
			{
				for (int i = 0; i < 2; ++i)
				{
					int index = (currentBufferIndex + 1 + i) % 2;
					if (!shmBuffers[index].busy)
					{
						currentBufferIndex = index;
						SHMBuffer& buf = shmBuffers[index];
	
						buf.busy = true;
	
						wl_surface_attach(newSurface, buf.buffer, 0, 0);
						wl_surface_damage(
							newSurface, 
							0, 
							0, 
							buf.width, 
							buf.height);
						wl_surface_damage_buffer(
							newSurface, 
							0, 
							0, 
							buf.width, 
							buf.height);
						wl_surface_commit(newSurface);
						wl_display_flush(newDisplay);
	
						return true;
					}
				}
	
				if (KalaWindow::GetDebugType() == DebugType::DEBUG_ALL
					|| KalaWindow::GetDebugType() == DebugType::DEBUG_WAYLAND_DISPLAY_CHECK)
				{
					LOG_DEBUG("TryRender returned false: no free buffer (both busy)");
				}
				return false;
			}
			return true;
		}
    
		static void WaylandPoll()
		{
			int fd = wl_display_get_fd(newDisplay);
			struct pollfd pfd = 
			{ 
				fd, 
				POLLIN, 
				0 
			};

			//wait up to 5ms for incoming events
			int pollResult = poll(&pfd, 1, 5);
			if (pollResult > 0
				&& (pfd.revents & POLLIN))
			{
				int readStatus = wl_display_read_events(newDisplay);
				if (KalaWindow::GetDebugType() == DebugType::DEBUG_ALL
					|| KalaWindow::GetDebugType() == DebugType::DEBUG_WAYLAND_DISPLAY_CHECK)
				{
					LOG_DEBUG("Polled and read Wayland events, status: '" + to_string(readStatus) + "'");
				}
				if (readStatus == -1)
				{
					LOG_ERROR("Failed to read display events! Shutting down...");
					KalaWindow::SetShouldCloseState(true);
					return;
				}
			}
			else
			{
				//no events ready, cancel read to avoid getting stuck
				wl_display_cancel_read(newDisplay);

				if (KalaWindow::GetDebugType() == DebugType::DEBUG_ALL
					|| KalaWindow::GetDebugType() == DebugType::DEBUG_WAYLAND_DISPLAY_CHECK)
				{
					LOG_DEBUG("No events ready, canceled Wayland read.");
				}
			}
		}

		//Enables drawing only 60 frames per second 
		static void Pause()
		{
			static auto lastDrawTime = steady_clock::now();
			auto now = steady_clock::now();
			auto elapsed = duration_cast<microseconds>(now - lastDrawTime);
				
			//target: 60 FPS = 16.667 ms = 16,667 us
			constexpr int64_t targetDelay = 16667;
				
			if (elapsed.count() < targetDelay)
			{
				auto sleepDuration = microseconds(targetDelay - elapsed.count());
				sleep_for(sleepDuration);
			}
				
			now = steady_clock::now();
			lastDrawTime = now;
		}
	private:
        static inline struct wl_compositor* compositor;
        static inline struct wl_shm* shm;
        static inline struct xdg_wm_base* xdgWmBase;
		static inline SHMBuffer shmBuffers[2];
		static inline int currentBufferIndex = 0;

		static inline ButtonRect minButton;
		static inline ButtonRect maxButton;
		static inline ButtonRect closeButton;
    };
}

#endif // KALAKIT_WAYLAND