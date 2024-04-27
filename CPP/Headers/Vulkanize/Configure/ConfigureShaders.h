#pragma once

#include "Vulkanize/Vulkanize.h"

namespace VKZ
{
  struct ConfigureShaders : ContextOperation<Context>
  {
    virtual bool activate();
    virtual void deactivate();
  };
};
