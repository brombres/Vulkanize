#pragma once

#include "Vulkanize/Vulkanize.h"

namespace VKZ
{
  struct ConfigureDevice : ContextOperation<Context>
  {
    int major_version;
    int minor_version;
    VkPhysicalDeviceFeatures         required_features      = {};
    VkPhysicalDeviceVulkan11Features required_features_v1_1 = {};
    VkPhysicalDeviceVulkan12Features required_features_v1_2 = {};
    VkPhysicalDeviceVulkan13Features required_features_v1_3 = {};

    ConfigureDevice( int major_version=1, int minor_version=2 );
    virtual bool on_activate();
    virtual void on_deactivate();
  };
};
