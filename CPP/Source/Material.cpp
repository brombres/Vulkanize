#include "Vulkanize/Vulkanize.h"
using namespace std;
using namespace VKZ;

//==============================================================================
// Material
//==============================================================================
Material::Material( Context* context ) : context(context)
{
}

bool Material::create()
{
  if ( !descriptors.create(context) ) return false;
  return true;
}

void Material::destroy()
{
  if (created)
  {
    created = false;
    shader = nullptr;
  }

  vertex_descriptions.clear();
}

Ref<CombinedImageSamplerDescriptor> Material::add_combined_image_sampler(
    uint32_t binding, size_t initial_count )
{
  CombinedImageSamplerDescriptor* descriptor = new CombinedImageSamplerDescriptor(
    context, binding, VK_SHADER_STAGE_FRAGMENT_BIT, initial_count
  );
  descriptors.add( descriptor );
  return descriptor;
}

Ref<CombinedImageSamplerDescriptor> Material::add_combined_image_sampler(
    uint32_t binding, Ref<Image> image, Ref<Sampler> sampler )
{
  if ( !sampler.exists() )
  {
    // Create default sampler
    SamplerInfo sampler_info( context );
    sampler = new Sampler( sampler_info );
  }

  CombinedImageSamplerDescriptor* descriptor = new CombinedImageSamplerDescriptor(
    context, binding, VK_SHADER_STAGE_FRAGMENT_BIT, image, sampler
  );
  descriptors.add( descriptor );
  return descriptor;
}

void Material::add_vertex_description( Ref<VertexDescription> vertex_description )
{
  vertex_descriptions.push_back( vertex_description );
}


/*
void Material::cmd_bind( VkCommandBuffer cmd )
{
  context->device_dispatch.cmdBindPipeline( cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline );
  descriptors->cmd_bind( cmd, this );
  cmd_bind_descriptor_set( cmd );
}

void Material::cmd_set_default_viewports_and_scissor_rects( VkCommandBuffer cmd )
{
  viewports.clear();
  scissor_rects.clear();
  set_default_viewport( 0 );
  set_default_scissor_rect( 0 );
  cmd_set_viewports_and_scissor_rects( cmd );
}

void Material::cmd_set_viewports_and_scissor_rects( VkCommandBuffer cmd )
{
  context->device_dispatch.cmdSetViewport( cmd, 0, (uint32_t)viewports.size(), viewports.data() );
  context->device_dispatch.cmdSetScissor(  cmd, 0, (uint32_t)scissor_rects.size(), scissor_rects.data() );
}

void Material::cmd_bind_descriptor_set( VkCommandBuffer cmd )
{
  if (descriptors)
  {
    descriptors->cmd_bind( cmd, this );
  }
}

void Material::set_default_scissor_rect( int index )
{
  set_scissor_rect( index, {}, context->surface_size );
}

void Material::set_default_viewport( int index, float min_depth, float max_depth )
{
  set_viewport( index, {}, context->surface_size, min_depth, max_depth );
}

void Material::set_scissor_rect( int index, VkExtent2D extent )
{
  set_scissor_rect( index, {}, extent );
}

void Material::set_scissor_rect( int index, VkOffset2D offset, VkExtent2D extent )
{
  if (scissor_rects.size() <= index) scissor_rects.resize( index+1 );
  scissor_rects[index] =
    {
      .offset = offset,
      .extent = extent
    };
}

void Material::set_viewport( int index, VkExtent2D extent, float min_depth, float max_depth )
{
  set_viewport( index, {}, extent, min_depth, max_depth );
}

void Material::set_viewport( int index, VkOffset2D offset, VkExtent2D extent, float min_depth, float max_depth )
{
  if (viewports.size() <= index) viewports.resize( index+1 );
  viewports[index] =
    {
      .x        = (float)offset.x,
      .y        = (float)offset.y,
      .width    = (float)extent.width,
      .height   = (float)extent.height,
      .minDepth = min_depth,
      .maxDepth = max_depth
    };
}

*/


/*
void Material::_configure_color_blend_info( VkPipelineColorBlendStateCreateInfo& color_blend_info )
{
  color_blend_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  color_blend_info.logicOpEnable = VK_FALSE;
  color_blend_info.logicOp = VK_LOGIC_OP_COPY;
  color_blend_info.attachmentCount = 1;
  color_blend_info.pAttachments = &color_blend_attachment;
  color_blend_info.blendConstants[0] = 0.0f;
  color_blend_info.blendConstants[1] = 0.0f;
  color_blend_info.blendConstants[2] = 0.0f;
  color_blend_info.blendConstants[3] = 0.0f;
}

void Material::_configure_multisampling_info( VkPipelineMultisampleStateCreateInfo& multisampling_info )
{
  multisampling_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling_info.sampleShadingEnable = VK_FALSE;
  multisampling_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
}

void Material::_configure_pipeline_info( VkGraphicsPipelineCreateInfo& pipeline_info )
{
  // pipeline_info is pre-filled; adjust config as desired.
}

void Material::_configure_pipeline_layout_info( VkPipelineLayoutCreateInfo& pipeline_layout_info )
{
  pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipeline_layout_info.setLayoutCount = 0;
  pipeline_layout_info.pushConstantRangeCount = 0;

  if (graphics_pipeline->descriptors)
  {
    pipeline_layout_info.setLayoutCount = 1;
    pipeline_layout_info.pSetLayouts = &graphics_pipeline->descriptors->layout;
  }
}

void Material::_configure_rasterizer_info( VkPipelineRasterizationStateCreateInfo& rasterizer_info )
{
  rasterizer_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer_info.depthClampEnable = VK_FALSE;
  rasterizer_info.rasterizerDiscardEnable = VK_FALSE;
  rasterizer_info.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizer_info.lineWidth = 1.0f;
  rasterizer_info.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterizer_info.frontFace = VK_FRONT_FACE_CLOCKWISE;
  rasterizer_info.depthBiasEnable = VK_FALSE;
}

void Material::_configure_viewports_and_scissor_rects()
{
  graphics_pipeline->set_default_viewport( 0 );
  graphics_pipeline->set_default_scissor_rect( 0 );

}

*/
