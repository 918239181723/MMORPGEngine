#pragma once

#include <string>
#include <fstream>
#include <mutex>
#include <memory>
#include <sstream>

// Engine/include/Engine/Core/Logger.h

namespace MMORPGEngine {
    namespace Core {

        enum class LogLevel {
            Debug = 0,
            Info = 1,
            Warning = 2,
            Error = 3,
            Critical = 4
        };

        class Logger {
        private:
            std::ofstream logFile;
            mutable std::mutex logMutex;
            LogLevel minLogLevel;
            bool consoleOutput;
            bool fileOutput;
            std::string logFileName;

        public:
            Logger();
            ~Logger();

            // Configuration
            bool Initialize(const std::string& filename = "engine.log");
            void SetMinLogLevel(LogLevel level) { minLogLevel = level; }
            void SetConsoleOutput(bool enable) { consoleOutput = enable; }
            void SetFileOutput(bool enable) { fileOutput = enable; }
            void Shutdown();

            // Logging methods
            void Log(LogLevel level, const std::string& message, const char* file = nullptr, int line = 0);
            void Debug(const std::string& message, const char* file = nullptr, int line = 0);
            void Info(const std::string& message, const char* file = nullptr, int line = 0);
            void Warning(const std::string& message, const char* file = nullptr, int line = 0);
            void Error(const std::string& message, const char* file = nullptr, int line = 0);
            void Critical(const std::string& message, const char* file = nullptr, int line = 0);

            // Template logging for formatted messages
            template<typename... Args>
            void LogFormat(LogLevel level, const std::string& format, Args&&... args) {
                std::ostringstream oss;
                LogFormatImpl(oss, format, std::forward<Args>(args)...);
                Log(level, oss.str());
            }

        private:
            std::string GetCurrentTime() const;
            std::string LogLevelToString(LogLevel level) const;
            void WriteToConsole(const std::string& message, LogLevel level) const;
            void WriteToFile(const std::string& message) const;

            // Template implementation for formatted logging
            template<typename T>
            void LogFormatImpl(std::ostringstream& oss, const std::string& format, T&& value) {
                size_t pos = format.find("{}");
                if (pos != std::string::npos) {
                    oss << format.substr(0, pos) << std::forward<T>(value) << format.substr(pos + 2);
                }
                else {
                    oss << format;
                }
            }

            template<typename T, typename... Args>
            void LogFormatImpl(std::ostringstream& oss, const std::string& format, T&& value, Args&&... args) {
                size_t pos = format.find("{}");
                if (pos != std::string::npos) {
                    std::string newFormat = format.substr(0, pos) + std::forward<T>(value) + format.substr(pos + 2);
                    LogFormatImpl(oss, newFormat, std::forward<Args>(args)...);
                }
                else {
                    oss << format;
                }
            }
        };

        // Convenience macros
#define LOG_DEBUG(msg) ENGINE().GetLogger()->Debug(msg, __FILE__, __LINE__)
#define LOG_INFO(msg) ENGINE().GetLogger()->Info(msg, __FILE__, __LINE__)
#define LOG_WARNING(msg) ENGINE().GetLogger()->Warning(msg, __FILE__, __LINE__)
#define LOG_ERROR(msg) ENGINE().GetLogger()->Error(msg, __FILE__, __LINE__)
#define LOG_CRITICAL(msg) ENGINE().GetLogger()->Critical(msg, __FILE__, __LINE__)

// Formatted logging macros
#define LOG_DEBUG_F(format, ...) ENGINE().GetLogger()->LogFormat(LogLevel::Debug, format, __VA_ARGS__)
#define LOG_INFO_F(format, ...) ENGINE().GetLogger()->LogFormat(LogLevel::Info, format, __VA_ARGS__)
#define LOG_WARNING_F(format, ...) ENGINE().GetLogger()->LogFormat(LogLevel::Warning, format, __VA_ARGS__)
#define LOG_ERROR_F(format, ...) ENGINE().GetLogger()->LogFormat(LogLevel::Error, format, __VA_ARGS__)
#define LOG_CRITICAL_F(format, ...) ENGINE().GetLogger()->LogFormat(LogLevel::Critical, format, __VA_ARGS__)

    } // namespace Core
} // namespace MMORPGEngine