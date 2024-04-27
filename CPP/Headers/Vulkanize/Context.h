#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <vulkan/vulkan.h>
#include "VkBootstrap.h"

#include "Vulkanize.h"
#include "Vulkanize/Image.h"

namespace VKZ
{
  struct Vulkanize;

  struct Context : Process
  {
    // PROPERTIES
    VkSurfaceKHR        surface;
    VkExtent2D          surface_size;
    vkb::PhysicalDevice physical_device;
    vkb::Device         device;
    vkb::DispatchTable  device_dispatch;

    VkSurfaceFormatKHR         swapchain_surface_format;
    vkb::Swapchain             swapchain;
    bool                       swapchain_created = false;
	  std::vector<VkImage>       swapchain_images;
    std::vector<VkImageView>   swapchain_image_views;
    VKZ::Image                 depth_stencil;
    std::vector<VkFramebuffer> framebuffers;
    uint32_t                   swapchain_count = 0;

    uint32_t graphics_QueueFamilyIndex;
    uint32_t present_QueueFamilyIndex;
    VkQueue  graphics_queue;
    VkQueue  present_queue;

    VkRenderPass     render_pass;

    VkPipelineLayout pipeline_layout;
    VkPipeline       graphics_pipeline;

    std::vector<StandardVertex>  vertices; // TEST

    VkCommandPool                command_pool;
    VkBuffer                     vertex_buffer;
    VkDeviceMemory               vertex_buffer_memory;
    std::vector<VkCommandBuffer> command_buffers;
    VkCommandBuffer              cmd;

    std::vector<VkSemaphore> image_available_semaphores;
    std::vector<VkSemaphore> rendering_finished_semaphores;

    std::vector<VkFence> fences;

    // Rendering
    uint32_t swap_index = 0;
    uint32_t image_index = 0;

    // METHODS
    Context( VkSurfaceKHR surface );

    virtual ~Context();

    virtual void configure_operations();

    virtual void add_operation( std::string phase, Operation* operation );
    virtual void destroy();
    virtual int  find_memory_type( uint32_t typeFilter, VkMemoryPropertyFlags properties );
    virtual void recreate_swapchain();
    virtual void set_operation( std::string phase, Operation* operation );
  };
};
