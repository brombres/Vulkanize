#include "VkBootstrap.h"
#include "Vulkanize/Vulkanize.h"
using namespace VKZ;

bool Subpass::get_description( VkSubpassDescription* description )
{
  if (depth_attachments.size() > 1) {
    VKZ_REPORT_ERROR( "configuring render pass - only one depth/stencil attachment is allowed" );
    return false;
  }
  if (color_attachments.size() != resolve_attachments.size() && resolve_attachments.size()) {
    VKZ_REPORT_ERROR( "configuring render pass - resolve attachments are used but do not match the number of color attachments" );
    return false;
  }

  *description = (VkSubpassDescription){};

  description->pipelineBindPoint       = bind_point;
  description->colorAttachmentCount    = (uint32_t) color_attachments.size();
  description->pColorAttachments       = color_attachments.data();
  if (depth_attachments.size()) description->pDepthStencilAttachment = depth_attachments.data();
  description->inputAttachmentCount    = (uint32_t) input_attachments.size();
  description->pInputAttachments       = input_attachments.data();
  description->preserveAttachmentCount = (uint32_t) preserve_attachments.size();
  description->pPreserveAttachments    = preserve_attachments.data();
  description->pResolveAttachments     = resolve_attachments.data();

  return true;
}

ConfigureRenderPass::~ConfigureRenderPass()
{
  clear();
}

Subpass* ConfigureRenderPass::add_subpass()
{
  Subpass* subpass = new Subpass( subpasses.size() );
  subpasses.push_back( subpass );
  return subpass;
}

VkSubpassDependency& ConfigureRenderPass::add_subpass_dependency(
    uint32_t source_subpass_index, uint32_t dest_subpass_index )
{
  subpass_dependencies.resize( subpass_dependencies.size() + 1 );
  VkSubpassDependency& dependency = subpass_dependencies.back();

  dependency.srcSubpass = source_subpass_index;
  dependency.dstSubpass = dest_subpass_index;
  dependency.srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
  dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

  return dependency;
}

bool ConfigureRenderPass::activate()
{
  clear();

  Subpass* subpass = add_subpass();

  VkSubpassDependency& dependency = add_subpass_dependency( VK_SUBPASS_EXTERNAL, subpass->index );
  // These are the default settings for the new dependency - adjust as needed.
  // dependency.srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
  // dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  // dependency.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
  // dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
  // dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

  subpass->add_color_attachment( create_color_attachment() );
  subpass->add_depth_attachment( create_depth_attachment() );

  subpass_descriptions.resize( subpasses.size() );
  for (uint32_t i=0; i<subpasses.size(); ++i)
  {
    if ( !subpasses[i]->get_description(&subpass_descriptions[i]) )
    {
      return false;
    }
  }

  attachment_descriptions.clear();
  for (uint32_t i=0; i<attachments.size(); ++i)
  {
    attachment_descriptions.push_back( attachments[i]->description );
  }

  VkRenderPassCreateInfo render_pass_info = {};
  render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  render_pass_info.attachmentCount = (uint32_t)(attachment_descriptions.size());
  render_pass_info.pAttachments = attachment_descriptions.data();
  render_pass_info.subpassCount = (uint32_t)subpasses.size();
  render_pass_info.pSubpasses = subpass_descriptions.data();
  render_pass_info.dependencyCount = subpass_dependencies.size();
  render_pass_info.pDependencies = subpass_dependencies.data();

  VKZ_REQUIRE(
    "creating render pass",
    context->device_dispatch.createRenderPass(
      &render_pass_info,
      nullptr,
      &context->render_pass.vk_render_pass
    )
  );
  return true;
}

void ConfigureRenderPass::clear()
{
  for (int i=0; i<attachments.size(); ++i)
  {
    delete attachments[i];
  }
  attachments.clear();

  for (int i=0; i<subpasses.size(); ++i)
  {
    delete subpasses[i];
  }
  subpasses.clear();

  subpass_dependencies.clear();
}

void ConfigureRenderPass::deactivate()
{
  context->device_dispatch.destroyRenderPass( context->render_pass.vk_render_pass, nullptr );
  clear();
}

Attachment* ConfigureRenderPass::create_attachment()
{
  Attachment* attachment = new Attachment( attachments.size() );
  attachments.push_back( attachment );
  attachment->description.samples        = VK_SAMPLE_COUNT_1_BIT;
  attachment->description.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
  attachment->description.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
  attachment->description.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  attachment->description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  attachment->description.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
  return attachment;
}

Attachment* ConfigureRenderPass::create_color_attachment()
{
  Attachment* attachment = create_attachment();
  attachment->description.format = context->swapchain_surface_format.format;
  attachment->description.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  return attachment;
}

Attachment* ConfigureRenderPass::create_depth_attachment()
{
  Attachment* attachment = create_attachment();
  attachment->description.format        = context->depth_stencil.format;
  attachment->description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  attachment->description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  attachment->description.finalLayout   = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
  return attachment;
}

