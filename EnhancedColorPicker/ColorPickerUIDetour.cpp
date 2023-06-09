#include "stdafx.h"
#include "ColorPickerUIDetour.h"
#include "ColorWheelSwatchUI.h"

#include <Spore\Properties.h>

bool ColorPickerUI_Load_detour::DETOUR(
	UTFWin::IWindow* pWindow, uint32_t propID, uint32_t regionFilter, vector<ColorRGB>* pColors)
{
	// We have to recreate the original method
	// Usually you don't need to do so much, just call the original method and
	// do your thing
	// For this mod, however, we must ensure the original buttons are smaller so they can fit the new one

	mRegionFilter = regionFilter;
	mpWindow = pWindow;
	field_0C = nullptr;
	mpPropList = nullptr;

	// Try to get the configuration .prop file
	if (mpWindow && PropManager.GetPropertyList(
		propID, id("ColorPickerConfig"), mpPropList))
	{
		// Ensure the window colors are correct and set it visible
		mpWindow->SetShadeColor(Math::Color::WHITE);
		mpWindow->SetFillColor(Math::Color(255, 255, 255, 0));
		mpWindow->SetFlag(UTFWin::kWinFlagVisible, true);

		// Set the width and height members, which are taken from the window
		mWidth = mpWindow->GetRealArea().GetWidth();
		mHeight = mpWindow->GetRealArea().GetHeight();

		// These are either defined by the program (in the last parameter)
		// or taken from the config file
		Math::ColorRGB* usedColors;
		if (pColors) {
			mColorsCount = pColors->size();
			usedColors = pColors->data();
		}
		else {
			// A default value, if no colors are specified they are autogenerated
			mColorsCount = 10;

			// mColorsCount and usedColors are output parameters,
			// their value will be changed according the property
			// 0xD29675EC: colorpickerColors
			App::Property::GetArrayColorRGB(mpPropList.get(), 0xD29675EC,
				mColorsCount, usedColors);
		}

		bool colorpickerAddCustomColor = true;
		bool colorpickerAddDefaultColor = true;
		bool colorpickerAddEnhancedColorChooser = false;
		// Not all properties can be obtained with id(), be careful!
		App::Property::GetBool(mpPropList.get(), 0x55A847E, colorpickerAddCustomColor);
		App::Property::GetBool(mpPropList.get(), 0x55A847F, colorpickerAddDefaultColor);
		// A default property we have defined
		// As you can see, there's no such thing as "defining" properties: you just get them whenever you want
		App::Property::GetBool(mpPropList.get(), id("colorpickerAddEnhancedColorChooser"), colorpickerAddEnhancedColorChooser);

		// How many swatches are added in a loop
		int autoColorCount = mColorsCount;

		if (colorpickerAddCustomColor) {
			mCustomColorIndex = mColorsCount;
			++mColorsCount;
		}
		if (colorpickerAddDefaultColor) {
			mDefaultColorIndex = mColorsCount;
			++mColorsCount;
		}
		if (colorpickerAddEnhancedColorChooser) {
			++mColorsCount;
		}

		// First add the default colors
		for (int i = 0; i < autoColorCount; ++i)
		{
			ColorSwatchUIPtr swatch = new Palettes::ColorSwatchUI();
			// This is how Spore decides the color, I think?
			Math::ColorRGB color;
			if (usedColors) color = usedColors[i];
			else if (i == 0) color = Math::ColorRGB(1.0f, 1.0f, 1.0f);
			else color = HSVtoRGB(Math::ColorHSV(i / float(mColorsCount - 1) * 360.0f, 1.0f, 1.0f));

			swatch->mColorIndex = i;
			swatch->Load(mpPropList.get(), color, GetSwatchArea(i), mpWindow, (Object*)this);
			swatch->AddTooltip(0x5BF125F);

			mpColorUIs.push_back(swatch);
		}

		// Now special swatches used by Spore
		if (colorpickerAddCustomColor) {
			ColorSwatchUIPtr swatch = new Palettes::ColorSwatchUI();
			swatch->mIsCustomColor = true;
			swatch->mColorIndex = mCustomColorIndex;
			swatch->Load(mpPropList.get(), Math::ColorRGB(0.5f, 0.5f, 0.5f),
				GetSwatchArea(mCustomColorIndex), mpWindow, (Object*)this);
			swatch->AddTooltip(0x5BF125F);

			mpColorUIs.push_back(swatch);
		}
		if (colorpickerAddDefaultColor) {
			ColorSwatchUIPtr swatch = new Palettes::ColorSwatchUI();
			swatch->mIsDefaultColor = true;
			swatch->mColorIndex = mDefaultColorIndex;
			swatch->Load(mpPropList.get(), Math::ColorRGB(1.0f, 0.94f, 0.83f),
				GetSwatchArea(mDefaultColorIndex), mpWindow, (Object*)this);
			swatch->AddTooltip(0x5BF125F);

			mpColorUIs.push_back(swatch);
		}

		// Finally, our turn
		if (colorpickerAddEnhancedColorChooser) {
			intrusive_ptr<ColorWheelSwatchUI> swatch = new ColorWheelSwatchUI();
			swatch->mColorIndex = mColorsCount - 1;
			swatch->Load(mpPropList.get(), GetSwatchArea(swatch->mColorIndex), mpWindow, (Object*)this);
			swatch->AddTooltip(0x5BF125F);

			mpColorUIs.push_back(swatch);
		}
	}

	// We must return whether it loaded correctly (so always true?)
	return true;
}