#pragma once

#include "Vulkanize/Vulkanize.h"

namespace VKZ
{
  struct ConfigureSemaphores : ContextOperation<Context>
  {
    virtual bool activate();
    virtual void deactivate();

    void _create_semaphore( VkSemaphore *semaphore );
  };
};
