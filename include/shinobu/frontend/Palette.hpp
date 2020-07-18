#pragma once
#include "shinobu/frontend/opengl/Vertex.hpp"
#include <array>

namespace Shinobu {
    namespace Frontend {
        namespace Palette {

            typedef std::array<Shinobu::Frontend::OpenGL::Color, 4> palette;

            const palette GrayScale = {
                {
                    { 255.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f },
                    { 170.0f / 255.0f, 170.0f / 255.0f, 170.0f / 255.0f },
                    {  85.0f / 255.0f,  85.0f / 255.0f,  85.0f / 255.0f },
                    {   0.0f / 255.0f,   0.0f / 255.0f,   0.0f / 255.0f },
                }
            };

            // https://lospec.com/palette-list/ice-cream-gb
            const palette IceCreamGB = {
                {
                    { 255.0f / 255.0f, 246.0f / 255.0f, 211.0f / 255.0f },
                    { 249.0f / 255.0f, 168.0f / 255.0f, 117.0f / 255.0f },
                    { 235.0f / 255.0f, 107.0f / 255.0f, 111.0f / 255.0f },
                    { 124.0f / 255.0f,  63.0f / 255.0f,  88.0f / 255.0f },
                }
            };

            // https://lospec.com/palette-list/kirokaze-gameboy
            const palette KirokaseGameBoyPalette = {
                {
                    { 226.0f / 255.0f, 243.0f / 255.0f, 228.0f / 255.0f },
                    { 148.0f / 255.0f, 227.0f / 255.0f,  68.0f / 255.0f },
                    {  70.0f / 255.0f, 135.0f / 255.0f, 143.0f / 255.0f },
                    {  51.0f / 255.0f,  44.0f / 255.0f,  80.0f / 255.0f },
                }
            };

            // https://lospec.com/palette-list/rustic-gb
            const palette RusticGB = {
                {
                    { 168.0f / 255.0f, 104.0f / 255.0f, 104.0f / 255.0f },
                    { 237.0f / 255.0f, 180.0f / 255.0f, 161.0f / 255.0f },
                    { 118.0f / 255.0f,  68.0f / 255.0f,  98.0f / 255.0f },
                    {  44.0f / 255.0f,  33.0f / 255.0f,  55.0f / 255.0f },
                }
            };

            // https://lospec.com/palette-list/mist-gb
            const palette MistGBPalette = {
                {
                    { 196.0f / 255.0f, 240.0f / 255.0f, 194.0f / 255.0f },
                    {  90.0f / 255.0f, 185.0f / 255.0f, 168.0f / 255.0f },
                    {  30.0f / 255.0f,  96.0f / 255.0f, 110.0f / 255.0f },
                    {  45.0f / 255.0f,  27.0f / 255.0f,   0.0f / 255.0f },
                }
            };

            // https://lospec.com/palette-list/ayy4
            const palette AYY4 = {
                {
                    { 241.0f / 255.0f, 242.0f / 255.0f, 218.0f / 255.0f },
                    { 255.0f / 255.0f, 206.0f / 255.0f, 150.0f / 255.0f },
                    { 255.0f / 255.0f, 119.0f / 255.0f, 119.0f / 255.0f },
                    {   0.0f / 255.0f,  48.0f / 255.0f,  59.0f / 255.0f },
                }
            };

            // https://lospec.com/palette-list/spacehaze
            const palette SpaceHazePalette = {
                {
                    { 248.0f / 255.0f, 227.0f / 255.0f, 196.0f / 255.0f },
                    { 204.0f / 255.0f,  52.0f / 255.0f, 149.0f / 255.0f },
                    { 107.0f / 255.0f,  31.0f / 255.0f, 177.0f / 255.0f },
                    {  11.0f / 255.0f,   6.0f / 255.0f,  48.0f / 255.0f },
                }
            };

            // https://lospec.com/palette-list/crimson
            const palette Crimson = {
                {
                    { 239.0f / 255.0f, 249.0f / 255.0f, 214.0f / 255.0f },
                    { 186.0f / 255.0f,  80.0f / 255.0f,  68.0f / 255.0f },
                    { 122.0f / 255.0f,  28.0f / 255.0f,  75.0f / 255.0f },
                    {  27.0f / 255.0f,   3.0f / 255.0f,  38.0f / 255.0f },
                }
            };

            // https://lospec.com/palette-list/wish-gb
            const palette WishGB = {
                {
                    { 139.0f / 255.0f, 229.0f / 255.0f, 255.0f / 255.0f },
                    {  96.0f / 255.0f, 143.0f / 255.0f, 207.0f / 255.0f },
                    { 117.0f / 255.0f,  80.0f / 255.0f, 232.0f / 255.0f },
                    {  98.0f / 255.0f,  46.0f / 255.0f,  76.0f / 255.0f },
                }
            };

            // https://lospec.com/palette-list/nostalgia
            const palette Nostalgia = {
                {
                    { 208.0f / 255.0f, 208.0f / 255.0f,  88.0f / 255.0f },
                    { 160.0f / 255.0f, 168.0f / 255.0f,  64.0f / 255.0f },
                    { 112.0f / 255.0f, 128.0f / 255.0f,  40.0f / 255.0f },
                    {  64.0f / 255.0f,  80.0f / 255.0f,  16.0f / 255.0f },
                }
            };

            const std::array<palette, 10> Palettes = {
                GrayScale,
                IceCreamGB,
                KirokaseGameBoyPalette,
                RusticGB,
                MistGBPalette,
                AYY4,
                SpaceHazePalette,
                Crimson,
                WishGB,
                Nostalgia
            };

            class Selector {
                std::array<palette, 10>::size_type selectedPalette;
            public:
                Selector(std::array<palette, 10>::size_type selectedPalette);
                ~Selector();

                palette currentSelection() const;
                void forwardSelector();
                void backwardSelector();
            };

        };
    };
};
