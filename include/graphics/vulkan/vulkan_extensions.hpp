﻿//Copyright(C) 2025 Lost Empire Entertainment
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

		V_IE_debug_report,
		V_IE_validation_flags,

		// Device & Properties

		V_IE_device_group_creation,
		V_IE_get_physical_device_properties2,

		// External & Synchronization

		V_IE_external_fence_capabilities,
		V_IE_external_memory_capabilities,
		V_IE_external_semaphore_capabilities,

		// Miscellaneous

		V_IE_portability_enumeration,

		// Surface & Windowing

		V_IE_acquire_xlib_display,
		V_IE_surface_maintenance1,
		V_IE_surface_protected_capabilities,
		V_IE_win32_surface,
		V_IE_xcb_surface,
		V_IE_xlib_surface
	};

	static const unordered_map<VulkanInstanceExtensions, const char*> vulkanInstanceExtensionsInfo =
	{
		// Debugging & Tooling

		{ VulkanInstanceExtensions::V_IE_debug_report,     "VK_EXT_debug_report" },
		{ VulkanInstanceExtensions::V_IE_validation_flags, "VK_EXT_validation_flags" },

		// Device & Properties

		{ VulkanInstanceExtensions::V_IE_device_group_creation,           "VK_KHR_device_group_creation" },
		{ VulkanInstanceExtensions::V_IE_get_physical_device_properties2, "VK_KHR_get_physical_device_properties2" },

		// External & Synchronization

		{ VulkanInstanceExtensions::V_IE_external_fence_capabilities,     "VK_KHR_external_fence_capabilities" },
		{ VulkanInstanceExtensions::V_IE_external_memory_capabilities,    "VK_KHR_external_memory_capabilities" },
		{ VulkanInstanceExtensions::V_IE_external_semaphore_capabilities, "VK_KHR_external_semaphore_capabilities" },

		// Miscellaneous

		{ VulkanInstanceExtensions::V_IE_portability_enumeration, "VK_KHR_portability_enumeration" },

		// Surface & Windowing

		{ VulkanInstanceExtensions::V_IE_acquire_xlib_display,           "VK_EXT_acquire_xlib_display" },
		{ VulkanInstanceExtensions::V_IE_surface_maintenance1,           "VK_EXT_surface_maintenance1" },
		{ VulkanInstanceExtensions::V_IE_surface_protected_capabilities, "VK_KHR_surface_protected_capabilities" },
		{ VulkanInstanceExtensions::V_IE_win32_surface,                  "VK_KHR_win32_surface" },
		{ VulkanInstanceExtensions::V_IE_xcb_surface,                    "VK_KHR_xcb_surface" },
		{ VulkanInstanceExtensions::V_IE_xlib_surface,                   "VK_KHR_xlib_surface" }
	};

	enum class VulkanDeviceExtensions
	{
		// VIDEO

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

		// RAY TRACING

		V_DE_khr_acceleration_structure,
		V_DE_khr_ray_query,
		V_DE_khr_ray_tracing_maintenance1,
		V_DE_khr_ray_tracing_pipeline,
		V_DE_khr_ray_tracing_position_fetch,

		// SHADER / SPIR-V

		V_DE_ext_mesh_shader,
		V_DE_ext_scalar_block_layout,
		V_DE_ext_shader_atomic_float2,
		V_DE_ext_shader_float8,
		V_DE_ext_shader_module_identifier,
		V_DE_ext_shader_object,
		V_DE_ext_shader_replicated_composites,
		V_DE_ext_shader_subgroup_ballot,
		V_DE_ext_shader_subgroup_vote,
		V_DE_ext_shader_tile_image,
		V_DE_ext_shader_viewport_index_layer,
		V_DE_khr_16bit_storage,
		V_DE_khr_8bit_storage,
		V_DE_khr_cooperative_matrix,
		V_DE_khr_compute_shader_derivatives,
		V_DE_khr_fragment_shader_barycentric,
		V_DE_khr_shader_atomic_int64,
		V_DE_khr_shader_bfloat16,
		V_DE_khr_shader_draw_parameters,
		V_DE_khr_shader_expect_assume,
		V_DE_khr_shader_float16_int8,
		V_DE_khr_shader_float_controls,
		V_DE_khr_shader_float_controls2,
		V_DE_khr_shader_integer_dot_product,
		V_DE_khr_shader_maximal_reconvergence,
		V_DE_khr_shader_non_semantic_info,
		V_DE_khr_shader_quad_control,
		V_DE_khr_shader_relaxed_extended_instruction,
		V_DE_khr_shader_subgroup_extended_types,
		V_DE_khr_shader_subgroup_rotate,
		V_DE_khr_shader_subgroup_uniform_control_flow,
		V_DE_khr_spirv_1_4,
		V_DE_khr_storage_buffer_storage_class,
		V_DE_khr_variable_pointers,
		V_DE_khr_vulkan_memory_model,
		V_DE_khr_workgroup_memory_explicit_layout,
		V_DE_khr_zero_initialize_workgroup_memory,

		// RENDERING & PIPELINE

		V_DE_ext_attachment_feedback_loop_dynamic_state,
		V_DE_ext_attachment_feedback_loop_layout,
		V_DE_ext_border_color_swizzle,
		V_DE_ext_conditional_rendering,
		V_DE_ext_depth_bias_control,
		V_DE_ext_depth_clamp_control,
		V_DE_ext_depth_clamp_zero_one,
		V_DE_ext_depth_clip_control,
		V_DE_ext_device_generated_commands,
		V_DE_ext_dynamic_rendering_unused_attachments,
		V_DE_ext_extended_dynamic_state3,
		V_DE_ext_fragment_density_map,
		V_DE_ext_fragment_density_map2,
		V_DE_ext_fragment_density_map_offset,
		V_DE_ext_frame_boundary,
		V_DE_ext_image_2d_view_of_3d,
		V_DE_ext_image_compression_control,
		V_DE_ext_image_compression_control_swapchain,
		V_DE_ext_image_sliced_view_of_3d,
		V_DE_ext_image_view_min_lod,
		V_DE_ext_legacy_dithering,
		V_DE_ext_legacy_vertex_attributes,
		V_DE_ext_load_store_op_none,
		V_DE_ext_multisampled_render_to_single_sampled,
		V_DE_ext_multi_draw,
		V_DE_ext_nested_command_buffer,
		V_DE_ext_non_seamless_cube_map,
		V_DE_ext_opacity_micromap,
		V_DE_ext_pipeline_creation_cache_control,
		V_DE_ext_pipeline_creation_feedback,
		V_DE_ext_pipeline_properties,
		V_DE_ext_pipeline_protected_access,
		V_DE_ext_pipeline_robustness,
		V_DE_ext_primitives_generated_query,
		V_DE_ext_primitive_topology_list_restart,
		V_DE_ext_rasterization_order_attachment_access,
		V_DE_ext_rgba10x6_formats,
		V_DE_ext_sampler_filter_minmax,
		V_DE_ext_separate_stencil_usage,
		V_DE_ext_subpass_merge_feedback,
		V_DE_ext_transform_feedback,
		V_DE_ext_validation_cache,
		V_DE_khr_create_renderpass2,
		V_DE_khr_depth_clamp_zero_one,
		V_DE_khr_depth_stencil_resolve,
		V_DE_khr_draw_indirect_count,
		V_DE_khr_dynamic_rendering,
		V_DE_khr_dynamic_rendering_local_read,
		V_DE_khr_format_feature_flags2,
		V_DE_khr_imageless_framebuffer,
		V_DE_khr_load_store_op_none,
		V_DE_khr_multiview,
		V_DE_khr_portability_subset,
		V_DE_khr_sampler_mirror_clamp_to_edge,
		V_DE_khr_sampler_ycbcr_conversion,
		V_DE_khr_separate_depth_stencil_layouts,

		// DESCRIPTORS

		V_DE_ext_descriptor_buffer,
		V_DE_ext_descriptor_indexing,
		V_DE_ext_inline_uniform_block,
		V_DE_ext_mutable_descriptor_type,
		V_DE_khr_descriptor_update_template,
		V_DE_khr_push_descriptor,

		// MEMORY & ADDRESSING

		V_DE_ext_buffer_device_address,
		V_DE_ext_device_address_binding_report,
		V_DE_ext_device_memory_report,
		V_DE_ext_external_memory_acquire_unmodified,
		V_DE_ext_map_memory_placed,
		V_DE_ext_memory_priority,
		V_DE_ext_pageable_device_local_memory,
		V_DE_ext_zero_initialize_device_memory,
		V_DE_khr_bind_memory2,
		V_DE_khr_buffer_device_address,
		V_DE_khr_dedicated_allocation,
		V_DE_khr_external_memory,
		V_DE_khr_external_memory_win32,
		V_DE_khr_get_memory_requirements2,
		V_DE_khr_image_format_list,
		V_DE_khr_map_memory2,
		V_DE_khr_unified_image_layouts,

		// SYNCHRONIZATION

		V_DE_khr_external_fence,
		V_DE_khr_external_fence_win32,
		V_DE_khr_external_semaphore,
		V_DE_khr_external_semaphore_win32,
		V_DE_khr_timeline_semaphore,
		V_DE_khr_win32_keyed_mutex,

		// PRESENTATION / SWAPCHAIN

		V_DE_ext_full_screen_exclusive,
		V_DE_ext_present_mode_fifo_latest_ready,
		V_DE_ext_swapchain_maintenance1,
		V_DE_khr_present_id,
		V_DE_khr_present_id2,
		V_DE_khr_present_wait,
		V_DE_khr_present_wait2,

		// DEBUG & TOOLING

		V_DE_ext_calibrated_timestamps,
		V_DE_ext_debug_marker,
		V_DE_ext_device_fault,
		V_DE_ext_global_priority,
		V_DE_ext_global_priority_query,
		V_DE_ext_host_image_copy,
		V_DE_ext_host_query_reset,
		V_DE_ext_private_data,
		V_DE_ext_tooling_info,
		V_DE_khr_deferred_host_operations,
		V_DE_khr_driver_properties,
		V_DE_khr_pipeline_executable_properties,

		// PIPELINE LIBRARIES

		V_DE_ext_graphics_pipeline_library,
		V_DE_ext_pipeline_library_group_handles,
		V_DE_khr_pipeline_binary,
		V_DE_khr_pipeline_library,

		// MAINTENANCE

		V_DE_khr_maintenance1,
		V_DE_khr_maintenance2,
		V_DE_khr_maintenance3,
		V_DE_khr_maintenance4,
		V_DE_khr_maintenance5,
		V_DE_khr_maintenance6,
		V_DE_khr_maintenance7,
		V_DE_khr_maintenance8,
		V_DE_khr_maintenance9,

		// MISC / OTHER

		V_DE_khr_device_group,
		V_DE_ext_provoking_vertex,
		V_DE_ext_vertex_attribute_divisor,
		V_DE_ext_vertex_attribute_robustness,
		V_DE_khr_relaxed_block_layout,
		V_DE_khr_robustness2,
		V_DE_khr_uniform_buffer_standard_layout
	};

	static const unordered_map<VulkanDeviceExtensions, const char*> vulkanDeviceExtensionsInfo =
	{
		// VIDEO

		{ VulkanDeviceExtensions::V_DE_khr_video_decode_av1,                    "VK_KHR_video_decode_av1"},
		{ VulkanDeviceExtensions::V_DE_khr_video_decode_h264,                   "VK_KHR_video_decode_h264"},
		{ VulkanDeviceExtensions::V_DE_khr_video_decode_h265,                   "VK_KHR_video_decode_h265"},
		{ VulkanDeviceExtensions::V_DE_khr_video_decode_queue,                  "VK_KHR_video_decode_queue"},
		{ VulkanDeviceExtensions::V_DE_khr_video_decode_vp9,                    "VK_KHR_video_decode_vp9"},
		{ VulkanDeviceExtensions::V_DE_khr_video_encode_av1,                    "VK_KHR_video_encode_av1"},
		{ VulkanDeviceExtensions::V_DE_khr_video_encode_h264,                   "VK_KHR_video_encode_h264"},
		{ VulkanDeviceExtensions::V_DE_khr_video_encode_h265,                   "VK_KHR_video_encode_h265"},
		{ VulkanDeviceExtensions::V_DE_khr_video_encode_quantization_map,       "VK_KHR_video_encode_quantization_map"},
		{ VulkanDeviceExtensions::V_DE_khr_video_encode_queue,                  "VK_KHR_video_encode_queue"},
		{ VulkanDeviceExtensions::V_DE_khr_video_maintenance1,                  "VK_KHR_video_maintenance1"},
		{ VulkanDeviceExtensions::V_DE_khr_video_maintenance2,                  "VK_KHR_video_maintenance2"},
		{ VulkanDeviceExtensions::V_DE_khr_video_queue,                         "VK_KHR_video_queue"},

		// RAY TRACING

		{ VulkanDeviceExtensions::V_DE_khr_acceleration_structure,              "VK_KHR_acceleration_structure"},
		{ VulkanDeviceExtensions::V_DE_khr_ray_query,                           "VK_KHR_ray_query"},
		{ VulkanDeviceExtensions::V_DE_khr_ray_tracing_maintenance1,            "VK_KHR_ray_tracing_maintenance1"},
		{ VulkanDeviceExtensions::V_DE_khr_ray_tracing_pipeline,                "VK_KHR_ray_tracing_pipeline"},
		{ VulkanDeviceExtensions::V_DE_khr_ray_tracing_position_fetch,          "VK_KHR_ray_tracing_position_fetch"},

		// SHADER / SPIR-V

		{ VulkanDeviceExtensions::V_DE_ext_mesh_shader,                        "VK_EXT_mesh_shader"},
		{ VulkanDeviceExtensions::V_DE_ext_scalar_block_layout,                "VK_EXT_scalar_block_layout"},
		{ VulkanDeviceExtensions::V_DE_ext_shader_atomic_float2,               "VK_EXT_shader_atomic_float2"},
		{ VulkanDeviceExtensions::V_DE_ext_shader_float8,                      "VK_EXT_shader_float8"},
		{ VulkanDeviceExtensions::V_DE_ext_shader_module_identifier,           "VK_EXT_shader_module_identifier"},
		{ VulkanDeviceExtensions::V_DE_ext_shader_object,                      "VK_EXT_shader_object"},
		{ VulkanDeviceExtensions::V_DE_ext_shader_replicated_composites,       "VK_EXT_shader_replicated_composites"},
		{ VulkanDeviceExtensions::V_DE_ext_shader_subgroup_ballot,             "VK_EXT_shader_subgroup_ballot"},
		{ VulkanDeviceExtensions::V_DE_ext_shader_subgroup_vote,               "VK_EXT_shader_subgroup_vote"},
		{ VulkanDeviceExtensions::V_DE_ext_shader_tile_image,                  "VK_EXT_shader_tile_image"},
		{ VulkanDeviceExtensions::V_DE_ext_shader_viewport_index_layer,        "VK_EXT_shader_viewport_index_layer"},
		{ VulkanDeviceExtensions::V_DE_khr_16bit_storage,                      "VK_KHR_16bit_storage"},
		{ VulkanDeviceExtensions::V_DE_khr_8bit_storage,                       "VK_KHR_8bit_storage"},
		{ VulkanDeviceExtensions::V_DE_khr_cooperative_matrix,                 "VK_KHR_cooperative_matrix"},
		{ VulkanDeviceExtensions::V_DE_khr_compute_shader_derivatives,         "VK_KHR_compute_shader_derivatives"},
		{ VulkanDeviceExtensions::V_DE_khr_fragment_shader_barycentric,        "VK_KHR_fragment_shader_barycentric"},
		{ VulkanDeviceExtensions::V_DE_khr_shader_atomic_int64,                "VK_KHR_shader_atomic_int64"},
		{ VulkanDeviceExtensions::V_DE_khr_shader_bfloat16,                    "VK_KHR_shader_bfloat16"},
		{ VulkanDeviceExtensions::V_DE_khr_shader_draw_parameters,             "VK_KHR_shader_draw_parameters"},
		{ VulkanDeviceExtensions::V_DE_khr_shader_expect_assume,               "VK_KHR_shader_expect_assume"},
		{ VulkanDeviceExtensions::V_DE_khr_shader_float16_int8,                "VK_KHR_shader_float16_int8"},
		{ VulkanDeviceExtensions::V_DE_khr_shader_float_controls,              "VK_KHR_shader_float_controls"},
		{ VulkanDeviceExtensions::V_DE_khr_shader_float_controls2,             "VK_KHR_shader_float_controls2"},
		{ VulkanDeviceExtensions::V_DE_khr_shader_integer_dot_product,         "VK_KHR_shader_integer_dot_product"},
		{ VulkanDeviceExtensions::V_DE_khr_shader_maximal_reconvergence,       "VK_KHR_shader_maximal_reconvergence"},
		{ VulkanDeviceExtensions::V_DE_khr_shader_non_semantic_info,           "VK_KHR_shader_non_semantic_info"},
		{ VulkanDeviceExtensions::V_DE_khr_shader_quad_control,                "VK_KHR_shader_quad_control"},
		{ VulkanDeviceExtensions::V_DE_khr_shader_relaxed_extended_instruction,"VK_KHR_shader_relaxed_extended_instruction"},
		{ VulkanDeviceExtensions::V_DE_khr_shader_subgroup_extended_types,     "VK_KHR_shader_subgroup_extended_types"},
		{ VulkanDeviceExtensions::V_DE_khr_shader_subgroup_rotate,             "VK_KHR_shader_subgroup_rotate"},
		{ VulkanDeviceExtensions::V_DE_khr_shader_subgroup_uniform_control_flow,"VK_KHR_shader_subgroup_uniform_control_flow"},
		{ VulkanDeviceExtensions::V_DE_khr_spirv_1_4,                          "VK_KHR_spirv_1_4"},
		{ VulkanDeviceExtensions::V_DE_khr_storage_buffer_storage_class,       "VK_KHR_storage_buffer_storage_class"},
		{ VulkanDeviceExtensions::V_DE_khr_variable_pointers,                  "VK_KHR_variable_pointers"},
		{ VulkanDeviceExtensions::V_DE_khr_vulkan_memory_model,                "VK_KHR_vulkan_memory_model"},
		{ VulkanDeviceExtensions::V_DE_khr_workgroup_memory_explicit_layout,   "VK_KHR_workgroup_memory_explicit_layout"},
		{ VulkanDeviceExtensions::V_DE_khr_zero_initialize_workgroup_memory,   "VK_KHR_zero_initialize_workgroup_memory"},

		// RENDERING & PIPELINE

		{ VulkanDeviceExtensions::V_DE_ext_attachment_feedback_loop_dynamic_state,"VK_EXT_attachment_feedback_loop_dynamic_state"},
		{ VulkanDeviceExtensions::V_DE_ext_attachment_feedback_loop_layout,    "VK_EXT_attachment_feedback_loop_layout"},
		{ VulkanDeviceExtensions::V_DE_ext_border_color_swizzle,               "VK_EXT_border_color_swizzle"},
		{ VulkanDeviceExtensions::V_DE_ext_conditional_rendering,              "VK_EXT_conditional_rendering"},
		{ VulkanDeviceExtensions::V_DE_ext_depth_bias_control,                 "VK_EXT_depth_bias_control"},
		{ VulkanDeviceExtensions::V_DE_ext_depth_clamp_control,                "VK_EXT_depth_clamp_control"},
		{ VulkanDeviceExtensions::V_DE_ext_depth_clamp_zero_one,               "VK_EXT_depth_clamp_zero_one"},
		{ VulkanDeviceExtensions::V_DE_ext_depth_clip_control,                 "VK_EXT_depth_clip_control"},
		{ VulkanDeviceExtensions::V_DE_ext_device_generated_commands,          "VK_EXT_device_generated_commands"},
		{ VulkanDeviceExtensions::V_DE_ext_dynamic_rendering_unused_attachments,"VK_EXT_dynamic_rendering_unused_attachments"},
		{ VulkanDeviceExtensions::V_DE_ext_extended_dynamic_state3,            "VK_EXT_extended_dynamic_state3"},
		{ VulkanDeviceExtensions::V_DE_ext_fragment_density_map,               "VK_EXT_fragment_density_map"},
		{ VulkanDeviceExtensions::V_DE_ext_fragment_density_map2,              "VK_EXT_fragment_density_map2"},
		{ VulkanDeviceExtensions::V_DE_ext_fragment_density_map_offset,        "VK_EXT_fragment_density_map_offset"},
		{ VulkanDeviceExtensions::V_DE_ext_frame_boundary,                     "VK_EXT_frame_boundary"},
		{ VulkanDeviceExtensions::V_DE_ext_image_2d_view_of_3d,                "VK_EXT_image_2d_view_of_3d"},
		{ VulkanDeviceExtensions::V_DE_ext_image_compression_control,          "VK_EXT_image_compression_control"},
		{ VulkanDeviceExtensions::V_DE_ext_image_compression_control_swapchain,"VK_EXT_image_compression_control_swapchain"},
		{ VulkanDeviceExtensions::V_DE_ext_image_sliced_view_of_3d,            "VK_EXT_image_sliced_view_of_3d"},
		{ VulkanDeviceExtensions::V_DE_ext_image_view_min_lod,                 "VK_EXT_image_view_min_lod"},
		{ VulkanDeviceExtensions::V_DE_ext_legacy_dithering,                   "VK_EXT_legacy_dithering"},
		{ VulkanDeviceExtensions::V_DE_ext_legacy_vertex_attributes,           "VK_EXT_legacy_vertex_attributes"},
		{ VulkanDeviceExtensions::V_DE_ext_load_store_op_none,                 "VK_EXT_load_store_op_none"},
		{ VulkanDeviceExtensions::V_DE_ext_multisampled_render_to_single_sampled,"VK_EXT_multisampled_render_to_single_sampled"},
		{ VulkanDeviceExtensions::V_DE_ext_multi_draw,                         "VK_EXT_multi_draw"},
		{ VulkanDeviceExtensions::V_DE_ext_nested_command_buffer,              "VK_EXT_nested_command_buffer"},
		{ VulkanDeviceExtensions::V_DE_ext_non_seamless_cube_map,              "VK_EXT_non_seamless_cube_map"},
		{ VulkanDeviceExtensions::V_DE_ext_opacity_micromap,                   "VK_EXT_opacity_micromap"},
		{ VulkanDeviceExtensions::V_DE_ext_pipeline_creation_cache_control,    "VK_EXT_pipeline_creation_cache_control"},
		{ VulkanDeviceExtensions::V_DE_ext_pipeline_creation_feedback,         "VK_EXT_pipeline_creation_feedback"},
		{ VulkanDeviceExtensions::V_DE_ext_pipeline_properties,                "VK_EXT_pipeline_properties"},
		{ VulkanDeviceExtensions::V_DE_ext_pipeline_protected_access,          "VK_EXT_pipeline_protected_access"},
		{ VulkanDeviceExtensions::V_DE_ext_pipeline_robustness,                "VK_EXT_pipeline_robustness"},
		{ VulkanDeviceExtensions::V_DE_ext_primitives_generated_query,         "VK_EXT_primitives_generated_query"},
		{ VulkanDeviceExtensions::V_DE_ext_primitive_topology_list_restart,    "VK_EXT_primitive_topology_list_restart"},
		{ VulkanDeviceExtensions::V_DE_ext_rasterization_order_attachment_access,"VK_EXT_rasterization_order_attachment_access"},
		{ VulkanDeviceExtensions::V_DE_ext_rgba10x6_formats,                   "VK_EXT_rgba10x6_formats"},
		{ VulkanDeviceExtensions::V_DE_ext_sampler_filter_minmax,              "VK_EXT_sampler_filter_minmax"},
		{ VulkanDeviceExtensions::V_DE_ext_separate_stencil_usage,             "VK_EXT_separate_stencil_usage"},
		{ VulkanDeviceExtensions::V_DE_ext_subpass_merge_feedback,             "VK_EXT_subpass_merge_feedback"},
		{ VulkanDeviceExtensions::V_DE_ext_transform_feedback,                 "VK_EXT_transform_feedback"},
		{ VulkanDeviceExtensions::V_DE_ext_validation_cache,                   "VK_EXT_validation_cache"},
		{ VulkanDeviceExtensions::V_DE_khr_create_renderpass2,                 "VK_KHR_create_renderpass2"},
		{ VulkanDeviceExtensions::V_DE_khr_depth_clamp_zero_one,               "VK_KHR_depth_clamp_zero_one"},
		{ VulkanDeviceExtensions::V_DE_khr_depth_stencil_resolve,              "VK_KHR_depth_stencil_resolve"},
		{ VulkanDeviceExtensions::V_DE_khr_draw_indirect_count,                "VK_KHR_draw_indirect_count"},
		{ VulkanDeviceExtensions::V_DE_khr_dynamic_rendering,                  "VK_KHR_dynamic_rendering"},
		{ VulkanDeviceExtensions::V_DE_khr_dynamic_rendering_local_read,       "VK_KHR_dynamic_rendering_local_read"},
		{ VulkanDeviceExtensions::V_DE_khr_format_feature_flags2,              "VK_KHR_format_feature_flags2"},
		{ VulkanDeviceExtensions::V_DE_khr_imageless_framebuffer,              "VK_KHR_imageless_framebuffer"},
		{ VulkanDeviceExtensions::V_DE_khr_load_store_op_none,                 "VK_KHR_load_store_op_none"},
		{ VulkanDeviceExtensions::V_DE_khr_multiview,                          "VK_KHR_multiview"},
		{ VulkanDeviceExtensions::V_DE_khr_portability_subset,                 "VK_KHR_portability_subset"},
		{ VulkanDeviceExtensions::V_DE_khr_sampler_mirror_clamp_to_edge,       "VK_KHR_sampler_mirror_clamp_to_edge"},
		{ VulkanDeviceExtensions::V_DE_khr_sampler_ycbcr_conversion,           "VK_KHR_sampler_ycbcr_conversion"},
		{ VulkanDeviceExtensions::V_DE_khr_separate_depth_stencil_layouts,     "VK_KHR_separate_depth_stencil_layouts"},

		// DESCRIPTORS

		{ VulkanDeviceExtensions::V_DE_ext_descriptor_buffer,                  "VK_EXT_descriptor_buffer"},
		{ VulkanDeviceExtensions::V_DE_ext_descriptor_indexing,                "VK_EXT_descriptor_indexing"},
		{ VulkanDeviceExtensions::V_DE_ext_inline_uniform_block,               "VK_EXT_inline_uniform_block"},
		{ VulkanDeviceExtensions::V_DE_ext_mutable_descriptor_type,            "VK_EXT_mutable_descriptor_type"},
		{ VulkanDeviceExtensions::V_DE_khr_descriptor_update_template,         "VK_KHR_descriptor_update_template"},
		{ VulkanDeviceExtensions::V_DE_khr_push_descriptor,                    "VK_KHR_push_descriptor"},

		// MEMORY & ADDRESSING

		{ VulkanDeviceExtensions::V_DE_ext_buffer_device_address,              "VK_EXT_buffer_device_address"},
		{ VulkanDeviceExtensions::V_DE_ext_device_address_binding_report,      "VK_EXT_device_address_binding_report"},
		{ VulkanDeviceExtensions::V_DE_ext_device_memory_report,               "VK_EXT_device_memory_report"},
		{ VulkanDeviceExtensions::V_DE_ext_external_memory_acquire_unmodified, "VK_EXT_external_memory_acquire_unmodified"},
		{ VulkanDeviceExtensions::V_DE_ext_map_memory_placed,                  "VK_EXT_map_memory_placed"},
		{ VulkanDeviceExtensions::V_DE_ext_memory_priority,                    "VK_EXT_memory_priority"},
		{ VulkanDeviceExtensions::V_DE_ext_pageable_device_local_memory,       "VK_EXT_pageable_device_local_memory"},
		{ VulkanDeviceExtensions::V_DE_ext_zero_initialize_device_memory,      "VK_EXT_zero_initialize_device_memory"},
		{ VulkanDeviceExtensions::V_DE_khr_bind_memory2,                       "VK_KHR_bind_memory2"},
		{ VulkanDeviceExtensions::V_DE_khr_buffer_device_address,              "VK_KHR_buffer_device_address"},
		{ VulkanDeviceExtensions::V_DE_khr_dedicated_allocation,               "VK_KHR_dedicated_allocation"},
		{ VulkanDeviceExtensions::V_DE_khr_external_memory,                    "VK_KHR_external_memory"},
		{ VulkanDeviceExtensions::V_DE_khr_external_memory_win32,              "VK_KHR_external_memory_win32"},
		{ VulkanDeviceExtensions::V_DE_khr_get_memory_requirements2,           "VK_KHR_get_memory_requirements2"},
		{ VulkanDeviceExtensions::V_DE_khr_image_format_list,                  "VK_KHR_image_format_list"},
		{ VulkanDeviceExtensions::V_DE_khr_map_memory2,                        "VK_KHR_map_memory2"},
		{ VulkanDeviceExtensions::V_DE_khr_unified_image_layouts,              "VK_KHR_unified_image_layouts"},

		// SYNCHRONIZATION

		{ VulkanDeviceExtensions::V_DE_khr_external_fence,                     "VK_KHR_external_fence"},
		{ VulkanDeviceExtensions::V_DE_khr_external_fence_win32,               "VK_KHR_external_fence_win32"},
		{ VulkanDeviceExtensions::V_DE_khr_external_semaphore,                 "VK_KHR_external_semaphore"},
		{ VulkanDeviceExtensions::V_DE_khr_external_semaphore_win32,           "VK_KHR_external_semaphore_win32"},
		{ VulkanDeviceExtensions::V_DE_khr_timeline_semaphore,                 "VK_KHR_timeline_semaphore"},
		{ VulkanDeviceExtensions::V_DE_khr_win32_keyed_mutex,                  "VK_KHR_win32_keyed_mutex"},

		// PRESENTATION / SWAPCHAIN

		{ VulkanDeviceExtensions::V_DE_ext_full_screen_exclusive,              "VK_EXT_full_screen_exclusive"},
		{ VulkanDeviceExtensions::V_DE_ext_present_mode_fifo_latest_ready,     "VK_EXT_present_mode_fifo_latest_ready"},
		{ VulkanDeviceExtensions::V_DE_ext_swapchain_maintenance1,             "VK_EXT_swapchain_maintenance1"},
		{ VulkanDeviceExtensions::V_DE_khr_present_id,                         "VK_KHR_present_id"},
		{ VulkanDeviceExtensions::V_DE_khr_present_id2,                        "VK_KHR_present_id2"},
		{ VulkanDeviceExtensions::V_DE_khr_present_wait,                       "VK_KHR_present_wait"},
		{ VulkanDeviceExtensions::V_DE_khr_present_wait2,                      "VK_KHR_present_wait2"},

		// DEBUG & TOOLING

		{ VulkanDeviceExtensions::V_DE_ext_calibrated_timestamps,              "VK_EXT_calibrated_timestamps"},
		{ VulkanDeviceExtensions::V_DE_ext_debug_marker,                       "VK_EXT_debug_marker"},
		{ VulkanDeviceExtensions::V_DE_ext_device_fault,                       "VK_EXT_device_fault"},
		{ VulkanDeviceExtensions::V_DE_ext_global_priority,                    "VK_EXT_global_priority"},
		{ VulkanDeviceExtensions::V_DE_ext_global_priority_query,              "VK_EXT_global_priority_query"},
		{ VulkanDeviceExtensions::V_DE_ext_host_image_copy,                    "VK_EXT_host_image_copy"},
		{ VulkanDeviceExtensions::V_DE_ext_host_query_reset,                   "VK_EXT_host_query_reset"},
		{ VulkanDeviceExtensions::V_DE_ext_private_data,                       "VK_EXT_private_data"},
		{ VulkanDeviceExtensions::V_DE_ext_tooling_info,                       "VK_EXT_tooling_info"},
		{ VulkanDeviceExtensions::V_DE_khr_deferred_host_operations,           "VK_KHR_deferred_host_operations"},
		{ VulkanDeviceExtensions::V_DE_khr_driver_properties,                  "VK_KHR_driver_properties"},
		{ VulkanDeviceExtensions::V_DE_khr_pipeline_executable_properties,     "VK_KHR_pipeline_executable_properties"},

		// PIPELINE LIBRARIES

		{ VulkanDeviceExtensions::V_DE_ext_graphics_pipeline_library,          "VK_EXT_graphics_pipeline_library"},
		{ VulkanDeviceExtensions::V_DE_ext_pipeline_library_group_handles,     "VK_EXT_pipeline_library_group_handles"},
		{ VulkanDeviceExtensions::V_DE_khr_pipeline_binary,                    "VK_KHR_pipeline_binary"},
		{ VulkanDeviceExtensions::V_DE_khr_pipeline_library,                   "VK_KHR_pipeline_library"},

		// MAINTENANCE

		{ VulkanDeviceExtensions::V_DE_khr_maintenance1,                       "VK_KHR_maintenance1"},
		{ VulkanDeviceExtensions::V_DE_khr_maintenance2,                       "VK_KHR_maintenance2"},
		{ VulkanDeviceExtensions::V_DE_khr_maintenance3,                       "VK_KHR_maintenance3"},
		{ VulkanDeviceExtensions::V_DE_khr_maintenance4,                       "VK_KHR_maintenance4"},
		{ VulkanDeviceExtensions::V_DE_khr_maintenance5,                       "VK_KHR_maintenance5"},
		{ VulkanDeviceExtensions::V_DE_khr_maintenance6,                       "VK_KHR_maintenance6"},
		{ VulkanDeviceExtensions::V_DE_khr_maintenance7,                       "VK_KHR_maintenance7"},
		{ VulkanDeviceExtensions::V_DE_khr_maintenance8,                       "VK_KHR_maintenance8"},
		{ VulkanDeviceExtensions::V_DE_khr_maintenance9,                       "VK_KHR_maintenance9"},

		// DEVICE / GROUPING

		{ VulkanDeviceExtensions::V_DE_khr_device_group,                       "VK_KHR_device_group"},

		// MISC / OTHER

		{ VulkanDeviceExtensions::V_DE_ext_provoking_vertex,                   "VK_EXT_provoking_vertex"},
		{ VulkanDeviceExtensions::V_DE_ext_vertex_attribute_divisor,           "VK_EXT_vertex_attribute_divisor"},
		{ VulkanDeviceExtensions::V_DE_ext_vertex_attribute_robustness,        "VK_EXT_vertex_attribute_robustness"},
		{ VulkanDeviceExtensions::V_DE_khr_relaxed_block_layout,               "VK_KHR_relaxed_block_layout"},
		{ VulkanDeviceExtensions::V_DE_khr_robustness2,                        "VK_KHR_robustness2"},
		{ VulkanDeviceExtensions::V_DE_khr_uniform_buffer_standard_layout,     "VK_KHR_uniform_buffer_standard_layout"}
	};
}

#endif //KALAWINDOW_SUPPORT_VULKAN