#include <cstdio>

#include "VkBootstrap.h"

#include "Vulkanize/Vulkanize.h"
#include "Vulkanize/Context.h"
#include "Vulkanize/Configure/ConfigureDevice.h"
using namespace VKZ;
using namespace std;

#include "glslang/Include/glslang_c_interface.h"

// Required for use of glslang_default_resource
#include <glslang/Public/resource_limits_c.h>


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
  set_operation( "configure.graphics_pipelines",        nullptr );
  set_operation( "configure.swapchain.framebuffers",    new ConfigureFramebuffers() );
  set_operation( "configure.swapchain.command_pool",    new ConfigureCommandPool() );
  set_operation( "configure.vertex_buffers",            nullptr );
  set_operation( "configure.swapchain.command_buffers", new ConfigureCommandBuffers() );
  set_operation( "configure.semaphores",                new ConfigureSemaphores() );
  set_operation( "configure.fences",                    new ConfigureFences() );
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

void Context::prepare_command_buffer( VkCommandBuffer cmd, VkCommandBufferUsageFlagBits usage_flags,
    VkCommandBufferResetFlagBits reset_flags )
{
  device_dispatch.resetCommandBuffer( cmd, reset_flags );

  VkCommandBufferBeginInfo begin_info = {};
  begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  begin_info.flags = usage_flags;
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
