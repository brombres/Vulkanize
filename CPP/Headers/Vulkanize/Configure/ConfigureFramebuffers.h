#pragma once

#include "Vulkanize/Vulkanize.h"

namespace VKZ
{
  struct ConfigureFramebuffers : ContextOperation<Context>
  {
    virtual bool activate();
    virtual void deactivate();
  };
};
