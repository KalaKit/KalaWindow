//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef KALAWINDOW_SUPPORT_VULKAN

#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>

#include "core/platform.hpp"
#include "graphics/window.hpp"

namespace KalaWindow::Graphics
{
	using std::string;
	using std::unique_ptr;
	using std::unordered_map;
	using std::vector;

	enum class ShaderType
	{
		Shader_Vertex,         //VK_SHADER_STAGE_VERTEX_BIT
		Shader_TessControl,    //VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT
		Shader_TessEvaluation, //VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT
		Shader_Geometry,       //VK_SHADER_STAGE_GEOMETRY_BIT
		Shader_Fragment,       //VK_SHADER_STAGE_FRAGMENT_BIT
		Shader_Compute,        //VK_SHADER_STAGE_COMPUTE_BIT

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

	struct ShaderStage
	{
		ShaderType shaderType;
		string shaderPath;
		uintptr_t shaderModule;
	};
	struct ShaderData
	{
		vector<ShaderStage> stages;

		uintptr_t pipeline;            //vkpipeline
		uintptr_t layout;              //vkpipelinelayout
		uintptr_t descriptorSetLayout; //vkdescriptorsetlayout
	};

	class KALAWINDOW_API Shader_Vulkan
	{
	public:
		static inline unordered_map<string, Shader_Vulkan*> createdShaders{};

		static unique_ptr<Shader_Vulkan> CreateShader(
			const string& shaderName,
			const vector<ShaderStage>& shaderStages);

		static Shader_Vulkan* GetShaderByName(const string& name)
		{
			auto it = createdShaders.find(name);
			return it != createdShaders.end() ? it->second : nullptr;
		}

		vector<ShaderData> GetAllShaderData() { return shaders; }

		//Returns true if this shader is loaded
		bool IsShaderLoaded(
			ShaderType targetType,
			const ShaderData& shaderData)
		{
			if (shaderData.stages.empty()
				|| shaderData.pipeline == 0)
			{
				return false;
			}

			for (const auto& stage : shaderData.stages)
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
		bool ShaderExists(
			ShaderType targetType,
			const ShaderData& shaderData)
		{
			if (shaderData.stages.empty()
				|| shaderData.pipeline == 0)
			{
				return false;
			}

			for (const auto& stage : shaderData.stages)
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
		//Uses vkcommandbuffer internally.
		bool Bind(
			uintptr_t commandBuffer,
			const ShaderData& shaderData) const;

		void HotReload(
			Shader_Vulkan* shader, 
			Window* window);

		//Destroys this created shader and its data
		void DestroyShader();
	private:
		string name;
		vector<ShaderData> shaders;
	};
}

#endif //KALAWINDOW_SUPPORT_VULKAN