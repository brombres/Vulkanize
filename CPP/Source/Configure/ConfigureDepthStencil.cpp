#include "VkBootstrap.h"
#include "Vulkanize/Vulkanize.h"
using namespace VKZ;

bool ConfigureDepthStencil::activate()
{
  VkFormat depth_format;
  if ( !_find_supported_depth_format( &depth_format ) )
  {
    VKZ_LOG_ERROR( "finding supported depth format" );
    return false;
  }

  context->depth_stencil.create(
    context,
    context->surface_size.width,
    context->surface_size.height,
    depth_format,
    VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
    VK_IMAGE_ASPECT_DEPTH_BIT,
    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
  );

  return true;
}

void ConfigureDepthStencil::deactivate()
{
  context->depth_stencil.destroy();
}

bool ConfigureDepthStencil::_find_supported_depth_format( VkFormat* depth_format )
{
  std::vector<VkFormat> depth_format_candidates = {
    VK_FORMAT_D32_SFLOAT_S8_UINT,
    VK_FORMAT_D32_SFLOAT,
    VK_FORMAT_D24_UNORM_S8_UINT,
    VK_FORMAT_D16_UNORM_S8_UINT,
    VK_FORMAT_D16_UNORM
  };

  auto instance_dispatch = Vulkanize::instance->instance_dispatch;
  for (auto& format_candidate : depth_format_candidates)
  {
    VkFormatProperties format_properties;
    instance_dispatch.getPhysicalDeviceFormatProperties(
      context->physical_device,
      format_candidate,
      &format_properties
    );
    if (format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
    {
      *depth_format = format_candidate;
      return true;
    }
  }

  return false;
}
