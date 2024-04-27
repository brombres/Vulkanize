#include <cstdio>
#include "Vulkanize/Vulkanize.h"
using namespace VKZ;

Vulkanize* Vulkanize::instance = nullptr;

Vulkanize::Vulkanize()
{
  Vulkanize::instance = this;
}

Vulkanize::~Vulkanize()
{
  reset();
  Vulkanize::instance = nullptr;
}

bool Vulkanize::reset()
{
  while (context_instances.size())
  {
    delete context_instances.back();  // Context will remove itself from this list
  }

  if (instance_builder)
  {
    delete instance_builder;
    instance_builder = nullptr;
  }

  if (configured)
  {
    destroy();
    vkb::destroy_instance( vulkan_instance );
  }

  return false;
}

bool Vulkanize::configure( PFN_vkGetInstanceProcAddr fp_vkGetInstanceProcAddr )
{
  if (configured) reset();

  auto builder = vulkan_instance_builder( fp_vkGetInstanceProcAddr );
  builder.request_validation_layers( _request_validation_layers );
  builder.require_api_version( _v_major, _v_minor, _v_bugfix );
  builder.set_app_name( _app_name.c_str() );
  if (_use_default_debug_messenger) builder.use_default_debug_messenger();

  VKZ_SET( vulkan_instance, builder.build(), "creating Vulkan instance", return reset() );
	debug_messenger = vulkan_instance.debug_messenger;
  instance_dispatch = vulkan_instance.make_table();

  Process::configure();
  return true;
}

void Vulkanize::configure_operations()
{
  #if (VKZ_USE_GLSLANG)
    set_operation( "configure.glslang", new ConfigureGLSLang() );
  #endif
}

Vulkanize& Vulkanize::request_validation_layers( bool setting )
{
  _request_validation_layers = setting;
  return *this;
}

Vulkanize& Vulkanize::require_api_version( int major, int minor, int bugfix )
{
  _v_major = major;
  _v_minor = minor;
  _v_bugfix = bugfix;
  return *this;
}

Vulkanize& Vulkanize::set_app_name( const char* app_name )
{
  _app_name = app_name;
  return *this;
}

Vulkanize& Vulkanize::use_default_debug_messenger()
{
  _use_default_debug_messenger = true;
  return *this;
}

vkb::InstanceBuilder& Vulkanize::vulkan_instance_builder( PFN_vkGetInstanceProcAddr fp_vkGetInstanceProcAddr )
{
  if ( !instance_builder ) instance_builder = new vkb::InstanceBuilder( fp_vkGetInstanceProcAddr );
  return *instance_builder;
}

// https://www.reddit.com/r/vulkan/comments/zxgst4/comment/j29a1kl/?utm_source=share&utm_medium=web2x&context=3
const char* Vulkanize::vkResult_to_c_string( VkResult result )
{
	switch (result)
  {
#define VK_RESULT_CASE(x) case VK_##x: return #x;
	VK_RESULT_CASE(SUCCESS)                        VK_RESULT_CASE(NOT_READY)
	VK_RESULT_CASE(TIMEOUT)                        VK_RESULT_CASE(EVENT_SET)
	VK_RESULT_CASE(EVENT_RESET)                    VK_RESULT_CASE(INCOMPLETE)
	VK_RESULT_CASE(ERROR_OUT_OF_HOST_MEMORY)       VK_RESULT_CASE(ERROR_OUT_OF_DEVICE_MEMORY)
	VK_RESULT_CASE(ERROR_INITIALIZATION_FAILED)    VK_RESULT_CASE(ERROR_DEVICE_LOST)
	VK_RESULT_CASE(ERROR_MEMORY_MAP_FAILED)        VK_RESULT_CASE(ERROR_LAYER_NOT_PRESENT)
	VK_RESULT_CASE(ERROR_EXTENSION_NOT_PRESENT)    VK_RESULT_CASE(ERROR_FEATURE_NOT_PRESENT)
	VK_RESULT_CASE(ERROR_INCOMPATIBLE_DRIVER)      VK_RESULT_CASE(ERROR_TOO_MANY_OBJECTS)
	VK_RESULT_CASE(ERROR_FORMAT_NOT_SUPPORTED)     VK_RESULT_CASE(ERROR_FRAGMENTED_POOL)
	VK_RESULT_CASE(ERROR_UNKNOWN)                  VK_RESULT_CASE(ERROR_OUT_OF_POOL_MEMORY)
	VK_RESULT_CASE(ERROR_INVALID_EXTERNAL_HANDLE)  VK_RESULT_CASE(ERROR_FRAGMENTATION)
	VK_RESULT_CASE(ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS)
	VK_RESULT_CASE(PIPELINE_COMPILE_REQUIRED)      VK_RESULT_CASE(ERROR_SURFACE_LOST_KHR)
	VK_RESULT_CASE(ERROR_NATIVE_WINDOW_IN_USE_KHR) VK_RESULT_CASE(SUBOPTIMAL_KHR)
	VK_RESULT_CASE(ERROR_OUT_OF_DATE_KHR)          VK_RESULT_CASE(ERROR_INCOMPATIBLE_DISPLAY_KHR)
	VK_RESULT_CASE(ERROR_VALIDATION_FAILED_EXT)    VK_RESULT_CASE(ERROR_INVALID_SHADER_NV)
#ifdef VK_ENABLE_BETA_EXTENSIONS
	VK_RESULT_CASE(ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR)
	VK_RESULT_CASE(ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR)
	VK_RESULT_CASE(ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR)
	VK_RESULT_CASE(ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR)
	VK_RESULT_CASE(ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR)
	VK_RESULT_CASE(ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR)
#endif
	VK_RESULT_CASE(ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT)
	VK_RESULT_CASE(ERROR_NOT_PERMITTED_KHR)
	VK_RESULT_CASE(ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT)
	VK_RESULT_CASE(THREAD_IDLE_KHR)        VK_RESULT_CASE(THREAD_DONE_KHR)
	VK_RESULT_CASE(OPERATION_DEFERRED_KHR) VK_RESULT_CASE(OPERATION_NOT_DEFERRED_KHR)
	default: return "unknown";
	}
#undef VK_RESULT_CASE
}
