//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef KALAKIT_WAYLAND

#define KALAKIT_MODULE "WINDOW"

#include <chrono>
#include <fstream>

#include "window.hpp"
namespace KalaKit
{
	using std::chrono::steady_clock;
	using std::chrono::milliseconds;
	using std::to_string;
	using std::ostringstream;
	using std::hex;
	using std::uppercase;
	using std::setw;
	using std::setfill;
	using std::abs;

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

        static bool CreateSHMBuffers(
			int width,
			int height,
			wl_display* newDisplay,
			wl_surface* newSurface)
		{
			int titlebar_height = 32;
		
			for (int i = 0; i < 2; ++i)
			{
				int stride = width * 4;
				int size = stride * height;
		
				//create an in-memory file
				int fd = memfd_create(
					"wayland-shm",
					MFD_CLOEXEC
					| MFD_ALLOW_SEALING);
				if (fd < 0)
				{
					LOG_ERROR("memfd_create failed for SHM buffer.");
					return false;
				}
		
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

				//
				// DRAW CONTENT
				//
		
				uint32_t dark_gray = 0xFF202020;

				//flip both buffers at the start
				FlipPixels(shmBuffers[i], i == 0 ? color_red : color_blue);
		
				//draw title bar (top 32 pixels)
				for (int y = 0; y < titlebar_height; ++y)
				{
					for (int x = 0; x < width; ++x)
					{
						pixels[y * width + x] = dark_gray;
					}
				}
			}
		
			//set up the first frame callback (draw will begin in FrameDone)
			wl_callback* frame_cb = wl_surface_frame(newSurface);
			wl_callback_add_listener(frame_cb, &FrameListener, newSurface);

			//attach and commit the first buffer to trigger the first frame
			wl_surface_attach(newSurface, shmBuffers[0].buffer, 0, 0);
			wl_surface_damage_buffer(
				newSurface, 
				0, 
				32, 
				shmBuffers[0].width, 
				shmBuffers[0].height - 32);
			wl_surface_commit(newSurface);
			shmBuffers[0].busy = true;
	
			LOG_DEBUG("Initial frame callback listener added (double buffer mode).");
		
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
		
		static void FrameDone(void* data, wl_callback* callback, uint32_t time)
		{
			auto now = steady_clock::now();
			LOG_DEBUG("Frame callback fired at: " 
				+ to_string(duration_cast<milliseconds>(now.time_since_epoch()).count()) 
				+ "ms");

			wl_surface* surface = reinterpret_cast<wl_surface*>(data);

			//destroy the current frame callback
			wl_callback_destroy(callback);

			bool shouldFlip = (now - lastFlipTime >= flipInterval);

			//try to find an available buffer
			int nextBuffer = -1;
			for (int i = 0; i < 2; ++i)
			{
				int candidate = (currentBufferIndex + 1 + i) % 2;
				if (!shmBuffers[candidate].busy)
				{
					nextBuffer = candidate;
					break;
				}
			}

			if (nextBuffer == -1)
			{
				if (KalaWindow::GetDebugType() == DebugType::DEBUG_ALL
					|| KalaWindow::GetDebugType() == DebugType::DEBUG_WAYLAND_CALLBACK_CHECK)
				{
					LOG_DEBUG("Both SHM buffers are busy, skipping frame...");
				}
			}
			else
			{
				//commit this buffer
				currentBufferIndex = nextBuffer;
				SHMBuffer& shmBuffer = shmBuffers[currentBufferIndex];

				if (shouldFlip)
				{
					uint32_t color = flip ? color_red : color_blue;
					FlipPixels(shmBuffer, color);

					wl_surface_attach(surface, shmBuffer.buffer, 0, 0);
					wl_surface_damage_buffer(
						surface, 
						0, 
						32, 
						shmBuffer.width, 
						shmBuffer.height - 32);
	
					//mark buffer busy and then commit
					shmBuffer.busy = true;
					wl_surface_commit(surface);

					lastFlipTime = now;
					flip = !flip;
	
					if (KalaWindow::GetDebugType() == DebugType::DEBUG_ALL
						|| KalaWindow::GetDebugType() == DebugType::DEBUG_WAYLAND_CALLBACK_CHECK)
					{
						LOG_DEBUG("Time since last flip: " 
							+ to_string(duration_cast<milliseconds>(now - lastFlipTime).count())
							+ "ms, shouldFlip: " 
							+ to_string(shouldFlip)
							+ ", Using buffer index " 
							+ to_string(currentBufferIndex) 
							+ ", busy=" 
							+ to_string(shmBuffers[currentBufferIndex].busy)
							+ ", color= "
							+ Uint_To_Color(color));
					}

					shouldFlip = false;
				}
			}

			//request the next frame
			wl_callback* next_frame = wl_surface_frame(surface);
			wl_callback_add_listener(next_frame, &FrameListener, data);
		}
		static inline const wl_callback_listener FrameListener = { .done = FrameDone };

		static inline string Uint_To_Color(uint32_t value)
		{
			uint8_t a = (value >> 24) & 0xFF;
			uint8_t r = (value >> 16) & 0xFF;
			uint8_t g = (value >> 8) & 0xFF;
			uint8_t b = (value >> 0) & 0xFF;

			struct NamedColor
    		{
        		const char* name;
        		uint8_t r, g, b;
    		};

			static const NamedColor namedColors[] = {
				// === RGB Primaries ===
				{ "Red",        0xFF, 0x00, 0x00 },
				{ "Green",      0x00, 0xFF, 0x00 },
				{ "Blue",       0x00, 0x00, 0xFF },
			
				// === RGB Secondaries ===
				{ "Yellow",     0xFF, 0xFF, 0x00 },
				{ "Cyan",       0x00, 0xFF, 0xFF },
				{ "Magenta",    0xFF, 0x00, 0xFF },
			
				// === Perceptual + UI colors ===
				{ "Orange",     0xFF, 0xA5, 0x00 },
				{ "Pink",       0xFF, 0xC0, 0xCB },
				{ "Purple",     0x80, 0x00, 0x80 },
				{ "Brown",      0xA5, 0x2A, 0x2A },
				{ "Lime",       0x32, 0xCD, 0x32 },
				{ "Light Blue", 0xAD, 0xD8, 0xE6 },
				{ "Teal",       0x00, 0x80, 0x80 },
			
				// === Grayscale ===
				{ "Black",      0x00, 0x00, 0x00 },
				{ "Gray",       0x80, 0x80, 0x80 },
				{ "White",      0xFF, 0xFF, 0xFF }
			};											

			const NamedColor* closest = nullptr;
    		int smallestTotalDelta = INT_MAX;
    		bool isExact = false;

    		for (const auto& named : namedColors)
    		{
        		int dr = abs(int(r) - int(named.r));
        		int dg = abs(int(g) - int(named.g));
        		int db = abs(int(b) - int(named.b));

        		int totalDelta = dr + dg + db;

        		if (dr <= 10 
					&& dg <= 10 
					&& db <= 10)
        		{
            		closest = &named;
            		isExact = true;
            		break;
       			}

        		if (totalDelta < smallestTotalDelta)
        		{
            		smallestTotalDelta = totalDelta;
            		closest = &named;
        		}
    		}

			ostringstream oss;
			oss << "A: " << (int)a
				<< "R: " << (int)r
				<< "G: " << (int)g
				<< "B: " << (int)b
				<< " (0x" 
				<< hex 
				<< uppercase 
				<< setw(8) 
				<< setfill('0')
				<< value
				<< ")";

				if (!closest 
					|| smallestTotalDelta > 150)
				{
					oss << " [Unknown]";
				}
				else
				{
					oss << " [" << (isExact ? "" : "~") << closest->name << "]";
				}

			return oss.str();
		}
    private:
        static inline struct wl_compositor* compositor;
        static inline struct wl_shm* shm;
        static inline struct xdg_wm_base* xdgWmBase;
		static inline SHMBuffer shmBuffers[2];
		static inline int currentBufferIndex = 0;

		static inline bool flip = false;
		static inline steady_clock::time_point lastFlipTime = steady_clock::now();
		static inline constexpr milliseconds flipInterval = milliseconds(16);
		static inline uint32_t color_red = 0xFFFF0000;
		static inline uint32_t color_blue = 0xFF0000FF;

		//Flip screen pixels below title bar red and blue
		static void FlipPixels(SHMBuffer& shmBuffer, uint32_t color)
		{
			uint32_t* pixels = static_cast<uint32_t*>(shmBuffer.pixels);
			LOG_DEBUG("Flipping pixels with color: " + Uint_To_Color(color));

			for (int y = 32; y < shmBuffer.height; ++y) // skip title bar
			{
				for (int x = 0; x < shmBuffer.width; ++x)
				{
					pixels[y * shmBuffer.width + x] = color;
				}
			}
		}
    };
}

#endif