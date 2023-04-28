cmake_minimum_required(VERSION 3.20)
project(wormhole VERSION 0.1 LANGUAGES CXX)

find_package(glfw3 3.3 REQUIRED)

add_subdirectory("./libs/glad/")

set(
  SOURCES
  "./src/main.cpp"
  # To add more...
)

add_executable(${PROJECT_NAME} ${SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED ON
)

target_link_libraries(${PROJECT_NAME} PRIVATE glad glfw)
target_include_directories(${PROJECT_NAME} PRIVATE ${GLFW_INCLUDE_DIRS})


