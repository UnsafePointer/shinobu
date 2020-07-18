#include "shinobu/frontend/Palette.hpp"

using namespace Shinobu::Frontend::Palette;

Selector::Selector() : selectedPalette() {

}

Selector::~Selector() {

}

palette Selector::currentSelection() {
    return Palettes[selectedPalette];
}
