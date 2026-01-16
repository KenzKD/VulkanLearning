#include "first_app.hpp"

// std
#include <cstdlib>
#include <iostream>

int main()
{
    lve::FirstApp app{};
    try
    {
        app.run();
        std::cout << "Vulkan window Open successfully!" << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
