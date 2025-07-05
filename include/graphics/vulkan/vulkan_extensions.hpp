//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef KALAWINDOW_SUPPORT_VULKAN

#pragma once

#include <unordered_map>

namespace KalaWindow::Graphics
{
	using std::unordered_map;

	enum class VulkanInstanceExtensions
	{
		// Debugging & Tooling

		VI_debug_report,
		VI_validation_flags,

		// Device & Properties


		// External & Synchronization

		VI_win32_surface,

		// Miscellaneous

		VI_portability_enumeration,

		// Surface & Windowing

		VI_acquire_xlib_display,
		VI_android_surface,
		VI_surface_maintenance1,
		VI_surface_protected_capabilities,
		VI_xcb_surface,
		VI_xlib_surface
	};

	static const unordered_map<VulkanInstanceExtensions, const char*> vulkanInstanceExtensionsInfo =
	{
		// Debugging & Tooling

		{ VulkanInstanceExtensions::VI_debug_report,              "VK_EXT_debug_report" },
		{ VulkanInstanceExtensions::VI_validation_flags,          "VK_EXT_validation_flags" },

		// Device & Properties


		// External & Synchronization

		{ VulkanInstanceExtensions::VI_win32_surface,                   "VK_KHR_win32_surface" },

		// Miscellaneous

		{ VulkanInstanceExtensions::VI_portability_enumeration,         "VK_KHR_portability_enumeration" },

		// Surface & Windowing

		{ VulkanInstanceExtensions::VI_acquire_xlib_display,            "VK_EXT_acquire_xlib_display" },
		{ VulkanInstanceExtensions::VI_android_surface,                 "VK_KHR_android_surface" },
		{ VulkanInstanceExtensions::VI_surface_maintenance1,            "VK_EXT_surface_maintenance1" },
		{ VulkanInstanceExtensions::VI_surface_protected_capabilities,  "VK_KHR_surface_protected_capabilities" },
		{ VulkanInstanceExtensions::VI_xcb_surface,                     "VK_KHR_xcb_surface" },
		{ VulkanInstanceExtensions::VI_xlib_surface,                    "VK_KHR_xlib_surface" }
	};

	enum class VulkanDeviceExtensions
	{
		// Debugging & Tooling
		
		VE_debug_marker,
		VE_tooling_info,
		VE_validation_cache,

		// Device & Properties
		
		VE_device_address_binding_report,
		VE_device_fault,
		VE_device_generated_commands,
		VE_maintenance4,
		VE_maintenance5,
		VE_maintenance6,
		VE_maintenance7,
		VE_maintenance8,
		VE_maintenance9,

		// External & Synchronization
		
		VE_device_memory_report,
		VE_external_fence_win32,
		VE_external_memory_acquire_unmodified,
		VE_external_memory_win32,
		VE_external_semaphore_win32,
		VE_map_memory2,
		VE_map_memory_placed,
		VE_pageable_device_local_memory,
		VE_win32_keyed_mutex,
		VE_workgroup_memory_explicit_layout,
		VE_zero_initialize_device_memory,
		VE_zero_initialize_workgroup_memory,

		// Memory & Layout
		
		VE_attachment_feedback_loop_layout,
		VE_unified_image_layouts,

		// Presentation & Swapchain
		
		VE_full_screen_exclusive,
		VE_present_id,
		VE_present_id2,
		VE_present_mode_fifo_latest_ready,
		VE_present_wait,
		VE_present_wait2,
		VE_swapchain_maintenance1,

		// Format & Compression
		
		VE_format_feature_flags2,
		VE_image_compression_control,
		VE_image_compression_control_swapchain,
		VE_rgba10x6_formats,

		// Portability & Compatibility
		
		VE_cooperative_matrix,
		VE_legacy_dithering,
		VE_legacy_vertex_attributes,
		VE_non_seamless_cube_map,
		VE_portability_subset,

		// Memory & Resource Management
		
		VE_buffer_device_address,
		VE_deferred_host_operations,
		VE_host_image_copy,
		VE_inline_uniform_block,
		VE_load_store_op_none,
		VE_memory_priority,
		VE_private_data,

		// Features & Capabilities
		
		VE_attachment_feedback_loop_dynamic_state,
		VE_extended_dynamic_state3,
		VE_frame_boundary,
		VE_global_priority,
		VE_global_priority_query,
		VE_primitives_generated_query,
		VE_rasterization_order_attachment_access,
		VE_subpass_merge_feedback,
		VE_vertex_attribute_robustness,
		VE_robustness2,

		// Visual, Sampling & Image Ops
		
		VE_border_color_swizzle,
		VE_depth_bias_control,
		VE_depth_clamp_control,
		VE_depth_clamp_zero_one,
		VE_depth_clip_control,
		VE_fragment_density_map,
		VE_fragment_density_map2,
		VE_fragment_density_map_offset,
		VE_image_2d_view_of_3d,
		VE_image_sliced_view_of_3d,
		VE_image_view_min_lod,
		VE_mesh_shader,
		VE_multisampled_render_to_single_sampled,
		VE_nested_command_buffer,
		VE_opacity_micromap,
		VE_provoking_vertex,

		// Pipeline & Descriptors
		
		VE_descriptor_buffer,
		VE_graphics_pipeline_library,
		VE_mutable_descriptor_type,
		VE_pipeline_binary,
		VE_pipeline_creation_cache_control,
		VE_pipeline_creation_feedback,
		VE_pipeline_executable_properties,
		VE_pipeline_library,
		VE_pipeline_library_group_handles,
		VE_pipeline_properties,
		VE_pipeline_protected_access,
		VE_pipeline_robustness,
		VE_push_descriptor,

		// Ray Tracing
		
		VE_acceleration_structure,
		VE_ray_query,
		VE_ray_tracing_maintenance1,
		VE_ray_tracing_pipeline,
		VE_ray_tracing_position_fetch,

		// Rendering
		
		VE_conditional_rendering,
		VE_dynamic_rendering,
		VE_dynamic_rendering_local_read,
		VE_dynamic_rendering_unused_attachments,

		// Shaders
		
		VE_compute_shader_derivatives,
		VE_fragment_shader_barycentric,
		VE_shader_atomic_float2,
		VE_shader_bfloat16,
		VE_shader_expect_assume,
		VE_shader_float_controls2,
		VE_shader_float8,
		VE_shader_integer_dot_product,
		VE_shader_maximal_reconvergence,
		VE_shader_module_identifier,
		VE_shader_non_semantic_info,
		VE_shader_object,
		VE_shader_quad_control,
		VE_shader_replicated_composites,
		VE_shader_relaxed_extended_instruction,
		VE_shader_subgroup_ballot,
		VE_shader_subgroup_rotate,
		VE_shader_subgroup_uniform_control_flow,
		VE_shader_subgroup_vote,
		VE_shader_tile_image,

		// Synchronization
		
		VE_calibrated_timestamps,

		// Vertex & Drawing
		
		VE_multi_draw,
		VE_primitive_topology_list_restart,
		VE_transform_feedback,
		VE_vertex_attribute_divisor,

		// Video
		
		VE_video_decode_av1,
		VE_video_decode_h264,
		VE_video_decode_h265,
		VE_video_decode_queue,
		VE_video_encode_av1,
		VE_video_encode_h264,
		VE_video_encode_h265,
		VE_video_encode_quantization_map,
		VE_video_encode_queue,
		VE_video_maintenance1,
		VE_video_maintenance2,
		VE_video_queue,
		VE_video_decode_vp9
	};

	static const unordered_map<VulkanDeviceExtensions, const char*> vulkanDeviceExtensionsInfo =
	{
		// Debugging & Tooling
		
		{ VulkanDeviceExtensions::VE_debug_marker,                  "VK_EXT_debug_marker" },
		{ VulkanDeviceExtensions::VE_tooling_info,                  "VK_EXT_tooling_info" },
		{ VulkanDeviceExtensions::VE_validation_cache,              "VK_EXT_validation_cache" },

		// Device & Properties
		
		{ VulkanDeviceExtensions::VE_device_address_binding_report, "VK_EXT_device_address_binding_report" },
		{ VulkanDeviceExtensions::VE_device_fault,                  "VK_EXT_device_fault" },
		{ VulkanDeviceExtensions::VE_device_generated_commands,     "VK_EXT_device_generated_commands" },
		{ VulkanDeviceExtensions::VE_maintenance4,                  "VK_KHR_maintenance4" },
		{ VulkanDeviceExtensions::VE_maintenance5,                  "VK_KHR_maintenance5" },
		{ VulkanDeviceExtensions::VE_maintenance6,                  "VK_KHR_maintenance6" },
		{ VulkanDeviceExtensions::VE_maintenance7,                  "VK_KHR_maintenance7" },
		{ VulkanDeviceExtensions::VE_maintenance8,                  "VK_KHR_maintenance8" },
		{ VulkanDeviceExtensions::VE_maintenance9,                  "VK_KHR_maintenance9" },

		// External & Synchronization
		
		{ VulkanDeviceExtensions::VE_device_memory_report,               "VK_EXT_device_memory_report" },
		{ VulkanDeviceExtensions::VE_external_fence_win32,              "VK_KHR_external_fence_win32" },
		{ VulkanDeviceExtensions::VE_external_memory_acquire_unmodified, "VK_EXT_external_memory_acquire_unmodified" },
		{ VulkanDeviceExtensions::VE_external_memory_win32,             "VK_KHR_external_memory_win32" },
		{ VulkanDeviceExtensions::VE_external_semaphore_win32,          "VK_KHR_external_semaphore_win32" },
		{ VulkanDeviceExtensions::VE_map_memory2,                       "VK_KHR_map_memory2" },
		{ VulkanDeviceExtensions::VE_map_memory_placed,                 "VK_EXT_map_memory_placed" },
		{ VulkanDeviceExtensions::VE_pageable_device_local_memory,      "VK_EXT_pageable_device_local_memory" },
		{ VulkanDeviceExtensions::VE_win32_keyed_mutex,                 "VK_KHR_win32_keyed_mutex" },
		{ VulkanDeviceExtensions::VE_workgroup_memory_explicit_layout,  "VK_KHR_workgroup_memory_explicit_layout" },
		{ VulkanDeviceExtensions::VE_zero_initialize_device_memory,     "VK_EXT_zero_initialize_device_memory" },
		{ VulkanDeviceExtensions::VE_zero_initialize_workgroup_memory,  "VK_KHR_zero_initialize_workgroup_memory" },

		// Memory & Layout
		
		{ VulkanDeviceExtensions::VE_attachment_feedback_loop_layout,   "VK_EXT_attachment_feedback_loop_layout" },
		{ VulkanDeviceExtensions::VE_unified_image_layouts,             "VK_KHR_unified_image_layouts" },

		// Presentation & Swapchain
		
		{ VulkanDeviceExtensions::VE_full_screen_exclusive,             "VK_EXT_full_screen_exclusive" },
		{ VulkanDeviceExtensions::VE_present_id,                        "VK_KHR_present_id" },
		{ VulkanDeviceExtensions::VE_present_id2,                       "VK_KHR_present_id2" },
		{ VulkanDeviceExtensions::VE_present_mode_fifo_latest_ready,    "VK_EXT_present_mode_fifo_latest_ready" },
		{ VulkanDeviceExtensions::VE_present_wait,                      "VK_KHR_present_wait" },
		{ VulkanDeviceExtensions::VE_present_wait2,                     "VK_KHR_present_wait2" },
		{ VulkanDeviceExtensions::VE_swapchain_maintenance1,            "VK_EXT_swapchain_maintenance1" },

		// Format & Compression
		
		{ VulkanDeviceExtensions::VE_format_feature_flags2,             "VK_KHR_format_feature_flags2" },
		{ VulkanDeviceExtensions::VE_image_compression_control,         "VK_EXT_image_compression_control" },
		{ VulkanDeviceExtensions::VE_image_compression_control_swapchain,"VK_EXT_image_compression_control_swapchain" },
		{ VulkanDeviceExtensions::VE_rgba10x6_formats,                  "VK_EXT_rgba10x6_formats" },

		// Portability & Compatibility
		
		{ VulkanDeviceExtensions::VE_cooperative_matrix,                "VK_KHR_cooperative_matrix" },
		{ VulkanDeviceExtensions::VE_legacy_dithering,                  "VK_EXT_legacy_dithering" },
		{ VulkanDeviceExtensions::VE_legacy_vertex_attributes,          "VK_EXT_legacy_vertex_attributes" },
		{ VulkanDeviceExtensions::VE_non_seamless_cube_map,             "VK_EXT_non_seamless_cube_map" },
		{ VulkanDeviceExtensions::VE_portability_subset,                "VK_KHR_portability_subset" },

		// Memory & Resource Management
		
		{ VulkanDeviceExtensions::VE_buffer_device_address,             "VK_EXT_buffer_device_address" },
		{ VulkanDeviceExtensions::VE_deferred_host_operations,          "VK_KHR_deferred_host_operations" },
		{ VulkanDeviceExtensions::VE_host_image_copy,                   "VK_EXT_host_image_copy" },
		{ VulkanDeviceExtensions::VE_inline_uniform_block,              "VK_EXT_inline_uniform_block" },
		{ VulkanDeviceExtensions::VE_load_store_op_none,                "VK_KHR_load_store_op_none" },
		{ VulkanDeviceExtensions::VE_memory_priority,                   "VK_EXT_memory_priority" },
		{ VulkanDeviceExtensions::VE_private_data,                      "VK_EXT_private_data" },

		// Features & Capabilities
		
		{ VulkanDeviceExtensions::VE_attachment_feedback_loop_dynamic_state, "VK_EXT_attachment_feedback_loop_dynamic_state" },
		{ VulkanDeviceExtensions::VE_extended_dynamic_state3,               "VK_EXT_extended_dynamic_state3" },
		{ VulkanDeviceExtensions::VE_frame_boundary,                        "VK_EXT_frame_boundary" },
		{ VulkanDeviceExtensions::VE_global_priority,                       "VK_EXT_global_priority" },
		{ VulkanDeviceExtensions::VE_global_priority_query,                 "VK_EXT_global_priority_query" },
		{ VulkanDeviceExtensions::VE_primitives_generated_query,            "VK_EXT_primitives_generated_query" },
		{ VulkanDeviceExtensions::VE_rasterization_order_attachment_access, "VK_EXT_rasterization_order_attachment_access" },
		{ VulkanDeviceExtensions::VE_subpass_merge_feedback,                "VK_EXT_subpass_merge_feedback" },
		{ VulkanDeviceExtensions::VE_vertex_attribute_robustness,           "VK_EXT_vertex_attribute_robustness" },
		{ VulkanDeviceExtensions::VE_robustness2,                           "VK_KHR_robustness2" },

		// Visual, Sampling & Image Ops
		
		{ VulkanDeviceExtensions::VE_border_color_swizzle,               "VK_EXT_border_color_swizzle" },
		{ VulkanDeviceExtensions::VE_depth_bias_control,                 "VK_EXT_depth_bias_control" },
		{ VulkanDeviceExtensions::VE_depth_clamp_control,                "VK_EXT_depth_clamp_control" },
		{ VulkanDeviceExtensions::VE_depth_clamp_zero_one,               "VK_KHR_depth_clamp_zero_one" },
		{ VulkanDeviceExtensions::VE_depth_clip_control,                 "VK_EXT_depth_clip_control" },
		{ VulkanDeviceExtensions::VE_fragment_density_map,               "VK_EXT_fragment_density_map" },
		{ VulkanDeviceExtensions::VE_fragment_density_map2,              "VK_EXT_fragment_density_map2" },
		{ VulkanDeviceExtensions::VE_fragment_density_map_offset,        "VK_EXT_fragment_density_map_offset" },
		{ VulkanDeviceExtensions::VE_image_2d_view_of_3d,                "VK_EXT_image_2d_view_of_3d" },
		{ VulkanDeviceExtensions::VE_image_sliced_view_of_3d,            "VK_EXT_image_sliced_view_of_3d" },
		{ VulkanDeviceExtensions::VE_image_view_min_lod,                 "VK_EXT_image_view_min_lod" },
		{ VulkanDeviceExtensions::VE_mesh_shader,                        "VK_EXT_mesh_shader" },
		{ VulkanDeviceExtensions::VE_multisampled_render_to_single_sampled, "VK_EXT_multisampled_render_to_single_sampled" },
		{ VulkanDeviceExtensions::VE_nested_command_buffer,              "VK_EXT_nested_command_buffer" },
		{ VulkanDeviceExtensions::VE_opacity_micromap,                   "VK_EXT_opacity_micromap" },
		{ VulkanDeviceExtensions::VE_provoking_vertex,                   "VK_EXT_provoking_vertex" },

		// Pipeline & Descriptors
		
		{ VulkanDeviceExtensions::VE_descriptor_buffer,               "VK_EXT_descriptor_buffer" },
		{ VulkanDeviceExtensions::VE_graphics_pipeline_library,       "VK_EXT_graphics_pipeline_library" },
		{ VulkanDeviceExtensions::VE_mutable_descriptor_type,         "VK_EXT_mutable_descriptor_type" },
		{ VulkanDeviceExtensions::VE_pipeline_binary,                 "VK_KHR_pipeline_binary" },
		{ VulkanDeviceExtensions::VE_pipeline_creation_cache_control, "VK_EXT_pipeline_creation_cache_control" },
		{ VulkanDeviceExtensions::VE_pipeline_creation_feedback,      "VK_EXT_pipeline_creation_feedback" },
		{ VulkanDeviceExtensions::VE_pipeline_executable_properties,  "VK_KHR_pipeline_executable_properties" },
		{ VulkanDeviceExtensions::VE_pipeline_library,                "VK_KHR_pipeline_library" },
		{ VulkanDeviceExtensions::VE_pipeline_library_group_handles,  "VK_EXT_pipeline_library_group_handles" },
		{ VulkanDeviceExtensions::VE_pipeline_properties,             "VK_EXT_pipeline_properties" },
		{ VulkanDeviceExtensions::VE_pipeline_protected_access,       "VK_EXT_pipeline_protected_access" },
		{ VulkanDeviceExtensions::VE_pipeline_robustness,             "VK_EXT_pipeline_robustness" },
		{ VulkanDeviceExtensions::VE_push_descriptor,                 "VK_KHR_push_descriptor" },

		// Ray Tracing
		
		{ VulkanDeviceExtensions::VE_acceleration_structure,          "VK_KHR_acceleration_structure" },
		{ VulkanDeviceExtensions::VE_ray_query,                       "VK_KHR_ray_query" },
		{ VulkanDeviceExtensions::VE_ray_tracing_maintenance1,        "VK_KHR_ray_tracing_maintenance1" },
		{ VulkanDeviceExtensions::VE_ray_tracing_pipeline,            "VK_KHR_ray_tracing_pipeline" },
		{ VulkanDeviceExtensions::VE_ray_tracing_position_fetch,      "VK_KHR_ray_tracing_position_fetch" },

		// Rendering
		
		{ VulkanDeviceExtensions::VE_conditional_rendering,           "VK_EXT_conditional_rendering" },
		{ VulkanDeviceExtensions::VE_dynamic_rendering,               "VK_KHR_dynamic_rendering" },
		{ VulkanDeviceExtensions::VE_dynamic_rendering_local_read,    "VK_KHR_dynamic_rendering_local_read" },
		{ VulkanDeviceExtensions::VE_dynamic_rendering_unused_attachments, "VK_EXT_dynamic_rendering_unused_attachments" },

		// Shaders
		
		{ VulkanDeviceExtensions::VE_compute_shader_derivatives,          "VK_KHR_compute_shader_derivatives" },
		{ VulkanDeviceExtensions::VE_fragment_shader_barycentric,         "VK_KHR_fragment_shader_barycentric" },
		{ VulkanDeviceExtensions::VE_shader_atomic_float2,                "VK_EXT_shader_atomic_float2" },
		{ VulkanDeviceExtensions::VE_shader_bfloat16,                     "VK_KHR_shader_bfloat16" },
		{ VulkanDeviceExtensions::VE_shader_expect_assume,                "VK_KHR_shader_expect_assume" },
		{ VulkanDeviceExtensions::VE_shader_float_controls2,              "VK_KHR_shader_float_controls2" },
		{ VulkanDeviceExtensions::VE_shader_float8,                       "VK_EXT_shader_float8" },
		{ VulkanDeviceExtensions::VE_shader_integer_dot_product,          "VK_KHR_shader_integer_dot_product" },
		{ VulkanDeviceExtensions::VE_shader_maximal_reconvergence,        "VK_KHR_shader_maximal_reconvergence" },
		{ VulkanDeviceExtensions::VE_shader_module_identifier,            "VK_EXT_shader_module_identifier" },
		{ VulkanDeviceExtensions::VE_shader_non_semantic_info,            "VK_KHR_shader_non_semantic_info" },
		{ VulkanDeviceExtensions::VE_shader_object,                       "VK_EXT_shader_object" },
		{ VulkanDeviceExtensions::VE_shader_quad_control,                 "VK_KHR_shader_quad_control" },
		{ VulkanDeviceExtensions::VE_shader_replicated_composites,        "VK_EXT_shader_replicated_composites" },
		{ VulkanDeviceExtensions::VE_shader_relaxed_extended_instruction, "VK_KHR_shader_relaxed_extended_instruction" },
		{ VulkanDeviceExtensions::VE_shader_subgroup_ballot,              "VK_EXT_shader_subgroup_ballot" },
		{ VulkanDeviceExtensions::VE_shader_subgroup_rotate,              "VK_KHR_shader_subgroup_rotate" },
		{ VulkanDeviceExtensions::VE_shader_subgroup_uniform_control_flow,"VK_KHR_shader_subgroup_uniform_control_flow" },
		{ VulkanDeviceExtensions::VE_shader_subgroup_vote,                "VK_EXT_shader_subgroup_vote" },
		{ VulkanDeviceExtensions::VE_shader_tile_image,                   "VK_EXT_shader_tile_image" },

		// Synchronization
		
		{ VulkanDeviceExtensions::VE_calibrated_timestamps,           "VK_EXT_calibrated_timestamps" },

		// Vertex & Drawing
		
		{ VulkanDeviceExtensions::VE_multi_draw,                      "VK_EXT_multi_draw" },
		{ VulkanDeviceExtensions::VE_primitive_topology_list_restart, "VK_EXT_primitive_topology_list_restart" },
		{ VulkanDeviceExtensions::VE_transform_feedback,              "VK_EXT_transform_feedback" },
		{ VulkanDeviceExtensions::VE_vertex_attribute_divisor,        "VK_EXT_vertex_attribute_divisor" },

		// Video
		
		{ VulkanDeviceExtensions::VE_video_decode_av1,             "VK_KHR_video_decode_av1" },
		{ VulkanDeviceExtensions::VE_video_decode_h264,            "VK_KHR_video_decode_h264" },
		{ VulkanDeviceExtensions::VE_video_decode_h265,            "VK_KHR_video_decode_h265" },
		{ VulkanDeviceExtensions::VE_video_decode_queue,           "VK_KHR_video_decode_queue" },
		{ VulkanDeviceExtensions::VE_video_decode_vp9,             "VK_KHR_video_decode_vp9" },
		{ VulkanDeviceExtensions::VE_video_encode_av1,             "VK_KHR_video_encode_av1" },
		{ VulkanDeviceExtensions::VE_video_encode_h264,            "VK_KHR_video_encode_h264" },
		{ VulkanDeviceExtensions::VE_video_encode_h265,            "VK_KHR_video_encode_h265" },
		{ VulkanDeviceExtensions::VE_video_encode_quantization_map,"VK_KHR_video_encode_quantization_map" },
		{ VulkanDeviceExtensions::VE_video_encode_queue,           "VK_KHR_video_encode_queue" },
		{ VulkanDeviceExtensions::VE_video_maintenance1,           "VK_KHR_video_maintenance1" },
		{ VulkanDeviceExtensions::VE_video_maintenance2,           "VK_KHR_video_maintenance2" },
		{ VulkanDeviceExtensions::VE_video_queue,                  "VK_KHR_video_queue" }
	};
}

#endif //KALAWINDOW_SUPPORT_VULKAN