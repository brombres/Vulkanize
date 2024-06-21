#pragma once

#include <vector>
#include "Vulkanize/Vulkanize.h"

namespace VKZ
{
  struct MaterialInfo
  {
    // PROPERTIES
    Context* context;

    // METHODS
    MaterialInfo( Context* context ) : context(context) {}
  };

  struct Material
  {
    Context*         context;
    GraphicsPipeline gfx;
    Descriptors      descriptors;

    bool create( MaterialInfo& info );
  };
};
