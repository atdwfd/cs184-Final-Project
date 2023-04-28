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

#include "shader.hpp"


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

  /* Initialize GLFW. */
  
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

  /* Initialize GLAD. */

  gladLoadGL(glfwGetProcAddress);
  const GLubyte* version = glGetString(GL_VERSION);
  std::cout << "OpenGL version available: " << version << '\n';

  /* Load shaders, create program. */

  /* TODO: Have CMake copy the resources directory into the build directory to
     avoid the relative paths. */
  ShaderProgram program{};
  {
    auto vertex_shader = Shader::from_file("../resources/shaders/vertex.glsl", ShaderStep::Vertex);
    vertex_shader.compile();
    auto fragment_shader = Shader::from_file("../resources/shaders/fragment.glsl", ShaderStep::Fragment);
    fragment_shader.compile();
    if (!program.attach(vertex_shader).attach(fragment_shader).link()) {
      return -1;
    }
  }

  // Set up vertex data, buffers, and configure vertex attributes
  GLuint VBO, VAO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(image_vertices), image_vertices, GL_STATIC_DRAW);

  // Position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  // Texture coordinate attribute
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3* sizeof(float)));
  glEnableVertexAttribArray(1);

  while (!glfwWindowShouldClose(window.get())) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    program.use();
    glBindVertexArray(VAO);
    
    glDrawArrays(GL_TRIANGLES, 0, 4);

    glfwSwapBuffers(window.get());
    glfwPollEvents();
  }
}
