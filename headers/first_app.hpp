#pragma once
#define SHADER_PATH(x) "C:/Users/Lenovo/Desktop/Game_Making/Vulkan_Stuff/VulkanLearning/shaders/" x

#include "lve_window.hpp"
#include "lve_pipeline.hpp"

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
		LvePipeline lve_pipeline
		{
			SHADER_PATH("simple_shader.vert.spv"),
			SHADER_PATH("simple_shader.frag.spv")
		};
	};
}
