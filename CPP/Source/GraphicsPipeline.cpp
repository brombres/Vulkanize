#include "Vulkanize/Vulkanize.h"
using namespace std;
using namespace VKZ;

void GraphicsPipeline::set_default_scissor_rect( int index )
{
  set_scissor_rect( index, {}, context->surface_size );
}

void GraphicsPipeline::set_default_viewport( int index, float min_depth, float max_depth )
{
  set_viewport( index, {}, context->surface_size, min_depth, max_depth );
}

void GraphicsPipeline::set_scissor_rect( int index, VkExtent2D extent )
{
  set_scissor_rect( index, {}, extent );
}

void GraphicsPipeline::set_scissor_rect( int index, VkOffset2D offset, VkExtent2D extent )
{
  if (scissor_rects.size() <= index) scissor_rects.resize( index+1 );
  scissor_rects[index] =
    {
      .offset = offset,
      .extent = extent
    };
}

void GraphicsPipeline::set_viewport( int index, VkExtent2D extent, float min_depth, float max_depth )
{
  set_viewport( index, {}, extent, min_depth, max_depth );
}

void GraphicsPipeline::set_viewport( int index, VkOffset2D offset, VkExtent2D extent, float min_depth, float max_depth )
{
  if (viewports.size() <= index) viewports.resize( index+1 );
  viewports[index] =
    {
      .x        = (float)offset.x,
      .y        = (float)offset.y,
      .width    = (float)extent.width,
      .height   = (float)extent.height,
      .minDepth = min_depth,
      .maxDepth = max_depth
    };
}

