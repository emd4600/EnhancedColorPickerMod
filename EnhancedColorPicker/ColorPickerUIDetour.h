#pragma once

#include <Spore\BasicIncludes.h>

// We will detour the Load method so we can add our color picker button
member_detour(ColorPickerUI_Load__detour, Palettes::ColorPickerUI, bool(UTFWin::IWindow*, uint32_t, uint32_t, vector<Math::ColorRGB>*)) {};
