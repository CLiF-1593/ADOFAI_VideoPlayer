#pragma once

namespace Color {
	struct RGB_Color {
		int r;
		int g;
		int b;
	};

	struct HSV_Color {
		int h;
		int s;
		int v;
	};

	HSV_Color RGBtoHSV(RGB_Color in);
	RGB_Color HSVtoRGB(HSV_Color in);
}

