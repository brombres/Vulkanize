#pragma once

#include "Vulkanize/Vulkanize.h"

namespace VKZ
{
  struct ConfigureCommandBuffers : ContextOperation<Context>
  {
    virtual bool activate();
    virtual void deactivate();
  };
};
