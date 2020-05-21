#include <iostream>
#include <shinobu/Runner.hpp>

using namespace Shinobu;

int main(int argc, char* argv[]) {
    Runner runner = Runner();
    runner.configure(argc, argv);
    std::cout << "Hello, world!" << std::endl;
    return 0;
}
