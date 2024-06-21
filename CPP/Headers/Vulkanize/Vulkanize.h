#pragma once

#include <string>
#include <vector>
#include <vulkan/vulkan.h>
#include "VkBootstrap.h"
#include "Vulkanize/Types.h"
#include "Vulkanize/Event.h"
#include "Vulkanize/Process.h"

#if !defined(VKZ_USE_GLSLANG)
  #define VKZ_USE_GLSLANG 1
#endif

namespace VKZ
{
  struct Context;
  struct Operation;

  struct Vulkanize : Process
  {
    // GLOBAL PROPERTIES
    static Vulkanize* instance;  // set by Vulkanize()

    // PROPERTIES
    vkb::InstanceBuilder* instance_builder = nullptr;
    bool _request_validation_layers = false;
    bool _v_major  = 1;
    bool _v_minor  = 2;
    bool _v_bugfix = 0;
    std::string _app_name = "Vulkanized App";
    bool _use_default_debug_messenger = true;

    vkb::Instance              vulkan_instance;
    vkb::InstanceDispatchTable instance_dispatch;
    VkDebugUtilsMessengerEXT   debug_messenger;

    std::vector<Context*>      context_instances;

    // METHODS
    Vulkanize();
    virtual ~Vulkanize();

    bool        reset();

    bool        configure( PFN_vkGetInstanceProcAddr fp_vkGetInstanceProcAddr );
    void        configure_operations();
    Vulkanize&  request_validation_layers( bool setting=true );
    Vulkanize&  require_api_version( int major=1, int minor=2, int bugfix=0 );
    Vulkanize&  set_app_name( const char* app_name );
    Vulkanize&  use_default_debug_messenger();

    vkb::InstanceBuilder& vulkan_instance_builder( PFN_vkGetInstanceProcAddr fp_vkGetInstanceProcAddr );

    static const char* vkResult_to_c_string( VkResult result );
  };
};

#define VKZ_SET( variable, expression, description, on_error ) \
{                                                              \
  auto result = expression;                                    \
  if (result)                                                  \
  {                                                            \
    variable = result.value();                                 \
  }                                                            \
  else                                                         \
  {                                                            \
    VKZ_LOG_ERROR( "[Vulkanize] Error %s; %s.\n",              \
        description, result.error().message().c_str() );       \
    on_error;                                                  \
  }                                                            \
}

#define VKZ_REPORT_ERROR(action)                         \
  VKZ_LOG_ERROR( "[Vulkanize] Error %s.\n", action );

#define VKZ_ATTEMPT(action,cmd,on_error)                \
	{                                                      \
		auto err = cmd;                                      \
		if (err)                                             \
		{                                                    \
      VKZ_LOG_ERROR(                                     \
          "[Vulkanize] Error %s (%s).\n",                \
          action, Vulkanize::vkResult_to_c_string(err)   \
      );                                                 \
      on_error;                                          \
		}                                                    \
	}

#include "Vulkanize/Buffer.h"
#include "Vulkanize/Shader.h"
#include "Vulkanize/Image.h"
#include "Vulkanize/VertexDescription.h"
#include "Vulkanize/CustomVertexDescription.h"
#include "Vulkanize/GraphicsPipeline.h"
#include "Vulkanize/Node.h"
#include "Vulkanize/Operation.h"
#include "Vulkanize/ContextOperation.h"
#include "Vulkanize/Process.h"
#include "Vulkanize/Context.h"
#include "Vulkanize/Sampler.h"
#include "Vulkanize/Descriptors.h"
#include "Vulkanize/Configure/ConfigureDevice.h"
#include "Vulkanize/Configure/ConfigureFormats.h"
#include "Vulkanize/Configure/ConfigureSurfaceSize.h"
#include "Vulkanize/Configure/ConfigureSwapchain.h"
#include "Vulkanize/Configure/ConfigureDepthStencil.h"
#include "Vulkanize/Configure/ConfigureQueues.h"
#include "Vulkanize/Configure/ConfigureRenderPass.h"
#include "Vulkanize/Configure/ConfigureGraphicsPipeline.h"
#include "Vulkanize/Configure/ConfigureFramebuffers.h"
#include "Vulkanize/Configure/ConfigureCommandPool.h"
#include "Vulkanize/Configure/ConfigureVertexBuffers.h"
#include "Vulkanize/Configure/ConfigureCommandBuffers.h"
#include "Vulkanize/Configure/ConfigureSemaphores.h"
#include "Vulkanize/Configure/ConfigureFences.h"
#include "Vulkanize/Configure/ConfigureDescriptors.h"
#include "Vulkanize/Configure/ConfigureTextureSampler.h"
#include "Vulkanize/Render/RenderBegin.h"
#include "Vulkanize/Render/RenderEnd.h"
#include "Vulkanize/Material.h"

#if (VKZ_USE_GLSLANG)
  #include "Vulkanize/Configure/ConfigureGLSLang.h"
  #include "Vulkanize/GLSLangInterface.h"
#endif
