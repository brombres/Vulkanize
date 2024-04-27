#include "VkBootstrap.h"
#include "Vulkanize/Vulkanize.h"
using namespace VKZ;

bool ConfigureSurfaceSize::activate()
{
  VkSurfaceCapabilitiesKHR surface_capabilities;

  Vulkanize::instance->instance_dispatch.getPhysicalDeviceSurfaceCapabilitiesKHR(
    context->physical_device,
    context->surface,
    &surface_capabilities
  );

  int width = (int)surface_capabilities.currentExtent.width;
  int height = (int)surface_capabilities.currentExtent.height;

  context->surface_size.width = width;
  context->surface_size.height = height;

  return true;
}

