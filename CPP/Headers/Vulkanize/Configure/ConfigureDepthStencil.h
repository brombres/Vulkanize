#pragma once

#include "Vulkanize/Vulkanize.h"

namespace VKZ
{
  struct ConfigureDepthStencil : ContextOperation<Context>
  {
    virtual bool activate();
    virtual void deactivate();
    virtual bool _find_supported_depth_format( VkFormat* depth_format );
  };
};
