#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG_APP
#undef DEBUG_APP

#ifndef DEBUG_APP
	const uint8_t enable_validation_layers = 0;
#else
	const uint8_t enable_validation_layers = 1;
#endif

typedef struct QueueFamilyIndices {
	uint32_t graphics_family;
} QueueFamilyIndices;

/* GENERAL */
const uint32_t window_width = 800;
const uint32_t window_height = 600;
void quit();
/* GENERAL */

/* GLFW */
GLFWwindow *glfw_window;
uint32_t vk_extensions_count;
void init_glfw();
void process_input(GLFWwindow *window);
/* GLFW */

/* VULKAN */
VkInstance vk_instance;
VkApplicationInfo vk_application_info;
VkInstanceCreateInfo vk_create_info;
VkPhysicalDevice vk_physical_device;
VkResult vk_result;
VkDevice vk_device;

void init_vulkan();
/* VULKAN */

int main() {
	printf("\n-------------------------------------------------\n");
	init_glfw();
	init_vulkan();

	while(!glfwWindowShouldClose(glfw_window)) {
		process_input(glfw_window);
		glfwPollEvents();
	}

	quit();
	printf("-------------------------------------------------\n");
	return 0;
}

void init_glfw() {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	glfw_window = glfwCreateWindow(window_width, window_height, "Vulkan", NULL, NULL);
}

void init_vulkan() {
	vk_application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	vk_application_info.pApplicationName = "Vulkan Triangle";
	vk_application_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	vk_application_info.pEngineName = "No Engine";
	vk_application_info.apiVersion = VK_API_VERSION_1_0;

	vkEnumerateInstanceExtensionProperties(NULL, &vk_extensions_count, NULL);
	VkExtensionProperties *vk_extension_properties = (VkExtensionProperties*)malloc(sizeof(VkExtensionProperties) * vk_extensions_count);
	vkEnumerateInstanceExtensionProperties(NULL, &vk_extensions_count, vk_extension_properties);

	vk_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	vk_create_info.pApplicationInfo = &vk_application_info;

	/* @TODO: Add MessageCallback() -> take it from Vulkan Tutorial.pdf. */
	const uint32_t num_validation_layers = 1;
	char** validation_layers = (char**)malloc(sizeof(char*) * num_validation_layers);
	for(uint32_t i = 0; i < num_validation_layers; ++i)
		validation_layers[i] = (char*)malloc(sizeof(char) * 100);

	strcpy(validation_layers[0], "VK_LAYER_KHRONOS_validation");
	
	uint32_t layer_count;
	vkEnumerateInstanceLayerProperties(&layer_count, NULL);

	VkLayerProperties *available_layers = (VkLayerProperties*)malloc(sizeof(VkLayerProperties) * layer_count);
	vkEnumerateInstanceLayerProperties(&layer_count, available_layers);

	for(uint32_t i = 0; i < num_validation_layers; ++i) {
		uint8_t layer_found = 0;
		
		for(uint32_t j = 0; j < layer_count; ++j) {
			if(strcmp(validation_layers[i], available_layers[j].layerName) == 0) {
					layer_found = 1;
					break;
			}
		}

		if(!layer_found) {
			printf("-- Validation layer requested, but not avalilable.\n");
		}
	}

	vk_create_info.enabledExtensionCount = 0;
	if(enable_validation_layers) {
		printf("-- Validation layers enabled.\n");
		vk_create_info.enabledLayerCount = num_validation_layers;
		vk_create_info.ppEnabledLayerNames = validation_layers;
	}
	else {
		printf("-- Validation layers disabled.\n");
		vk_create_info.enabledLayerCount = 0;
	}
	/* Validation layers */
	
	/* Creating VkResult */
	vk_result = vkCreateInstance(&vk_create_info, NULL, &vk_instance);

	if(vk_result != VK_SUCCESS) {
		printf("-- Failed to create VK_INSTANCE\n");
	}
	else {
		printf("-- Created a VK_INSTANCE successfully.\n");
	}
	/* Creating VkResult */

	/* Picking physical device */
	vk_physical_device = VK_NULL_HANDLE;
	uint32_t device_count = 0;

	vkEnumeratePhysicalDevices(vk_instance, &device_count, NULL);
	VkPhysicalDevice *physical_devices = (VkPhysicalDevice*)malloc(sizeof(VkPhysicalDevice) * device_count);
	vkEnumeratePhysicalDevices(vk_instance, &device_count, physical_devices);

	printf("\n-- Graphic cards available are:\n");
	for(uint32_t i = 0; i < device_count; ++i) {
		VkPhysicalDevice *current_device = &physical_devices[i];

		VkPhysicalDeviceProperties device_properties;
		vkGetPhysicalDeviceProperties(*current_device, &device_properties);

		VkPhysicalDeviceFeatures device_features;
		vkGetPhysicalDeviceFeatures(*current_device, &device_features);

		printf("%d. %s", i + 1, device_properties.deviceName);
		if(device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
			vk_physical_device = *current_device;
			printf(" -- PICKED");
		}
		printf("\n");
	}
	printf("\n");

	/* Queue Families */
	uint32_t queue_family_count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(vk_physical_device, &queue_family_count, NULL);
	VkQueueFamilyProperties *queue_families = (VkQueueFamilyProperties*)malloc(sizeof(VkQueueFamilyProperties) * queue_family_count);
	vkGetPhysicalDeviceQueueFamilyProperties(vk_physical_device, &queue_family_count, queue_families);

	QueueFamilyIndices queue_family_indices;
	for(uint32_t i = 0; i < queue_family_count; ++i) {
		if(queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			queue_family_indices.graphics_family = i;
			printf("-- Found VK_QUEUE_GRAPHICS_BIT.\n");
			break;
		}
	}
	/* Queue Families */
	/* Picking physical device */

	/* Logical device */
	VkDeviceQueueCreateInfo queue_create_info;
	queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queue_create_info.queueFamilyIndex = queue_family_indices.graphics_family;
	queue_create_info.queueCount = 1;
	float queue_priority = 1.0f;
	queue_create_info.pQueuePriorities = &queue_priority;

	VkPhysicalDeviceFeatures device_features;

	VkDeviceCreateInfo vk_device_create_info;
	vk_device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	vk_device_create_info.pQueueCreateInfos = &queue_create_info;
	vk_device_create_info.queueCreateInfoCount = 1;
	vk_device_create_info.pEnabledFeatures = &device_features;

	if(vkCreateDevice(vk_physical_device, &vk_create_info, NULL, &vk_device) != VK_SUCCESS) {
		printf("-- Failed to create logical device.\n");
	}
	else {
		printf("-- Created logical device.\n");
	}
	/* Logical device */
}

void process_input(GLFWwindow *window) {
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, 1);
	}
}

void quit() {
	vkDestroyInstance(vk_instance, NULL);
	glfwDestroyWindow(glfw_window);
	glfwTerminate();

	printf("\nQuit the application successfully.\n");
}
