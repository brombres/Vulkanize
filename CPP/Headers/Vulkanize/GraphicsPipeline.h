#pragma once

#include "Vulkanize.h"

namespace VKZ
{
  struct GraphicsPipeline
  {
    Context*         context;

    VkPipelineLayout layout;
    VkPipeline       pipeline;

    std::vector<VkViewport> viewports;
    std::vector<VkRect2D>   scissor_rects;

    void cmd_bind( VkCommandBuffer cmd );
    void cmd_set_viewports_and_scissor_rects( VkCommandBuffer cmd );

    void set_default_scissor_rect( int index );
    void set_default_viewport( int index, float min_depth=0, float max_depth=1.0f );

    void set_scissor_rect( int index, VkExtent2D extent );
    void set_scissor_rect( int index, VkOffset2D offset, VkExtent2D extent );

    void set_viewport( int index, VkExtent2D extent, float min_depth=0, float max_depth=1.0f );
    void set_viewport( int index, VkOffset2D offset, VkExtent2D extent, float min_depth=0, float max_depth=1.0f );
  };
};
