#pragma once

#include <string>

namespace VKZ
{
  struct ConfigureSwapchain : ContextOperation<Context>
  {
    bool reconfiguring = false;

    virtual bool on_activate();
    virtual void on_deactivate();
    virtual void on_surface_lost();
  };
};
