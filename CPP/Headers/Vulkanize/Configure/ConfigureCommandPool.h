#pragma once

#include "Vulkanize/Vulkanize.h"

namespace VKZ
{
  struct ConfigureCommandPool : ContextOperation<Context>
  {
    virtual bool activate();
    virtual void deactivate();
  };
};
