#include <iostream>

#include "util/Logger.h"
#include <glm/glm.hpp>

#define AV_UNUSED(x) (void)(x)

int main(int argc, char** argv)
{
  AV_UNUSED(argc);
  AV_UNUSED(argv);
  glm::vec3 v1(1.0f, 0.2f, 0.7f);

  Logger::Init();
  Logger::SetLogLevel(Logger::LogLevel::INFO);
  LOG_INFO("V1 = {}, {}, {}", v1.x, v1.y, v1.z);
}