#include "shinobu/frontend/Palette.hpp"

using namespace Shinobu::Frontend::Palette;

Selector::Selector(std::array<palette, 10>::size_type selectedPalette) : selectedPalette(selectedPalette) {

}

Selector::~Selector() {

}

palette Selector::currentSelection() const {
    return Palettes[selectedPalette];
}

void Selector::forwardSelector() {
    selectedPalette++;
    selectedPalette %= Palettes.size();
}

void Selector::backwardSelector() {
    if (selectedPalette == 0) {
        selectedPalette = Palettes.size() - 1;
    } else {
        selectedPalette--;
    }
}
