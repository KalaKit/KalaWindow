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

		VK_INS_DEV_debug_report,
		VK_INS_DEV_validation_flags,

		// Device & Properties


		// External & Synchronization

		VK_INS_DEV_win32_surface,

		// Miscellaneous

		VK_INS_DEV_portability_enumeration,

		// Surface & Windowing

		VK_INS_DEV_acquire_xlib_display,
		VK_INS_DEV_surface_maintenance1,
		VK_INS_DEV_surface_protected_capabilities,
		VK_INS_DEV_xcb_surface,
		VK_INS_DEV_xlib_surface
	};

	static const unordered_map<VulkanInstanceExtensions, const char*> vulkanInstanceExtensionsInfo =
	{
		// Debugging & Tooling

		{ VulkanInstanceExtensions::VK_INS_DEV_debug_report,              "VK_EXT_debug_report" },
		{ VulkanInstanceExtensions::VK_INS_DEV_validation_flags,          "VK_EXT_validation_flags" },

		// Device & Properties


		// External & Synchronization

		{ VulkanInstanceExtensions::VK_INS_DEV_win32_surface,                   "VK_KHR_win32_surface" },

		// Miscellaneous

		{ VulkanInstanceExtensions::VK_INS_DEV_portability_enumeration,         "VK_KHR_portability_enumeration" },

		// Surface & Windowing

		{ VulkanInstanceExtensions::VK_INS_DEV_acquire_xlib_display,            "VK_EXT_acquire_xlib_display" },
		{ VulkanInstanceExtensions::VK_INS_DEV_surface_maintenance1,            "VK_EXT_surface_maintenance1" },
		{ VulkanInstanceExtensions::VK_INS_DEV_surface_protected_capabilities,  "VK_KHR_surface_protected_capabilities" },
		{ VulkanInstanceExtensions::VK_INS_DEV_xcb_surface,                     "VK_KHR_xcb_surface" },
		{ VulkanInstanceExtensions::VK_INS_DEV_xlib_surface,                    "VK_KHR_xlib_surface" }
	};

	enum class VulkanDeviceExtensions
	{
		// Debugging & Tooling
		
		VK_EXT_DEV_debug_marker,
		VK_EXT_DEV_tooling_info,
		VK_EXT_DEV_validation_cache,

		// Device & Properties
		
		VK_EXT_DEV_device_address_binding_report,
		VK_EXT_DEV_device_fault,
		VK_EXT_DEV_device_generated_commands,
		VK_EXT_DEV_maintenance4,
		VK_EXT_DEV_maintenance5,
		VK_EXT_DEV_maintenance6,
		VK_EXT_DEV_maintenance7,
		VK_EXT_DEV_maintenance8,
		VK_EXT_DEV_maintenance9,

		// External & Synchronization
		
		VK_EXT_DEV_device_memory_report,
		VK_EXT_DEV_external_fence_win32,
		VK_EXT_DEV_external_memory_acquire_unmodified,
		VK_EXT_DEV_external_memory_win32,
		VK_EXT_DEV_external_semaphore_win32,
		VK_EXT_DEV_map_memory2,
		VK_EXT_DEV_map_memory_placed,
		VK_EXT_DEV_pageable_device_local_memory,
		VK_EXT_DEV_win32_keyed_mutex,
		VK_EXT_DEV_workgroup_memory_explicit_layout,
		VK_EXT_DEV_zero_initialize_device_memory,
		VK_EXT_DEV_zero_initialize_workgroup_memory,

		// Memory & Layout
		
		VK_EXT_DEV_attachment_feedback_loop_layout,
		VK_EXT_DEV_unified_image_layouts,

		// Presentation & Swapchain
		
		VK_EXT_DEV_full_screen_exclusive,
		VK_EXT_DEV_present_id,
		VK_EXT_DEV_present_id2,
		VK_EXT_DEV_present_mode_fifo_latest_ready,
		VK_EXT_DEV_present_wait,
		VK_EXT_DEV_present_wait2,
		VK_EXT_DEV_swapchain_maintenance1,

		// Format & Compression
		
		VK_EXT_DEV_format_feature_flags2,
		VK_EXT_DEV_image_compression_control,
		VK_EXT_DEV_image_compression_control_swapchain,
		VK_EXT_DEV_rgba10x6_formats,

		// Portability & Compatibility
		
		VK_EXT_DEV_cooperative_matrix,
		VK_EXT_DEV_legacy_dithering,
		VK_EXT_DEV_legacy_vertex_attributes,
		VK_EXT_DEV_non_seamless_cube_map,
		VK_EXT_DEV_portability_subset,

		// Memory & Resource Management
		
		VK_EXT_DEV_buffer_device_address,
		VK_EXT_DEV_deferred_host_operations,
		VK_EXT_DEV_host_image_copy,
		VK_EXT_DEV_inline_uniform_block,
		VK_EXT_DEV_load_store_op_none,
		VK_EXT_DEV_memory_priority,
		VK_EXT_DEV_private_data,

		// Features & Capabilities
		
		VK_EXT_DEV_attachment_feedback_loop_dynamic_state,
		VK_EXT_DEV_extended_dynamic_state3,
		VK_EXT_DEV_frame_boundary,
		VK_EXT_DEV_global_priority,
		VK_EXT_DEV_global_priority_query,
		VK_EXT_DEV_primitives_generated_query,
		VK_EXT_DEV_rasterization_order_attachment_access,
		VK_EXT_DEV_subpass_merge_feedback,
		VK_EXT_DEV_vertex_attribute_robustness,
		VK_EXT_DEV_robustness2,

		// Visual, Sampling & Image Ops
		
		VK_EXT_DEV_border_color_swizzle,
		VK_EXT_DEV_depth_bias_control,
		VK_EXT_DEV_depth_clamp_control,
		VK_EXT_DEV_depth_clamp_zero_one,
		VK_EXT_DEV_depth_clip_control,
		VK_EXT_DEV_fragment_density_map,
		VK_EXT_DEV_fragment_density_map2,
		VK_EXT_DEV_fragment_density_map_offset,
		VK_EXT_DEV_image_2d_view_of_3d,
		VK_EXT_DEV_image_sliced_view_of_3d,
		VK_EXT_DEV_image_view_min_lod,
		VK_EXT_DEV_mesh_shader,
		VK_EXT_DEV_multisampled_render_to_single_sampled,
		VK_EXT_DEV_nested_command_buffer,
		VK_EXT_DEV_opacity_micromap,
		VK_EXT_DEV_provoking_vertex,

		// Pipeline & Descriptors
		
		VK_EXT_DEV_descriptor_buffer,
		VK_EXT_DEV_graphics_pipeline_library,
		VK_EXT_DEV_mutable_descriptor_type,
		VK_EXT_DEV_pipeline_binary,
		VK_EXT_DEV_pipeline_creation_cache_control,
		VK_EXT_DEV_pipeline_creation_feedback,
		VK_EXT_DEV_pipeline_executable_properties,
		VK_EXT_DEV_pipeline_library,
		VK_EXT_DEV_pipeline_library_group_handles,
		VK_EXT_DEV_pipeline_properties,
		VK_EXT_DEV_pipeline_protected_access,
		VK_EXT_DEV_pipeline_robustness,
		VK_EXT_DEV_push_descriptor,

		// Ray Tracing
		
		VK_EXT_DEV_acceleration_structure,
		VK_EXT_DEV_ray_query,
		VK_EXT_DEV_ray_tracing_maintenance1,
		VK_EXT_DEV_ray_tracing_pipeline,
		VK_EXT_DEV_ray_tracing_position_fetch,

		// Rendering
		
		VK_EXT_DEV_conditional_rendering,
		VK_EXT_DEV_dynamic_rendering,
		VK_EXT_DEV_dynamic_rendering_local_read,
		VK_EXT_DEV_dynamic_rendering_unused_attachments,

		// Shaders
		
		VK_EXT_DEV_compute_shader_derivatives,
		VK_EXT_DEV_fragment_shader_barycentric,
		VK_EXT_DEV_shader_atomic_float2,
		VK_EXT_DEV_shader_bfloat16,
		VK_EXT_DEV_shader_expect_assume,
		VK_EXT_DEV_shader_float_controls2,
		VK_EXT_DEV_shader_float8,
		VK_EXT_DEV_shader_integer_dot_product,
		VK_EXT_DEV_shader_maximal_reconvergence,
		VK_EXT_DEV_shader_module_identifier,
		VK_EXT_DEV_shader_non_semantic_info,
		VK_EXT_DEV_shader_object,
		VK_EXT_DEV_shader_quad_control,
		VK_EXT_DEV_shader_replicated_composites,
		VK_EXT_DEV_shader_relaxed_extended_instruction,
		VK_EXT_DEV_shader_subgroup_ballot,
		VK_EXT_DEV_shader_subgroup_rotate,
		VK_EXT_DEV_shader_subgroup_uniform_control_flow,
		VK_EXT_DEV_shader_subgroup_vote,
		VK_EXT_DEV_shader_tile_image,

		// Synchronization
		
		VK_EXT_DEV_calibrated_timestamps,

		// Vertex & Drawing
		
		VK_EXT_DEV_multi_draw,
		VK_EXT_DEV_primitive_topology_list_restart,
		VK_EXT_DEV_transform_feedback,
		VK_EXT_DEV_vertex_attribute_divisor,

		// Video
		
		VK_EXT_DEV_video_decode_av1,
		VK_EXT_DEV_video_decode_h264,
		VK_EXT_DEV_video_decode_h265,
		VK_EXT_DEV_video_decode_queue,
		VK_EXT_DEV_video_encode_av1,
		VK_EXT_DEV_video_encode_h264,
		VK_EXT_DEV_video_encode_h265,
		VK_EXT_DEV_video_encode_quantization_map,
		VK_EXT_DEV_video_encode_queue,
		VK_EXT_DEV_video_maintenance1,
		VK_EXT_DEV_video_maintenance2,
		VK_EXT_DEV_video_queue,
		VK_EXT_DEV_video_decode_vp9
	};

	static const unordered_map<VulkanDeviceExtensions, const char*> vulkanDeviceExtensionsInfo =
	{
		// Debugging & Tooling
		
		{ VulkanDeviceExtensions::VK_EXT_DEV_debug_marker,                  "VK_EXT_debug_marker" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_tooling_info,                  "VK_EXT_tooling_info" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_validation_cache,              "VK_EXT_validation_cache" },

		// Device & Properties
		
		{ VulkanDeviceExtensions::VK_EXT_DEV_device_address_binding_report, "VK_EXT_device_address_binding_report" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_device_fault,                  "VK_EXT_device_fault" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_device_generated_commands,     "VK_EXT_device_generated_commands" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_maintenance4,                  "VK_KHR_maintenance4" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_maintenance5,                  "VK_KHR_maintenance5" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_maintenance6,                  "VK_KHR_maintenance6" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_maintenance7,                  "VK_KHR_maintenance7" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_maintenance8,                  "VK_KHR_maintenance8" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_maintenance9,                  "VK_KHR_maintenance9" },

		// External & Synchronization
		
		{ VulkanDeviceExtensions::VK_EXT_DEV_device_memory_report,               "VK_EXT_device_memory_report" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_external_fence_win32,              "VK_KHR_external_fence_win32" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_external_memory_acquire_unmodified, "VK_EXT_external_memory_acquire_unmodified" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_external_memory_win32,             "VK_KHR_external_memory_win32" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_external_semaphore_win32,          "VK_KHR_external_semaphore_win32" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_map_memory2,                       "VK_KHR_map_memory2" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_map_memory_placed,                 "VK_EXT_map_memory_placed" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_pageable_device_local_memory,      "VK_EXT_pageable_device_local_memory" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_win32_keyed_mutex,                 "VK_KHR_win32_keyed_mutex" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_workgroup_memory_explicit_layout,  "VK_KHR_workgroup_memory_explicit_layout" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_zero_initialize_device_memory,     "VK_EXT_zero_initialize_device_memory" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_zero_initialize_workgroup_memory,  "VK_KHR_zero_initialize_workgroup_memory" },

		// Memory & Layout
		
		{ VulkanDeviceExtensions::VK_EXT_DEV_attachment_feedback_loop_layout,   "VK_EXT_attachment_feedback_loop_layout" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_unified_image_layouts,             "VK_KHR_unified_image_layouts" },

		// Presentation & Swapchain
		
		{ VulkanDeviceExtensions::VK_EXT_DEV_full_screen_exclusive,             "VK_EXT_full_screen_exclusive" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_present_id,                        "VK_KHR_present_id" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_present_id2,                       "VK_KHR_present_id2" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_present_mode_fifo_latest_ready,    "VK_EXT_present_mode_fifo_latest_ready" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_present_wait,                      "VK_KHR_present_wait" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_present_wait2,                     "VK_KHR_present_wait2" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_swapchain_maintenance1,            "VK_EXT_swapchain_maintenance1" },

		// Format & Compression
		
		{ VulkanDeviceExtensions::VK_EXT_DEV_format_feature_flags2,             "VK_KHR_format_feature_flags2" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_image_compression_control,         "VK_EXT_image_compression_control" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_image_compression_control_swapchain,"VK_EXT_image_compression_control_swapchain" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_rgba10x6_formats,                  "VK_EXT_rgba10x6_formats" },

		// Portability & Compatibility
		
		{ VulkanDeviceExtensions::VK_EXT_DEV_cooperative_matrix,                "VK_KHR_cooperative_matrix" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_legacy_dithering,                  "VK_EXT_legacy_dithering" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_legacy_vertex_attributes,          "VK_EXT_legacy_vertex_attributes" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_non_seamless_cube_map,             "VK_EXT_non_seamless_cube_map" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_portability_subset,                "VK_KHR_portability_subset" },

		// Memory & Resource Management
		
		{ VulkanDeviceExtensions::VK_EXT_DEV_buffer_device_address,             "VK_EXT_buffer_device_address" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_deferred_host_operations,          "VK_KHR_deferred_host_operations" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_host_image_copy,                   "VK_EXT_host_image_copy" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_inline_uniform_block,              "VK_EXT_inline_uniform_block" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_load_store_op_none,                "VK_KHR_load_store_op_none" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_memory_priority,                   "VK_EXT_memory_priority" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_private_data,                      "VK_EXT_private_data" },

		// Features & Capabilities
		
		{ VulkanDeviceExtensions::VK_EXT_DEV_attachment_feedback_loop_dynamic_state, "VK_EXT_attachment_feedback_loop_dynamic_state" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_extended_dynamic_state3,               "VK_EXT_extended_dynamic_state3" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_frame_boundary,                        "VK_EXT_frame_boundary" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_global_priority,                       "VK_EXT_global_priority" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_global_priority_query,                 "VK_EXT_global_priority_query" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_primitives_generated_query,            "VK_EXT_primitives_generated_query" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_rasterization_order_attachment_access, "VK_EXT_rasterization_order_attachment_access" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_subpass_merge_feedback,                "VK_EXT_subpass_merge_feedback" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_vertex_attribute_robustness,           "VK_EXT_vertex_attribute_robustness" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_robustness2,                           "VK_KHR_robustness2" },

		// Visual, Sampling & Image Ops
		
		{ VulkanDeviceExtensions::VK_EXT_DEV_border_color_swizzle,               "VK_EXT_border_color_swizzle" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_depth_bias_control,                 "VK_EXT_depth_bias_control" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_depth_clamp_control,                "VK_EXT_depth_clamp_control" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_depth_clamp_zero_one,               "VK_KHR_depth_clamp_zero_one" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_depth_clip_control,                 "VK_EXT_depth_clip_control" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_fragment_density_map,               "VK_EXT_fragment_density_map" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_fragment_density_map2,              "VK_EXT_fragment_density_map2" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_fragment_density_map_offset,        "VK_EXT_fragment_density_map_offset" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_image_2d_view_of_3d,                "VK_EXT_image_2d_view_of_3d" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_image_sliced_view_of_3d,            "VK_EXT_image_sliced_view_of_3d" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_image_view_min_lod,                 "VK_EXT_image_view_min_lod" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_mesh_shader,                        "VK_EXT_mesh_shader" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_multisampled_render_to_single_sampled, "VK_EXT_multisampled_render_to_single_sampled" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_nested_command_buffer,              "VK_EXT_nested_command_buffer" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_opacity_micromap,                   "VK_EXT_opacity_micromap" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_provoking_vertex,                   "VK_EXT_provoking_vertex" },

		// Pipeline & Descriptors
		
		{ VulkanDeviceExtensions::VK_EXT_DEV_descriptor_buffer,               "VK_EXT_descriptor_buffer" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_graphics_pipeline_library,       "VK_EXT_graphics_pipeline_library" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_mutable_descriptor_type,         "VK_EXT_mutable_descriptor_type" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_pipeline_binary,                 "VK_KHR_pipeline_binary" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_pipeline_creation_cache_control, "VK_EXT_pipeline_creation_cache_control" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_pipeline_creation_feedback,      "VK_EXT_pipeline_creation_feedback" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_pipeline_executable_properties,  "VK_KHR_pipeline_executable_properties" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_pipeline_library,                "VK_KHR_pipeline_library" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_pipeline_library_group_handles,  "VK_EXT_pipeline_library_group_handles" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_pipeline_properties,             "VK_EXT_pipeline_properties" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_pipeline_protected_access,       "VK_EXT_pipeline_protected_access" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_pipeline_robustness,             "VK_EXT_pipeline_robustness" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_push_descriptor,                 "VK_KHR_push_descriptor" },

		// Ray Tracing
		
		{ VulkanDeviceExtensions::VK_EXT_DEV_acceleration_structure,          "VK_KHR_acceleration_structure" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_ray_query,                       "VK_KHR_ray_query" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_ray_tracing_maintenance1,        "VK_KHR_ray_tracing_maintenance1" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_ray_tracing_pipeline,            "VK_KHR_ray_tracing_pipeline" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_ray_tracing_position_fetch,      "VK_KHR_ray_tracing_position_fetch" },

		// Rendering
		
		{ VulkanDeviceExtensions::VK_EXT_DEV_conditional_rendering,           "VK_EXT_conditional_rendering" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_dynamic_rendering,               "VK_KHR_dynamic_rendering" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_dynamic_rendering_local_read,    "VK_KHR_dynamic_rendering_local_read" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_dynamic_rendering_unused_attachments, "VK_EXT_dynamic_rendering_unused_attachments" },

		// Shaders
		
		{ VulkanDeviceExtensions::VK_EXT_DEV_compute_shader_derivatives,          "VK_KHR_compute_shader_derivatives" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_fragment_shader_barycentric,         "VK_KHR_fragment_shader_barycentric" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_shader_atomic_float2,                "VK_EXT_shader_atomic_float2" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_shader_bfloat16,                     "VK_KHR_shader_bfloat16" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_shader_expect_assume,                "VK_KHR_shader_expect_assume" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_shader_float_controls2,              "VK_KHR_shader_float_controls2" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_shader_float8,                       "VK_EXT_shader_float8" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_shader_integer_dot_product,          "VK_KHR_shader_integer_dot_product" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_shader_maximal_reconvergence,        "VK_KHR_shader_maximal_reconvergence" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_shader_module_identifier,            "VK_EXT_shader_module_identifier" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_shader_non_semantic_info,            "VK_KHR_shader_non_semantic_info" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_shader_object,                       "VK_EXT_shader_object" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_shader_quad_control,                 "VK_KHR_shader_quad_control" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_shader_replicated_composites,        "VK_EXT_shader_replicated_composites" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_shader_relaxed_extended_instruction, "VK_KHR_shader_relaxed_extended_instruction" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_shader_subgroup_ballot,              "VK_EXT_shader_subgroup_ballot" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_shader_subgroup_rotate,              "VK_KHR_shader_subgroup_rotate" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_shader_subgroup_uniform_control_flow,"VK_KHR_shader_subgroup_uniform_control_flow" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_shader_subgroup_vote,                "VK_EXT_shader_subgroup_vote" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_shader_tile_image,                   "VK_EXT_shader_tile_image" },

		// Synchronization
		
		{ VulkanDeviceExtensions::VK_EXT_DEV_calibrated_timestamps,           "VK_EXT_calibrated_timestamps" },

		// Vertex & Drawing
		
		{ VulkanDeviceExtensions::VK_EXT_DEV_multi_draw,                      "VK_EXT_multi_draw" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_primitive_topology_list_restart, "VK_EXT_primitive_topology_list_restart" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_transform_feedback,              "VK_EXT_transform_feedback" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_vertex_attribute_divisor,        "VK_EXT_vertex_attribute_divisor" },

		// Video
		
		{ VulkanDeviceExtensions::VK_EXT_DEV_video_decode_av1,             "VK_KHR_video_decode_av1" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_video_decode_h264,            "VK_KHR_video_decode_h264" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_video_decode_h265,            "VK_KHR_video_decode_h265" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_video_decode_queue,           "VK_KHR_video_decode_queue" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_video_decode_vp9,             "VK_KHR_video_decode_vp9" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_video_encode_av1,             "VK_KHR_video_encode_av1" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_video_encode_h264,            "VK_KHR_video_encode_h264" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_video_encode_h265,            "VK_KHR_video_encode_h265" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_video_encode_quantization_map,"VK_KHR_video_encode_quantization_map" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_video_encode_queue,           "VK_KHR_video_encode_queue" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_video_maintenance1,           "VK_KHR_video_maintenance1" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_video_maintenance2,           "VK_KHR_video_maintenance2" },
		{ VulkanDeviceExtensions::VK_EXT_DEV_video_queue,                  "VK_KHR_video_queue" }
	};
}

#endif //KALAWINDOW_SUPPORT_VULKAN