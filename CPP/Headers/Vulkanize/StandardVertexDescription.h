#pragma once

#include "Vulkanize/Vulkanize.h"

namespace VKZ
{
  struct StandardVertexDescription : CustomVertexDescription<StandardVertex>
  {
    virtual void collect_attribute_descriptions( std::vector<VkVertexInputAttributeDescription>& list )
    {
      VKZ_VERTEX_ATTRIBUTE( StandardVertex, 0, position, VK_FORMAT_R32G32B32_SFLOAT );
      VKZ_VERTEX_ATTRIBUTE( StandardVertex, 1, color,    VK_FORMAT_R8G8B8A8_SRGB );
      VKZ_VERTEX_ATTRIBUTE( StandardVertex, 2, data,     VK_FORMAT_R32_UINT );
      VKZ_VERTEX_ATTRIBUTE( StandardVertex, 3, uv,       VK_FORMAT_R32G32_SFLOAT );
    }
  };
};
