#include <algorithm>
#include <chrono>
#include <future>
#include <iostream>
#include <random>
#include <thread> // Allows us the creates new threads

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

// Shuffles a random array of numbers from 0 to 1 using mersenne twister engine generator
void GetArray(std::vector<SortableRenderData>& array)
{
  array[0].data = 0.0f;
  array[array.size() - 1].data = 1.0f;

  // Difference
  float diff = 1.0f / static_cast<float>(array.size());
  float currentValue = diff;
  for (size_t i = 1; i < array.size() - 1; i++) {
    array[i].data = currentValue;
    currentValue += diff;
  }

  std::random_device rd;
  std::mt19937 g(rd());
  std::shuffle(array.begin(), array.end(), g);
}

int main(int argc, char** argv)
{
  AV_UNUSED(argc);
  AV_UNUSED(argv);

  uint32_t width = 1280;
  uint32_t height = 720;

  srand(static_cast<uint32_t>(time(NULL)));

  Logger::Init();
  Logger::SetLogLevel(Logger::LogLevel::INFO);

  SortType sortTypeComboList[] = {
    SortType::SELECTION, SortType::BUBBLE, SortType::INSERTION, SortType::MERGE, SortType::QUICK
  };

  ////////////////////////////////////////////////////////
  // CREATING GLFW WINDOW SECTION
  if (glfwInit() != GLFW_TRUE) {
    LOG_ERROR("GLFW failed to initialize");
    std::exit(-1);
  }

  // setting window hints
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE); -> needed for mac
  glfwWindowHint(GLFW_RESIZABLE, false);

  // glfwcreateWindow(width, height, title, monitor, share)
  GLFWwindow* window = glfwCreateWindow(width, height, "AlgoViz", nullptr, nullptr);
  glfwMakeContextCurrent(window);
  // Disable vsync
  glfwSwapInterval(0);

  // Loading all function pointers for OpenGL
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    LOG_ERROR("Failed to initialize GLAD");
    std::exit(-1);
  }

  ////////////////////////////////////////////////////////
  // SHADER SECTION
  // Setting up Shader
  // --Vertex shader source
  const char* vertSource = "#version 410 core\n"
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
  const char* fragSource = "#version 410 core\n"
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

  glViewport(0, 0, width, height);

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  // Creating random array (NUMBER OF ELEMENTS HERE)
  int32_t dataSize = 100;
  std::vector<SortableRenderData> array(dataSize);
  GetArray(array);

  // # # # # # # # # # #

  glm::vec2 position(0.0f, 0.0f);
  float paddingInPixels = 2.0f;
  float totalPaddingInPixels = paddingInPixels * (array.size() - 1);
  float diff = static_cast<float>(width) / array.size();

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
  ImGui_ImplOpenGL3_Init("#version 410");

  std::promise<void> p;
  std::shared_future<void> promise_f = p.get_future();
  std::future<void> async_future;

  float deltaTime = 0.0f;
  float lastTime = 0.0f;

  while (!glfwWindowShouldClose(window)) {
    // Calculating time to render one frame
    float currentTime = static_cast<float>(glfwGetTime());
    deltaTime = currentTime - lastTime;
    lastTime = currentTime;

    bool startClicked = false;
    bool resetClicked = false;
    static int item_current = 0;

    // Get user input
    glfwPollEvents();

    // Render
    glClear(GL_COLOR_BUFFER_BIT);

    {
      std::lock_guard<std::mutex> sortLock(sortMutex);
      for (unsigned int i = 0; i < array.size(); i++) {
        SortableRenderData srd = array[i];
        glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f);
        glm::mat4 model = glm::mat4(1);
        model = glm::translate(
          model,
          glm::vec3(position.x, static_cast<float>(height) - (srd.data * static_cast<float>(height)), 0.0f));

        // Intentionally oversizing the last one, you can't see it though :)
        float xScale = i != array.size() - 1
                         ? (static_cast<float>(width) - totalPaddingInPixels) / array.size()
                         : (static_cast<float>(width) / array.size());
        model = glm::scale(model, glm::vec3(xScale, srd.data * static_cast<float>(height), 1.0f));

        glUseProgram(program);
        glUniformMatrix4fv(
          glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(model));

        if (srd.selected) {

          glUniform3f(
            glGetUniformLocation(program, "color"), selectedColor.r, selectedColor.g, selectedColor.b);
        } else if (srd.high) {

          glUniform3f(glGetUniformLocation(program, "color"), highColor.r, highColor.g, highColor.b);
        } else if (srd.low) {

          glUniform3f(glGetUniformLocation(program, "color"), lowColor.r, lowColor.g, lowColor.b);
        } else {
          glUniform3f(glGetUniformLocation(program, "color"), defaultColor.r, defaultColor.g, defaultColor.b);
        }

        glBindVertexArray(rect.vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        position.x += diff;
      }

      position = glm::vec3(0.0f, 0.0f, 0.0f);

      // Displaying the UI Window
      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();
      {
        ImGui::Begin("AlgoViz");
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", deltaTime, ImGui::GetIO().Framerate);
        {
          std::lock_guard<std::mutex> freqLock(updateFreqMutex);
          ImGui::DragInt("Updates Per Second", &numUpdatesPerSec, 1.0f, 1, 100);
        }
        const char* items[] = { "Selection", "Bubble", "Insertion", "Merge", "Quick" };
        ImGui::Combo("Available Sorts", &item_current, items, IM_ARRAYSIZE(items));
        startClicked = ImGui::Button("Start");
        ImGui::SameLine();
        resetClicked = ImGui::Button("Reset");
        ImGui::End();
      }
      ImGui::Render();
      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    if (startClicked || resetClicked) {
      // Signal sort thread to stop
      p.set_value();

      // Wait...
      bool was_running = false;
      if (async_future.valid()) {
        was_running = true;
        async_future.wait();
        async_future.get();
      }

      // Reset data in the right situation
      if (resetClicked || (startClicked && was_running) || (startClicked && array.size() != dataSize)) {
        array = std::vector<SortableRenderData>(dataSize);
        diff = static_cast<float>(width) / array.size();

        // Regenerate the data to sort for next sort
        GetArray(array);
      }

      // Reset promise and future and launch thread for sorting again
      p = std::promise<void>();
      promise_f = p.get_future();
      if (startClicked) {
        async_future = std::async(std::launch::async, [&array, sortTypeComboList, &promise_f]() {
          Sort(array, sortTypeComboList[item_current], promise_f);
        });
      }
    }
    // Put the render on the screen
    glfwSwapBuffers(window);
  }

  // Terminate all for shutdown section
  p.set_value();
  if (async_future.valid()) {
    async_future.get();
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwTerminate();
}
