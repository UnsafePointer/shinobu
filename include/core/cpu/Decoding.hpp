#pragma once
#include <vector>
#include <cstdint>

namespace Core {
    namespace CPU {
        namespace Instructions {
            const std::vector<uint8_t> RPTable = { 0x1, 0x2, 0x3, 0x4 };
            const std::vector<uint8_t> RP2Table = { 0x1, 0x2, 0x3, 0x0 };
            const std::vector<uint8_t> RTable = { 0x3, 0x2, 0x5, 0x4, 0x7, 0x6, 0xFF, 0x1 };
            const std::vector<std::function<bool(Flag&)>> CCTable = {
                [](Flag& flags) {
                    return flags.zero == 0;
                },
                [](Flag& flags) {
                    return flags.zero == 1;
                },
                [](Flag& flags) {
                    return flags.carry == 0;
                },
                [](Flag& flags) {
                    return flags.carry == 1;
                },
            };
            namespace Disassembler {
                const std::vector<std::string> RPTable = { "BC", "DE", "HL", "SP" };
                const std::vector<std::string> RP2Table = { "BC", "DE", "HL", "AF" };
                const std::vector<std::string> RTable = { "B", "C", "D", "E", "H", "L", "(HL)", "A" };
                const std::vector<std::string> CCTable = {
                    "NZ",
                    "Z",
                    "NC",
                    "C",
                };
            };
        };
    };
};
