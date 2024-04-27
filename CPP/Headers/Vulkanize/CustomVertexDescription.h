#pragma once

#include <vector>
#include "Vulkanize/Vulkanize.h"

namespace VKZ
{
  template <typename VertexType>
  struct CustomVertexDescription : VertexDescription
  {
    // Typically no need to override this.
    virtual void collect_binding_description( std::vector<VkVertexInputBindingDescription>& list )
    {
      VkVertexInputBindingDescription description = {};
      // description.binding is set during graphics pipeline construction.
      description.stride = sizeof(VertexType);
      description.inputRate = per_instance() ? VK_VERTEX_INPUT_RATE_INSTANCE : VK_VERTEX_INPUT_RATE_VERTEX;
      list.push_back( description );
    }

    // Overload this method in your extended CustomVertexDescription.
    //
    // EXAMPLE
    //   VKZ_VERTEX_ATTRIBUTE( StandardVertex, 0, position, VK_FORMAT_R32G32B32_SFLOAT );
    //   VKZ_VERTEX_ATTRIBUTE( StandardVertex, 1, rgba,     VK_FORMAT_R8G8B8A8_SRGB );
    // virtual void collect_attribute_descriptions( std::vector<VkVertexInputAttributeDescription>& list );
  };
};
