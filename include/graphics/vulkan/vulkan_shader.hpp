//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

/*
#include <string>
#include <functional>
#include <variant>

#include "KalaHeaders/api.hpp"
#include "KalaHeaders/core_types.hpp"
#include "KalaHeaders/logging.hpp"

#include "core/glm_global.hpp"
#include "graphics/window.hpp"

namespace KalaWindow::Graphics::Vulkan
{
	//TODO: FIGURE OUT A BETTER ALTERNATIVE TO LARGE VULKAN STRUCTS

	using KalaHeaders::Log;
	using KalaHeaders::LogType;
	using KalaWindow::Graphics::Window;

	using std::string;
	using std::function;
	using std::variant;

	//Variable type allowed to be used in 'SetPushConstant'
	using PushConstantValue = variant
	<
		bool,
		i32,
		f32,
		vec2,
		vec3,
		vec4,
		mat2,
		mat3,
		mat4
	>;

	enum class ShaderType
	{
		Shader_Vertex,         //VK_SHADER_STAGE_VERTEX_BIT
		Shader_Fragment,       //VK_SHADER_STAGE_FRAGMENT_BIT
		Shader_Geometry,       //VK_SHADER_STAGE_GEOMETRY_BIT
		Shader_Compute,        //VK_SHADER_STAGE_COMPUTE_BIT
		Shader_TessControl,    //VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT
		Shader_TessEvaluation, //VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT

		//REQUIRES OPT-IN EXTENSIONS

		Shader_RayGen,       //VK_SHADER_STAGE_RAYGEN_BIT_KHR
		Shader_AnyHit,       //VK_SHADER_STAGE_ANY_HIT_BIT_KHR
		Shader_ClosestHit,   //VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR
		Shader_Miss,         //VK_SHADER_STAGE_MISS_BIT_KHR
		Shader_Intersection, //VK_SHADER_STAGE_INTERSECTION_BIT_KHR
		Shader_Callable,     //VK_SHADER_STAGE_CALLABLE_BIT_KHR
		Shader_Task,         //VK_SHADER_STAGE_TASK_BIT_EXT
		Shader_Mesh          //VK_SHADER_STAGE_MESH_BIT_EXT
	};

	//Data related to each individual shader file in this shader context
	struct ShaderStage
	{
		ShaderType shaderType;
		string shaderPath;
		uintptr_t shaderModule;
	};

	//Contents of pVertexBindingDescriptions in VulkanData_VertexInputState
	struct VD_VII_BindingDescriptions
	{
		//Binding index, usually 0
		u32 binding = 0;
		//Each vertex size in bytes, usually sizeof(Vertex)
		u32 stride{};
		//VkVertexInputRate, usually VK_VERTEX_INPUT_RATE_VERTEX or VK_VERTEX_INPUT_RATE_INSTANCE
		u32 inputRate = 0;
	};
	//Contents of pVertexAttributeDescriptions in VulkanData_VertexInputState
	struct VD_VII_AttributeDescriptions
	{
		//Location in shader, usually 0
		u32 location = 0;
		//Binding index of this attribute, usually matches VD_VII_BindingDescriptions.binding
		u32 binding = 0;
		//VkFormat, usually VK_FORMAT_R32G32B32_SFLOAT or similar
		u32 format = 106;
		//Vertex struct byte offset, usually calculated via offsetof()
		u32 offset{};
	};
	//VkPipelineVertexInputStateCreateInfo
	struct VulkanData_VertexInputState
	{
		//VkVertexInputBindingDescription, struct to VD_VII_BindingDescriptions
		vector<VD_VII_BindingDescriptions> pVertexBindingDescriptions{};
		//VkVertexInputAttributeDescription, struct to VD_VII_AttributeDescriptions
		vector<VD_VII_AttributeDescriptions> pVertexAttributeDescriptions{};
	};

	//VkPipelineInputAssemblyStateCreateInfo
	struct VulkanData_InputAssemblyState
	{
		//VkStructureType, always VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO
		u32 sType = 20;
		//Extension-specific structure, usually NULL
		uintptr_t pNext = NULL;
		//VkPipelineInputAssemblyStateCreateFlags, usually 0
		u32 flags = 0;
		//VkPrimitiveTopology enum, usually VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST
		u32 topology = 3;
		//VkBool32, usually VK_FALSE
		u32 primitiveRestartEnable = 0U;
	};

	//VkPipelineRasterizationStateCreateInfo
	struct VulkanData_RasterizationState
	{
		//VkStructureType, always VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO
		u32 sType = 23;
		//Extension-specific structure, usually NULL
		uintptr_t pNext = NULL;
		//VkPipelineRasterizationStateCreateFlags, usually 0
		u32 flags = 0;
		//VkBool32, usually VK_FALSE
		u32 depthClampEnable = 0U;
		//VkBool32, usually VK_FALSE
		u32 rasterizerDiscardEnable = 0U;
		//VkPolygonMode enum, usually VK_POLYGON_MODE_FILL
		u32 polygonMode = 0;
		//VkCullModeFlags, usually VK_CULL_MODE_BACK_BIT
		u32 cullMode = 0x00000002;
		//VkFrontFace enum, usually VK_FRONT_FACE_COUNTER_CLOCKWISE
		u32 frontFace = 0;
		//VkBool32, usually VK_FALSE
		u32 depthBiasEnable = 0U;
		//Constant depth bias value, usually 0.0f
		f32 depthBiasConstantFactor = 0.0f;
		//Maximum depth bias clamp, usually 0.0f
		f32 depthBiasClamp = 0.0f;
		//Scalar factor for slope-based bias, usually 0.0f
		f32 depthBiasSlopeFactor = 0.0f;
		//Line width in pixels, usually 1.0f
		f32 lineWidth = 0.0f;
	};

	//VkPipelineColorBlendAttachmentState, contents of pAttachments in VulkanData_ColorBlendState
	struct VD_CBS_Attachments
	{
		//VkBool32, usually VK_FALSE
		u32 blendEnable = 0U;
		//VkBlendFactor enum, usually VK_BLEND_FACTOR_ONE
		u32 srcColorBlendFactor = 1;
		//VkBlendFactor enum, usually VK_BLEND_FACTOR_ZERO
		u32 dstColorBlendFactor = 0;
		//VkBlendOp enum, usually VK_BLEND_OP_ADD
		u32 colorBlendOp = 0;
		//VkBlendFactor enum, usually VK_BLEND_FACTOR_ONE
		u32 srcAlphaBlendFactor = 1;
		//VkBlendFactor enum, usually VK_BLEND_FACTOR_ZERO
		u32 dstAlphaBlendFactor = 0;
		//VkColorComponentFlags, usually
		//  VK_COLOR_COMPONENT_R_BIT
		//  | VK_COLOR_COMPONENT_G_BIT
		//  | VK_COLOR_COMPONENT_B_BIT
		//  | VK_COLOR_COMPONENT_A_BIT
		vector<u32> colorWriteMask =
		{
			0x00000001
			| 0x00000002
			| 0x00000004
			| 0x00000008
		};
	};
	//VkPipelineColorBlendStateCreateInfo
	struct VulkanData_ColorBlendState
	{
		//VkStructureType, always VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO
		u32 sType = 26;
		//Extension-specific structure, usually NULL
		uintptr_t pNext = NULL;
		//VkPipelineColorBlendStateCreateFlags, usually 0
		u32 flags = 0;
		//VkBool32, usually VK_FALSE
		u32 logicOpEnable = 0U;
		//VkLogicOp enum, usually VK_LOGIC_OP_COPY
		u32 logicOp = 3;
		//Number of color attachments, usually 1
		u32 attachmentCount = 1;
		//VkPipelineColorBlendAttachmentState, struct to VD_CBS_Attachments
		VD_CBS_Attachments pAttachments{};
		//Array of blend constants (R, G, B, A), usually all 0.0f
		f32 blendConstants[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	};

	//VkStencilOpState, contents of front/back in VulkanData_DepthStencilState
	struct VD_DSS_FrontBack
	{
		//VkStencilOp enum, usually VK_STENCIL_OP_KEEP
		u32 failOp = 0;
		//VkStencilOp enum, usually VK_STENCIL_OP_KEEP
		u32 passOp = 0;
		//VkStencilOp enum, usually VK_STENCIL_OP_KEEP
		u32 depthFailOp = 0;
		//VkStencilOp enum, usually VK_COMPARE_OP_ALWAYS or VK_COMPARE_OP_LESS
		u32 CompareOp = 7;
		//Bitmask used for stencil comparison, usually 0xFF
		u32 compareMask = 0xFF;
		//Bitmask controlling which bits get written, usually 0xFF
		u32 writeMask = 0xFF;
		//Reference value for stencil test, usually 0
		u32 reference = 0;
	};
	//VkPipelineDepthStencilStateCreateInfo
	//Defines how depth testing and stencil testing works.
	struct VulkanData_DepthStencilState
	{
		//VkStructureType, always VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO
		u32 sType = 25;
		//Extension-specific structure, usually NULL
		uintptr_t pNext = NULL;
		//VkPipelineDepthStencilStateCreateFlags, usually 0
		u32 flags = 0;
		//VkBool32, usually VK_TRUE
		u32 depthTestEnable = 1U;
		//VkBool32, usually VK_TRUE
		u32 depthWriteEnable = 1U;
		//VkCompareOp enum, usually VK_COMPARE_OP_LESS
		u32 depthCompareOp = 1;
		//VkBool32, usually VK_FALSE
		u32 depthBoundsTestEnable = 0U;
		//VkBool32, usually VK_FALSE
		u32 stencilTestEnable = 0U;
		//VkStencilOpState, struct to VD_DSS_Front
		VD_DSS_FrontBack front{};
		//VkStencilOpState, struct to VD_DSS_Back
		VD_DSS_FrontBack back{};
	};

	//VkPipelineColorBlendAttachmentState
	//Only used if 'Shader_TessControl' and 'Shader_TessEvaluation' shaders are also used
	struct VulkanData_TesselationState
	{
		//VkStructureType, always VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO
		u32 sType = 21;
		//Extension-specific structure, usually NULL
		uintptr_t pNext = NULL;
		//VkPipelineTessellationStateCreateFlags, usually 0
		u32 flags = 0;
		//Number of control points per patch, usually 3 or 4
		u32 patchControlPoints = 3;
	};

	//Shader-level shader data passed by the user in its original format
	struct VulkanShaderData
	{
		VulkanData_VertexInputState userVertexInputState{};
		VulkanData_InputAssemblyState userInputAssemblyState{};
		VulkanData_RasterizationState userRasterizationState{};
		VD_CBS_Attachments userColorBlendAttachmentState{};
		VulkanData_ColorBlendState userColorBlendState{};
	};

	class LIB_API Shader_Vulkan
	{
	public:
		//Compiles raw .vert, .frag etc shader files into .spv shader files,
		//should be called BEFORE CreateShader or else CreateShader will not work if spv shaders are missing.
		//  - compiles if no spv files exist of the same name
		//  - compiles if spv files exist but original files are newer
		//  - compiles if forceCompile bool is set to true
		static bool CompileShader(
			const vector<string>& originShaderPaths,
			const vector<string>& targetShaderPaths,
			bool forceCompile = false);

		//Uses previously compiled .spv shader files to create the shader structure
		//required for Vulkan to draw stuff on screen
		static Shader_Vulkan* CreateShader(
			const string& shaderName,
			const vector<ShaderStage>& shaderStages,
			Window* targetWindow,
			const VulkanShaderData& userData);

		static const string& GetShaderTypeName(ShaderType type)
		{
			static string result{};

			switch (type)
			{
			case ShaderType::Shader_Vertex:
				result = "vertex";
				break;
			case ShaderType::Shader_Fragment:
				result = "fragment";
				break;
			case ShaderType::Shader_Geometry:
				result = "geometry";
				break;
			case ShaderType::Shader_Compute:
				result = "compute";
				break;
			case ShaderType::Shader_TessControl:
				result = "tesselation control";
				break;
			case ShaderType::Shader_TessEvaluation:
				result = "tesselation evaluation";
				break;
			case ShaderType::Shader_RayGen:
				result = "raygen";
				break;
			case ShaderType::Shader_AnyHit:
				result = "any hit";
				break;
			case ShaderType::Shader_ClosestHit:
				result = "closest hit";
				break;
			case ShaderType::Shader_Miss:
				result = "miss";
				break;
			case ShaderType::Shader_Intersection:
				result = "intersection";
				break;
			case ShaderType::Shader_Callable:
				result = "callable";
				break;
			case ShaderType::Shader_Task:
				result = "task";
				break;
			case ShaderType::Shader_Mesh:
				result = "mesh";
				break;
			}

			return result;
		}

		const string& GetName() const { return name; }
		void SetName(const string& newName);

		u32 GetID() const { return ID; }
		void SetID(u32 newID) { ID = newID; }

		//Assign new draw commands to be used right after Bind.
		void SetDrawCommands(const function<void()>& newDrawCommands) { drawCommands = newDrawCommands; }

		Window* GetTargetWindow() { return targetWindow; }

		const VulkanShaderData& GetVulkanShaderUserStruct() const { return vulkanShaderData; }
		void SetVulkanShaderUserStruct(const VulkanShaderData& newVulkanShaderData)
		{
			vulkanShaderData = newVulkanShaderData;
		}

		u32 GetPipeline() const { return pipeline; }
		u32 GetLayout() const { return layout; }
		u32 GetDescriptorSetLayout() const { return descriptorSetLayout; }

		const vector<ShaderStage>& GetAllShaders() { return shaders; }

		u32 GetShaderModule(ShaderType type)
		{
			for (const auto& stage : shaders)
			{
				if (stage.shaderType == type)
				{
					return stage.shaderModule;
				}
			}

			string typeStr = GetShaderTypeName(type);

			Log::Print(
				"Shader with type '" + typeStr + "' was not assigned! Returning module 0.",
				"SHADER_VULKAN",
				LogType::LOG_ERROR,
				2);
			return 0;
		}
		const string& GetShaderPath(ShaderType type) const
		{
			static string result{};

			for (const auto& stage : shaders)
			{
				if (stage.shaderType == type)
				{
					return stage.shaderPath;
					break;
				}
			}

			string typeStr = GetShaderTypeName(type);

			Log::Print(
				"Shader with type '" + typeStr + "' was not assigned! Returning empty path.",
				"SHADER_VULKAN",
				LogType::LOG_ERROR,
				2);
			return result;
		}

		//Returns true if this shader is loaded
		bool IsShaderLoaded(ShaderType targetType)
		{
			if (shaders.empty()
				|| pipeline == 0)
			{
				return false;
			}

			for (const auto& stage : shaders)
			{
				if (stage.shaderType == targetType
					&& !stage.shaderPath.empty()
					&& stage.shaderModule != 0)
				{
					return true;
				}
			}

			return false;
		}
		//Returns true if the shader path of this shader type exists
		bool ShaderExists(ShaderType targetType)
		{
			if (shaders.empty()
				|| pipeline == 0)
			{
				return false;
			}

			for (const auto& stage : shaders)
			{
				if (stage.shaderType == targetType
					&& !stage.shaderPath.empty())
				{
					return true;
				}
			}

			return false;
		}

		//Binds the shader pipeline for use in the command buffer.
		//Do not call manually! Already handled via RecordCommandBuffer.
		bool Bind(
			uintptr_t commandBuffer,
			Window* window) const;

		void HotReload();

		//Send a small, fast piece of data that you can send directly to
		//shaders without creating or updating a buffer.
		void SetPushConstant(
			uintptr_t cmdBuffer,
			uintptr_t layout,
			u32 stageFlags,
			u32 offset,
			const PushConstantValue& value);

		//Do not destroy manually, erase from containers.hpp instead
		~Shader_Vulkan();
	private:
		string name{};
		u32 ID{};
		function<void()> drawCommands{}; //The commands relative to this shader that are called inside bind
		Window* targetWindow{};          //The window this shader is attached to

		VulkanShaderData vulkanShaderData{}; //Shader-level VkPipeline data

		uintptr_t pipeline{};            //vkpipeline
		uintptr_t layout{};              //vkpipelinelayout
		uintptr_t descriptorSetLayout{}; //vkdescriptorsetlayout

		vector<ShaderStage> shaders{};
	};
}
*/