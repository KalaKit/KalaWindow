//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <vector>
#include <string>
#include <filesystem>
#include <fstream>
#include <format>
#include <sstream>
#include <cassert>

#include <vulkan/vulkan.h>

#include "graphics/vulkan/shader_vulkan.hpp"
#include "graphics/vulkan/vulkan.hpp"
#include "graphics/window.hpp"
#include "core/log.hpp"

using KalaWindow::Graphics::ShutdownState;
using KalaWindow::Graphics::Window;
using KalaWindow::Graphics::PopupAction;
using KalaWindow::Graphics::PopupType;
using KalaWindow::Graphics::PopupResult;
using KalaWindow::Core::Logger;
using KalaWindow::Core::LogType;
using KalaWindow::Core::TimeFormat;
using KalaWindow::Core::DateFormat;
using KalaWindow::Graphics::Vulkan::ShaderType;
using KalaWindow::Graphics::Vulkan::Shader_Vulkan;

using std::vector;
using std::string;
using std::to_string;
using std::ifstream;
using std::ios;
using std::make_unique;
using std::filesystem::path;
using std::filesystem::exists;
using std::streamsize;
using std::format;
using std::stringstream;

static void ForceClose(
    const string& title,
    const string& reason);

static vector<uint32_t> ReadFileBinary(const string& filePath)
{
    ifstream file(filePath, ios::ate | ios::binary);
    if (!file.is_open()) return {};

    streamsize size = file.tellg();
    string fileName = path(filePath).filename().string();

    if (size == 0)
    {
        ForceClose(
            "Vulkan error [shader_vulkan]",
            "Shader '" + fileName + "' is empty or unreadable!");

        return {};
    }
    if (size % 4 != 0)
    {
        ForceClose(
            "Vulkan error [shader_vulkan]",
            "Shader '" + fileName + "' is not aligned to 4 bytes!");

        return {};
    }

    vector<uint32_t> buffer(size / 4);
    file.seekg(0);
    file.read(reinterpret_cast<char*>(buffer.data()), size);
    file.close();

    if (buffer.empty())
    {
        ForceClose(
            "Vulkan error [shader_vulkan]",
            "Shader '" + fileName + "' is empty after read!");

        return {};
    }
    if (buffer.size() >= 4)
    {
        auto msg = format
        (
            "SPV magic    = {:#010x}\n"
            "version      = {:#010x}\n"
            "generator    = {:#010x}\n"
            "id bound     = {}\n",
            buffer[0], //0x07230203
            buffer[1], //version
            buffer[2], //generator
            buffer[3]  //bound
        );

        Logger::Print(
            "\nShader '" + fileName + "' data:\n" + msg,
            "SHADER_VULKAN",
            LogType::LOG_DEBUG);
    }

    return buffer;
}

static bool InitShader(
    ShaderType type,
    const string& shaderPath,
    VkDevice device,
    VkShaderModule& shaderModule,
    VkPipelineShaderStageCreateInfo& shaderStageInfo)
{
    string shaderType = Shader_Vulkan::GetShaderTypeName(type);

    Logger::Print(
        "Loading " + shaderType + " shader: " + shaderPath,
        "SHADER_VULKAN",
        LogType::LOG_INFO);

    auto code = ReadFileBinary(shaderPath);

    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size() * sizeof(uint32_t);
    createInfo.pCode = code.data();

    auto ptrHex = format("{:#x}", reinterpret_cast<uintptr_t>(createInfo.pCode));
    auto sizeDec = to_string(createInfo.codeSize);
    auto result = format("pCode ptr = {}, codeSize = {}", ptrHex, sizeDec);

    string fileName = path(shaderPath).filename().string();

    Logger::Print(
        "\nShader '" + fileName + "' values:\n" + result,
        "SHADER_VULKAN",
        LogType::LOG_DEBUG);

    VkResult res = vkCreateShaderModule(
        device,
        &createInfo,
        nullptr,
        &shaderModule);

    auto resHex = format(
        "{:#x}",
        static_cast<int>(res));
    auto moduleHex = format(
        "{:#x}",
        reinterpret_cast<uintptr_t>(shaderModule));

    Logger::Print(
        "vkCreateShaderModule: " + to_string(res) + " (" + resHex + "), module = " + moduleHex,
        "SHADER_VULKAN",
        LogType::LOG_DEBUG);

    if (res != VK_SUCCESS)
    {
        ForceClose(
            "Vulkan error [shader_vulkan]",
            "Failed to create " + shaderType + " shader module for shader file: " + shaderPath);
        return false;
    }

    shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    switch (type)
    {
    case ShaderType::Shader_Vertex:
        shaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        break;
    case ShaderType::Shader_Fragment:
        shaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        break;
    }
    shaderStageInfo.module = shaderModule;
    shaderStageInfo.pName = "main";

    Logger::Print(
        "Created module for " + shaderType + " shader!",
        "SHADER_VULKAN",
        LogType::LOG_SUCCESS);

    return true;
}

namespace KalaWindow::Graphics::Vulkan
{
	Shader_Vulkan* Shader_Vulkan::CreateShader(
		const string& shaderName,
		const vector<ShaderStage>& shaderStages,
        Window* newWindow)
	{
        unique_ptr<Shader_Vulkan> newShader = make_unique<Shader_Vulkan>();
        Shader_Vulkan* shaderPtr = newShader.get();
        ShaderStage newVertStage{};
        ShaderStage newFragStage{};

        if (shaderName.empty())
        {
            Logger::Print(
                "Cannot create a shader with no name!",
                "SHADER_VULKAN",
                LogType::LOG_ERROR,
                2);
            return nullptr;
        }
        for (const auto& [key, _] : createdShaders)
        {
            if (key == shaderName)
            {
                Logger::Print(
                    "Cannot create a shader with the name '" + shaderName
                    + "' because a shader with that name already exists!",
                    "SHADER_VULKAN",
                    LogType::LOG_ERROR,
                    2);
                return nullptr;
            }
        }

        if (shaderStages.empty())
        {
            Logger::Print(
                "Cannot create a shader with no stages!",
                "SHADER_VULKAN",
                LogType::LOG_ERROR,
                2);
            return nullptr;
        }

        for (const auto& stage : shaderStages)
        {
            string shaderType = GetShaderTypeName(stage.shaderType);

            if (stage.shaderPath.empty())
            {
                Logger::Print(
                    "Shader '" + shaderName + "' with type '"
                    + shaderType + "' has no assigned path!",
                    "SHADER_VULKAN",
                    LogType::LOG_ERROR,
                    2);
                return nullptr;
            }
            else if (!exists(stage.shaderPath))
            {
                Logger::Print(
                    "Shader '" + shaderName + "' with type '"
                    + shaderType + "' has an invalid path '" + stage.shaderPath + "'!",
                    "SHADER_VULKAN",
                    LogType::LOG_ERROR,
                    2);
                return nullptr;
            }
            else
            {
                switch (stage.shaderType)
                {
                case ShaderType::Shader_Vertex:
                    newVertStage.shaderPath = stage.shaderPath;
                    newVertStage.shaderType = stage.shaderType;
                    break;
                case ShaderType::Shader_Fragment:
                    newFragStage.shaderPath = stage.shaderPath;
                    newFragStage.shaderType = stage.shaderType;
                    break;
                }
            }
        }

        bool vertShaderExists = !newVertStage.shaderPath.empty();
        bool fragShaderExists = !newFragStage.shaderPath.empty();

        WindowStruct_Windows& wData = newWindow->GetWindow_Windows();
        Window_VulkanData& vData = newWindow->GetVulkanStruct();

        VkDevice device = ToVar<VkDevice>(Renderer_Vulkan::GetDevice());
        if (device == VK_NULL_HANDLE)
        {
            ForceClose(
                "Vulkan error [shader_vulkan]",
                "Failed to initialize shader " + shaderName + " because device was invalid!\n");
            return nullptr;
        }

        vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos{};

        //
        // CREATE AND COMPILE VERTEX SHADER
        //

        VkShaderModule vertModule = VK_NULL_HANDLE;
        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
        if (!vertShaderExists)
        {
            Logger::Print(
                "Skipped loading vertex shader because it was not assigned as a shader stage.",
                "SHADER_VULKAN",
                LogType::LOG_INFO);
        }
        else
        {
            if (!InitShader(
                ShaderType::Shader_Vertex,
                newVertStage.shaderPath,
                device,
                vertModule,
                vertShaderStageInfo))
            {
                return nullptr;
            }
            shaderStageCreateInfos.push_back(vertShaderStageInfo);
            newVertStage.shaderModule = FromVar<VkShaderModule>(vertModule);
        }

        //
        // CREATE AND COMPILE FRAGMENT SHADER
        //

        VkShaderModule fragModule = VK_NULL_HANDLE;
        VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
        if (!fragShaderExists)
        {
            if (!vertShaderExists)
            {
                Logger::Print(
                    "Skipped loading vertex shader because it was not assigned as a shader stage.",
                    "SHADER_VULKAN",
                    LogType::LOG_INFO);
            }
        }
        else
        {
            if (!InitShader(
                ShaderType::Shader_Fragment,
                newFragStage.shaderPath,
                device,
                fragModule,
                fragShaderStageInfo))
            {
                return nullptr;
            }
            shaderStageCreateInfos.push_back(fragShaderStageInfo);
            newFragStage.shaderModule = FromVar<VkShaderModule>(fragModule);
        }

        //
        // CREATE PIPELINE LAYOUT
        //

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;

        VkPipelineLayout newLayout = VK_NULL_HANDLE;
        if (vkCreatePipelineLayout(
            device,
            &pipelineLayoutInfo,
            nullptr,
            &newLayout) != VK_SUCCESS)
        {
            ForceClose(
                "Vulkan error [shader_vulkan]",
                "Failed to create pipeline layout!");
            return nullptr;
        }
        shaderPtr->layout = FromVar<VkPipelineLayout>(newLayout);

        //
        // CREATE GRAPHICS PIPELINE
        //

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = static_cast<uint32_t>(shaderStageCreateInfos.size());
        pipelineInfo.pStages = shaderStageCreateInfos.data();
        pipelineInfo.layout = ToVar<VkPipelineLayout>(shaderPtr->layout);
        pipelineInfo.renderPass = ToVar<VkRenderPass>(vData.renderPass);
        pipelineInfo.subpass = 0;
        //TODO: add more?

        VkPipeline newPipeline = VK_NULL_HANDLE;
        if (vkCreateGraphicsPipelines(
            device,
            VK_NULL_HANDLE,
            1,
            &pipelineInfo,
            nullptr,
            &newPipeline) != VK_SUCCESS)
        {
            ForceClose(
                "Vulkan error [shader_vulkan]",
                "Failed to create graphics pipeline!");
            return nullptr;
        }
        shaderPtr->pipeline = FromVar<VkPipeline>(newPipeline);

        //
        // CREATE DESCRIPTOR LAYOUT
        //

        //TODO: add as optional?

        //
        // CLEANUP
        //

        if (vertShaderExists) shaderPtr->shaders.push_back(newVertStage);
        if (fragShaderExists) shaderPtr->shaders.push_back(newFragStage);

        newShader->name = shaderName;
        newShader->targetWindow = newWindow;
        createdShaders[shaderName] = move(newShader);

        return shaderPtr;
	}

	bool Shader_Vulkan::Bind(uintptr_t commandBuffer) const
	{
        if (pipeline == 0)
        {
            Logger::Print(
                "Cannot bind Vulkan shader because pipeline handle is null!",
                "SHADER_VULKAN",
                LogType::LOG_ERROR,
                2);
            return false;
        }

        VkCommandBuffer cmd = ToVar<VkCommandBuffer>(commandBuffer);
        VkPipeline boundPipeline = ToVar<VkPipeline>(pipeline);

        vkCmdBindPipeline(
            cmd,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            boundPipeline);

		return true;
	}

	void Shader_Vulkan::HotReload()
	{
        //back up old data
        vector<ShaderStage> oldShaders = GetAllShaders();

        //attepmt to recreate

        vector<ShaderStage> stagesToReload{};
        for (const auto& stage : oldShaders)
        {
            stagesToReload.push_back(
                {
                    stage.shaderType,
                    stage.shaderPath,
                    0
                });
        }

        auto reloadedShader = Shader_Vulkan::CreateShader(
            name,
            stagesToReload,
            targetWindow);
        if (!reloadedShader)
        {
            Logger::Print(
                "Hot reload failed for shader '" + name + "'! Keeping old version.",
                "SHADER_VULKAN",
                LogType::LOG_ERROR,
                2);
            return;
        }

        shaders = reloadedShader->shaders;
        pipeline = reloadedShader->pipeline;
        layout = reloadedShader->layout;
        descriptorSetLayout = reloadedShader->descriptorSetLayout;

        Logger::Print(
            "Shader '" + name + "' was hot reloaded!",
            "SHADER_OPENGL",
            LogType::LOG_SUCCESS);
	}

	Shader_Vulkan::~Shader_Vulkan()
	{
        WindowStruct_Windows& wData = targetWindow->GetWindow_Windows();
        Window_VulkanData& vData = targetWindow->GetVulkanStruct();

        VkDevice device = ToVar<VkDevice>(Renderer_Vulkan::GetDevice());

        if (pipeline)
        {
            vkDestroyPipeline(
                device,
                ToVar<VkPipeline>(pipeline),
                nullptr);
        }
        if (layout)
        {
            vkDestroyPipelineLayout(
                device,
                ToVar<VkPipelineLayout>(layout),
                nullptr);
        }

        for (auto& stage : shaders)
        {
            if (stage.shaderModule)
            {
                vkDestroyShaderModule(
                    device,
                    ToVar<VkShaderModule>(stage.shaderModule),
                    nullptr);
            }
        }
	}
}

void ForceClose(
    const string& title,
    const string& reason)
{
    Logger::Print(
        reason,
        "SHADER_VULKAN",
        LogType::LOG_ERROR,
        2,
        TimeFormat::TIME_NONE,
        DateFormat::DATE_NONE);

    Window* mainWindow = Window::windows.front();
    if (mainWindow->CreatePopup(
        title,
        reason,
        PopupAction::POPUP_ACTION_OK,
        PopupType::POPUP_TYPE_ERROR)
        == PopupResult::POPUP_RESULT_OK)
    {
        Window::Shutdown(ShutdownState::SHUTDOWN_FAILURE);
    }
}