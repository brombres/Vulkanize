#pragma once

#include "Vulkanize.h"

namespace VKZ
{
  struct Descriptors;

  struct GraphicsPipeline
  {
    Context*         context;

    VkPipelineLayout layout;
    VkPipeline       vk_pipeline;

    Descriptors*     descriptors = nullptr;

    std::vector<VkViewport> viewports;
    std::vector<VkRect2D>   scissor_rects;

    virtual void cmd_bind( VkCommandBuffer cmd );
    virtual void cmd_set_default_viewports_and_scissor_rects( VkCommandBuffer cmd );
    virtual void cmd_set_viewports_and_scissor_rects( VkCommandBuffer cmd );
    virtual void cmd_bind_descriptor_set( VkCommandBuffer cmd );

    virtual void set_default_scissor_rect( int index );
    virtual void set_default_viewport( int index, float min_depth=0, float max_depth=1.0f );

    virtual void set_scissor_rect( int index, VkExtent2D extent );
    virtual void set_scissor_rect( int index, VkOffset2D offset, VkExtent2D extent );

    virtual void set_viewport( int index, VkExtent2D extent,
                               float min_depth=0, float max_depth=1.0f );
    virtual void set_viewport( int index, VkOffset2D offset, VkExtent2D extent,
                               float min_depth=0, float max_depth=1.0f );
  };
};
