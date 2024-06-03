#include <cstdio>

#include "VkBootstrap.h"

#include "Vulkanize/Vulkanize.h"
#include "Vulkanize/Context.h"
#include "Vulkanize/Configure/ConfigureDevice.h"
using namespace VKZ;
using namespace std;

Context::Context( VkSurfaceKHR surface ) : surface(surface)
{
  if ( !Vulkanize::instance )
  {
    VKZ_LOG_ERROR( "Error creating Context - a Vulkanize instance must be created before creating a Context.\n" );
    exit(1);
  }

  Vulkanize::instance->context_instances.push_back( this );
}

Context::~Context()
{
  if (Vulkanize::instance)
  {
    vector<Context*>& instances = Vulkanize::instance->context_instances;
    for (auto it=instances.begin(); it!=instances.end(); ++it)
    {
      if (*it == this)
      {
        instances.erase( it );
        break;
      }
    }
  }

  destroy();

  vkb::destroy_surface( Vulkanize::instance->vulkan_instance, surface );
  surface = nullptr;
}

void Context::configure_operations()
{
  set_operation( "configure.device",                    new ConfigureDevice(1,2) );
  set_operation( "configure.formats",                   new ConfigureFormats() );
  set_operation( "configure.swapchain.surface_size",    new ConfigureSurfaceSize() );
  set_operation( "configure.swapchain",                 new ConfigureSwapchain() );
  set_operation( "configure.swapchain.depth_stencil",   new ConfigureDepthStencil() );
  set_operation( "configure.queues",                    new ConfigureQueues() );
  set_operation( "configure.render_passes",             new ConfigureRenderPass() );
  set_operation( "configure.swapchain.framebuffers",    new ConfigureFramebuffers() );
  set_operation( "configure.swapchain.command_pool",    new ConfigureCommandPool() );
  set_operation( "configure.swapchain.command_buffers", new ConfigureCommandBuffers() );
  set_operation( "configure.semaphores",                new ConfigureSemaphores() );
  set_operation( "configure.fences",                    new ConfigureFences() );
  set_operation( "configure.buffers",                   nullptr );
  set_operation( "configure.samplers",                  nullptr );
  set_operation( "configure.descriptors",               nullptr );
  set_operation( "configure.graphics_pipelines",        nullptr );
  set_operation( "render.begin",                        new RenderBegin() );
  set_operation( "render.end",                          new RenderEnd() );
}

void Context::add_operation( std::string phase, Operation* operation )
{
  if (operation) operation->set_context( this );
  Process::add_operation( phase, operation );
}

void Context::destroy()
{
  device_dispatch.deviceWaitIdle();
  Process::destroy();
}

int Context::find_memory_type( uint32_t typeFilter, VkMemoryPropertyFlags properties )
{
  VkPhysicalDeviceMemoryProperties memory_properties;
  Vulkanize::instance->instance_dispatch.getPhysicalDeviceMemoryProperties( physical_device, &memory_properties );

  for (uint32_t i=0; i<memory_properties.memoryTypeCount; ++i)
  {
    if ( (typeFilter & (1 << i)) &&
        (memory_properties.memoryTypes[i].propertyFlags & properties) == properties)
    {
      return i;
    }
  }

  return -1;
}

void Context::reset_cmd( VkCommandBuffer cmd )
{
  device_dispatch.resetCommandBuffer( cmd, (VkCommandBufferResetFlagBits)0 );

  VkCommandBufferBeginInfo begin_info = {};
  begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  begin_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
  device_dispatch.beginCommandBuffer( cmd, &begin_info );
}

void Context::recreate_swapchain()
{
  device_dispatch.deviceWaitIdle();
  dispatch_event( "configure.swapchain", Event(EventType::SURFACE_LOST,true) );
  deactivate( "configure.swapchain" );
  activate( "configure.swapchain" );
}

void Context::set_operation( std::string phase, Operation* operation )
{
  if (operation) operation->set_context( this );
  Process::set_operation( phase, operation );
}

VkCommandBuffer Context::begin_cmd()
{
  // TODO: Use a separate command pool with VK_COMMAND_POOL_CREATE_TRANSIENT_BIT?
  VkCommandBufferAllocateInfo alloc_info = {};
  alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  alloc_info.commandPool = command_pool;
  alloc_info.commandBufferCount = 1;

  VkCommandBuffer cmd;
  device_dispatch.allocateCommandBuffers( &alloc_info, &cmd );

  VkCommandBufferBeginInfo begin_info = {};
  begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  device_dispatch.beginCommandBuffer( cmd, &begin_info );

  return cmd;
}

void Context::end_cmd( VkCommandBuffer cmd )
{
  device_dispatch.endCommandBuffer( cmd );

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &cmd;

  device_dispatch.queueSubmit( graphics_queue, 1, &submitInfo, VK_NULL_HANDLE );
  device_dispatch.queueWaitIdle( graphics_queue );

  device_dispatch.freeCommandBuffers( command_pool, 1, &cmd );
}

