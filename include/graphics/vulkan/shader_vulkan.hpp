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
		kvec2,
		kvec3,
		kvec4,
		kmat2,
		kmat3,
		kmat4
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

	//VkPipelineVertexInputStateCreateInfo
	struct VulkanData_VertexInputState
	{
		//VkStructureType, always VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO
		uint32_t sType{};
		//always nullptr
		uintptr_t pNext{};
		//VkPipelineVertexInputStateCreateFlags, 0 unless using extensions
		uint32_t flags{};
		//number of bindings
		uint32_t vertexBindingDescriptionCount{};
		//VkVertexInputBindingDescription, struct to VD_VII_BindingDescriptions
		vector<VD_VII_BindingDescriptions> pVertexBindingDescriptions{};
		//number of attributes
		uint32_t vertexAttributeDescriptionCount{}; 
		//VkVertexInputAttributeDescription, struct to VD_VII_AttributeDescriptions
		vector<VD_VII_AttributeDescriptions> pVertexAttributeDescriptions{};
	};
	//Contents of pVertexBindingDescriptions in VulkanData_VertexInputState
	struct VD_VII_BindingDescriptions
	{
		//binding index - mostly 0
		uint32_t binding{};
		//each vertex size in bytes
		uint32_t stride{};
		//VkVertexInputRate, 0 (VK_VERTEX_INPUT_RATE_VERTEX) or 1 (_INSTANCE)
		uint32_t inputRate{};
	};
	//Contents of pVertexAttributeDescriptions in VulkanData_VertexInputState
	struct VD_VII_AttributeDescriptions
	{
		//location in shader
		uint32_t location{};
		//binding index of this attribute
		uint32_t binding{};
		//VkFormat
		uint32_t format{};
		//vertex struct byte offset
		uint32_t offset{};
	};

	//VkPipelineInputAssemblyStateCreateInfo
	struct VulkanData_InputAssemblyState
	{
		//VkStructureType, always VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO
		uint32_t sType{};
		//???
		uintptr_t pNext{};
		//VkPipelineInputAssemblyStateCreateFlags
		uint32_t flags{};
		//VkPrimitiveTopology enum
		uint32_t topology{};
		//VkBool32
		uint32_t primitiveRestartEnable{};
	};

	//VkPipelineRasterizationStateCreateInfo
	struct VulkanData_RasterizationState
	{
		//VkStructureType, always VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO
		uint32_t sType{};
		//???
		uintptr_t pNext{};
		//VkPipelineRasterizationStateCreateFlags
		uint32_t flags{};
		//VkBool32, usually VK_FALSE
		uint32_t depthClampEnable{};
		//VkBool32, usually VK_FALSE
		uint32_t rasterizerDiscardEnable{};
		//VkPolygonMode enum
		uint32_t polygonMode{};
		//VkCullModeFlags
		uint32_t cullMode{};
		//VkFrontFace enum
		uint32_t frontFace{};
		//VkBool32, usually VK_FALSE
		uint32_t depthBiasEnable{};
		//???
		float depthBiasConstantFactor{};
		//???
		float depthBiasClamp{};
		//???
		float depthBiasSlopeFactor{};
		//usually 1.0f
		float lineWidth{};
	};

	//VkPipelineColorBlendStateCreateInfo
	struct VulkanData_ColorBlendState
	{
		//VkStructureType, always VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO
		uint32_t sType{};
		//???
		uintptr_t pNext{};
		//VkPipelineColorBlendStateCreateFlags
		uint32_t flags{};
		//VkBool32, usually VK_FALSE
		uint32_t logicOpEnable{};
		//VkLogicOp enum
		uint32_t logicOp{};
		//???
		uint32_t attachmentCount{};
		//VkPipelineColorBlendAttachmentState, struct to VD_CBS_Attachments
		VD_CBS_Attachments pAttachments{};
		//???
		float blendConstants[4];
	};
	//VkPipelineColorBlendAttachmentState, contents of pAttachments in VulkanData_ColorBlendState
	struct VD_CBS_Attachments
	{
		//VkBool32, usually VK_FALSE
		uint32_t blendEnable{};
		//VkBlendFactor enum
		uint32_t srcColorBlendFactor{};
		//VkBlendFactor enum
		uint32_t dstColorBlendFactor{};
		//VkBlendOp enum
		uint32_t colorBlendOp{};
		//VkBlendFactor enum
		uint32_t srcAlphaBlendFactor{};
		//VkBlendFactor enum
		uint32_t dstAlphaBlendFactor{};
		//VkColorComponentFlags
		vector<uint32_t> colorWriteMask{};
	};

	//VkPipelineDepthStencilStateCreateInfo
	//Defines how depth testing and stencil testing works.
	struct VulkanData_DepthStencilState
	{
		//VkStructureType
		uint32_t sType{};
		//???
		uintptr_t pNext{};
		//VkPipelineDepthStencilStateCreateFlags
		uint32_t flags{};
		//VkBool32
		uint32_t depthTestEnable{};
		//VkBool32
		uint32_t depthWriteEnable{};
		//VkCompareOp enum
		uint32_t depthCompareOp{};
		//VkBool32
		uint32_t depthBoundsTestEnable{};
		//VkBool32
		uint32_t stencilTestEnable{};
		//VkStencilOpState, struct to VD_DSS_Front
		VD_DSS_Front front{};
		//VkStencilOpState, struct to VD_DSS_Back
		VD_DSS_Back back{};
	};
	//VkStencilOpState, contents of front in VulkanData_DepthStencilState
	struct VD_DSS_Front
	{
		//VkStencilOp enum
		uint32_t failOp{};
		//VkStencilOp enum
		uint32_t passOp{};
		//VkStencilOp enum
		uint32_t depthFailOp{};
		//VkStencilOp enum
		uint32_t CompareOp{};
		//???
		uint32_t compareMask{};
		//???
		uint32_t writeMask{};
		//???
		uint32_t reference{};
	};
	//VkStencilOpState, contents of back in VulkanData_DepthStencilState
	struct VD_DSS_Back
	{
		//VkStencilOp enum
		uint32_t failOp{};
		//VkStencilOp enum
		uint32_t passOp{};
		//VkStencilOp enum
		uint32_t depthFailOp{};
		//VkStencilOp enum
		uint32_t CompareOp{};
		//???
		uint32_t compareMask{};
		//???
		uint32_t writeMask{};
		//???
		uint32_t reference{};
	};

	//VkPipelineColorBlendAttachmentState
	//Only used if 'Shader_TessControl' and 'Shader_TessEvaluation' shaders are also used
	struct VulkanData_TesselationState
	{
		//VkStructureType
		uint32_t sType{};
		//???
		uintptr_t pNext{};
		//VkPipelineTessellationStateCreateFlags
		uint32_t flags{};
		//???
		uint32_t patchControlPoints{};
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
			Window* targetWindow);

		static string GetShaderTypeName(ShaderType type)
		{
			switch (type)
			{
			case ShaderType::Shader_Vertex:
				return "vertex";
			case ShaderType::Shader_Fragment:
				return "fragment";
			case ShaderType::Shader_Geometry:
				return "geometry";
			case ShaderType::Shader_Compute:
				return "compute";
			case ShaderType::Shader_TessControl:
				return "tesselation control";
			case ShaderType::Shader_TessEvaluation:
				return "tesselation evaluation";

			case ShaderType::Shader_RayGen:
				return "raygen";
			case ShaderType::Shader_AnyHit:
				return "any hit";
			case ShaderType::Shader_ClosestHit:
				return "closest hit";
			case ShaderType::Shader_Miss:
				return "miss";
			case ShaderType::Shader_Intersection:
				return "intersection";
			case ShaderType::Shader_Callable:
				return "callable";
			case ShaderType::Shader_Task:
				return "task";
			case ShaderType::Shader_Mesh:
				return "mesh";
			}

			return "";
		}

		const string& GetName() { return name; }
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
		void SetDrawCommands(function<void()> newDrawCommands) { drawCommands = newDrawCommands; }
		//Get the currently assigned draw commands.
		function<void()> GetDrawCommands() { return drawCommands; }

		Window* GetTargetWindow() { return targetWindow; }

		unsigned int GetPipeline() { return pipeline; }
		unsigned int GetLayout() { return layout; }
		unsigned int GetDescriptorSetLayout() { return descriptorSetLayout; }

		vector<ShaderStage> GetAllShaders() { return shaders; }

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
		string GetShaderPath(ShaderType type)
		{
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
			return "";
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

		uintptr_t pipeline{};            //vkpipeline
		uintptr_t layout{};              //vkpipelinelayout
		uintptr_t descriptorSetLayout{}; //vkdescriptorsetlayout

		vector<ShaderStage> shaders{};
	};
}