// clang-format off

// GLAD must be included before GLFW. 
#include <glad/gl.h>
#include <GLFW/glfw3.h>

// clang-format on 
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>


constexpr int opengl_version_major = 3;
constexpr int opengl_version_minor = 3;
constexpr int default_screen_width = 800;
constexpr int default_screen_height = 600;
constexpr const char *window_title = "Visualizing Wormholes";

/* We render two triangles (a rectangle) that cover the entire screen
   as we're rendering a single image. */
constexpr float image_vertices[] = {
    // Positions         // Texture Coords
    -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
    -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
     1.0f, -1.0f, 0.0f,  1.0f, 0.0f,

     1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
     1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
    -1.0f,  1.0f, 0.0f,  0.0f, 1.0f
};

auto main(int argc, char **argv) -> int {

  if (argc == 1) {
    std::cout << "Using default resolution: " << default_screen_width << " x "
              << default_screen_height << '\n';
  }
  /* TODO: Allow user to provide resolution themselves. */

  if (!glfwInit()) {
    std::cerr << "[ERROR]: Failed to initialize GLFW.\n";
    return -1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, opengl_version_major);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, opengl_version_minor);

  std::unique_ptr<GLFWwindow, decltype(&glfwDestroyWindow)> window{
      glfwCreateWindow(default_screen_width, default_screen_height,
                       window_title, nullptr, nullptr),
      glfwDestroyWindow};

  if (!window) {
    std::cerr << "[ERROR]: Failed to initialize GLFW window.\n";
    glfwTerminate();
  }

  glfwMakeContextCurrent(window.get());

  const auto version = gladLoadGL(glfwGetProcAddress);

  std::cout << "Using GL " << version << "." << version << '\n';

  while (!glfwWindowShouldClose(window.get())) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glfwSwapBuffers(window.get());
    glfwPollEvents();
  }
}
