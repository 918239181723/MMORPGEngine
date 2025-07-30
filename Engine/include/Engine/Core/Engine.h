#pragma once

#include <memory>
#include <string>
#include <vector>
#include <chrono>

// Engine/include/Engine/Core/Engine.h

namespace MMORPGEngine {
    namespace Core {

        // Forward declarations
        class Application;
        class Logger;
        class Timer;

        // Engine subsystem interface
        class ISubsystem {
        public:
            virtual ~ISubsystem() = default;
            virtual bool Initialize() = 0;
            virtual void Update(float deltaTime) = 0;
            virtual void Shutdown() = 0;
            virtual const char* GetName() const = 0;
        };

        // Main Engine class - Singleton pattern
        class Engine {
        private:
            static std::unique_ptr<Engine> instance;

            // Core components
            std::unique_ptr<Logger> logger;
            std::unique_ptr<Timer> timer;
            std::vector<std::unique_ptr<ISubsystem>> subsystems;

            // Engine state
            bool isRunning;
            bool isInitialized;
            float targetFPS;

            // Private constructor for singleton
            Engine();

        public:
            // Singleton access
            static Engine& GetInstance();
            static void CreateInstance();
            static void DestroyInstance();

            // Core lifecycle
            bool Initialize();
            void Run();
            void Shutdown();
            void Stop();

            // Subsystem management
            template<typename T, typename... Args>
            T* AddSubsystem(Args&&... args) {
                static_assert(std::is_base_of_v<ISubsystem, T>, "T must inherit from ISubsystem");

                auto subsystem = std::make_unique<T>(std::forward<Args>(args)...);
                T* ptr = subsystem.get();
                subsystems.push_back(std::move(subsystem));
                return ptr;
            }

            template<typename T>
            T* GetSubsystem() {
                static_assert(std::is_base_of_v<ISubsystem, T>, "T must inherit from ISubsystem");

                for (auto& subsystem : subsystems) {
                    if (auto* casted = dynamic_cast<T*>(subsystem.get())) {
                        return casted;
                    }
                }
                return nullptr;
            }

            // Getters
            Logger* GetLogger() const { return logger.get(); }
            Timer* GetTimer() const { return timer.get(); }
            bool IsRunning() const { return isRunning; }
            bool IsInitialized() const { return isInitialized; }
            float GetTargetFPS() const { return targetFPS; }

            // Setters
            void SetTargetFPS(float fps) { targetFPS = fps; }

            // Utility
            double GetTime() const;
            float GetDeltaTime() const;

        private:
            void UpdateSubsystems(float deltaTime);
            void CalculateFrameRate();

            // Non-copyable
            Engine(const Engine&) = delete;
            Engine& operator=(const Engine&) = delete;
        };

        // Convenience macro
#define ENGINE() MMORPGEngine::Core::Engine::GetInstance()

    } // namespace Core
} // namespace MMORPGEngine