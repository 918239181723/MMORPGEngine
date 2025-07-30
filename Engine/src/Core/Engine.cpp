// Engine/src/Core/Engine.cpp

#include <iostream>
#include <thread>
#include <chrono>
#include "../../include/Engine/Core/Engine.h"
#include "../../include/Engine/Core/Logger.h"


namespace MMORPGEngine {
    namespace Core {

        // Static instance
        std::unique_ptr<Engine> Engine::instance = nullptr;

        Engine::Engine()
            : isRunning(false)
            , isInitialized(false)
            , targetFPS(60.0f)
        {
        }

        Engine& Engine::GetInstance() {
            if (!instance) {
                CreateInstance();
            }
            return *instance;
        }

        void Engine::CreateInstance() {
            if (!instance) {
                instance = std::unique_ptr<Engine>(new Engine());
            }
        }

        void Engine::DestroyInstance() {
            if (instance) {
                instance->Shutdown();
                instance.reset();
            }
        }

        bool Engine::Initialize() {
            if (isInitialized) {
                return true;
            }

            std::cout << "Initializing MMORPG Engine..." << std::endl;

            // Initialize Logger
            logger = std::make_unique<Logger>();
            if (!logger->Initialize("engine.log")) {
                std::cerr << "Failed to initialize logger!" << std::endl;
                return false;
            }

            logger->Info("Engine initialization started");

            // Initialize Timer (simple implementation for now)
            // timer = std::make_unique<Timer>();

            // Initialize all subsystems
            for (auto& subsystem : subsystems) {
                if (!subsystem->Initialize()) {
                    logger->Error("Failed to initialize subsystem: " + std::string(subsystem->GetName()));
                    return false;
                }
                logger->Info("Initialized subsystem: " + std::string(subsystem->GetName()));
            }

            isInitialized = true;
            logger->Info("Engine initialization completed successfully");

            return true;
        }

        void Engine::Run() {
            if (!isInitialized) {
                std::cerr << "Engine not initialized! Call Initialize() first." << std::endl;
                return;
            }

            isRunning = true;
            logger->Info("Engine started running");

            // Calculate frame time
            const float frameTime = 1.0f / targetFPS;

            auto lastTime = std::chrono::high_resolution_clock::now();

            while (isRunning) {
                auto currentTime = std::chrono::high_resolution_clock::now();
                auto deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
                lastTime = currentTime;

                // Update all subsystems
                UpdateSubsystems(deltaTime);

                // Frame rate limiting
                if (deltaTime < frameTime) {
                    auto sleepTime = std::chrono::duration<float>(frameTime - deltaTime);
                    std::this_thread::sleep_for(sleepTime);
                }
            }

            logger->Info("Engine stopped running");
        }

        void Engine::Shutdown() {
            if (!isInitialized) {
                return;
            }

            logger->Info("Shutting down engine...");

            isRunning = false;

            // Shutdown all subsystems in reverse order
            for (auto it = subsystems.rbegin(); it != subsystems.rend(); ++it) {
                (*it)->Shutdown();
                logger->Info("Shutdown subsystem: " + std::string((*it)->GetName()));
            }

            subsystems.clear();

            if (logger) {
                logger->Info("Engine shutdown completed");
                logger->Shutdown();
                logger.reset();
            }

            isInitialized = false;
        }

        void Engine::Stop() {
            isRunning = false;
        }

        double Engine::GetTime() const {
            auto now = std::chrono::high_resolution_clock::now();
            auto duration = now.time_since_epoch();
            return std::chrono::duration<double>(duration).count();
        }

        float Engine::GetDeltaTime() const {
            // For now, return fixed delta time
            return 1.0f / targetFPS;
        }

        void Engine::UpdateSubsystems(float deltaTime) {
            for (auto& subsystem : subsystems) {
                subsystem->Update(deltaTime);
            }
        }

        void Engine::CalculateFrameRate() {
            // Frame rate calculation implementation
            // TODO: Implement actual frame rate calculation
        }

    } // namespace Core
} // namespace MMORPGEngine