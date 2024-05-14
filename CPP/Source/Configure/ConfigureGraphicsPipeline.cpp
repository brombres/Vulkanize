#include "VkBootstrap.h"
#include "Vulkanize/Vulkanize.h"
using namespace std;
using namespace VKZ;

ConfigureGraphicsPipeline::ConfigureGraphicsPipeline( GraphicsPipeline* graphics_pipeline )
  : graphics_pipeline(graphics_pipeline)
{
  // Default dynamic states
  dynamic_states.push_back( VK_DYNAMIC_STATE_VIEWPORT );
  dynamic_states.push_back( VK_DYNAMIC_STATE_SCISSOR );
  dynamic_states.push_back( VK_DYNAMIC_STATE_DEPTH_BIAS );

  // Default color blend attachment
  color_blend_attachment.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  color_blend_attachment.blendEnable = VK_FALSE;
}

ConfigureGraphicsPipeline::~ConfigureGraphicsPipeline()
{
  while (shader_stages.size())
  {
    delete shader_stages.back();
    shader_stages.pop_back();
  }

  while (vertex_descriptions.size())
  {
    delete vertex_descriptions.back();
    vertex_descriptions.pop_back();
  }
}

bool ConfigureGraphicsPipeline::activate()
{
  graphics_pipeline->context = context;

  vector<VkPipelineShaderStageCreateInfo> shader_create_info;
  shader_create_info.resize( shader_stages.size() );
  for (size_t i=0; i<shader_stages.size(); ++i)
  {
    if ( !shader_stages[i]->get_create_info( shader_create_info[i] ) ) return false;
  }

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
  input_assembly.primitiveRestartEnable = enable_primitive_restart;

  graphics_pipeline->viewports.clear();
  graphics_pipeline->scissor_rects.clear();
  configure_viewports_and_scissor_rects();

  VkPipelineViewportStateCreateInfo viewport_info = {};
  viewport_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewport_info.viewportCount = (uint32_t)graphics_pipeline->viewports.size();
  viewport_info.pViewports    = graphics_pipeline->viewports.data();
  viewport_info.scissorCount  = (uint32_t)graphics_pipeline->scissor_rects.size();
  viewport_info.pScissors     = graphics_pipeline->scissor_rects.data();

  VkPipelineRasterizationStateCreateInfo rasterizer_info =  {};
  configure_rasterizer_info( rasterizer_info );

  VkPipelineMultisampleStateCreateInfo multisampling_info =  {};
  configure_multisampling_info( multisampling_info );

  VkPipelineColorBlendStateCreateInfo color_blend_info =  {};
  configure_color_blend_info( color_blend_info );

  VkPipelineLayoutCreateInfo pipeline_layout_info = {};
  configure_pipeline_layout_info( pipeline_layout_info );

  VKZ_ATTEMPT(
    "creating pipeline layout",
    context->device_dispatch.createPipelineLayout(
      &pipeline_layout_info, nullptr, &graphics_pipeline->layout
    ),
    return false
  );
  progress = 1;

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
  pipeline_info.layout = graphics_pipeline->layout;
  pipeline_info.renderPass = context->render_pass;
  pipeline_info.subpass = 0;
  pipeline_info.basePipelineHandle = VK_NULL_HANDLE;

  configure_pipeline_info( pipeline_info );  // adjust any of the above settings as desired

  VKZ_ATTEMPT(
    "creating graphics pipeline",
    context->device_dispatch.createGraphicsPipelines(
      VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &graphics_pipeline->vk_pipeline
    ),
    return false
  );
  progress = 2;

  for (auto shader_stage : shader_stages) shader_stage->destroy_module();

  return true;
}

void ConfigureGraphicsPipeline::add_shader_stage( VkShaderStageFlagBits stage,
    string shader_filename, string shader_source, const char* main_function_name )
{
  ShaderStageInfo* stage_info = new ShaderStageInfo( context, stage, shader_filename, shader_source, main_function_name );
  shader_stages.push_back( stage_info );
}

void ConfigureGraphicsPipeline::add_shader_stage( VkShaderStageFlagBits stage, string shader_filename,
    const char* spirv_bytes, size_t spirv_byte_count, const char* main_function_name )
{
  ShaderStageInfo* stage_info = new ShaderStageInfo( context, stage, shader_filename, spirv_bytes,
      spirv_byte_count, main_function_name );
  shader_stages.push_back( stage_info );
}

void ConfigureGraphicsPipeline::add_vertex_description( VertexDescription* vertex_description )
{
  vertex_descriptions.push_back( vertex_description );
}

void ConfigureGraphicsPipeline::deactivate()
{
  if (progress >= 2) context->device_dispatch.destroyPipeline( graphics_pipeline->vk_pipeline, nullptr );
  if (progress >= 1) context->device_dispatch.destroyPipelineLayout( graphics_pipeline->layout, nullptr );

  for (auto shader_stage : shader_stages)
  {
    shader_stage->destroy_module();
  }
}

void ConfigureGraphicsPipeline::configure_color_blend_info( VkPipelineColorBlendStateCreateInfo& color_blend_info )
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

void ConfigureGraphicsPipeline::configure_multisampling_info( VkPipelineMultisampleStateCreateInfo& multisampling_info )
{
  multisampling_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling_info.sampleShadingEnable = VK_FALSE;
  multisampling_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
}

void ConfigureGraphicsPipeline::configure_pipeline_info( VkGraphicsPipelineCreateInfo& pipeline_info )
{
  // pipeline_info is pre-filled; adjust config as desired.
}

void ConfigureGraphicsPipeline::configure_pipeline_layout_info( VkPipelineLayoutCreateInfo& pipeline_layout_info )
{
  pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipeline_layout_info.setLayoutCount = 0;
  pipeline_layout_info.pushConstantRangeCount = 0;
}

void ConfigureGraphicsPipeline::configure_rasterizer_info( VkPipelineRasterizationStateCreateInfo& rasterizer_info )
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

void ConfigureGraphicsPipeline::configure_viewports_and_scissor_rects()
{
  graphics_pipeline->set_default_viewport( 0 );
  graphics_pipeline->set_default_scissor_rect( 0 );

}

