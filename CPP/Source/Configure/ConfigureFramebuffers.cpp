#include "VkBootstrap.h"
#include "Vulkanize/Vulkanize.h"
using namespace VKZ;

bool ConfigureFramebuffers::activate()
{
  context->framebuffers.resize( context->swapchain_image_views.size() );

  for (size_t i=0; i<context->swapchain_image_views.size(); ++i)
  {
    std::vector<VkImageView> attachments(2);
    attachments[0] = context->swapchain_image_views[i];
    attachments[1] = context->depth_stencil.view;

    VkFramebufferCreateInfo framebuffer_info = {};
    framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebuffer_info.renderPass = context->render_pass;
    framebuffer_info.attachmentCount = static_cast<uint32_t>(attachments.size());
    framebuffer_info.pAttachments = attachments.data();
    framebuffer_info.width = context->surface_size.width;
    framebuffer_info.height = context->surface_size.height;
    framebuffer_info.layers = 1;

    VKZ_REQUIRE(
      "creating framebuffer",
      context->device_dispatch.createFramebuffer( &framebuffer_info, nullptr, &context->framebuffers[i] )
    );
  }
  return true;
}

void ConfigureFramebuffers::deactivate()
{
  for (auto framebuffer : context->framebuffers)
  {
    context->device_dispatch.destroyFramebuffer( framebuffer, nullptr );
  }
}

