#include <iostream>

#include <imgui.h>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "util/Logger.h"

#define AV_UNUSED(x) (void)(x)

int main(int argc, char** argv)
{
  AV_UNUSED(argc);
  AV_UNUSED(argv);

  Logger::Init();
  Logger::SetLogLevel(Logger::LogLevel::INFO);

  if (glfwInit() != GLFW_TRUE) {
    LOG_ERROR("GLFW failed to initialize");
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

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    LOG_ERROR("Failed to initialize GLAD");
    std::exit(-1);
  }

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  AV_UNUSED(io);

  ImGui::StyleColorsDark();

  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 460");

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  bool show = true;
  while (!glfwWindowShouldClose(window)) {
    // Get user input
    glfwPollEvents();

    // Render
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::ShowDemoWindow(&show);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Put the render on the screen
    glfwSwapBuffers(window);
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwTerminate();
}