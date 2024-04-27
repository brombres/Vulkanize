#pragma once

#include <string>

namespace VKZ
{
  struct ConfigureSwapchain : ContextOperation<Context>
  {
    bool reconfiguring = false;

    virtual bool activate();
    virtual void deactivate();
    virtual bool on_event( std::string event_type );
  };
};
