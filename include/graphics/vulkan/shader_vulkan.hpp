//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <functional>
#include <variant>

#include "core/platform.hpp"
#include "core/log.hpp"
#include "graphics/window.hpp"

namespace KalaWindow::Graphics::Vulkan
{
	//TODO: FIGURE OUT A BETTER ALTERNATIVE TO LARGE VULKAN STRUCTS

	using KalaWindow::Core::Logger;
	using KalaWindow::Core::LogType;
	using KalaWindow::Graphics::Window;

	using std::string;
	using std::unique_ptr;
	using std::unordered_map;
	using std::vector;
	using std::function;
	using std::variant;

	//Variable type allowed to be used in 'SetPushConstant'
	using PushConstantValue = variant
	<
		bool,
		int32_t,
		float,
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
		uint32_t binding = 0;
		//Each vertex size in bytes, usually sizeof(Vertex)
		uint32_t stride{};
		//VkVertexInputRate, usually VK_VERTEX_INPUT_RATE_VERTEX or VK_VERTEX_INPUT_RATE_INSTANCE
		uint32_t inputRate = 0;
	};
	//Contents of pVertexAttributeDescriptions in VulkanData_VertexInputState
	struct VD_VII_AttributeDescriptions
	{
		//Location in shader, usually 0
		uint32_t location = 0;
		//Binding index of this attribute, usually matches VD_VII_BindingDescriptions.binding
		uint32_t binding = 0;
		//VkFormat, usually VK_FORMAT_R32G32B32_SFLOAT or similar
		uint32_t format = 106;
		//Vertex struct byte offset, usually calculated via offsetof()
		uint32_t offset{};
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
		uint32_t sType = 20;
		//Extension-specific structure, usually NULL
		uintptr_t pNext = NULL;
		//VkPipelineInputAssemblyStateCreateFlags, usually 0
		uint32_t flags = 0;
		//VkPrimitiveTopology enum, usually VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST
		uint32_t topology = 3;
		//VkBool32, usually VK_FALSE
		uint32_t primitiveRestartEnable = 0U;
	};

	//VkPipelineRasterizationStateCreateInfo
	struct VulkanData_RasterizationState
	{
		//VkStructureType, always VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO
		uint32_t sType = 23;
		//Extension-specific structure, usually NULL
		uintptr_t pNext = NULL;
		//VkPipelineRasterizationStateCreateFlags, usually 0
		uint32_t flags = 0;
		//VkBool32, usually VK_FALSE
		uint32_t depthClampEnable = 0U;
		//VkBool32, usually VK_FALSE
		uint32_t rasterizerDiscardEnable = 0U;
		//VkPolygonMode enum, usually VK_POLYGON_MODE_FILL
		uint32_t polygonMode = 0;
		//VkCullModeFlags, usually VK_CULL_MODE_BACK_BIT
		uint32_t cullMode = 0x00000002;
		//VkFrontFace enum, usually VK_FRONT_FACE_COUNTER_CLOCKWISE
		uint32_t frontFace = 0;
		//VkBool32, usually VK_FALSE
		uint32_t depthBiasEnable = 0U;
		//Constant depth bias value, usually 0.0f
		float depthBiasConstantFactor = 0.0f;
		//Maximum depth bias clamp, usually 0.0f
		float depthBiasClamp = 0.0f;
		//Scalar factor for slope-based bias, usually 0.0f
		float depthBiasSlopeFactor = 0.0f;
		//Line width in pixels, usually 1.0f
		float lineWidth = 0.0f;
	};

	//VkPipelineColorBlendAttachmentState, contents of pAttachments in VulkanData_ColorBlendState
	struct VD_CBS_Attachments
	{
		//VkBool32, usually VK_FALSE
		uint32_t blendEnable = 0U;
		//VkBlendFactor enum, usually VK_BLEND_FACTOR_ONE
		uint32_t srcColorBlendFactor = 1;
		//VkBlendFactor enum, usually VK_BLEND_FACTOR_ZERO
		uint32_t dstColorBlendFactor = 0;
		//VkBlendOp enum, usually VK_BLEND_OP_ADD
		uint32_t colorBlendOp = 0;
		//VkBlendFactor enum, usually VK_BLEND_FACTOR_ONE
		uint32_t srcAlphaBlendFactor = 1;
		//VkBlendFactor enum, usually VK_BLEND_FACTOR_ZERO
		uint32_t dstAlphaBlendFactor = 0;
		//VkColorComponentFlags, usually
		//  VK_COLOR_COMPONENT_R_BIT
		//  | VK_COLOR_COMPONENT_G_BIT
		//  | VK_COLOR_COMPONENT_B_BIT
		//  | VK_COLOR_COMPONENT_A_BIT
		vector<uint32_t> colorWriteMask =
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
		uint32_t sType = 26;
		//Extension-specific structure, usually NULL
		uintptr_t pNext = NULL;
		//VkPipelineColorBlendStateCreateFlags, usually 0
		uint32_t flags = 0;
		//VkBool32, usually VK_FALSE
		uint32_t logicOpEnable = 0U;
		//VkLogicOp enum, usually VK_LOGIC_OP_COPY
		uint32_t logicOp = 3;
		//Number of color attachments, usually 1
		uint32_t attachmentCount = 1;
		//VkPipelineColorBlendAttachmentState, struct to VD_CBS_Attachments
		VD_CBS_Attachments pAttachments{};
		//Array of blend constants (R, G, B, A), usually all 0.0f
		float blendConstants[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	};

	//VkStencilOpState, contents of front/back in VulkanData_DepthStencilState
	struct VD_DSS_FrontBack
	{
		//VkStencilOp enum, usually VK_STENCIL_OP_KEEP
		uint32_t failOp = 0;
		//VkStencilOp enum, usually VK_STENCIL_OP_KEEP
		uint32_t passOp = 0;
		//VkStencilOp enum, usually VK_STENCIL_OP_KEEP
		uint32_t depthFailOp = 0;
		//VkStencilOp enum, usually VK_COMPARE_OP_ALWAYS or VK_COMPARE_OP_LESS
		uint32_t CompareOp = 7;
		//Bitmask used for stencil comparison, usually 0xFF
		uint32_t compareMask = 0xFF;
		//Bitmask controlling which bits get written, usually 0xFF
		uint32_t writeMask = 0xFF;
		//Reference value for stencil test, usually 0
		uint32_t reference = 0;
	};
	//VkPipelineDepthStencilStateCreateInfo
	//Defines how depth testing and stencil testing works.
	struct VulkanData_DepthStencilState
	{
		//VkStructureType, always VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO
		uint32_t sType = 25;
		//Extension-specific structure, usually NULL
		uintptr_t pNext = NULL;
		//VkPipelineDepthStencilStateCreateFlags, usually 0
		uint32_t flags = 0;
		//VkBool32, usually VK_TRUE
		uint32_t depthTestEnable = 1U;
		//VkBool32, usually VK_TRUE
		uint32_t depthWriteEnable = 1U;
		//VkCompareOp enum, usually VK_COMPARE_OP_LESS
		uint32_t depthCompareOp = 1;
		//VkBool32, usually VK_FALSE
		uint32_t depthBoundsTestEnable = 0U;
		//VkBool32, usually VK_FALSE
		uint32_t stencilTestEnable = 0U;
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
		uint32_t sType = 21;
		//Extension-specific structure, usually NULL
		uintptr_t pNext = NULL;
		//VkPipelineTessellationStateCreateFlags, usually 0
		uint32_t flags = 0;
		//Number of control points per patch, usually 3 or 4
		uint32_t patchControlPoints = 3;
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

	class KALAWINDOW_API Shader_Vulkan
	{
	public:
		static inline unordered_map<string, unique_ptr<Shader_Vulkan>> createdShaders{};

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
			case ShaderType::Shader_Fragment:
				result = "fragment";
			case ShaderType::Shader_Geometry:
				result = "geometry";
			case ShaderType::Shader_Compute:
				result = "compute";
			case ShaderType::Shader_TessControl:
				result = "tesselation control";
			case ShaderType::Shader_TessEvaluation:
				result = "tesselation evaluation";

			case ShaderType::Shader_RayGen:
				result = "raygen";
			case ShaderType::Shader_AnyHit:
				result = "any hit";
			case ShaderType::Shader_ClosestHit:
				result = "closest hit";
			case ShaderType::Shader_Miss:
				result = "miss";
			case ShaderType::Shader_Intersection:
				result = "intersection";
			case ShaderType::Shader_Callable:
				result = "callable";
			case ShaderType::Shader_Task:
				result = "task";
			case ShaderType::Shader_Mesh:
				result = "mesh";
			}

			return result;
		}

		const string& GetName() const { return name; }
		void SetName(const string& newName)
		{
			if (newName.empty())
			{
				Logger::Print(
					"Cannot set shader name to empty name!",
					"SHADER_VULKAN",
					LogType::LOG_ERROR,
					2);
				return;
			}
			for (const auto& createdShader : createdShaders)
			{
				string thisName = createdShader.first.c_str();
				if (newName == thisName)
				{
					Logger::Print(
						"Cannot set shader name to already existing shader name '" + thisName + "'!",
						"SHADER_VULKAN",
						LogType::LOG_ERROR,
						2);
					return;
				}
			}
			name = newName;
		}

		//Assign new draw commands to be used right after Bind.
		void SetDrawCommands(const function<void()>& newDrawCommands) { drawCommands = newDrawCommands; }

		Window* GetTargetWindow() { return targetWindow; }

		const VulkanShaderData& GetVulkanShaderUserStruct() const { return vulkanShaderData; }
		void SetVulkanShaderUserStruct(const VulkanShaderData& newVulkanShaderData)
		{
			vulkanShaderData = newVulkanShaderData;
		}

		unsigned int GetPipeline() const { return pipeline; }
		unsigned int GetLayout() const { return layout; }
		unsigned int GetDescriptorSetLayout() const { return descriptorSetLayout; }

		const vector<ShaderStage>& GetAllShaders() { return shaders; }

		void SetShaderPath(
			const string& path,
			ShaderType type)
		{
			if (path.empty())
			{
				Logger::Print(
					"Cannot set shader path to empty path!",
					"SHADER_VULKAN",
					LogType::LOG_ERROR,
					2);
				return;
			}

			for (auto& stage : shaders)
			{
				if (stage.shaderType == type)
				{
					stage.shaderPath = path;
					break;
				}
			}
		}

		unsigned int GetShaderModule(ShaderType type)
		{
			for (const auto& stage : shaders)
			{
				if (stage.shaderType == type)
				{
					return stage.shaderModule;
				}
			}

			string typeStr = GetShaderTypeName(type);

			Logger::Print(
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

			Logger::Print(
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
			uint32_t stageFlags,
			uint32_t offset,
			const PushConstantValue& value);

		//Destroys this created shader and its data
		~Shader_Vulkan();
	private:
		string name{};
		function<void()> drawCommands{}; //The commands relative to this shader that are called inside bind
		Window* targetWindow{};          //The window this shader is attached to

		VulkanShaderData vulkanShaderData{}; //Shader-level VkPipeline data

		uintptr_t pipeline{};            //vkpipeline
		uintptr_t layout{};              //vkpipelinelayout
		uintptr_t descriptorSetLayout{}; //vkdescriptorsetlayout

		vector<ShaderStage> shaders{};
	};
}