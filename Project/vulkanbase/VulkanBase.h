#ifndef _VULKANBASE_H
#define _VULKANBASE_H
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

#define GLFW_EXPOSE_NATIVE_WIN32

#include <GLFW/glfw3native.h>

#include "VulkanUtil.h"
#include <iostream>
#include <stdexcept>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <optional>
#include <set>
#include <limits>
#include <algorithm>
#include <chrono>
#include "CommandPool.h"
#include "CommandBuffer.h"
#include "DataBuffer.h"
#include "GraphicsPipeline.h"
#include "Scene.h"
#include "Singleton.h"
#include "DeltaTime.h"
#include "Camera.h"


const std::vector<const char*> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
};


struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR        capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR>   presentModes;
};

class VulkanBase
{
public:
    void run()
    {
        InitWindow();
        InitVulkan();
        mainLoop();
        cleanup();
    }

    void DrawFrame(uint32_t imageIndex);
    inline static VkDevice         device;
    inline static VkPhysicalDevice physicalDevice;
    inline static VkExtent2D       swapChainExtent;
private:

    void InitVulkan();

    void mainLoop()
    {

        auto lastTime{std::chrono::high_resolution_clock::now()};
        while (!glfwWindowShouldClose(window))
        {
            const auto currentTime{std::chrono::high_resolution_clock::now()};

            dae::Singleton<DeltaTime>::GetInstance().SetDeltaTime(
                    std::chrono::duration<float>(currentTime - lastTime).count());
            lastTime = currentTime;
            glfwPollEvents();
            // week 06
            drawFrame();
        }
        vkDeviceWaitIdle(device);
    }

    void cleanup()
    {

        vkDestroySemaphore(device, renderFinishedSemaphore, nullptr);
        vkDestroySemaphore(device, imageAvailableSemaphore, nullptr);
        vkDestroyFence(device, inFlightFence, nullptr);
        m_CommandPool.reset();
        m_pScene->Destroy();
        for (auto framebuffer: swapChainFramebuffers)
        {
            vkDestroyFramebuffer(device, framebuffer, nullptr);
        }

        m_pGraphicsPipeline2D->DestroyPipeline();
        m_pGraphicsPipeline3D->DestroyPipeline();
        for (auto imageView: swapChainImageViews)
        {
            vkDestroyImageView(device, imageView, nullptr);
        }

        if (enableValidationLayers)
        {
            DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
        }
        vkDestroySwapchainKHR(device, swapChain, nullptr);
        vkDestroyDevice(device, nullptr);

        vkDestroySurfaceKHR(instance, surface, nullptr);
        vkDestroyInstance(instance, nullptr);

        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void createSurface()
    {
        if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create window surface!");
        }
    }

    GLFWwindow                     * window;
    void InitWindow();

    // Week 02
    // Queue families
    // CommandBuffer concept


    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice physDevice);
    std::unique_ptr<CommandBuffer> m_CommandBuffer;
    std::unique_ptr<CommandPool>   m_CommandPool;
    VkPipelineVertexInputStateCreateInfo CreateVertexInputStateInfo();
    // Week 03
    // Renderpass concept
    // Graphics pipeline

    std::vector<VkFramebuffer>        swapChainFramebuffers;
    std::unique_ptr<GraphicsPipeline> m_pGraphicsPipeline2D;
    std::unique_ptr<GraphicsPipeline> m_pGraphicsPipeline3D;
    std::unique_ptr<Scene>            m_pScene;
    void createFrameBuffers();

    // Week 04
    // Swap chain and image view support

    VkSwapchainKHR       swapChain;
    std::vector<VkImage> swapChainImages;
    VkFormat             swapChainImageFormat;

    std::vector<VkImageView> swapChainImageViews;

    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice physDevice);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    void createSwapChain();
    void createImageViews();

    // Week 05
    // Logical and physical device

    VkQueue graphicsQueue;
    VkQueue presentQueue;

    void pickPhysicalDevice();
    bool isDeviceSuitable(VkPhysicalDevice phyDevice);
    void createLogicalDevice();

    // Week 06
    // Main initialization

    VkInstance               instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkSurfaceKHR             surface;

    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    VkFence     inFlightFence;

    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    void setupDebugMessenger();
    std::vector<const char*> getRequiredExtensions();
    bool checkDeviceExtensionSupport(VkPhysicalDevice physDevice);
    void createInstance();

    void createSyncObjects();
    void drawFrame();

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                        VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                        void* pUserData)
    {
        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
        return VK_FALSE;
    }

    void KeyEvent(int key, int scancode, int action, int mods);
    void MouseMove(GLFWwindow* window, double xpos, double ypos);
    void MouseEvent(GLFWwindow* window, int button, int action, int mods);
    glm::vec2   m_DragStart{0, 0};
    std::unique_ptr<Camera>      m_Camera;
};

#endif