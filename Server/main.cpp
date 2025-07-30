// Server/main.cpp
#include <iostream>
#include "../Engine/include/Engine/Core/Engine.h"

int main() {
    std::cout << "MMORPG Server Starting..." << std::endl;

    // Engine initialize
    MMORPGEngine::Core::Engine::CreateInstance();
    auto& engine = MMORPGEngine::Core::Engine::GetInstance();

    if (engine.Initialize()) {
        std::cout << "Engine initialized successfully!" << std::endl;
        engine.Run();
    }
    else {
        std::cout << "Failed to initialize engine!" << std::endl;
        return -1;
    }

    return 0;
}