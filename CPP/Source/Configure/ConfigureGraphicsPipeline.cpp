#include "VkBootstrap.h"
#include "Vulkanize/Vulkanize.h"
using namespace std;
using namespace VKZ;

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
  VkShaderModule vertex_module = compile_shader(
    context,
    VK_SHADER_STAGE_VERTEX_BIT,
    "shader.vert",
    "#version 450\n"
    "#extension GL_ARB_separate_shader_objects : enable\n"
    "\n"
    "layout (location = 0) in vec2 position;\n"
    "layout (location = 2) in vec3 color;\n"
    "\n"
    "layout (location = 0) out vec3 fragColor;\n"
    "\n"
    "void main ()\n"
    "{\n"
      "gl_Position = vec4 (position, 0.0, 1.0);\n"
      "fragColor = color;\n"
    "}\n"
  );
  if (vertex_module == VK_NULL_HANDLE) return false;

  VkShaderModule fragment_module = compile_shader(
    context,
    VK_SHADER_STAGE_FRAGMENT_BIT,
    "shader.frag",
    "#version 450\n"
    "#extension GL_ARB_separate_shader_objects : enable\n"
    "\n"
    "layout (location = 0) in vec3 fragColor;\n"
    "\n"
    "layout (location = 0) out vec4 outColor;\n"
    "\n"
    "void main () { outColor = vec4 (fragColor, 1.0); }\n"
  );
  if (fragment_module == VK_NULL_HANDLE)
  {
    context->device_dispatch.destroyShaderModule( vertex_module, nullptr );
    return false;
  }

  VkPipelineShaderStageCreateInfo vertex_stage_info = {};
  vertex_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertex_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vertex_stage_info.module = vertex_module;
  vertex_stage_info.pName = "main";

  VkPipelineShaderStageCreateInfo fragment_stage_info = {};
  fragment_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  fragment_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  fragment_stage_info.module = fragment_module;
  fragment_stage_info.pName = "main";

  VkPipelineShaderStageCreateInfo shader_stages[] =
      { vertex_stage_info, fragment_stage_info };

  vector<VkVertexInputBindingDescription>   binding_descriptions;
  vector<VkVertexInputAttributeDescription> attribute_descriptions;
  uint32_t binding = 0;
  for (auto vertex_description : vertex_descriptions)
  {
    vertex_description->collect_binding_description( binding_descriptions );
    binding_descriptions.back().binding = binding;

    uint32_t i = attribute_descriptions.size();
    vertex_description->collect_attribute_descriptions( attribute_descriptions );
    for (; i<attribute_descriptions.size(); ++i)
    {
      attribute_descriptions[i].binding = binding;
    }

    ++binding;
  }

  VkPipelineVertexInputStateCreateInfo vertex_input_info = {};
  vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertex_input_info.vertexBindingDescriptionCount = binding_descriptions.size();
  vertex_input_info.vertexAttributeDescriptionCount = attribute_descriptions.size();
  vertex_input_info.pVertexBindingDescriptions = binding_descriptions.data();
  vertex_input_info.pVertexAttributeDescriptions = attribute_descriptions.data();

  VkPipelineInputAssemblyStateCreateInfo input_assembly = {};
  input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  input_assembly.primitiveRestartEnable = VK_FALSE;

  VkViewport viewport = {};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width  = context->surface_size.width;
  viewport.height = context->surface_size.height;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;

  VkRect2D scissor = {};
  scissor.offset = { 0, 0 };
  scissor.extent = context->surface_size;

  VkPipelineViewportStateCreateInfo viewport_state = {};
  viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewport_state.viewportCount = 1;
  viewport_state.pViewports = &viewport;
  viewport_state.scissorCount = 1;
  viewport_state.pScissors = &scissor;

  VkPipelineRasterizationStateCreateInfo rasterizer = {};
  rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer.depthClampEnable = VK_FALSE;
  rasterizer.rasterizerDiscardEnable = VK_FALSE;
  rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizer.lineWidth = 1.0f;
  rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
  rasterizer.depthBiasEnable = VK_FALSE;

  VkPipelineMultisampleStateCreateInfo multisampling = {};
  multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling.sampleShadingEnable = VK_FALSE;
  multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

  VkPipelineColorBlendAttachmentState color_blend_attachment = {};
  color_blend_attachment.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  color_blend_attachment.blendEnable = VK_FALSE;

  VkPipelineColorBlendStateCreateInfo color_blending = {};
  color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  color_blending.logicOpEnable = VK_FALSE;
  color_blending.logicOp = VK_LOGIC_OP_COPY;
  color_blending.attachmentCount = 1;
  color_blending.pAttachments = &color_blend_attachment;
  color_blending.blendConstants[0] = 0.0f;
  color_blending.blendConstants[1] = 0.0f;
  color_blending.blendConstants[2] = 0.0f;
  color_blending.blendConstants[3] = 0.0f;

  VkPipelineLayoutCreateInfo pipeline_layout_info = {};
  pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipeline_layout_info.setLayoutCount = 0;
  pipeline_layout_info.pushConstantRangeCount = 0;

  VKZ_REQUIRE(
    "creating pipeline layout",
    context->device_dispatch.createPipelineLayout(
      &pipeline_layout_info, nullptr, &context->pipeline_layout
    )
  );
  progress = 1;

  vector<VkDynamicState> dynamic_states =
  {
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_SCISSOR,
    VK_DYNAMIC_STATE_DEPTH_BIAS
  };

  VkPipelineDynamicStateCreateInfo dynamic_info = {};
  dynamic_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamic_info.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size());
  dynamic_info.pDynamicStates = dynamic_states.data();

  VkGraphicsPipelineCreateInfo pipeline_info = {};
  pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipeline_info.stageCount = 2;
  pipeline_info.pStages = shader_stages;
  pipeline_info.pVertexInputState = &vertex_input_info;
  pipeline_info.pInputAssemblyState = &input_assembly;
  pipeline_info.pViewportState = &viewport_state;
  pipeline_info.pRasterizationState = &rasterizer;
  pipeline_info.pMultisampleState = &multisampling;
  pipeline_info.pColorBlendState = &color_blending;
  pipeline_info.pDynamicState = &dynamic_info;
  pipeline_info.layout = context->pipeline_layout;
  pipeline_info.renderPass = context->render_pass;
  pipeline_info.subpass = 0;
  pipeline_info.basePipelineHandle = VK_NULL_HANDLE;

  VKZ_REQUIRE(
    "creating graphics pipeline",
    context->device_dispatch.createGraphicsPipelines(
      VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &context->graphics_pipeline
    )
  );
  progress = 2;

  context->device_dispatch.destroyShaderModule( fragment_module, nullptr );
  context->device_dispatch.destroyShaderModule( vertex_module, nullptr );
  return true;
}

void ConfigureGraphicsPipeline::add_shader_stage( VkShaderStageFlagBits stage, VkShaderModule module,
    const char* main_function_name )
{
  ShaderStageInfo* stage_info = new ShaderStageInfo( stage, module, main_function_name );
  shader_stages.push_back( stage_info );
}

void ConfigureGraphicsPipeline::add_shader_stage( VkShaderStageFlagBits stage,
    string shader_filename, string shader_source, const char* main_function_name )
{
  ShaderStageInfo* stage_info = new ShaderStageInfo( stage, shader_source, main_function_name );
  shader_stages.push_back( stage_info );
}

void ConfigureGraphicsPipeline::add_shader_stage( VkShaderStageFlagBits stage, const char* spirv_bytes,
    size_t spirv_byte_count, const char* main_function_name )
{
  ShaderStageInfo* stage_info = new ShaderStageInfo( stage, spirv_bytes, spirv_byte_count, main_function_name );
  shader_stages.push_back( stage_info );
}

void ConfigureGraphicsPipeline::add_vertex_description( VertexDescription* vertex_description )
{
  vertex_descriptions.push_back( vertex_description );
}

void ConfigureGraphicsPipeline::deactivate()
{
  if (progress >= 2) context->device_dispatch.destroyPipeline( context->graphics_pipeline, nullptr );
  if (progress >= 1) context->device_dispatch.destroyPipelineLayout( context->pipeline_layout, nullptr );
}

VkShaderModule ConfigureGraphicsPipeline::_create_shader_module( const Byte* code, int count )
{
  VkShaderModuleCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  create_info.codeSize = count;
  create_info.pCode = (const uint32_t*)code;

  VkShaderModule shader_module;
  VKZ_ON_ERROR(
    "creating shader module",
    context->device_dispatch.createShaderModule( &create_info, nullptr, &shader_module ),
    return VK_NULL_HANDLE;
  );

  return shader_module;
}

