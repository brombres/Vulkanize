#include "VkBootstrap.h"
#include "Vulkanize/Vulkanize.h"
using namespace VKZ;

bool ConfigureCommandPool::activate()
{
  VkCommandPoolCreateInfo pool_info = {};
  pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
  pool_info.queueFamilyIndex = context->graphics_QueueFamilyIndex;

  VKZ_ATTEMPT(
    "creating command pool",
    context->device_dispatch.createCommandPool( &pool_info, nullptr, &context->command_pool ),
    return false
  );

  return true;
}

void ConfigureCommandPool::deactivate()
{
  context->device_dispatch.destroyCommandPool( context->command_pool, nullptr );
}

