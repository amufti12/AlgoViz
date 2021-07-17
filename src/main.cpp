#include <iostream>

#include <algorithm>

#include <imgui.h>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <glad/glad.h>

#include <GLFW/glfw3.h>
#pragma warning(push)
#pragma warning(disable : 4201)
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#pragma warning(pop)

#include "util/Logger.h"

#include "Sorts.hpp"

#include "Utils.h"

#define AV_UNUSED(x) (void)(x)

void GetArray(SortableRenderData* array, int size)
{
  for (int i = 0; i < size; i++) {
    array[i].data = rand() / float(RAND_MAX);
  }
}

int main(int argc, char** argv)
{
  AV_UNUSED(argc);
  AV_UNUSED(argv);

  srand(static_cast<uint32_t>(time(NULL)));

  Logger::Init();
  Logger::SetLogLevel(Logger::LogLevel::INFO);

  // ////////////////////////////////////////////////////////
  // // SORTING ALGORITHM TEST SECTION
  // // Make array you want to test and call function from Sorts
  // SortableRenderData array[10];
  // GetArray(array, 10);
  // std::cout << "Test Array:\t";
  // for (int i = 0; i < 10; i++) {
  //   std::cout << array[i] << " ";
  // }
  // std::cout << std::endl;

  // // Selection Sort Test
  // selectionSort(array, 10);
  // std::cout << "Selection Sort:\t";
  // for (int i = 0; i < 10; i++) {
  //   std::cout << array[i] << " ";
  // }
  // std::cout << std::endl;

  // // Bubble Sort Test
  // GetArray(array, 10);
  // bubbleSort(array, 10);
  // std::cout << "Bubble Sort:\t";
  // for (int i = 0; i < 10; i++) {
  //   std::cout << array[i] << " ";
  // }
  // std::cout << std::endl;

  // // Insertion Sort Test
  // GetArray(array, 10);
  // insertionSort(array, 10);
  // std::cout << "Insertion Sort:\t";
  // for (int i = 0; i < 10; i++) {
  //   std::cout << array[i] << " ";
  // }
  // std::cout << std::endl;

  // // Merge Sort Test
  // GetArray(array, 10);
  // mergeSort(array, 0, 9);
  // std::cout << "Merge Sort:\t";
  // for (int i = 0; i < 10; i++) {
  //   std::cout << array[i] << " ";
  // }
  // std::cout << std::endl;

  // // Quick Sort Test
  // GetArray(array, 10);
  // mergeSort(array, 0, 9);
  // std::cout << "Quick Sort:\t";
  // for (int i = 0; i < 10; i++) {
  //   std::cout << array[i] << " ";
  // }
  // std::cout << std::endl;
  ////////////////////////////////////////////////////////
  // CREATING GLFW WINDOW SECTION
  if (glfwInit() != GLFW_TRUE) {
    LOG_ERROR("GLFW failed to initialize {}", 10);
    std::exit(-1);
  }

  // setting window hints
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE); -> needed for mac
  glfwWindowHint(GLFW_RESIZABLE, false);

  // glfwcreateWindow(width, height, title, monitor, share)
  GLFWwindow* window = glfwCreateWindow(1280, 720, "AlgoViz", nullptr, nullptr);
  glfwMakeContextCurrent(window);

  // Loading all function pointers for OpenGL
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    LOG_ERROR("Failed to initialize GLAD");
    std::exit(-1);
  }

  ////////////////////////////////////////////////////////
  // SHADER SECTION
  // Setting up Shader
  // --Vertex shader source
  const char* vertSource = "#version 460 core\n"
                           "layout (location = 0) in vec3 iPos;\n"
                           "layout (location = 1) in vec3 iMystery;\n"
                           "uniform mat4 model;\n"
                           "uniform mat4 projection;\n"
                           "\n"
                           "out vec3 oMystery;"
                           "void main() {\n"
                           "  gl_Position = projection * model * vec4(iPos, 1.0);\n"
                           "  oMystery = iMystery;\n"
                           "}\n";

  // --Fragment Shader source (outputs color)
  const char* fragSource = "#version 460 core\n"
                           "in vec3 oMystery;\n"
                           "out vec4 fragColor;\n"
                           "void main(){\n"
                           "  fragColor = vec4(oMystery, 1.0);\n"
                           "}\n";

  // Compiling the Shader
  uint32_t vertexShader, fragmentShader;
  int32_t success;
  char infoLog[512];

  // Creating the vertexShader
  vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertSource, nullptr);
  glCompileShader(vertexShader);

  // Getting vertexShader compile status and reporting
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
    LOG_ERROR("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n{}", infoLog);
    std::exit(-1);
  }

  // Creating the fragment shader
  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragSource, nullptr);
  glCompileShader(fragmentShader);

  // Getting fragementShader compile status and reporting
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
    LOG_ERROR("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n{}", infoLog);
    std::exit(-1);
  }

  // Making a Shader Program
  uint32_t program = glCreateProgram();
  glAttachShader(program, vertexShader);
  glAttachShader(program, fragmentShader);
  glLinkProgram(program);

  // Getting program link status and reporting
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(program, 512, nullptr, infoLog);
    LOG_ERROR("ERROR::SHADER::PROGRAM::LINKING_FAILED\n{}", infoLog);
    std::exit(-1);
  }

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
  ////////////////////////////////////////////////////////

  glViewport(0, 0, 1280, 720);

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  SortableRenderData array[100];
  GetArray(array, 100);
  selectionSort(array, 100);
  std::cout << "Selection Sort:\t";
  for (int i = 0; i < 100; i++) {
    std::cout << array[i] << " ";
  }
  std::cout << std::endl;

  glm::vec2 position(0.0f, 0.0f);
  float diff = 1280.0f / 100.0f;

  while (!glfwWindowShouldClose(window)) {
    // Get user input
    glfwPollEvents();

    // Render
    glClear(GL_COLOR_BUFFER_BIT);

    for (int i = 0; i < 100; i++) {
      SortableRenderData srd = array[i];
      glm::mat4 projection = glm::ortho(0.0f, 1280.0f, 720.0f, 0.0f);
      glm::mat4 model = glm::mat4(1);
      model = glm::translate(model, glm::vec3(position.x, position.y, 0.0f));
      model = glm::scale(model, glm::vec3(1280.0f / 100.0f, srd.data * 720.0f, 1.0f));

      glUseProgram(program);
      glUniformMatrix4fv(
        glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
      glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(model));
      glBindVertexArray(srd.vao);
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
      glBindVertexArray(0);
      position.x += diff;
    }
    position = glm::vec3(0.0f, 0.0f, 0.0f);

    // Put the render on the screen
    glfwSwapBuffers(window);
  }

  glfwTerminate();
}