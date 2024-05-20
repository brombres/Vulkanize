#include "VkBootstrap.h"
#include "Vulkanize/Vulkanize.h"
using namespace VKZ;

bool ConfigureSemaphores::on_activate()
{
  uint32_t swapchain_count = context->swapchain_count;
  context->image_available_semaphores.resize( swapchain_count );
  context->rendering_finished_semaphores.resize( swapchain_count );
  for (uint32_t i=0; i<swapchain_count; ++i)
  {
    _create_semaphore( &context->image_available_semaphores[i] );
    _create_semaphore( &context->rendering_finished_semaphores[i] );
  }
  return true;
}

void ConfigureSemaphores::on_deactivate()
{
  uint32_t semaphore_count = context->swapchain_count;
  for (uint32_t i=0; i<semaphore_count; ++i)
  {
    context->device_dispatch.destroySemaphore( context->image_available_semaphores[i], nullptr );
    context->device_dispatch.destroySemaphore( context->rendering_finished_semaphores[i], nullptr );
  }
}

void ConfigureSemaphores::_create_semaphore( VkSemaphore *semaphore )
{
  VkSemaphoreCreateInfo semaphore_info = {};
  semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  context->device_dispatch.createSemaphore( &semaphore_info, nullptr, semaphore );
}
