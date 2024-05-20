#pragma once

namespace VKZ
{
  struct ConfigureQueues : ContextOperation<Context>
  {
    virtual bool on_activate();
  };
};
