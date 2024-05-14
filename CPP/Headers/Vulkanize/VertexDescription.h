#pragma once

#include <vector>
#include "Vulkanize/Vulkanize.h"

// .binding will be set during graphics pipeline configuration
#define VKZ_VERTEX_ATTRIBUTE( _vertex_type, _location, _name, _format ) \
  list.push_back(                                                       \
    VkVertexInputAttributeDescription                                   \
    {                                                                   \
      .location = _location,                                            \
      .format   = _format,                                              \
      .offset   = offsetof( _vertex_type, _name)                        \
    }                                                                   \
  );

namespace VKZ
{
  // Extend 'CustomVertexDescription<CustomVertexType>' instead of extending
  // VertexDescription directly.
  struct VertexDescription
  {
    virtual ~VertexDescription() {}

    // Return 'true' if there is one vertex per instance rather than one vertex per vertex.
    virtual bool per_instance() { return false; }

    // This method is defined in CustomVertexDescription; typically no need to override.
    virtual void collect_binding_description( std::vector<VkVertexInputBindingDescription>& list ) = 0;

    // Overload this method in your extended CustomVertexDescription.
    //
    // EXAMPLE
    //   VKZ_VERTEX_ATTRIBUTE( StandardVertex, 0, position, VK_FORMAT_R32G32B32_SFLOAT );
    //   VKZ_VERTEX_ATTRIBUTE( StandardVertex, 1, rgba,     VK_FORMAT_R8G8B8A8_SRGB );
    virtual void collect_attribute_descriptions( std::vector<VkVertexInputAttributeDescription>& list ) = 0;
  };
};
