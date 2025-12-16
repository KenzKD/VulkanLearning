#pragma once
#define SHADER_PATH(x) "C:/Users/Lenovo/Desktop/Game_Making/Vulkan_Stuff/VulkanLearning/shaders/" x

#include "lve_pipeline.hpp"
#include "lve_window.hpp"
#include "lve_device.hpp"

namespace lve
{
	class FirstApp
	{
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		void run();

	private:
		LveWindow lveWindow{WIDTH, HEIGHT, "Vulkan window"};
		LveDevice lveDevice{lveWindow};
		LvePipeline lvePipeline
		{
			lveDevice,
			SHADER_PATH("simple_shader.vert.spv"),
			SHADER_PATH("simple_shader.frag.spv"),
			LvePipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT)
		};
	};
}
