#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

/* GENERAL */
const uint32_t window_width = 800;
const uint32_t window_height = 600;
void quit();
/* GENERAL */

/* GLFW */
GLFWwindow *window;
uint32_t vk_extensions_count;
void init_glfw();
void process_input(GLFWwindow *window);
/* GLFW */

/* VULKAN */
VkInstance vk_instance;
VkApplicationInfo vk_application_info;
VkInstanceCreateInfo vk_create_info;
VkResult vk_result;
void init_vulkan();
/* VULKAN */

int main() {
	init_glfw();
	init_vulkan();

	while(!glfwWindowShouldClose(window)) {
		process_input(window);
		glfwPollEvents();
	}

	quit();
	return 0;
}

void init_glfw() {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window = glfwCreateWindow(window_width, window_height, "Vulkan", NULL, NULL);
}

void init_vulkan() {
	vk_application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	vk_application_info.pApplicationName = "Vulkan Triangle";
	vk_application_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	vk_application_info.pEngineName = "No Engine";
	vk_application_info.apiVersion = VK_API_VERSION_1_0;

	vk_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	vk_create_info.pApplicationInfo = &vk_application_info;
	vk_create_info.enabledLayerCount = 0;

	vkEnumerateInstanceExtensionProperties(NULL, &vk_extensions_count, NULL);
	VkExtensionProperties *vk_extension_properties = (VkExtensionProperties*)malloc(sizeof(VkExtensionProperties) * vk_extensions_count);
	vkEnumerateInstanceExtensionProperties(NULL, &vk_extensions_count, vk_extension_properties);
	
	vk_result = vkCreateInstance(&vk_create_info, NULL, &vk_instance);

	if(vk_result != VK_SUCCESS) {
		printf("Failed to create VK_INSTANCE\n");
	}
	else {
		printf("Created a VK_INSTANCE successfully.\n");
	}
}

void process_input(GLFWwindow *window) {
	float camera_speed = 0.5f;

	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, 1);
	}
}

void quit() {
	glfwDestroyWindow(window);
	glfwTerminate();
}
