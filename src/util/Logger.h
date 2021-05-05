#ifndef LOGGER_H
#define LOGGER_H

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

class Logger
{
public:
  enum LogLevel
  {
    TRACE = spdlog::level::trace,
    INFO = spdlog::level::info,
    WARN = spdlog::level::warn,
    ERR = spdlog::level::err,
    CRITICAL = spdlog::level::critical
  };

public:
  static void Init();

  template<typename... Args>
  static void LogTrace(Args&&... args)
  {
    s_Logger->trace(std::forward<Args>(args)...);
  }

  template<typename... Args>
  static void LogInfo(Args&&... args)
  {
    s_Logger->info(std::forward<Args>(args)...);
  }

  template<typename... Args>
  static void LogWarning(Args&&... args)
  {
    s_Logger->warn(std::forward<Args>(args)...);
  }

  template<typename... Args>
  static void LogError(Args&&... args)
  {
    s_Logger->error(std::forward<Args>(args)...);
  }

  template<typename... Args>
  static void LogCritical(Args&&... args)
  {
    s_Logger->critical(std::forward<Args>(args)...);
  }

  static void SetLogLevel(LogLevel level)
  {
    s_Logger->set_level(static_cast<spdlog::level::level_enum>(level));
  }

private:
  static std::shared_ptr<spdlog::logger> s_Logger;
};

#if !defined(LOGL_STRIP_LOGGING)
#define LOG_TRACE(...) Logger::LogTrace(__VA_ARGS__)
#define LOG_INFO(...) Logger::LogInfo(__VA_ARGS__)
#define LOG_WARN(...) Logger::LogWarning(__VA_ARGS__)
#define LOG_ERROR(...) Logger::LogError(__VA_ARGS__)
#define LOG_CRITICAL(...) Logger::LogCritical(__VA_ARGS__)
#else
#define LOG_TRACE(...)
#define LOG_INFO(...)
#define LOG_WARN(...)
#define LOG_ERROR(...)
#define LOG_CRITICAL(...)
#endif

#endif