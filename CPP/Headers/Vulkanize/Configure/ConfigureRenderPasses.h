#pragma once

#include "Vulkanize/Vulkanize.h"

namespace VKZ
{
  struct ConfigureRenderPasses : ContextOperation<Context>
  {
    virtual bool activate();
    virtual void deactivate();
  };
};
