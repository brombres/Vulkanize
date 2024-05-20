#pragma once

#include "Vulkanize/Vulkanize.h"

namespace VKZ
{
  struct ConfigureFences : ContextOperation<Context>
  {
    virtual bool on_activate();
    virtual void on_deactivate();
  };
};
