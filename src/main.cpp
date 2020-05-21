#include <iostream>
#include <shinobu/Runner.hpp>
#include <shinobu/Emulator.hpp>

using namespace Shinobu;

int main(int argc, char* argv[]) {
    Runner runner = Runner();
    runner.configure(argc, argv);
    Emulator emulator = Emulator();
    runner.configureEmulator(emulator);
    return 0;
}
