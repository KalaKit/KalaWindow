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
#include <stdexcept>

#define VK_NO_PROTOTYPES
#include <Volk/volk.h>
//#include <vulkan/vulkan.h>

#include "graphics/vulkan/shader_vulkan.hpp"
#include "graphics/vulkan/vulkan.hpp"
#include "graphics/window.hpp"
#include "core/log.hpp"
#include "core/core.hpp"

using KalaWindow::Graphics::Window;
using KalaWindow::Core::Logger;
using KalaWindow::Core::LogType;
using KalaWindow::Core::KalaWindowCore;
using KalaWindow::Graphics::Vulkan::ShaderType;
using KalaWindow::Graphics::Vulkan::Shader_Vulkan;

using KalaWindow::Graphics::Vulkan::VulkanData_VertexInputState;
using KalaWindow::Graphics::Vulkan::VD_VII_BindingDescriptions;
using KalaWindow::Graphics::Vulkan::VD_VII_AttributeDescriptions;

using KalaWindow::Graphics::Vulkan::VulkanData_InputAssemblyState;

using KalaWindow::Graphics::Vulkan::VulkanData_RasterizationState;

using KalaWindow::Graphics::Vulkan::VulkanData_ColorBlendState;
using KalaWindow::Graphics::Vulkan::VD_CBS_Attachments;

using KalaWindow::Graphics::Vulkan::VulkanData_DepthStencilState;
using KalaWindow::Graphics::Vulkan::VD_DSS_FrontBack;

using KalaWindow::Graphics::Vulkan::VulkanData_TesselationState;

using KalaWindow::Graphics::Vulkan::VulkanShaderData;
using KalaWindow::Graphics::VulkanShaderWindowData;

using KalaWindow::Graphics::VulkanData_ViewportState;
using KalaWindow::Graphics::VD_VS_Scissors;
using KalaWindow::Graphics::VD_VS_Viewports;
using KalaWindow::Graphics::VD_VS_VkExtent2D;
using KalaWindow::Graphics::VD_VS_VkOffset2D;

using KalaWindow::Graphics::VulkanData_DynamicState;

using KalaWindow::Graphics::VulkanData_MultisampleState;

using std::vector;
using std::string;
using std::to_string;
using std::ifstream;
using std::ios;
using std::make_unique;
using std::filesystem::path;
using std::filesystem::exists;
using std::filesystem::last_write_time;
using std::filesystem::remove;
using std::streamsize;
using std::format;
using std::stringstream;
using std::exception;
using std::system;
using std::error_code;

static bool glslangValidatorExists = false;

//The window-level data passed by the user that has been converted to Vk variables
struct VulkanShaderWindowVKData
{
    VkPipelineViewportStateCreateInfo* viewportState{};
    VkPipelineDynamicStateCreateInfo* dynamicState{};
    VkPipelineMultisampleStateCreateInfo* multisampleState{};
};

//The shader-level data passed by the user that has been converted to Vk variables
struct VulkanShaderVKData
{
    VkPipelineVertexInputStateCreateInfo* vertexInputState{};
    VkPipelineInputAssemblyStateCreateInfo* inputAssemblyState{};
    VkPipelineRasterizationStateCreateInfo* rasterizationState{};
    VkPipelineColorBlendAttachmentState* colorBlendAttachmentState{};
    VkPipelineColorBlendStateCreateInfo* colorBlendState{};
};

static vector<uint32_t> ReadFileBinary(const string& filePath);

static bool InitShader(
    ShaderType type,
    const string& shaderPath,
    VkDevice device,
    VkShaderModule& shaderModule,
    VkPipelineShaderStageCreateInfo& shaderStageInfo);

static bool ValidatorExists();

static bool CompileToSPV(
    const string& origin,
    const string& target);

static VulkanShaderVKData InitVulkanShaderData(VulkanShaderData shaderData);
static VulkanShaderWindowVKData InitVulkanShaderWindowData(VulkanShaderWindowData windowData);

namespace KalaWindow::Graphics::Vulkan
{
    bool Shader_Vulkan::CompileShader(
        const vector<string>& originShaderPaths,
        const vector<string>& targetShaderPaths,
        bool forceCompile)
    {
        if (originShaderPaths.size() != targetShaderPaths.size())
        {
            Logger::Print(
                "Origin shader paths count does not match target shader paths count!",
                "SHADER_VULKAN",
                LogType::LOG_ERROR,
                2);

            return false;
        }

        for (size_t i = 0; i < originShaderPaths.size(); ++i)
        {
            string src = originShaderPaths[i];
            string spv = targetShaderPaths[i];

            string originName = path(src).filename().string();

            bool shouldCompile = forceCompile;

            bool upToDate = false;
            if (!shouldCompile)
            {
                if (!exists(spv)) shouldCompile = true;
                else
                {
                    auto srcTime = last_write_time(src);
                    auto spvTime = last_write_time(spv);
                    upToDate = srcTime > spvTime;
                    if (!upToDate) shouldCompile = true;
                }
            }

            if (upToDate)
            {
                Logger::Print(
                    "Skipped compiling shader file '" + originName + "' because it is already up to date.",
                    "SHADER_VULKAN",
                    LogType::LOG_DEBUG);

                continue;
            }

            if (shouldCompile)
            {
                if (exists(spv))
                {
                    error_code ec{};
                    if (!remove(spv, ec))
                    {
                        string title = "Vulkan Shader Error";
                        string reason = "Failed to remove existing spv file '" + originName + "'! Reason: " + ec.message();

                        KalaWindowCore::ForceClose(title, reason);
                        return false;
                    }
                    else
                    {
                        Logger::Print(
                            "Removed existing spv file '" + originName + "' before compilation.",
                            "SHADER_VULKAN",
                            LogType::LOG_DEBUG);
                    }
                }

                bool success = CompileToSPV(src, spv);
                if (!success) return false;
            }
        }

        return true;
    }

	Shader_Vulkan* Shader_Vulkan::CreateShader(
		const string& shaderName,
		const vector<ShaderStage>& shaderStages,
        Window* targetWindow,
        VulkanShaderData shaderData)
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

            if (!exists(stage.shaderPath))
            {
                Logger::Print(
                    "Shader '" + shaderName + "' with type '"
                    + shaderType + "' has an invalid path '" + stage.shaderPath + "'!",
                    "SHADER_VULKAN",
                    LogType::LOG_ERROR,
                    2);
                return nullptr;
            }
            
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

        bool vertShaderExists = !newVertStage.shaderPath.empty();
        bool fragShaderExists = !newFragStage.shaderPath.empty();

        VulkanData_Core& vData = targetWindow->GetVulkanCoreData();

        VkDevice device = ToVar<VkDevice>(Renderer_Vulkan::GetDevice());
        if (device == VK_NULL_HANDLE)
        {
            KalaWindowCore::ForceClose(
                "Vulkan Shader Error",
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
            newVertStage.shaderModule = FromVar(vertModule);
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
            newFragStage.shaderModule = FromVar(fragModule);
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
            KalaWindowCore::ForceClose(
                "Vulkan Shader Error",
                "Failed to create pipeline layout!");
            return nullptr;
        }
        shaderPtr->layout = FromVar(newLayout);

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

        VulkanShaderVKData userShaderVKData = InitVulkanShaderData(shaderData);

        VulkanShaderWindowData& vkWinData = targetWindow->GetVulkanShaderWindowStruct();
        VulkanShaderWindowVKData userShaderWindowVKData = InitVulkanShaderWindowData(vkWinData);

        pipelineInfo.pVertexInputState = userShaderVKData.vertexInputState;
        pipelineInfo.pInputAssemblyState = userShaderVKData.inputAssemblyState;
        pipelineInfo.pViewportState = userShaderWindowVKData.viewportState;
        pipelineInfo.pDynamicState = userShaderWindowVKData.dynamicState;
        pipelineInfo.pRasterizationState = userShaderVKData.rasterizationState;
        pipelineInfo.pMultisampleState = userShaderWindowVKData.multisampleState;
        pipelineInfo.pColorBlendState = userShaderVKData.colorBlendState;

        VkPipeline newPipeline = VK_NULL_HANDLE;
        if (vkCreateGraphicsPipelines(
            device,
            VK_NULL_HANDLE,
            1,
            &pipelineInfo,
            nullptr,
            &newPipeline) != VK_SUCCESS)
        {
            KalaWindowCore::ForceClose(
                "Vulkan Shader Error",
                "Failed to create graphics pipeline!");
            return nullptr;
        }
        shaderPtr->pipeline = FromVar(newPipeline);

        //
        // CLEANUP
        //

        if (vertShaderExists) shaderPtr->shaders.push_back(newVertStage);
        if (fragShaderExists) shaderPtr->shaders.push_back(newFragStage);

        newShader->name = shaderName;
        newShader->targetWindow = targetWindow;
        createdShaders[shaderName] = move(newShader);

        return shaderPtr;
	}

	bool Shader_Vulkan::Bind(
        uintptr_t commandBuffer,
        Window* window) const
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

        VulkanData_Core& vData = window->GetVulkanCoreData();

        VkCommandBuffer cmd = ToVar<VkCommandBuffer>(commandBuffer);
        VkPipeline boundPipeline = ToVar<VkPipeline>(pipeline);

        vkCmdBindPipeline(
            cmd,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            boundPipeline);

        VkExtent2D currentExtent =
        {
            vData.swapchainExtentWidth,
            vData.swapchainExtentHeight
        };

        VkViewport vp
        {
            0.0f,
            0.0f,
            float(currentExtent.width),
            float(currentExtent.height),
            0.0f,
            1.0f
        };

        vkCmdSetViewport(
            cmd,
            0,
            1,
            &vp);

        VkRect2D sc{ { 0, 0 }, currentExtent };
        vkCmdSetScissor(
            cmd,
            0,
            1,
            &sc);

        try
        {
            if (drawCommands)
            {
                Logger::Print(
                    "User draw commands start.",
                    "VULKAN",
                    LogType::LOG_DEBUG);

                drawCommands();

                Logger::Print(
                    "User draw commands end.",
                    "VULKAN",
                    LogType::LOG_DEBUG);
            }
        }
        catch (const exception& e)
        {
            Logger::Print(
                "Error during DrawCommands: " + string(e.what()),
                "VULKAN",
                LogType::LOG_ERROR,
                2);
            return false;
        }

		return true;
	}

	void Shader_Vulkan::HotReload()
	{
        //back up old data
        vector<ShaderStage> oldShaders = GetAllShaders();

        //attempt to recreate

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

        VulkanShaderData& thisShaderData = GetVulkanShaderUserStruct();
        auto reloadedShader = Shader_Vulkan::CreateShader(
            name,
            stagesToReload,
            targetWindow,
            thisShaderData);
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

    void Shader_Vulkan::SetPushConstant(
        uintptr_t cmdBuffer,
        uintptr_t layout,
        uint32_t stageFlags,
        uint32_t offset,
        const PushConstantValue& value)
    {
        //TODO: finish this + update the opengl version to this system as well
    }

	Shader_Vulkan::~Shader_Vulkan()
	{
        WindowData& wData = targetWindow->GetWindowData();
        VulkanData_Core& vData = targetWindow->GetVulkanCoreData();

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

vector<uint32_t> ReadFileBinary(const string& filePath)
{
    ifstream file(filePath, ios::ate | ios::binary);
    if (!file.is_open()) return {};

    streamsize size = file.tellg();
    string fileName = path(filePath).filename().string();

    if (size == 0)
    {
        KalaWindowCore::ForceClose(
            "Vulkan Shader Error",
            "Shader '" + fileName + "' is empty or unreadable!");

        return {};
    }
    if (size % 4 != 0)
    {
        KalaWindowCore::ForceClose(
            "Vulkan Shader Error",
            "Shader '" + fileName + "' is not aligned to 4 bytes!");

        return {};
    }

    vector<uint32_t> buffer(size / 4);
    file.seekg(0);
    file.read(reinterpret_cast<char*>(buffer.data()), size);
    file.close();

    if (buffer.empty())
    {
        KalaWindowCore::ForceClose(
            "Vulkan Shader Error",
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

bool InitShader(
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
        KalaWindowCore::ForceClose(
            "Vulkan Shader Error",
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

bool ValidatorExists()
{
#ifdef _WIN32
    string whereCommand = "cmd /C where glslangValidator";
#elif __linux__
    string whereCommand = "which glslangValidator >/dev/null 2>&1";
#endif

    Logger::Print(
        "Searching for glslangValidator...",
        "SHADER_VULKAN",
        LogType::LOG_DEBUG);

    bool result = system(whereCommand.c_str()) == 0;

    glslangValidatorExists = result;
    return result;
}

bool CompileToSPV(
    const string& origin,
    const string& target)
{
    string originName = path(origin).filename().string();
    string targetName = path(target).filename().string();

    Logger::Print(
        "Starting to compile shader file '" + originName + "' into '" + targetName + "'.",
        "SHADER_VULKAN",
        LogType::LOG_DEBUG);

#ifdef _WIN32
    string glslCommand = "cmd /C glslangValidator -V --target-env vulkan1.2 -o \"" + target + "\" \"" + origin + "\" >nul";
#elif __linux__
    string glslCommand = "glslangValidator -V --target-env vulkan1.2 -o \"" + target + "\" \"" + origin + "\" >/dev/null";
#endif

    if (!glslangValidatorExists
        && !ValidatorExists())
    {
        string title = "Vulkan Shader Error";
        string reason = "Failed to compile to spv because glslangValidator was not found! You probably didn't install the Vulkan SDK.";

        KalaWindowCore::ForceClose(title, reason);

        return false;
    }

    int glslResult = system(glslCommand.c_str());
    if (glslResult != 0)
    {
        string title = "Vulkan Shader Error";
        string reason = "Failed to compile shader '" + originName + "' to spv!";

        KalaWindowCore::ForceClose(title, reason);

        return false;
    }

    Logger::Print(
        "Compiled '" + originName + "' into '" + targetName + "'!",
        "SHADER_VULKAN",
        LogType::LOG_SUCCESS);

    return true;
}

VulkanShaderVKData InitVulkanShaderData(VulkanShaderData userData)
{
    VulkanShaderVKData data{};

    VkPipelineVertexInputStateCreateInfo vertexInputInfo
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .vertexBindingDescriptionCount = static_cast<uint32_t>(
            userData.userVertexInputState.pVertexBindingDescriptions.size()),
        .pVertexBindingDescriptions =
            userData.userVertexInputState.pVertexBindingDescriptions.empty()
            ? nullptr
            : reinterpret_cast<const VkVertexInputBindingDescription*>(
                userData.userVertexInputState.pVertexBindingDescriptions.data()),
        .vertexAttributeDescriptionCount = static_cast<uint32_t>(
            userData.userVertexInputState.pVertexAttributeDescriptions.size()),
        .pVertexAttributeDescriptions =
            userData.userVertexInputState.pVertexAttributeDescriptions.empty()
            ? nullptr
            : reinterpret_cast<const VkVertexInputAttributeDescription*>(
                userData.userVertexInputState.pVertexAttributeDescriptions.data())
    };

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
    inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT
        | VK_COLOR_COMPONENT_G_BIT
        | VK_COLOR_COMPONENT_B_BIT
        | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;

    return data;
}

VulkanShaderWindowVKData InitVulkanShaderWindowData(VulkanShaderWindowData windowData)
{
    VulkanShaderWindowVKData vkData{};

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = nullptr;
    viewportState.scissorCount = 1;
    viewportState.pScissors = nullptr;

    VkDynamicState dynamicStates[] =
    {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = 2;
    dynamicState.pDynamicStates = dynamicStates;

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    return vkData;
}