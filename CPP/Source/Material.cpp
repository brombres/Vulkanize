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
  if ( !_create_graphics_pipeline() ) return false;
  return true;
}

void Material::destroy()
{
  if (pipeline_construction_stage >= 2)
  {
    context->device_dispatch.destroyPipeline( pipeline, nullptr );
  }
  if (pipeline_construction_stage >= 1)
  {
    context->device_dispatch.destroyPipelineLayout( pipeline_layout, nullptr );
  }
  pipeline_construction_stage = 0;

  shader_stages.clear();
  vertex_descriptions.clear();
  created = false;
}

//------------------------------------------------------------------------------
// Descriptors
//------------------------------------------------------------------------------
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
  CombinedImageSamplerDescriptor* descriptor = new CombinedImageSamplerDescriptor(
    context, binding, VK_SHADER_STAGE_FRAGMENT_BIT, image, sampler
  );
  descriptors.add( descriptor );
  return descriptor;
}

Ref<SampledImageDescriptor> Material::add_sampled_image(
    uint32_t binding, size_t initial_count )
{
  SampledImageDescriptor* descriptor = new SampledImageDescriptor(
    context, binding, VK_SHADER_STAGE_FRAGMENT_BIT, initial_count
  );
  descriptors.add( descriptor );
  return descriptor;
}

Ref<SampledImageDescriptor> Material::add_sampled_image( uint32_t binding, Ref<Image> image )
{
  SampledImageDescriptor* descriptor = new SampledImageDescriptor(
    context, binding, VK_SHADER_STAGE_FRAGMENT_BIT, image
  );
  descriptors.add( descriptor );
  return descriptor;
}

Ref<SamplerDescriptor> Material::add_sampler(
    uint32_t binding, size_t initial_count )
{
  SamplerDescriptor* descriptor = new SamplerDescriptor(
    context, binding, VK_SHADER_STAGE_FRAGMENT_BIT, initial_count
  );
  descriptors.add( descriptor );
  return descriptor;
}

Ref<SamplerDescriptor> Material::add_sampler(
    uint32_t binding, Ref<Sampler> sampler )
{
  SamplerDescriptor* descriptor = new SamplerDescriptor(
    context, binding, VK_SHADER_STAGE_FRAGMENT_BIT, sampler
  );
  descriptors.add( descriptor );
  return descriptor;
}

//------------------------------------------------------------------------------
// Shaders
//------------------------------------------------------------------------------
Ref<Shader> Material::add_fragment_shader( string shader_filename, string shader_source,
    string main_function_name )
{
  return add_shader( VK_SHADER_STAGE_FRAGMENT_BIT, shader_filename, shader_source, main_function_name );
}

Ref<Shader> Material::add_fragment_shader( string shader_filename, const char* spirv_bytecode,
    size_t byte_count, string main_function_name )
{
  return add_shader( VK_SHADER_STAGE_FRAGMENT_BIT, shader_filename,
      spirv_bytecode, byte_count, main_function_name );
}

Ref<Shader> Material::add_shader( VkShaderStageFlagBits stage, std::string shader_filename,
    std::string shader_source, std::string main_function_name )
{
  Ref<Shader> result =
    new Shader( context, stage, shader_filename, shader_source, main_function_name );
  shader_stages.push_back( result );
  return result;
}

Ref<Shader> Material::add_shader( VkShaderStageFlagBits stage, std::string shader_filename,
    const char* spirv_bytecode, size_t byte_count, std::string main_function_name )
{
  Ref<Shader> result =
    new Shader( context, stage, shader_filename, spirv_bytecode, byte_count, main_function_name );
  shader_stages.push_back( result );
  return result;
}

Ref<Shader> Material::add_vertex_shader( string shader_filename, string shader_source,
    string main_function_name )
{
  return add_shader( VK_SHADER_STAGE_VERTEX_BIT, shader_filename, shader_source, main_function_name );
}

Ref<Shader> Material::add_vertex_shader( string shader_filename, const char* spirv_bytecode,
    size_t byte_count, string main_function_name )
{
  return add_shader( VK_SHADER_STAGE_VERTEX_BIT, shader_filename,
      spirv_bytecode, byte_count, main_function_name );
}

void Material::add_vertex_description( Ref<VertexDescription> vertex_description )
{
  vertex_descriptions.push_back( vertex_description );
}


void Material::cmd_bind( VkCommandBuffer cmd )
{
  context->device_dispatch.cmdBindPipeline( cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline );
  descriptors.cmd_bind( cmd, pipeline_layout );
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

bool Material::_create_graphics_pipeline()
{
  // Collect VkPipelineShaderStageCreateInfo from shader stages
  vector<VkPipelineShaderStageCreateInfo> shader_create_info;
  if (shader_stages.size())
  {
    shader_create_info.resize( shader_stages.size() );
    for (size_t i=0; i<shader_stages.size(); ++i)
    {
      if ( !shader_stages[i]->get_create_info( shader_create_info[i] ) ) return false;
    }
  }

  // VkVertexInputBindingDescriptions and VkVertexInputAttributeDescriptions
  vector<VkVertexInputBindingDescription>   binding_descriptions;
  vector<VkVertexInputAttributeDescription> attribute_descriptions;
  uint32_t binding = 0;
  for (auto vertex_description : vertex_descriptions)
  {
    vertex_description->collect_binding_description( binding_descriptions );
    binding_descriptions.back().binding = binding;

    size_t i = attribute_descriptions.size();
    vertex_description->collect_attribute_descriptions( attribute_descriptions );
    for (; i<attribute_descriptions.size(); ++i)
    {
      attribute_descriptions[i].binding = binding;
    }

    ++binding;
  }

  VkPipelineVertexInputStateCreateInfo vertex_input_info = {};
  vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertex_input_info.vertexBindingDescriptionCount = (uint32_t)binding_descriptions.size();
  vertex_input_info.vertexAttributeDescriptionCount = (uint32_t)attribute_descriptions.size();
  vertex_input_info.pVertexBindingDescriptions = binding_descriptions.data();
  vertex_input_info.pVertexAttributeDescriptions = attribute_descriptions.data();

  VkPipelineInputAssemblyStateCreateInfo input_assembly = {};
  input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  input_assembly.topology = topology;
  input_assembly.primitiveRestartEnable = primitive_restart_enabled;

  _configure_viewports_and_scissor_rects();

  VkPipelineViewportStateCreateInfo viewport_info = {};
  viewport_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewport_info.viewportCount = (uint32_t)viewports.size();
  viewport_info.pViewports    = viewports.data();
  viewport_info.scissorCount  = (uint32_t)scissor_rects.size();
  viewport_info.pScissors     = scissor_rects.data();

  VkPipelineRasterizationStateCreateInfo rasterizer_info =  {};
  _configure_rasterizer_info( rasterizer_info );

  VkPipelineMultisampleStateCreateInfo multisampling_info =  {};
  _configure_multisampling_info( multisampling_info );

  std::vector<VkPipelineColorBlendAttachmentState> color_blend_attachments;
  _configure_color_blend_attachments( color_blend_attachments );

  VkPipelineColorBlendStateCreateInfo color_blend_info =  {};
  _configure_color_blend_info( color_blend_info, color_blend_attachments );

  VkPipelineLayoutCreateInfo pipeline_layout_info = {};
  _configure_pipeline_layout_info( pipeline_layout_info );

  VKZ_ATTEMPT(
    "creating pipeline layout",
    context->device_dispatch.createPipelineLayout(
      &pipeline_layout_info, nullptr, &pipeline_layout
    ),
    return false
  );
  pipeline_construction_stage = 1;

  std::vector<VkDynamicState> dynamic_states;
  _configure_dynamic_states( dynamic_states );

  VkPipelineDynamicStateCreateInfo dynamic_info = {};
  dynamic_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamic_info.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size());
  dynamic_info.pDynamicStates = dynamic_states.data();

  VkGraphicsPipelineCreateInfo pipeline_info = {};
  pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipeline_info.stageCount = (uint32_t)shader_create_info.size();
  pipeline_info.pStages = shader_create_info.data();
  pipeline_info.pVertexInputState = &vertex_input_info;
  pipeline_info.pInputAssemblyState = &input_assembly;
  pipeline_info.pViewportState = &viewport_info;
  pipeline_info.pRasterizationState = &rasterizer_info;
  pipeline_info.pMultisampleState = &multisampling_info;
  pipeline_info.pColorBlendState = &color_blend_info;
  pipeline_info.pDynamicState = &dynamic_info;
  pipeline_info.layout = pipeline_layout;
  pipeline_info.renderPass = context->render_pass;
  pipeline_info.subpass = 0;
  pipeline_info.basePipelineHandle = VK_NULL_HANDLE;

  _configure_pipeline_info( pipeline_info );  // adjust any of the above settings as desired

  VKZ_ATTEMPT(
    "creating graphics pipeline",
    context->device_dispatch.createGraphicsPipelines(
      VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &pipeline
    ),
    return false
  );
  pipeline_construction_stage = 2;

  return true;
}

void Material::_configure_color_blend_info( VkPipelineColorBlendStateCreateInfo& color_blend_info,
    std::vector<VkPipelineColorBlendAttachmentState>& color_blend_attachments )
{
  color_blend_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  color_blend_info.logicOpEnable = VK_FALSE;
  color_blend_info.logicOp = VK_LOGIC_OP_COPY;
  color_blend_info.attachmentCount = (uint32_t)color_blend_attachments.size();
  color_blend_info.pAttachments = color_blend_attachments.data();
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

void Material::_configure_color_blend_attachments(
        std::vector<VkPipelineColorBlendAttachmentState>& color_blend_attachments )
{
  VkPipelineColorBlendAttachmentState attachment;
  attachment.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  attachment.blendEnable = VK_TRUE;
  attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
  attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  attachment.colorBlendOp = VK_BLEND_OP_ADD;
  attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
  attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
  attachment.alphaBlendOp = VK_BLEND_OP_ADD;

  color_blend_attachments.push_back( attachment );
}

void Material::_configure_dynamic_states( std::vector<VkDynamicState>& dynamic_states )
{
  dynamic_states.push_back( VK_DYNAMIC_STATE_VIEWPORT );
  dynamic_states.push_back( VK_DYNAMIC_STATE_SCISSOR );
  dynamic_states.push_back( VK_DYNAMIC_STATE_DEPTH_BIAS );
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

  pipeline_layout_info.setLayoutCount = 1;
  pipeline_layout_info.pSetLayouts = &descriptors.layout;
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
  set_default_viewport( 0 );
  set_default_scissor_rect( 0 );
}
