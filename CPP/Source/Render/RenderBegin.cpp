#include "VkBootstrap.h"
#include "Vulkanize/Vulkanize.h"
using namespace VKZ;

bool RenderBegin::on_execute()
{
  VkResult result = context->device_dispatch.acquireNextImageKHR(
    context->swapchain,
    UINT64_MAX,
    context->image_available_semaphores[context->swap_index],
    VK_NULL_HANDLE,
    &context->image_index
  );

  switch (result)
  {
    case VK_SUCCESS:
      break;
    case VK_SUBOPTIMAL_KHR:
    case VK_ERROR_OUT_OF_DATE_KHR:
      context->recreate_swapchain();
      return false;
    //case VK_ERROR_SURFACE_LOST_KHR: destroy & recreate surface
    default:
      VKZ_LOG_ERROR(
        "[Vulkanize] Error acquiring swapchain image (%s).\n",
        Vulkanize::vkResult_to_c_string(result)
      );
      return false;
  }

  context->device_dispatch.waitForFences( 1, &context->fences[context->swap_index], VK_FALSE, UINT64_MAX );
  context->device_dispatch.resetFences( 1, &context->fences[context->swap_index] );

  //context->staging_buffer = &context->staging_buffers[ context->swap_index ];
  //context->vertex_buffer  = &context->vertex_buffers[ context->swap_index ];

  context->cmd = context->command_buffers[ context->swap_index ];
  context->reset_cmd( context->cmd );

  VkClearColorValue clear_color = { 0.0f, 0.0f, 1.0f, 1.0f };
  VkClearDepthStencilValue clear_depth_stencil = { 1.0f, 0 };

  // Begin render pass
  VkRenderPassBeginInfo render_pass_info = {};
  render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  render_pass_info.renderPass = context->render_pass;
  render_pass_info.framebuffer = context->framebuffers[context->swap_index];
  render_pass_info.renderArea.offset = {0, 0};
  render_pass_info.renderArea.extent = context->surface_size;
  render_pass_info.clearValueCount = 1;

  std::vector<VkClearValue> clear_values(2);
  clear_values[0].color = clear_color;
  clear_values[1].depthStencil = clear_depth_stencil;

  render_pass_info.clearValueCount = static_cast<uint32_t>( clear_values.size() );
  render_pass_info.pClearValues = clear_values.data();

  context->device_dispatch.cmdBeginRenderPass( context->cmd, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE );

  return true;
}

