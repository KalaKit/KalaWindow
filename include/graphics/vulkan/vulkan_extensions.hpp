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
		// Surface & Swapchain

		V_IE_ext_headless_surface,
		V_IE_ext_surface_maintenance1,
		V_IE_ext_swapchain_colorspace,
		V_IE_khr_get_surface_capabilities2,
		V_IE_khr_surface,
		V_IE_khr_surface_protected_capabilities,
		V_IE_khr_win32_surface,
		V_IE_khr_xcb_surface,
		V_IE_khr_xlib_surface,

		// Display

		V_IE_ext_acquire_xlib_display,
		V_IE_ext_direct_mode_display,
		V_IE_ext_display_surface_counter,
		V_IE_khr_display,
		V_IE_khr_get_display_properties2,

		// Debug / Validation / Settings

		V_IE_ext_application_parameters,
		V_IE_ext_debug_report,
		V_IE_ext_debug_utils,
		V_IE_ext_layer_settings,
		V_IE_ext_validation_features,
		V_IE_ext_validation_flags,

		// Portability

		V_IE_khr_portability_enumeration
	};

    static const unordered_map<VulkanInstanceExtensions, const char*> vulkanInstanceExtensionsInfo =
    {
        // Surface & Swapchain

        { VulkanInstanceExtensions::V_IE_ext_headless_surface,                  "VK_EXT_headless_surface" },
        { VulkanInstanceExtensions::V_IE_ext_surface_maintenance1,              "VK_EXT_surface_maintenance1" },
        { VulkanInstanceExtensions::V_IE_ext_swapchain_colorspace,              "VK_EXT_swapchain_colorspace" },
        { VulkanInstanceExtensions::V_IE_khr_get_surface_capabilities2,         "VK_KHR_get_surface_capabilities2" },
        { VulkanInstanceExtensions::V_IE_khr_surface,                           "VK_KHR_surface" },
        { VulkanInstanceExtensions::V_IE_khr_surface_protected_capabilities,    "VK_KHR_surface_protected_capabilities" },
        { VulkanInstanceExtensions::V_IE_khr_win32_surface,                     "VK_KHR_win32_surface" },
        { VulkanInstanceExtensions::V_IE_khr_xcb_surface,                       "VK_KHR_xcb_surface" },
        { VulkanInstanceExtensions::V_IE_khr_xlib_surface,                      "VK_KHR_xlib_surface" },

        // Display

        { VulkanInstanceExtensions::V_IE_ext_acquire_xlib_display,              "VK_EXT_acquire_xlib_display" },
        { VulkanInstanceExtensions::V_IE_ext_direct_mode_display,               "VK_EXT_direct_mode_display" },
        { VulkanInstanceExtensions::V_IE_ext_display_surface_counter,           "VK_EXT_display_surface_counter" },
        { VulkanInstanceExtensions::V_IE_khr_display,                           "VK_KHR_display" },
        { VulkanInstanceExtensions::V_IE_khr_get_display_properties2,           "VK_KHR_get_display_properties2" },

        // Debug / Validation / Settings

        { VulkanInstanceExtensions::V_IE_ext_application_parameters,            "VK_EXT_application_parameters" },
        { VulkanInstanceExtensions::V_IE_ext_debug_report,                      "VK_EXT_debug_report" },
        { VulkanInstanceExtensions::V_IE_ext_debug_utils,                       "VK_EXT_debug_utils" },
        { VulkanInstanceExtensions::V_IE_ext_layer_settings,                    "VK_EXT_layer_settings" },
        { VulkanInstanceExtensions::V_IE_ext_validation_features,               "VK_EXT_validation_features" },
        { VulkanInstanceExtensions::V_IE_ext_validation_flags,                  "VK_EXT_validation_flags" },

        // Portability

        { VulkanInstanceExtensions::V_IE_khr_portability_enumeration,           "VK_KHR_portability_enumeration" }
    };

    enum class VulkanDeviceExtensions
    {
        // Swapchain & Presentation

        V_DE_ext_display_control,
        V_DE_ext_full_screen_exclusive,
        V_DE_ext_present_mode_fifo_latest_ready,
        V_DE_ext_swapchain_maintenance1,
        V_DE_khr_display_swapchain,
        V_DE_khr_incremental_present,
        V_DE_khr_present_id,
        V_DE_khr_present_id2,
        V_DE_khr_present_wait,
        V_DE_khr_present_wait2,
        V_DE_khr_shared_presentable_image,
        V_DE_khr_swapchain,
        V_DE_khr_swapchain_mutable_format,

        // Video (encode / decode / queues / maintenance)

        V_DE_khr_video_decode_av1,
        V_DE_khr_video_decode_h264,
        V_DE_khr_video_decode_h265,
        V_DE_khr_video_decode_queue,
        V_DE_khr_video_decode_vp9,
        V_DE_khr_video_encode_av1,
        V_DE_khr_video_encode_h264,
        V_DE_khr_video_encode_h265,
        V_DE_khr_video_encode_quantization_map,
        V_DE_khr_video_encode_queue,
        V_DE_khr_video_maintenance1,
        V_DE_khr_video_maintenance2,
        V_DE_khr_video_queue,

        // External Memory & Interop

        V_DE_ext_external_memory_acquire_unmodified,
        V_DE_ext_external_memory_dma_buf,
        V_DE_ext_external_memory_host,
        V_DE_ext_host_image_copy,
        V_DE_ext_queue_family_foreign,
        V_DE_khr_deferred_host_operations,
        V_DE_khr_external_fence_fd,
        V_DE_khr_external_fence_win32,
        V_DE_khr_external_memory_fd,
        V_DE_khr_external_memory_win32,
        V_DE_khr_external_semaphore_fd,
        V_DE_khr_external_semaphore_win32,
        V_DE_khr_win32_keyed_mutex,

        // Ray Tracing & Acceleration

        V_DE_ext_opacity_micromap,
        V_DE_khr_acceleration_structure,
        V_DE_khr_ray_query,
        V_DE_khr_ray_tracing_maintenance1,
        V_DE_khr_ray_tracing_pipeline,
        V_DE_khr_ray_tracing_position_fetch,

        // Shader Features

        V_DE_ext_fragment_shader_interlock,
        V_DE_ext_mesh_shader,
        V_DE_ext_shader_atomic_float,
        V_DE_ext_shader_atomic_float2,
        V_DE_ext_shader_demote_to_helper_invocation,
        V_DE_ext_shader_float8,
        V_DE_ext_shader_image_atomic_int64,
        V_DE_ext_shader_module_identifier,
        V_DE_ext_shader_object,
        V_DE_ext_shader_replicated_composites,
        V_DE_ext_shader_stencil_export,
        V_DE_ext_shader_subgroup_ballot,
        V_DE_ext_shader_subgroup_vote,
        V_DE_ext_shader_tile_image,
        V_DE_ext_subgroup_size_control,
        V_DE_khr_cooperative_matrix,
        V_DE_khr_compute_shader_derivatives,
        V_DE_khr_fragment_shader_barycentric,
        V_DE_khr_shader_bfloat16,
        V_DE_khr_shader_clock,
        V_DE_khr_shader_expect_assume,
        V_DE_khr_shader_float_controls2,
        V_DE_khr_shader_integer_dot_product,
        V_DE_khr_shader_maximal_reconvergence,
        V_DE_khr_shader_non_semantic_info,
        V_DE_khr_shader_quad_control,
        V_DE_khr_shader_relaxed_extended_instruction,
        V_DE_khr_shader_subgroup_rotate,
        V_DE_khr_shader_subgroup_uniform_control_flow,
        V_DE_khr_shader_terminate_invocation,
        V_DE_khr_workgroup_memory_explicit_layout,
        V_DE_khr_zero_initialize_workgroup_memory,

        // Pipeline & Rendering

        V_DE_ext_attachment_feedback_loop_dynamic_state,
        V_DE_ext_attachment_feedback_loop_layout,
        V_DE_ext_blend_operation_advanced,
        V_DE_ext_color_write_enable,
        V_DE_ext_conditional_rendering,
        V_DE_ext_conservative_rasterization,
        V_DE_ext_depth_bias_control,
        V_DE_ext_discard_rectangles,
        V_DE_ext_dynamic_rendering_unused_attachments,
        V_DE_ext_frame_boundary,
        V_DE_ext_graphics_pipeline_library,
        V_DE_ext_hdr_metadata,
        V_DE_ext_line_rasterization,
        V_DE_ext_load_store_op_none,
        V_DE_ext_multi_draw,
        V_DE_ext_multisampled_render_to_single_sampled,
        V_DE_ext_pipeline_creation_cache_control,
        V_DE_ext_pipeline_creation_feedback,
        V_DE_ext_pipeline_library_group_handles,
        V_DE_ext_pipeline_properties,
        V_DE_ext_pipeline_protected_access,
        V_DE_ext_pipeline_robustness,
        V_DE_ext_post_depth_coverage,
        V_DE_ext_primitives_generated_query,
        V_DE_ext_primitive_topology_list_restart,
        V_DE_ext_provoking_vertex,
        V_DE_ext_rasterization_order_attachment_access,
        V_DE_ext_sample_locations,
        V_DE_ext_subpass_merge_feedback,
        V_DE_ext_transform_feedback,
        V_DE_khr_dynamic_rendering,
        V_DE_khr_dynamic_rendering_local_read,
        V_DE_khr_fragment_shading_rate,
        V_DE_khr_line_rasterization,
        V_DE_khr_load_store_op_none,
        V_DE_khr_maintenance4,
        V_DE_khr_maintenance5,
        V_DE_khr_maintenance6,
        V_DE_khr_maintenance7,
        V_DE_khr_maintenance8,
        V_DE_khr_maintenance9,
        V_DE_khr_pipeline_binary,
        V_DE_khr_pipeline_executable_properties,
        V_DE_khr_pipeline_library,
        V_DE_khr_portability_subset,

        // Dynamic State & Commands

        V_DE_ext_descriptor_buffer,
        V_DE_ext_device_generated_commands,
        V_DE_ext_extended_dynamic_state,
        V_DE_ext_extended_dynamic_state2,
        V_DE_ext_extended_dynamic_state3,
        V_DE_ext_index_type_uint8,
        V_DE_ext_inline_uniform_block,
        V_DE_ext_mutable_descriptor_type,
        V_DE_ext_nested_command_buffer,
        V_DE_ext_vertex_attribute_divisor,
        V_DE_ext_vertex_input_dynamic_state,
        V_DE_khr_copy_commands2,
        V_DE_khr_push_descriptor,
        V_DE_khr_synchronization2,
        V_DE_khr_vertex_attribute_divisor,

        // Memory, Address & Robustness

        V_DE_ext_buffer_device_address,
        V_DE_ext_custom_border_color,
        V_DE_ext_depth_clamp_control,
        V_DE_ext_device_address_binding_report,
        V_DE_ext_device_fault,
        V_DE_ext_device_memory_report,
        V_DE_ext_map_memory_placed,
        V_DE_ext_memory_budget,
        V_DE_ext_memory_priority,
        V_DE_ext_pageable_device_local_memory,
        V_DE_ext_robustness2,
        V_DE_ext_vertex_attribute_robustness,
        V_DE_ext_zero_initialize_device_memory,
        V_DE_khr_map_memory2,
        V_DE_khr_robustness2,

        // Image & Format / Compression

        V_DE_ext_4444_formats,
        V_DE_ext_astc_decode_mode,
        V_DE_ext_border_color_swizzle,
        V_DE_ext_depth_clamp_zero_one,
        V_DE_ext_depth_clip_control,
        V_DE_ext_depth_clip_enable,
        V_DE_ext_depth_range_unrestricted,
        V_DE_ext_filter_cubic,
        V_DE_ext_fragment_density_map,
        V_DE_ext_fragment_density_map2,
        V_DE_ext_fragment_density_map_offset,
        V_DE_ext_image_2d_view_of_3d,
        V_DE_ext_image_compression_control,
        V_DE_ext_image_compression_control_swapchain,
        V_DE_ext_image_robustness,
        V_DE_ext_image_sliced_view_of_3d,
        V_DE_ext_image_view_min_lod,
        V_DE_ext_non_seamless_cube_map,
        V_DE_ext_rgba10x6_formats,
        V_DE_ext_texture_compression_astc_hdr,
        V_DE_ext_texel_buffer_alignment,
        V_DE_ext_ycbcr_2plane_444_formats,
        V_DE_ext_ycbcr_image_arrays,
        V_DE_khr_depth_clamp_zero_one,
        V_DE_khr_format_feature_flags2,
        V_DE_khr_index_type_uint8,
        V_DE_khr_unified_image_layouts,

        // Performance & Timing

        V_DE_ext_calibrated_timestamps,
        V_DE_ext_global_priority,
        V_DE_ext_global_priority_query,
        V_DE_khr_calibrated_timestamps,
        V_DE_khr_global_priority,
        V_DE_khr_performance_query,

        // Debug / Validation / Tooling

        V_DE_ext_debug_marker,
        V_DE_ext_private_data,
        V_DE_ext_tooling_info,
        V_DE_ext_validation_cache,

        // Miscellaneous

        V_DE_ext_legacy_dithering,
        V_DE_ext_legacy_vertex_attributes,
        V_DE_ext_pci_bus_info,
        V_DE_khr_object_refresh
    };

    static const unordered_map<VulkanDeviceExtensions, const char*> vulkanDeviceExtensionsInfo =
    {
        // Swapchain & Presentation

        { VulkanDeviceExtensions::V_DE_ext_display_control,                      "VK_EXT_display_control" },
        { VulkanDeviceExtensions::V_DE_ext_full_screen_exclusive,                "VK_EXT_full_screen_exclusive" },
        { VulkanDeviceExtensions::V_DE_ext_present_mode_fifo_latest_ready,       "VK_EXT_present_mode_fifo_latest_ready" },
        { VulkanDeviceExtensions::V_DE_ext_swapchain_maintenance1,               "VK_EXT_swapchain_maintenance1" },
        { VulkanDeviceExtensions::V_DE_khr_display_swapchain,                    "VK_KHR_display_swapchain" },
        { VulkanDeviceExtensions::V_DE_khr_incremental_present,                  "VK_KHR_incremental_present" },
        { VulkanDeviceExtensions::V_DE_khr_present_id,                           "VK_KHR_present_id" },
        { VulkanDeviceExtensions::V_DE_khr_present_id2,                          "VK_KHR_present_id2" },
        { VulkanDeviceExtensions::V_DE_khr_present_wait,                         "VK_KHR_present_wait" },
        { VulkanDeviceExtensions::V_DE_khr_present_wait2,                        "VK_KHR_present_wait2" },
        { VulkanDeviceExtensions::V_DE_khr_shared_presentable_image,             "VK_KHR_shared_presentable_image" },
        { VulkanDeviceExtensions::V_DE_khr_swapchain,                            "VK_KHR_swapchain" },
        { VulkanDeviceExtensions::V_DE_khr_swapchain_mutable_format,             "VK_KHR_swapchain_mutable_format" },

        // Video (encode / decode / queues / maintenance)

        { VulkanDeviceExtensions::V_DE_khr_video_decode_av1,                     "VK_KHR_video_decode_av1" },
        { VulkanDeviceExtensions::V_DE_khr_video_decode_h264,                    "VK_KHR_video_decode_h264" },
        { VulkanDeviceExtensions::V_DE_khr_video_decode_h265,                    "VK_KHR_video_decode_h265" },
        { VulkanDeviceExtensions::V_DE_khr_video_decode_queue,                   "VK_KHR_video_decode_queue" },
        { VulkanDeviceExtensions::V_DE_khr_video_decode_vp9,                     "VK_KHR_video_decode_vp9" },
        { VulkanDeviceExtensions::V_DE_khr_video_encode_av1,                     "VK_KHR_video_encode_av1" },
        { VulkanDeviceExtensions::V_DE_khr_video_encode_h264,                    "VK_KHR_video_encode_h264" },
        { VulkanDeviceExtensions::V_DE_khr_video_encode_h265,                    "VK_KHR_video_encode_h265" },
        { VulkanDeviceExtensions::V_DE_khr_video_encode_quantization_map,        "VK_KHR_video_encode_quantization_map" },
        { VulkanDeviceExtensions::V_DE_khr_video_encode_queue,                   "VK_KHR_video_encode_queue" },
        { VulkanDeviceExtensions::V_DE_khr_video_maintenance1,                   "VK_KHR_video_maintenance1" },
        { VulkanDeviceExtensions::V_DE_khr_video_maintenance2,                   "VK_KHR_video_maintenance2" },
        { VulkanDeviceExtensions::V_DE_khr_video_queue,                          "VK_KHR_video_queue" },

        // External Memory & Interop

        { VulkanDeviceExtensions::V_DE_ext_external_memory_acquire_unmodified,   "VK_EXT_external_memory_acquire_unmodified" },
        { VulkanDeviceExtensions::V_DE_ext_external_memory_dma_buf,              "VK_EXT_external_memory_dma_buf" },
        { VulkanDeviceExtensions::V_DE_ext_external_memory_host,                 "VK_EXT_external_memory_host" },
        { VulkanDeviceExtensions::V_DE_ext_host_image_copy,                      "VK_EXT_host_image_copy" },
        { VulkanDeviceExtensions::V_DE_ext_queue_family_foreign,                 "VK_EXT_queue_family_foreign" },
        { VulkanDeviceExtensions::V_DE_khr_deferred_host_operations,             "VK_KHR_deferred_host_operations" },
        { VulkanDeviceExtensions::V_DE_khr_external_fence_fd,                    "VK_KHR_external_fence_fd" },
        { VulkanDeviceExtensions::V_DE_khr_external_fence_win32,                 "VK_KHR_external_fence_win32" },
        { VulkanDeviceExtensions::V_DE_khr_external_memory_fd,                   "VK_KHR_external_memory_fd" },
        { VulkanDeviceExtensions::V_DE_khr_external_memory_win32,                "VK_KHR_external_memory_win32" },
        { VulkanDeviceExtensions::V_DE_khr_external_semaphore_fd,                "VK_KHR_external_semaphore_fd" },
        { VulkanDeviceExtensions::V_DE_khr_external_semaphore_win32,             "VK_KHR_external_semaphore_win32" },
        { VulkanDeviceExtensions::V_DE_khr_win32_keyed_mutex,                    "VK_KHR_win32_keyed_mutex" },

        // Ray Tracing & Acceleration

        { VulkanDeviceExtensions::V_DE_ext_opacity_micromap,                     "VK_EXT_opacity_micromap" },
        { VulkanDeviceExtensions::V_DE_khr_acceleration_structure,               "VK_KHR_acceleration_structure" },
        { VulkanDeviceExtensions::V_DE_khr_ray_query,                            "VK_KHR_ray_query" },
        { VulkanDeviceExtensions::V_DE_khr_ray_tracing_maintenance1,             "VK_KHR_ray_tracing_maintenance1" },
        { VulkanDeviceExtensions::V_DE_khr_ray_tracing_pipeline,                 "VK_KHR_ray_tracing_pipeline" },
        { VulkanDeviceExtensions::V_DE_khr_ray_tracing_position_fetch,           "VK_KHR_ray_tracing_position_fetch" },

        // Shader Features

        { VulkanDeviceExtensions::V_DE_ext_fragment_shader_interlock,            "VK_EXT_fragment_shader_interlock" },
        { VulkanDeviceExtensions::V_DE_ext_mesh_shader,                          "VK_EXT_mesh_shader" },
        { VulkanDeviceExtensions::V_DE_ext_shader_atomic_float,                  "VK_EXT_shader_atomic_float" },
        { VulkanDeviceExtensions::V_DE_ext_shader_atomic_float2,                 "VK_EXT_shader_atomic_float2" },
        { VulkanDeviceExtensions::V_DE_ext_shader_demote_to_helper_invocation,   "VK_EXT_shader_demote_to_helper_invocation" },
        { VulkanDeviceExtensions::V_DE_ext_shader_float8,                        "VK_EXT_shader_float8" },
        { VulkanDeviceExtensions::V_DE_ext_shader_image_atomic_int64,            "VK_EXT_shader_image_atomic_int64" },
        { VulkanDeviceExtensions::V_DE_ext_shader_module_identifier,             "VK_EXT_shader_module_identifier" },
        { VulkanDeviceExtensions::V_DE_ext_shader_object,                        "VK_EXT_shader_object" },
        { VulkanDeviceExtensions::V_DE_ext_shader_replicated_composites,         "VK_EXT_shader_replicated_composites" },
        { VulkanDeviceExtensions::V_DE_ext_shader_stencil_export,               "VK_EXT_shader_stencil_export" },
        { VulkanDeviceExtensions::V_DE_ext_shader_subgroup_ballot,               "VK_EXT_shader_subgroup_ballot" },
        { VulkanDeviceExtensions::V_DE_ext_shader_subgroup_vote,                 "VK_EXT_shader_subgroup_vote" },
        { VulkanDeviceExtensions::V_DE_ext_shader_tile_image,                    "VK_EXT_shader_tile_image" },
        { VulkanDeviceExtensions::V_DE_ext_subgroup_size_control,                "VK_EXT_subgroup_size_control" },
        { VulkanDeviceExtensions::V_DE_khr_cooperative_matrix,                   "VK_KHR_cooperative_matrix" },
        { VulkanDeviceExtensions::V_DE_khr_compute_shader_derivatives,           "VK_KHR_compute_shader_derivatives" },
        { VulkanDeviceExtensions::V_DE_khr_fragment_shader_barycentric,          "VK_KHR_fragment_shader_barycentric" },
        { VulkanDeviceExtensions::V_DE_khr_shader_bfloat16,                      "VK_KHR_shader_bfloat16" },
        { VulkanDeviceExtensions::V_DE_khr_shader_clock,                         "VK_KHR_shader_clock" },
        { VulkanDeviceExtensions::V_DE_khr_shader_expect_assume,                 "VK_KHR_shader_expect_assume" },
        { VulkanDeviceExtensions::V_DE_khr_shader_float_controls2,               "VK_KHR_shader_float_controls2" },
        { VulkanDeviceExtensions::V_DE_khr_shader_integer_dot_product,           "VK_KHR_shader_integer_dot_product" },
        { VulkanDeviceExtensions::V_DE_khr_shader_maximal_reconvergence,         "VK_KHR_shader_maximal_reconvergence" },
        { VulkanDeviceExtensions::V_DE_khr_shader_non_semantic_info,             "VK_KHR_shader_non_semantic_info" },
        { VulkanDeviceExtensions::V_DE_khr_shader_quad_control,                  "VK_KHR_shader_quad_control" },
        { VulkanDeviceExtensions::V_DE_khr_shader_relaxed_extended_instruction,  "VK_KHR_shader_relaxed_extended_instruction" },
        { VulkanDeviceExtensions::V_DE_khr_shader_subgroup_rotate,               "VK_KHR_shader_subgroup_rotate" },
        { VulkanDeviceExtensions::V_DE_khr_shader_subgroup_uniform_control_flow, "VK_KHR_shader_subgroup_uniform_control_flow" },
        { VulkanDeviceExtensions::V_DE_khr_shader_terminate_invocation,          "VK_KHR_shader_terminate_invocation" },
        { VulkanDeviceExtensions::V_DE_khr_workgroup_memory_explicit_layout,     "VK_KHR_workgroup_memory_explicit_layout" },
        { VulkanDeviceExtensions::V_DE_khr_zero_initialize_workgroup_memory,     "VK_KHR_zero_initialize_workgroup_memory" },

        // Pipeline & Rendering

        { VulkanDeviceExtensions::V_DE_ext_attachment_feedback_loop_dynamic_state, "VK_EXT_attachment_feedback_loop_dynamic_state" },
        { VulkanDeviceExtensions::V_DE_ext_attachment_feedback_loop_layout,      "VK_EXT_attachment_feedback_loop_layout" },
        { VulkanDeviceExtensions::V_DE_ext_blend_operation_advanced,             "VK_EXT_blend_operation_advanced" },
        { VulkanDeviceExtensions::V_DE_ext_color_write_enable,                   "VK_EXT_color_write_enable" },
        { VulkanDeviceExtensions::V_DE_ext_conditional_rendering,                "VK_EXT_conditional_rendering" },
        { VulkanDeviceExtensions::V_DE_ext_conservative_rasterization,           "VK_EXT_conservative_rasterization" },
        { VulkanDeviceExtensions::V_DE_ext_depth_bias_control,                   "VK_EXT_depth_bias_control" },
        { VulkanDeviceExtensions::V_DE_ext_discard_rectangles,                   "VK_EXT_discard_rectangles" },
        { VulkanDeviceExtensions::V_DE_ext_dynamic_rendering_unused_attachments, "VK_EXT_dynamic_rendering_unused_attachments" },
        { VulkanDeviceExtensions::V_DE_ext_frame_boundary,                       "VK_EXT_frame_boundary" },
        { VulkanDeviceExtensions::V_DE_ext_graphics_pipeline_library,            "VK_EXT_graphics_pipeline_library" },
        { VulkanDeviceExtensions::V_DE_ext_hdr_metadata,                         "VK_EXT_hdr_metadata" },
        { VulkanDeviceExtensions::V_DE_ext_line_rasterization,                   "VK_EXT_line_rasterization" },
        { VulkanDeviceExtensions::V_DE_ext_load_store_op_none,                   "VK_EXT_load_store_op_none" },
        { VulkanDeviceExtensions::V_DE_ext_multi_draw,                           "VK_EXT_multi_draw" },
        { VulkanDeviceExtensions::V_DE_ext_multisampled_render_to_single_sampled,"VK_EXT_multisampled_render_to_single_sampled" },
        { VulkanDeviceExtensions::V_DE_ext_pipeline_creation_cache_control,      "VK_EXT_pipeline_creation_cache_control" },
        { VulkanDeviceExtensions::V_DE_ext_pipeline_creation_feedback,           "VK_EXT_pipeline_creation_feedback" },
        { VulkanDeviceExtensions::V_DE_ext_pipeline_library_group_handles,       "VK_EXT_pipeline_library_group_handles" },
        { VulkanDeviceExtensions::V_DE_ext_pipeline_properties,                  "VK_EXT_pipeline_properties" },
        { VulkanDeviceExtensions::V_DE_ext_pipeline_protected_access,            "VK_EXT_pipeline_protected_access" },
        { VulkanDeviceExtensions::V_DE_ext_pipeline_robustness,                  "VK_EXT_pipeline_robustness" },
        { VulkanDeviceExtensions::V_DE_ext_post_depth_coverage,                  "VK_EXT_post_depth_coverage" },
        { VulkanDeviceExtensions::V_DE_ext_primitives_generated_query,           "VK_EXT_primitives_generated_query" },
        { VulkanDeviceExtensions::V_DE_ext_primitive_topology_list_restart,      "VK_EXT_primitive_topology_list_restart" },
        { VulkanDeviceExtensions::V_DE_ext_provoking_vertex,                     "VK_EXT_provoking_vertex" },
        { VulkanDeviceExtensions::V_DE_ext_rasterization_order_attachment_access,"VK_EXT_rasterization_order_attachment_access" },
        { VulkanDeviceExtensions::V_DE_ext_sample_locations,                     "VK_EXT_sample_locations" },
        { VulkanDeviceExtensions::V_DE_ext_subpass_merge_feedback,               "VK_EXT_subpass_merge_feedback" },
        { VulkanDeviceExtensions::V_DE_ext_transform_feedback,                   "VK_EXT_transform_feedback" },
        { VulkanDeviceExtensions::V_DE_khr_dynamic_rendering,                    "VK_KHR_dynamic_rendering" },
        { VulkanDeviceExtensions::V_DE_khr_dynamic_rendering_local_read,         "VK_KHR_dynamic_rendering_local_read" },
        { VulkanDeviceExtensions::V_DE_khr_fragment_shading_rate,                "VK_KHR_fragment_shading_rate" },
        { VulkanDeviceExtensions::V_DE_khr_line_rasterization,                   "VK_KHR_line_rasterization" },
        { VulkanDeviceExtensions::V_DE_khr_load_store_op_none,                   "VK_KHR_load_store_op_none" },
        { VulkanDeviceExtensions::V_DE_khr_maintenance4,                         "VK_KHR_maintenance4" },
        { VulkanDeviceExtensions::V_DE_khr_maintenance5,                         "VK_KHR_maintenance5" },
        { VulkanDeviceExtensions::V_DE_khr_maintenance6,                         "VK_KHR_maintenance6" },
        { VulkanDeviceExtensions::V_DE_khr_maintenance7,                         "VK_KHR_maintenance7" },
        { VulkanDeviceExtensions::V_DE_khr_maintenance8,                         "VK_KHR_maintenance8" },
        { VulkanDeviceExtensions::V_DE_khr_maintenance9,                         "VK_KHR_maintenance9" },
        { VulkanDeviceExtensions::V_DE_khr_pipeline_binary,                      "VK_KHR_pipeline_binary" },
        { VulkanDeviceExtensions::V_DE_khr_pipeline_executable_properties,       "VK_KHR_pipeline_executable_properties" },
        { VulkanDeviceExtensions::V_DE_khr_pipeline_library,                     "VK_KHR_pipeline_library" },
        { VulkanDeviceExtensions::V_DE_khr_portability_subset,                   "VK_KHR_portability_subset" },

        // Dynamic State & Commands

        { VulkanDeviceExtensions::V_DE_ext_descriptor_buffer,                    "VK_EXT_descriptor_buffer" },
        { VulkanDeviceExtensions::V_DE_ext_device_generated_commands,            "VK_EXT_device_generated_commands" },
        { VulkanDeviceExtensions::V_DE_ext_extended_dynamic_state,               "VK_EXT_extended_dynamic_state" },
        { VulkanDeviceExtensions::V_DE_ext_extended_dynamic_state2,              "VK_EXT_extended_dynamic_state2" },
        { VulkanDeviceExtensions::V_DE_ext_extended_dynamic_state3,              "VK_EXT_extended_dynamic_state3" },
        { VulkanDeviceExtensions::V_DE_ext_index_type_uint8,                     "VK_EXT_index_type_uint8" },
        { VulkanDeviceExtensions::V_DE_ext_inline_uniform_block,                 "VK_EXT_inline_uniform_block" },
        { VulkanDeviceExtensions::V_DE_ext_mutable_descriptor_type,              "VK_EXT_mutable_descriptor_type" },
        { VulkanDeviceExtensions::V_DE_ext_nested_command_buffer,                "VK_EXT_nested_command_buffer" },
        { VulkanDeviceExtensions::V_DE_ext_vertex_attribute_divisor,             "VK_EXT_vertex_attribute_divisor" },
        { VulkanDeviceExtensions::V_DE_ext_vertex_input_dynamic_state,           "VK_EXT_vertex_input_dynamic_state" },
        { VulkanDeviceExtensions::V_DE_khr_copy_commands2,                       "VK_KHR_copy_commands2" },
        { VulkanDeviceExtensions::V_DE_khr_push_descriptor,                      "VK_KHR_push_descriptor" },
        { VulkanDeviceExtensions::V_DE_khr_synchronization2,                     "VK_KHR_synchronization2" },
        { VulkanDeviceExtensions::V_DE_khr_vertex_attribute_divisor,             "VK_KHR_vertex_attribute_divisor" },

        // Memory, Address & Robustness

        { VulkanDeviceExtensions::V_DE_ext_buffer_device_address,                "VK_EXT_buffer_device_address" },
        { VulkanDeviceExtensions::V_DE_ext_custom_border_color,                  "VK_EXT_custom_border_color" },
        { VulkanDeviceExtensions::V_DE_ext_depth_clamp_control,                  "VK_EXT_depth_clamp_control" },
        { VulkanDeviceExtensions::V_DE_ext_device_address_binding_report,        "VK_EXT_device_address_binding_report" },
        { VulkanDeviceExtensions::V_DE_ext_device_fault,                         "VK_EXT_device_fault" },
        { VulkanDeviceExtensions::V_DE_ext_device_memory_report,                 "VK_EXT_device_memory_report" },
        { VulkanDeviceExtensions::V_DE_ext_map_memory_placed,                    "VK_EXT_map_memory_placed" },
        { VulkanDeviceExtensions::V_DE_ext_memory_budget,                        "VK_EXT_memory_budget" },
        { VulkanDeviceExtensions::V_DE_ext_memory_priority,                      "VK_EXT_memory_priority" },
        { VulkanDeviceExtensions::V_DE_ext_pageable_device_local_memory,         "VK_EXT_pageable_device_local_memory" },
        { VulkanDeviceExtensions::V_DE_ext_robustness2,                          "VK_EXT_robustness2" },
        { VulkanDeviceExtensions::V_DE_ext_vertex_attribute_robustness,          "VK_EXT_vertex_attribute_robustness" },
        { VulkanDeviceExtensions::V_DE_ext_zero_initialize_device_memory,        "VK_EXT_zero_initialize_device_memory" },
        { VulkanDeviceExtensions::V_DE_khr_map_memory2,                          "VK_KHR_map_memory2" },
        { VulkanDeviceExtensions::V_DE_khr_robustness2,                          "VK_KHR_robustness2" },

        // Image & Format / Compression

        { VulkanDeviceExtensions::V_DE_ext_4444_formats,                         "VK_EXT_4444_formats" },
        { VulkanDeviceExtensions::V_DE_ext_astc_decode_mode,                     "VK_EXT_astc_decode_mode" },
        { VulkanDeviceExtensions::V_DE_ext_border_color_swizzle,                 "VK_EXT_border_color_swizzle" },
        { VulkanDeviceExtensions::V_DE_ext_depth_clamp_zero_one,                 "VK_EXT_depth_clamp_zero_one" },
        { VulkanDeviceExtensions::V_DE_ext_depth_clip_control,                   "VK_EXT_depth_clip_control" },
        { VulkanDeviceExtensions::V_DE_ext_depth_clip_enable,                    "VK_EXT_depth_clip_enable" },
        { VulkanDeviceExtensions::V_DE_ext_depth_range_unrestricted,             "VK_EXT_depth_range_unrestricted" },
        { VulkanDeviceExtensions::V_DE_ext_filter_cubic,                         "VK_EXT_filter_cubic" },
        { VulkanDeviceExtensions::V_DE_ext_fragment_density_map,                 "VK_EXT_fragment_density_map" },
        { VulkanDeviceExtensions::V_DE_ext_fragment_density_map2,                "VK_EXT_fragment_density_map2" },
        { VulkanDeviceExtensions::V_DE_ext_fragment_density_map_offset,          "VK_EXT_fragment_density_map_offset" },
        { VulkanDeviceExtensions::V_DE_ext_image_2d_view_of_3d,                  "VK_EXT_image_2d_view_of_3d" },
        { VulkanDeviceExtensions::V_DE_ext_image_compression_control,            "VK_EXT_image_compression_control" },
        { VulkanDeviceExtensions::V_DE_ext_image_compression_control_swapchain,  "VK_EXT_image_compression_control_swapchain" },
        { VulkanDeviceExtensions::V_DE_ext_image_robustness,                     "VK_EXT_image_robustness" },
        { VulkanDeviceExtensions::V_DE_ext_image_sliced_view_of_3d,              "VK_EXT_image_sliced_view_of_3d" },
        { VulkanDeviceExtensions::V_DE_ext_image_view_min_lod,                   "VK_EXT_image_view_min_lod" },
        { VulkanDeviceExtensions::V_DE_ext_non_seamless_cube_map,                "VK_EXT_non_seamless_cube_map" },
        { VulkanDeviceExtensions::V_DE_ext_rgba10x6_formats,                     "VK_EXT_rgba10x6_formats" },
        { VulkanDeviceExtensions::V_DE_ext_texture_compression_astc_hdr,         "VK_EXT_texture_compression_astc_hdr" },
        { VulkanDeviceExtensions::V_DE_ext_texel_buffer_alignment,               "VK_EXT_texel_buffer_alignment" },
        { VulkanDeviceExtensions::V_DE_ext_ycbcr_2plane_444_formats,             "VK_EXT_ycbcr_2plane_444_formats" },
        { VulkanDeviceExtensions::V_DE_ext_ycbcr_image_arrays,                   "VK_EXT_ycbcr_image_arrays" },
        { VulkanDeviceExtensions::V_DE_khr_depth_clamp_zero_one,                 "VK_KHR_depth_clamp_zero_one" },
        { VulkanDeviceExtensions::V_DE_khr_format_feature_flags2,                "VK_KHR_format_feature_flags2" },
        { VulkanDeviceExtensions::V_DE_khr_index_type_uint8,                     "VK_KHR_index_type_uint8" },
        { VulkanDeviceExtensions::V_DE_khr_unified_image_layouts,                "VK_KHR_unified_image_layouts" },

        // Performance & Timing

        { VulkanDeviceExtensions::V_DE_ext_calibrated_timestamps,                "VK_EXT_calibrated_timestamps" },
        { VulkanDeviceExtensions::V_DE_ext_global_priority,                      "VK_EXT_global_priority" },
        { VulkanDeviceExtensions::V_DE_ext_global_priority_query,                "VK_EXT_global_priority_query" },
        { VulkanDeviceExtensions::V_DE_khr_calibrated_timestamps,                "VK_KHR_calibrated_timestamps" },
        { VulkanDeviceExtensions::V_DE_khr_global_priority,                      "VK_KHR_global_priority" },
        { VulkanDeviceExtensions::V_DE_khr_performance_query,                    "VK_KHR_performance_query" },

        // Debug / Validation / Tooling

        { VulkanDeviceExtensions::V_DE_ext_debug_marker,                         "VK_EXT_debug_marker" },
        { VulkanDeviceExtensions::V_DE_ext_private_data,                         "VK_EXT_private_data" },
        { VulkanDeviceExtensions::V_DE_ext_tooling_info,                         "VK_EXT_tooling_info" },
        { VulkanDeviceExtensions::V_DE_ext_validation_cache,                     "VK_EXT_validation_cache" },

        // Miscellaneous

        { VulkanDeviceExtensions::V_DE_ext_legacy_dithering,                     "VK_EXT_legacy_dithering" },
        { VulkanDeviceExtensions::V_DE_ext_legacy_vertex_attributes,             "VK_EXT_legacy_vertex_attributes" },
        { VulkanDeviceExtensions::V_DE_ext_pci_bus_info,                         "VK_EXT_pci_bus_info" },
        { VulkanDeviceExtensions::V_DE_khr_object_refresh,                       "VK_KHR_object_refresh" }
    };
}

#endif //KALAWINDOW_SUPPORT_VULKAN