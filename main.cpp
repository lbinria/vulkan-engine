#include "HexEngine.h"
#include "HexApp.h"
#include <iostream>
#include <stdexcept>
#include <cstdlib>

int main() {

    hex::HexApp app{};
    try {
        app.run();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}