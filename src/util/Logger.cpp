#include "Logger.h"

std::shared_ptr<spdlog::logger> Logger::s_Logger;

void Logger::Init()
{
  s_Logger = spdlog::stdout_color_mt("AlgoViz");
  s_Logger->set_pattern("[%T] [%n] %^(%l)%$: %v");
  s_Logger->set_level(spdlog::level::trace);
}
