#include "VkBootstrap.h"
#include "Vulkanize/Vulkanize.h"
using namespace VKZ;

bool RenderEnd::execute()
{
  context->device_dispatch.cmdEndRenderPass( context->cmd );
  context->device_dispatch.endCommandBuffer( context->cmd );

  VkPipelineStageFlags wait_dest_stage_mask = VK_PIPELINE_STAGE_TRANSFER_BIT;
  VkSubmitInfo submitInfo = {};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = &context->image_available_semaphores[context->swap_index];
  submitInfo.pWaitDstStageMask = &wait_dest_stage_mask;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &context->cmd;
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = &context->rendering_finished_semaphores[context->swap_index];
  context->device_dispatch.queueSubmit( context->graphics_queue, 1, &submitInfo, context->fences[context->swap_index] );

  VkPresentInfoKHR present_info = {};
  present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  present_info.waitSemaphoreCount = 1;
  present_info.pWaitSemaphores = &context->rendering_finished_semaphores[context->swap_index];
  present_info.swapchainCount = 1;
  present_info.pSwapchains = &context->swapchain.swapchain;
  present_info.pImageIndices = &context->image_index;

  VkResult result = context->device_dispatch.queuePresentKHR( context->present_queue, &present_info );
  switch (result)
  {
    case VK_SUCCESS:
      break;
    case VK_ERROR_OUT_OF_DATE_KHR:
    case VK_SUBOPTIMAL_KHR:
      context->recreate_swapchain();
      return false;
        // SUBOPTIMAL could be due to resize
        //VkSurfaceCapabilitiesKHR surface_capabilities;
        //vkGetPhysicalDeviceSurfaceCapabilitiesKHR( context->physical_device, surface, &surface_capabilities );
        //if (surface_capabilities.currentExtent.width != context->surface_size.width ||
        //    surface_capabilities.currentExtent.height != context->surface_size.height)
        //{
        //  context->recreate_swapchain();
        //}
        //return;
    default:
      VKZ_LOG_ERROR(
        "[Vulkanize] Error presenting swapchain image (%s).\n",
        Vulkanize::vkResult_to_c_string(result)
      );
      return false;
  }

  context->device_dispatch.queueWaitIdle( context->present_queue );

  context->swap_index = (context->swap_index + 1) % context->swapchain_count;

  return true;
}

