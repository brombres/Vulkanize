#pragma once

#include "Vulkanize/Vulkanize.h"

namespace VKZ
{
  struct ConfigureFences : ContextOperation<Context>
  {
    virtual bool activate();
    virtual void deactivate();
  };
};
