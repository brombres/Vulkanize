#include "VkBootstrap.h"
#include "Vulkanize/Vulkanize.h"
using namespace VKZ;

bool ConfigureQueues::on_activate()
{
  context->graphics_QueueFamilyIndex = context->device.get_queue_index( vkb::QueueType::graphics ).value();
  context->present_QueueFamilyIndex  = context->device.get_queue_index( vkb::QueueType::present ).value();

  VKZ_SET(
    context->graphics_queue,
    context->device.get_queue( vkb::QueueType::graphics ),
    "obtaining graphics queue",
    return false
  );

  VKZ_SET(
    context->present_queue,
    context->device.get_queue( vkb::QueueType::present ),
    "obtaining present queue",
    return false
  );

  return true;
}

