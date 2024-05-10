#pragma once

#include "Vulkanize/Vulkanize.h"

namespace VKZ
{
  struct Attachment
  {
    uint32_t                index;
    VkAttachmentDescription description = {};

    Attachment( uint32_t index ) : index(index) {}
  };

  //struct AttachmentReference
  //{
  //  Attachment*   attachment;
  //  VkImageLayout layout;

  //  AttachmentReference( Attachment* attachment, VkImageLayout layout )
  //    : attachment(attachment), layout(layout) {}
  //};

  struct Subpass
  {
    uint32_t index;
    VkPipelineBindPoint bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS;
    std::vector<VkAttachmentReference> color_attachments;
    std::vector<VkAttachmentReference> depth_attachments;
    std::vector<VkAttachmentReference> input_attachments;
    std::vector<VkAttachmentReference> resolve_attachments;
    std::vector<uint32_t>              preserve_attachments;

    Subpass( uint32_t index ) : index(index) {}

    virtual void add_color_attachment( Attachment* attachment,
        VkImageLayout layout=VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL )
    {
      VkAttachmentReference reference = {};
      reference.attachment = attachment->index;
      reference.layout = layout;
      color_attachments.push_back( reference );
    }

    virtual void add_depth_attachment( Attachment* attachment,
        VkImageLayout layout=VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL )
    {
      VkAttachmentReference reference = {};
      reference.attachment = attachment->index;
      reference.layout = layout;
      depth_attachments.push_back( reference );
    }

    virtual void add_input_attachment( Attachment* attachment,
        VkImageLayout layout=VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL )
    {
      VkAttachmentReference reference = {};
      reference.attachment = attachment->index;
      reference.layout = layout;
      input_attachments.push_back( reference );
    }

    virtual void add_resolve_attachment( Attachment* attachment,
        VkImageLayout layout=VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL )
    {
      VkAttachmentReference reference = {};
      reference.attachment = attachment->index;
      reference.layout = layout;
      resolve_attachments.push_back( reference );
    }

    virtual void add_preserve_attachment( Attachment* attachment )
    {
      preserve_attachments.push_back( attachment->index );
    }

    virtual bool get_description( VkSubpassDescription* description );
  };

  struct ConfigureRenderPass : ContextOperation<Context>
  {
    std::vector<Attachment*>             attachments;
    std::vector<VkAttachmentDescription> attachment_descriptions;
    std::vector<Subpass*>                subpasses;
    std::vector<VkSubpassDependency>     subpass_dependencies;
    std::vector<VkSubpassDescription>    subpass_descriptions;

    virtual ~ConfigureRenderPass();

    virtual bool activate();
    virtual void deactivate();

    virtual Subpass*             add_subpass();
    virtual VkSubpassDependency& add_subpass_dependency( uint32_t source_subpass_index, uint32_t dest_subpass_index );
    virtual void                 clear();
    virtual Attachment*          create_attachment();
    virtual Attachment*          create_color_attachment();
    virtual Attachment*          create_depth_attachment();
  };
};
