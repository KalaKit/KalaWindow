//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

/*
#include <unordered_map>

namespace KalaWindow::Graphics::Vulkan
{
    using std::unordered_map;

    //
    // LAYERS
    //

    enum class VulkanLayers
    {
        // MAIN LAYER

        V_IL_khronos_validation,

        // SUB-LAYERS

        V_IL_khronos_profiles,
        V_IL_khronos_shader_object,
        V_IL_khronos_synchronization2,

        V_IL_lunarg_api_dump,
        V_IL_lunarg_crash_diagnostic,
        V_IL_lunarg_gfxreconstruct,
        V_IL_lunarg_monitor,
        V_IL_lunarg_screenshot
    };

    static const unordered_map<VulkanLayers, const char*> vulkanLayerInfo =
    {
        { VulkanLayers::V_IL_khronos_validation,       "VK_LAYER_KHRONOS_validation" },

        { VulkanLayers::V_IL_khronos_profiles,         "VK_LAYER_KHRONOS_profiles" },
        { VulkanLayers::V_IL_khronos_shader_object,    "VK_LAYER_KHRONOS_shader_object" },
        { VulkanLayers::V_IL_khronos_synchronization2, "VK_LAYER_KHRONOS_synchronization2" },

        { VulkanLayers::V_IL_lunarg_api_dump,          "VK_LAYER_LUNARG_api_dump" },
        { VulkanLayers::V_IL_lunarg_crash_diagnostic,  "VK_LAYER_LUNARG_crash_diagnostic" },
        { VulkanLayers::V_IL_lunarg_gfxreconstruct,    "VK_LAYER_LUNARG_gfxreconstruct" },
        { VulkanLayers::V_IL_lunarg_monitor,           "VK_LAYER_LUNARG_monitor" },
        { VulkanLayers::V_IL_lunarg_screenshot,        "VK_LAYER_LUNARG_screenshot" },
    };

    //
    // INSTANCE EXTENSIONS
    //

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
        V_IE_khr_xlib_surface,

        // Display

        V_IE_ext_acquire_xlib_display,
        V_IE_ext_direct_mode_display,
        V_IE_ext_display_surface_counter,
        V_IE_khr_display,
        V_IE_khr_get_display_properties2,

        // Device/Feature Introspection

        V_IE_khr_get_physical_device_properties2,

        // External Capabilities

        V_IE_khr_external_semaphore_capabilities,
        V_IE_khr_external_memory_capabilities,
        V_IE_khr_external_fence_capabilities,

        // Device Groups

        V_IE_khr_device_group_creation,

        // Debug / Validation / Settings

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
        { VulkanInstanceExtensions::V_IE_khr_xlib_surface,                      "VK_KHR_xlib_surface" },

        // Display

        { VulkanInstanceExtensions::V_IE_ext_acquire_xlib_display,              "VK_EXT_acquire_xlib_display" },
        { VulkanInstanceExtensions::V_IE_ext_direct_mode_display,               "VK_EXT_direct_mode_display" },
        { VulkanInstanceExtensions::V_IE_ext_display_surface_counter,           "VK_EXT_display_surface_counter" },
        { VulkanInstanceExtensions::V_IE_khr_display,                           "VK_KHR_display" },
        { VulkanInstanceExtensions::V_IE_khr_get_display_properties2,           "VK_KHR_get_display_properties2" },

        // Device/Feature Introspection

        { VulkanInstanceExtensions::V_IE_khr_get_physical_device_properties2,   "VK_KHR_get_physical_device_properties2" },

        // External Capabilities

        { VulkanInstanceExtensions::V_IE_khr_external_semaphore_capabilities,   "VK_KHR_external_semaphore_capabilities" },
        { VulkanInstanceExtensions::V_IE_khr_external_memory_capabilities,      "VK_KHR_external_memory_capabilities" },
        { VulkanInstanceExtensions::V_IE_khr_external_fence_capabilities,       "VK_KHR_external_fence_capabilities" },

        // Device Groups

        { VulkanInstanceExtensions::V_IE_khr_device_group_creation,             "VK_KHR_device_group_creation" },

        // Debug / Validation / Settings

        { VulkanInstanceExtensions::V_IE_ext_debug_report,                      "VK_EXT_debug_report" },
        { VulkanInstanceExtensions::V_IE_ext_debug_utils,                       "VK_EXT_debug_utils" },
        { VulkanInstanceExtensions::V_IE_ext_layer_settings,                    "VK_EXT_layer_settings" },
        { VulkanInstanceExtensions::V_IE_ext_validation_features,               "VK_EXT_validation_features" },
        { VulkanInstanceExtensions::V_IE_ext_validation_flags,                  "VK_EXT_validation_flags" },

        // Portability

        { VulkanInstanceExtensions::V_IE_khr_portability_enumeration,           "VK_KHR_portability_enumeration" }
    };

    //
    // DEVICE EXTENSIONS
    //

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

        // External Memory & Interop

        V_DE_ext_external_memory_acquire_unmodified,
        V_DE_ext_external_memory_dma_buf,
        V_DE_ext_external_memory_host,
        V_DE_ext_queue_family_foreign,
        V_DE_khr_deferred_host_operations,
        V_DE_khr_external_fence,
        V_DE_khr_external_fence_fd,
        V_DE_khr_external_fence_win32,
        V_DE_khr_external_memory,
        V_DE_khr_external_memory_fd,
        V_DE_khr_external_memory_win32,
        V_DE_khr_external_semaphore,
        V_DE_khr_external_semaphore_fd,
        V_DE_khr_external_semaphore_win32,
        V_DE_khr_win32_keyed_mutex,

        // Device Group & Multi-GPU

        V_DE_khr_device_group,

        // Ray Tracing & Acceleration

        V_DE_khr_acceleration_structure,
        V_DE_khr_ray_query,
        V_DE_khr_ray_tracing_maintenance1,
        V_DE_khr_ray_tracing_pipeline,
        V_DE_khr_ray_tracing_position_fetch,

        // Shader Features

        V_DE_ext_fragment_shader_interlock,
        V_DE_ext_mesh_shader,
        V_DE_ext_scalar_block_layout,
        V_DE_ext_shader_atomic_float,
        V_DE_ext_shader_atomic_float2,
        V_DE_ext_shader_float8,
        V_DE_ext_shader_image_atomic_int64,
        V_DE_ext_shader_replicated_composites,
        V_DE_ext_shader_stencil_export,
        V_DE_ext_shader_subgroup_ballot,
        V_DE_ext_shader_subgroup_vote,
        V_DE_ext_shader_viewport_index_layer,
        V_DE_khr_16bit_storage,
        V_DE_khr_8bit_storage,
        V_DE_khr_compute_shader_derivatives,
        V_DE_khr_cooperative_matrix,
        V_DE_khr_fragment_shader_barycentric,
        V_DE_khr_relaxed_block_layout,
        V_DE_khr_shader_atomic_int64,
        V_DE_khr_shader_bfloat16,
        V_DE_khr_shader_clock,
        V_DE_khr_shader_draw_parameters,
        V_DE_khr_shader_float16_int8,
        V_DE_khr_shader_float_controls,
        V_DE_khr_shader_maximal_reconvergence,
        V_DE_khr_shader_quad_control,
        V_DE_khr_shader_relaxed_extended_instruction,
        V_DE_khr_shader_subgroup_extended_types,
        V_DE_khr_shader_subgroup_uniform_control_flow,
        V_DE_khr_spirv_1_4,
        V_DE_khr_storage_buffer_storage_class,
        V_DE_khr_uniform_buffer_standard_layout,
        V_DE_khr_variable_pointers,
        V_DE_khr_vulkan_memory_model,
        V_DE_khr_workgroup_memory_explicit_layout,

        // Pipeline & Rendering

        V_DE_ext_attachment_feedback_loop_dynamic_state,
        V_DE_ext_attachment_feedback_loop_layout,
        V_DE_ext_blend_operation_advanced,
        V_DE_ext_color_write_enable,
        V_DE_ext_conditional_rendering,
        V_DE_ext_conservative_rasterization,
        V_DE_ext_depth_bias_control,
        V_DE_ext_discard_rectangles,
        V_DE_ext_frame_boundary,
        V_DE_ext_graphics_pipeline_library,
        V_DE_ext_hdr_metadata,
        V_DE_ext_multi_draw,
        V_DE_ext_multisampled_render_to_single_sampled,
        V_DE_ext_pipeline_library_group_handles,
        V_DE_ext_pipeline_properties,
        V_DE_ext_post_depth_coverage,
        V_DE_ext_primitive_topology_list_restart,
        V_DE_ext_primitives_generated_query,
        V_DE_ext_provoking_vertex,
        V_DE_ext_rasterization_order_attachment_access,
        V_DE_ext_sample_locations,
        V_DE_ext_separate_stencil_usage,
        V_DE_ext_subpass_merge_feedback,
        V_DE_ext_transform_feedback,
        V_DE_khr_create_renderpass2,
        V_DE_khr_depth_stencil_resolve,
        V_DE_khr_fragment_shading_rate,
        V_DE_khr_imageless_framebuffer,
        V_DE_khr_maintenance1,
        V_DE_khr_maintenance2,
        V_DE_khr_maintenance3,
        V_DE_khr_maintenance7,
        V_DE_khr_maintenance8,
        V_DE_khr_maintenance9,
        V_DE_khr_multiview,
        V_DE_khr_pipeline_executable_properties,
        V_DE_khr_pipeline_library,
        V_DE_khr_separate_depth_stencil_layouts,

        // Dynamic State & Commands

        V_DE_ext_descriptor_indexing,
        V_DE_ext_extended_dynamic_state3,
        V_DE_ext_host_query_reset,
        V_DE_ext_mutable_descriptor_type,
        V_DE_ext_nested_command_buffer,
        V_DE_ext_vertex_input_dynamic_state,
        V_DE_khr_descriptor_update_template,
        V_DE_khr_draw_indirect_count,
        V_DE_khr_timeline_semaphore,

        // Memory, Address & Robustness

        V_DE_ext_buffer_device_address,
        V_DE_ext_custom_border_color,
        V_DE_ext_depth_clamp_control,
        V_DE_ext_device_address_binding_report,
        V_DE_ext_device_fault,
        V_DE_ext_device_memory_report,
        V_DE_ext_memory_budget,
        V_DE_ext_memory_priority,
        V_DE_ext_pageable_device_local_memory,
        V_DE_ext_zero_initialize_device_memory,
        V_DE_khr_bind_memory2,
        V_DE_khr_buffer_device_address,
        V_DE_khr_dedicated_allocation,
        V_DE_khr_get_memory_requirements2,
        V_DE_khr_robustness2,

        // Image & Format / Compression

        V_DE_ext_astc_decode_mode,
        V_DE_ext_border_color_swizzle,
        V_DE_ext_depth_clip_control,
        V_DE_ext_depth_clip_enable,
        V_DE_ext_depth_range_unrestricted,
        V_DE_ext_filter_cubic,
        V_DE_ext_fragment_density_map,
        V_DE_ext_fragment_density_map2,
        V_DE_ext_image_2d_view_of_3d,
        V_DE_ext_image_compression_control,
        V_DE_ext_image_compression_control_swapchain,
        V_DE_ext_image_sliced_view_of_3d,
        V_DE_ext_image_view_min_lod,
        V_DE_ext_non_seamless_cube_map,
        V_DE_ext_rgba10x6_formats,
        V_DE_ext_sampler_filter_minmax,
        V_DE_ext_ycbcr_image_arrays,
        V_DE_khr_depth_clamp_zero_one,
        V_DE_khr_image_format_list,
        V_DE_khr_sampler_mirror_clamp_to_edge,
        V_DE_khr_sampler_ycbcr_conversion,
        V_DE_khr_unified_image_layouts,

        // Performance & Timing

        V_DE_khr_calibrated_timestamps,
        V_DE_khr_performance_query,

        // Debug / Validation / Tooling

        V_DE_ext_validation_cache,
        V_DE_khr_driver_properties,

        // Miscellaneous

        V_DE_ext_legacy_dithering,
        V_DE_ext_legacy_vertex_attributes,
        V_DE_ext_pci_bus_info
    };

    static const unordered_map<VulkanDeviceExtensions, const char*> vulkanDeviceExtensionsInfo =
    {
        // Swapchain & Presentation

        { VulkanDeviceExtensions::V_DE_ext_display_control,                   "VK_EXT_display_control" },
        { VulkanDeviceExtensions::V_DE_ext_full_screen_exclusive,             "VK_EXT_full_screen_exclusive" },
        { VulkanDeviceExtensions::V_DE_ext_present_mode_fifo_latest_ready,    "VK_EXT_present_mode_fifo_latest_ready" },
        { VulkanDeviceExtensions::V_DE_ext_swapchain_maintenance1,            "VK_EXT_swapchain_maintenance1" },
        { VulkanDeviceExtensions::V_DE_khr_display_swapchain,                 "VK_KHR_display_swapchain" },
        { VulkanDeviceExtensions::V_DE_khr_incremental_present,               "VK_KHR_incremental_present" },
        { VulkanDeviceExtensions::V_DE_khr_present_id,                        "VK_KHR_present_id" },
        { VulkanDeviceExtensions::V_DE_khr_present_id2,                       "VK_KHR_present_id2" },
        { VulkanDeviceExtensions::V_DE_khr_present_wait,                      "VK_KHR_present_wait" },
        { VulkanDeviceExtensions::V_DE_khr_present_wait2,                     "VK_KHR_present_wait2" },
        { VulkanDeviceExtensions::V_DE_khr_shared_presentable_image,          "VK_KHR_shared_presentable_image" },
        { VulkanDeviceExtensions::V_DE_khr_swapchain,                         "VK_KHR_swapchain" },
        { VulkanDeviceExtensions::V_DE_khr_swapchain_mutable_format,          "VK_KHR_swapchain_mutable_format" },

        // External Memory & Interop

        { VulkanDeviceExtensions::V_DE_ext_external_memory_acquire_unmodified,"VK_EXT_external_memory_acquire_unmodified" },
        { VulkanDeviceExtensions::V_DE_ext_external_memory_dma_buf,           "VK_EXT_external_memory_dma_buf" },
        { VulkanDeviceExtensions::V_DE_ext_external_memory_host,              "VK_EXT_external_memory_host" },
        { VulkanDeviceExtensions::V_DE_ext_queue_family_foreign,              "VK_EXT_queue_family_foreign" },
        { VulkanDeviceExtensions::V_DE_khr_deferred_host_operations,          "VK_KHR_deferred_host_operations" },
        { VulkanDeviceExtensions::V_DE_khr_external_fence,                    "VK_KHR_external_fence" },
        { VulkanDeviceExtensions::V_DE_khr_external_fence_fd,                 "VK_KHR_external_fence_fd" },
        { VulkanDeviceExtensions::V_DE_khr_external_fence_win32,              "VK_KHR_external_fence_win32" },
        { VulkanDeviceExtensions::V_DE_khr_external_memory,                   "VK_KHR_external_memory" },
        { VulkanDeviceExtensions::V_DE_khr_external_memory_fd,                "VK_KHR_external_memory_fd" },
        { VulkanDeviceExtensions::V_DE_khr_external_memory_win32,             "VK_KHR_external_memory_win32" },
        { VulkanDeviceExtensions::V_DE_khr_external_semaphore,                "VK_KHR_external_semaphore" },
        { VulkanDeviceExtensions::V_DE_khr_external_semaphore_fd,             "VK_KHR_external_semaphore_fd" },
        { VulkanDeviceExtensions::V_DE_khr_external_semaphore_win32,          "VK_KHR_external_semaphore_win32" },
        { VulkanDeviceExtensions::V_DE_khr_win32_keyed_mutex,                 "VK_KHR_win32_keyed_mutex" },

        // Device Group & Multi-GPU

        { VulkanDeviceExtensions::V_DE_khr_device_group,                      "VK_KHR_device_group" },

        // Ray Tracing & Acceleration

        { VulkanDeviceExtensions::V_DE_khr_acceleration_structure,            "VK_KHR_acceleration_structure" },
        { VulkanDeviceExtensions::V_DE_khr_ray_query,                         "VK_KHR_ray_query" },
        { VulkanDeviceExtensions::V_DE_khr_ray_tracing_maintenance1,          "VK_KHR_ray_tracing_maintenance1" },
        { VulkanDeviceExtensions::V_DE_khr_ray_tracing_pipeline,              "VK_KHR_ray_tracing_pipeline" },
        { VulkanDeviceExtensions::V_DE_khr_ray_tracing_position_fetch,        "VK_KHR_ray_tracing_position_fetch" },

        // Shader Features

        { VulkanDeviceExtensions::V_DE_ext_fragment_shader_interlock,         "VK_EXT_fragment_shader_interlock" },
        { VulkanDeviceExtensions::V_DE_ext_mesh_shader,                       "VK_EXT_mesh_shader" },
        { VulkanDeviceExtensions::V_DE_ext_scalar_block_layout,               "VK_EXT_scalar_block_layout" },
        { VulkanDeviceExtensions::V_DE_ext_shader_atomic_float,               "VK_EXT_shader_atomic_float" },
        { VulkanDeviceExtensions::V_DE_ext_shader_atomic_float2,              "VK_EXT_shader_atomic_float2" },
        { VulkanDeviceExtensions::V_DE_ext_shader_float8,                     "VK_EXT_shader_float8" },
        { VulkanDeviceExtensions::V_DE_ext_shader_image_atomic_int64,         "VK_EXT_shader_image_atomic_int64" },
        { VulkanDeviceExtensions::V_DE_ext_shader_replicated_composites,      "VK_EXT_shader_replicated_composites" },
        { VulkanDeviceExtensions::V_DE_ext_shader_stencil_export,             "VK_EXT_shader_stencil_export" },
        { VulkanDeviceExtensions::V_DE_ext_shader_subgroup_ballot,            "VK_EXT_shader_subgroup_ballot" },
        { VulkanDeviceExtensions::V_DE_ext_shader_subgroup_vote,              "VK_EXT_shader_subgroup_vote" },
        { VulkanDeviceExtensions::V_DE_ext_shader_viewport_index_layer,       "VK_EXT_shader_viewport_index_layer" },
        { VulkanDeviceExtensions::V_DE_khr_16bit_storage,                     "VK_KHR_16bit_storage" },
        { VulkanDeviceExtensions::V_DE_khr_8bit_storage,                      "VK_KHR_8bit_storage" },
        { VulkanDeviceExtensions::V_DE_khr_compute_shader_derivatives,        "VK_KHR_compute_shader_derivatives" },
        { VulkanDeviceExtensions::V_DE_khr_cooperative_matrix,                "VK_KHR_cooperative_matrix" },
        { VulkanDeviceExtensions::V_DE_khr_fragment_shader_barycentric,       "VK_KHR_fragment_shader_barycentric" },
        { VulkanDeviceExtensions::V_DE_khr_relaxed_block_layout,              "VK_KHR_relaxed_block_layout" },
        { VulkanDeviceExtensions::V_DE_khr_shader_atomic_int64,               "VK_KHR_shader_atomic_int64" },
        { VulkanDeviceExtensions::V_DE_khr_shader_bfloat16,                   "VK_KHR_shader_bfloat16" },
        { VulkanDeviceExtensions::V_DE_khr_shader_clock,                      "VK_KHR_shader_clock" },
        { VulkanDeviceExtensions::V_DE_khr_shader_draw_parameters,            "VK_KHR_shader_draw_parameters" },
        { VulkanDeviceExtensions::V_DE_khr_shader_float16_int8,               "VK_KHR_shader_float16_int8" },
        { VulkanDeviceExtensions::V_DE_khr_shader_float_controls,             "VK_KHR_shader_float_controls" },
        { VulkanDeviceExtensions::V_DE_khr_shader_maximal_reconvergence,      "VK_KHR_shader_maximal_reconvergence" },
        { VulkanDeviceExtensions::V_DE_khr_shader_quad_control,               "VK_KHR_shader_quad_control" },
        { VulkanDeviceExtensions::V_DE_khr_shader_relaxed_extended_instruction,"VK_KHR_shader_relaxed_extended_instruction" },
        { VulkanDeviceExtensions::V_DE_khr_shader_subgroup_extended_types,    "VK_KHR_shader_subgroup_extended_types" },
        { VulkanDeviceExtensions::V_DE_khr_shader_subgroup_uniform_control_flow,"VK_KHR_shader_subgroup_uniform_control_flow" },
        { VulkanDeviceExtensions::V_DE_khr_spirv_1_4,                         "VK_KHR_spirv_1_4" },
        { VulkanDeviceExtensions::V_DE_khr_storage_buffer_storage_class,      "VK_KHR_storage_buffer_storage_class" },
        { VulkanDeviceExtensions::V_DE_khr_uniform_buffer_standard_layout,    "VK_KHR_uniform_buffer_standard_layout" },
        { VulkanDeviceExtensions::V_DE_khr_variable_pointers,                 "VK_KHR_variable_pointers" },
        { VulkanDeviceExtensions::V_DE_khr_vulkan_memory_model,               "VK_KHR_vulkan_memory_model" },
        { VulkanDeviceExtensions::V_DE_khr_workgroup_memory_explicit_layout,  "VK_KHR_workgroup_memory_explicit_layout" },

        // Pipeline & Rendering

        { VulkanDeviceExtensions::V_DE_ext_attachment_feedback_loop_dynamic_state,"VK_EXT_attachment_feedback_loop_dynamic_state" },
        { VulkanDeviceExtensions::V_DE_ext_attachment_feedback_loop_layout,     "VK_EXT_attachment_feedback_loop_layout" },
        { VulkanDeviceExtensions::V_DE_ext_blend_operation_advanced,            "VK_EXT_blend_operation_advanced" },
        { VulkanDeviceExtensions::V_DE_ext_color_write_enable,                  "VK_EXT_color_write_enable" },
        { VulkanDeviceExtensions::V_DE_ext_conditional_rendering,               "VK_EXT_conditional_rendering" },
        { VulkanDeviceExtensions::V_DE_ext_conservative_rasterization,          "VK_EXT_conservative_rasterization" },
        { VulkanDeviceExtensions::V_DE_ext_depth_bias_control,                  "VK_EXT_depth_bias_control" },
        { VulkanDeviceExtensions::V_DE_ext_discard_rectangles,                  "VK_EXT_discard_rectangles" },
        { VulkanDeviceExtensions::V_DE_ext_frame_boundary,                      "VK_EXT_frame_boundary" },
        { VulkanDeviceExtensions::V_DE_ext_graphics_pipeline_library,           "VK_EXT_graphics_pipeline_library" },
        { VulkanDeviceExtensions::V_DE_ext_hdr_metadata,                        "VK_EXT_hdr_metadata" },
        { VulkanDeviceExtensions::V_DE_ext_multi_draw,                          "VK_EXT_multi_draw" },
        { VulkanDeviceExtensions::V_DE_ext_multisampled_render_to_single_sampled,"VK_EXT_multisampled_render_to_single_sampled" },
        { VulkanDeviceExtensions::V_DE_ext_pipeline_library_group_handles,      "VK_EXT_pipeline_library_group_handles" },
        { VulkanDeviceExtensions::V_DE_ext_pipeline_properties,                 "VK_EXT_pipeline_properties" },
        { VulkanDeviceExtensions::V_DE_ext_post_depth_coverage,                 "VK_EXT_post_depth_coverage" },
        { VulkanDeviceExtensions::V_DE_ext_primitive_topology_list_restart,     "VK_EXT_primitive_topology_list_restart" },
        { VulkanDeviceExtensions::V_DE_ext_primitives_generated_query,          "VK_EXT_primitives_generated_query" },
        { VulkanDeviceExtensions::V_DE_ext_provoking_vertex,                    "VK_EXT_provoking_vertex" },
        { VulkanDeviceExtensions::V_DE_ext_rasterization_order_attachment_access,"VK_EXT_rasterization_order_attachment_access" },
        { VulkanDeviceExtensions::V_DE_ext_sample_locations,                    "VK_EXT_sample_locations" },
        { VulkanDeviceExtensions::V_DE_ext_separate_stencil_usage,              "VK_EXT_separate_stencil_usage" },
        { VulkanDeviceExtensions::V_DE_ext_subpass_merge_feedback,              "VK_EXT_subpass_merge_feedback" },
        { VulkanDeviceExtensions::V_DE_ext_transform_feedback,                  "VK_EXT_transform_feedback" },
        { VulkanDeviceExtensions::V_DE_khr_create_renderpass2,                  "VK_KHR_create_renderpass2" },
        { VulkanDeviceExtensions::V_DE_khr_depth_stencil_resolve,               "VK_KHR_depth_stencil_resolve" },
        { VulkanDeviceExtensions::V_DE_khr_fragment_shading_rate,               "VK_KHR_fragment_shading_rate" },
        { VulkanDeviceExtensions::V_DE_khr_imageless_framebuffer,               "VK_KHR_imageless_framebuffer" },
        { VulkanDeviceExtensions::V_DE_khr_maintenance1,                        "VK_KHR_maintenance1" },
        { VulkanDeviceExtensions::V_DE_khr_maintenance2,                        "VK_KHR_maintenance2" },
        { VulkanDeviceExtensions::V_DE_khr_maintenance3,                        "VK_KHR_maintenance3" },
        { VulkanDeviceExtensions::V_DE_khr_maintenance7,                        "VK_KHR_maintenance7" },
        { VulkanDeviceExtensions::V_DE_khr_maintenance8,                        "VK_KHR_maintenance8" },
        { VulkanDeviceExtensions::V_DE_khr_maintenance9,                        "VK_KHR_maintenance9" },
        { VulkanDeviceExtensions::V_DE_khr_multiview,                           "VK_KHR_multiview" },
        { VulkanDeviceExtensions::V_DE_khr_pipeline_executable_properties,      "VK_KHR_pipeline_executable_properties" },
        { VulkanDeviceExtensions::V_DE_khr_pipeline_library,                    "VK_KHR_pipeline_library" },
        { VulkanDeviceExtensions::V_DE_khr_separate_depth_stencil_layouts,      "VK_KHR_separate_depth_stencil_layouts" },

        // Dynamic State & Commands

        { VulkanDeviceExtensions::V_DE_ext_descriptor_indexing,                 "VK_EXT_descriptor_indexing" },
        { VulkanDeviceExtensions::V_DE_ext_extended_dynamic_state3,             "VK_EXT_extended_dynamic_state3" },
        { VulkanDeviceExtensions::V_DE_ext_host_query_reset,                    "VK_EXT_host_query_reset" },
        { VulkanDeviceExtensions::V_DE_ext_mutable_descriptor_type,             "VK_EXT_mutable_descriptor_type" },
        { VulkanDeviceExtensions::V_DE_ext_nested_command_buffer,               "VK_EXT_nested_command_buffer" },
        { VulkanDeviceExtensions::V_DE_ext_vertex_input_dynamic_state,          "VK_EXT_vertex_input_dynamic_state" },
        { VulkanDeviceExtensions::V_DE_khr_descriptor_update_template,          "VK_KHR_descriptor_update_template" },
        { VulkanDeviceExtensions::V_DE_khr_draw_indirect_count,                 "VK_KHR_draw_indirect_count" },
        { VulkanDeviceExtensions::V_DE_khr_timeline_semaphore,                  "VK_KHR_timeline_semaphore" },

        // Memory, Address & Robustness

        { VulkanDeviceExtensions::V_DE_ext_buffer_device_address,               "VK_EXT_buffer_device_address" },
        { VulkanDeviceExtensions::V_DE_ext_custom_border_color,                 "VK_EXT_custom_border_color" },
        { VulkanDeviceExtensions::V_DE_ext_depth_clamp_control,                 "VK_EXT_depth_clamp_control" },
        { VulkanDeviceExtensions::V_DE_ext_device_address_binding_report,       "VK_EXT_device_address_binding_report" },
        { VulkanDeviceExtensions::V_DE_ext_device_fault,                        "VK_EXT_device_fault" },
        { VulkanDeviceExtensions::V_DE_ext_device_memory_report,                "VK_EXT_device_memory_report" },
        { VulkanDeviceExtensions::V_DE_ext_memory_budget,                       "VK_EXT_memory_budget" },
        { VulkanDeviceExtensions::V_DE_ext_memory_priority,                     "VK_EXT_memory_priority" },
        { VulkanDeviceExtensions::V_DE_ext_pageable_device_local_memory,        "VK_EXT_pageable_device_local_memory" },
        { VulkanDeviceExtensions::V_DE_ext_zero_initialize_device_memory,       "VK_EXT_zero_initialize_device_memory" },
        { VulkanDeviceExtensions::V_DE_khr_bind_memory2,                        "VK_KHR_bind_memory2" },
        { VulkanDeviceExtensions::V_DE_khr_buffer_device_address,              "VK_KHR_buffer_device_address" },
        { VulkanDeviceExtensions::V_DE_khr_dedicated_allocation,               "VK_KHR_dedicated_allocation" },
        { VulkanDeviceExtensions::V_DE_khr_get_memory_requirements2,            "VK_KHR_get_memory_requirements2" },
        { VulkanDeviceExtensions::V_DE_khr_robustness2,                        "VK_KHR_robustness2" },

        // Image & Format / Compression

        { VulkanDeviceExtensions::V_DE_ext_astc_decode_mode,                    "VK_EXT_astc_decode_mode" },
        { VulkanDeviceExtensions::V_DE_ext_border_color_swizzle,                "VK_EXT_border_color_swizzle" },
        { VulkanDeviceExtensions::V_DE_ext_depth_clip_control,                  "VK_EXT_depth_clip_control" },
        { VulkanDeviceExtensions::V_DE_ext_depth_clip_enable,                   "VK_EXT_depth_clip_enable" },
        { VulkanDeviceExtensions::V_DE_ext_depth_range_unrestricted,            "VK_EXT_depth_range_unrestricted" },
        { VulkanDeviceExtensions::V_DE_ext_filter_cubic,                        "VK_EXT_filter_cubic" },
        { VulkanDeviceExtensions::V_DE_ext_fragment_density_map,                "VK_EXT_fragment_density_map" },
        { VulkanDeviceExtensions::V_DE_ext_fragment_density_map2,               "VK_EXT_fragment_density_map2" },
        { VulkanDeviceExtensions::V_DE_ext_image_2d_view_of_3d,                 "VK_EXT_image_2d_view_of_3d" },
        { VulkanDeviceExtensions::V_DE_ext_image_compression_control,           "VK_EXT_image_compression_control" },
        { VulkanDeviceExtensions::V_DE_ext_image_compression_control_swapchain, "VK_EXT_image_compression_control_swapchain" },
        { VulkanDeviceExtensions::V_DE_ext_image_sliced_view_of_3d,             "VK_EXT_image_sliced_view_of_3d" },
        { VulkanDeviceExtensions::V_DE_ext_image_view_min_lod,                  "VK_EXT_image_view_min_lod" },
        { VulkanDeviceExtensions::V_DE_ext_non_seamless_cube_map,               "VK_EXT_non_seamless_cube_map" },
        { VulkanDeviceExtensions::V_DE_ext_rgba10x6_formats,                    "VK_EXT_rgba10x6_formats" },
        { VulkanDeviceExtensions::V_DE_ext_sampler_filter_minmax,               "VK_EXT_sampler_filter_minmax" },
        { VulkanDeviceExtensions::V_DE_ext_ycbcr_image_arrays,                  "VK_EXT_ycbcr_image_arrays" },
        { VulkanDeviceExtensions::V_DE_khr_depth_clamp_zero_one,                "VK_KHR_depth_clamp_zero_one" },
        { VulkanDeviceExtensions::V_DE_khr_image_format_list,                   "VK_KHR_image_format_list" },
        { VulkanDeviceExtensions::V_DE_khr_sampler_mirror_clamp_to_edge,        "VK_KHR_sampler_mirror_clamp_to_edge" },
        { VulkanDeviceExtensions::V_DE_khr_sampler_ycbcr_conversion,            "VK_KHR_sampler_ycbcr_conversion" },
        { VulkanDeviceExtensions::V_DE_khr_unified_image_layouts,               "VK_KHR_unified_image_layouts" },

        // Performance & Timing

        { VulkanDeviceExtensions::V_DE_khr_calibrated_timestamps,               "VK_KHR_calibrated_timestamps" },
        { VulkanDeviceExtensions::V_DE_khr_performance_query,                   "VK_KHR_performance_query" },

        // Debug / Validation / Tooling

        { VulkanDeviceExtensions::V_DE_ext_validation_cache,                    "VK_EXT_validation_cache" },
        { VulkanDeviceExtensions::V_DE_khr_driver_properties,                   "VK_KHR_driver_properties" },

        // Miscellaneous

        { VulkanDeviceExtensions::V_DE_ext_legacy_dithering,                    "VK_EXT_legacy_dithering" },
        { VulkanDeviceExtensions::V_DE_ext_legacy_vertex_attributes,            "VK_EXT_legacy_vertex_attributes" },
        { VulkanDeviceExtensions::V_DE_ext_pci_bus_info,                        "VK_EXT_pci_bus_info" }
    };
}
*/