#include <algorithm>
#include <chrono>
#include <future>
#include <iostream>
#include <thread> // Allows us the creates new threads

#include <Windows.h>
#include <processthreadsapi.h>

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

#include "Sorts.hpp"
#include "Utils.h"
#include "util/Logger.h"

#define AV_UNUSED(x) (void)(x)

using namespace std::chrono_literals;

//
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

  SortType sortTypeComboList[] = {
    SortType::SELECTION, SortType::BUBBLE, SortType::INSERTION, SortType::MERGE, SortType::QUICK
  };

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
                           "uniform mat4 model;\n"
                           "uniform mat4 projection;\n"
                           "uniform vec3 color;\n"
                           "\n"
                           "out vec3 oColor;"
                           "void main() {\n"
                           "  gl_Position = projection * model * vec4(iPos, 1.0);\n"
                           "  oColor = color;\n"
                           "}\n";

  // --Fragment Shader source (outputs color)
  const char* fragSource = "#version 460 core\n"
                           "in vec3 oColor;\n"
                           "out vec4 fragColor;\n"
                           "void main(){\n"
                           "  fragColor = vec4(oColor, 1.0);\n"
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

  // Creating random array (NUMBER OF ELEMENTS HERE)
  std::vector<SortableRenderData> array(dataSize);
  // SortableRenderData array[100];
  GetArray(array.data(), static_cast<int32_t>(array.size()));

  glm::vec2 position(0.0f, 0.0f);
  float diff = 1280.0f / array.size();

  static VisualizationRectangle rect;

  glm::vec3 defaultColor(1.0f, 1.0f, 1.0f);
  glm::vec3 selectedColor(1.0f, 0.0f, 0.0f);
  glm::vec3 highColor(0.709f, 0.341f, 0.925f);
  glm::vec3 lowColor(0.266f, 0.921f, 0.592f);

  //// IMGUI SECTION
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  AV_UNUSED(io);
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 460");

  std::thread launch_thread;
  std::promise<void> p;
  std::future<void> f;

  while (!glfwWindowShouldClose(window)) {
    // Get user input
    glfwPollEvents();

    // Render
    glClear(GL_COLOR_BUFFER_BIT);

    {
      std::lock_guard<std::mutex> lock(sortMutex);
      for (int i = 0; i < array.size(); i++) {
        SortableRenderData srd = array[i];
        glm::mat4 projection = glm::ortho(0.0f, 1280.0f, 720.0f, 0.0f);
        glm::mat4 model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(position.x, 720.0f - (srd.data * 720.0f), 0.0f));
        model = glm::scale(model, glm::vec3(1280.0f / array.size(), srd.data * 720.0f, 1.0f));

        glUseProgram(program);
        glUniformMatrix4fv(
          glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(model));

        if (srd.selected) {
          array[i].selected = false;
          glUniform3f(
            glGetUniformLocation(program, "color"), selectedColor.r, selectedColor.g, selectedColor.b);
        } else if (srd.high) {
          array[i].high = false;
          glUniform3f(glGetUniformLocation(program, "color"), highColor.r, highColor.g, highColor.b);
        } else if (srd.low) {
          array[i].high = false;
          glUniform3f(glGetUniformLocation(program, "color"), lowColor.r, lowColor.g, lowColor.b);
        } else {
          glUniform3f(glGetUniformLocation(program, "color"), defaultColor.r, defaultColor.g, defaultColor.b);
        }

        glBindVertexArray(rect.vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        position.x += diff;
      }
    }

    position = glm::vec3(0.0f, 0.0f, 0.0f);

    // Displaying the UI Window
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    bool clicked = false;
    bool resetClicked = false;
    static int item_current = 0;
    {
      ImGui::Begin("Test Window");
      ImGui::Text("This is some text");
      ImGui::DragInt("Updates Per Second", &numUpdatesPerSec, 1.0f, 1, 100);
      ImGui::DragInt("Number of Elements", &dataSize, 1.0f, 10, 10000);
      const char* items[] = { "Selection", "Bubble", "Insertion", "Merge", "Quick" };
      ImGui::Combo("Available Sorts", &item_current, items, IM_ARRAYSIZE(items));
      clicked = ImGui::Button("Start");
      ImGui::SameLine();
      resetClicked = ImGui::Button("Reset");
      ImGui::End();
    }
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (clicked || resetClicked) {
      // Signal sort thread to stop
      p.set_value();
      if (launch_thread.joinable()) {
        // Stop the thread
        launch_thread.join();
      }

      // Reading dataSize when clicked
      array = std::vector<SortableRenderData>(dataSize);
      diff = 1280.0f / array.size();

      // Regenerate the data to sort for next sort
      GetArray(array.data(), static_cast<int32_t>(array.size()));

      // Reset promise and future and launch thread for sorting again
      p = std::promise<void>();
      f = p.get_future();
      if (clicked) {
        launch_thread = std::thread(
          &Sort, array.data(), int32_t(array.size()), sortTypeComboList[item_current], std::ref(f));
      }
    }
    // Put the render on the screen
    glfwSwapBuffers(window);
  }

  // Terminate all for shutdown section
  p.set_value();
  launch_thread.join();

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwTerminate();
  f.get();
}
