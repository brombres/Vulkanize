#pragma once

#include <string>

namespace VKZ
{
  struct ConfigureSwapchain : ContextOperation<Context>
  {
    bool reconfiguring = false;

    virtual bool activate();
    virtual void deactivate();
    virtual void on_surface_lost();
  };
};
