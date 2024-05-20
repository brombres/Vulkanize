#pragma once

#include "Vulkanize/Vulkanize.h"

namespace VKZ
{
  struct ConfigureSemaphores : ContextOperation<Context>
  {
    virtual bool on_activate();
    virtual void on_deactivate();

    void _create_semaphore( VkSemaphore *semaphore );
  };
};
