#include "Vulkanize/Vulkanize.h"
using namespace VKZ;

bool ConfigureCommandBuffers::on_activate()
{
  VkCommandBufferAllocateInfo allocateInfo = {};
  allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocateInfo.commandPool = context->command_pool;
  allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocateInfo.commandBufferCount = context->swapchain_count;

  context->command_buffers.resize( context->swapchain_count );
  context->device_dispatch.allocateCommandBuffers( &allocateInfo, context->command_buffers.data() );
  return true;
}

void ConfigureCommandBuffers::on_deactivate()
{
  context->device_dispatch.freeCommandBuffers(
    context->command_pool,
    (uint32_t)context->command_buffers.size(),
    context->command_buffers.data()
  );
}

