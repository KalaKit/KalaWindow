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
		
	};
}

#endif //KALAWINDOW_SUPPORT_VULKAN