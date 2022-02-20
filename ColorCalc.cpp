#include "ColorCalc.h"
#include <iostream>
using namespace std;
using namespace Color;

#define RET_HSV(h, s, v) {(int)h, (int)(s * 100.0), (int)(v * 100.0)}
#define RET_RGB(r, g, b) {(int)(r * 255.0), (int)(g * 255.0), (int)(b * 255.0)}

HSV_Color Color::RGBtoHSV(RGB_Color in) {
	double h, s, v;
	double r, g, b;

	r = (double)in.r / 255.0;
	g = (double)in.g / 255.0;
	b = (double)in.b / 255.0;

	double min, max, delta;

	min = r < g ? r : g;
	min = min < b ? min : b;

	max = r > g ? r : g;
	max = max > b ? max : b;

	v = max;                                // v
	delta = max - min;
	if (max > 0.0) { // NOTE: if Max is == 0, this divide would cause a crash
		s = (delta / max);                  // s
	}
	else {
		// if max is 0, then r = g = b = 0
		// s = 0, v is undefined
		s = 0.0;
		h = 0.0;                            // its now undefined
		return RET_HSV(h, s, v);
	}
	if (r >= max)                           // > is bogus, just keeps compilor happy
		if (delta == 0) {
			h = 0.0;
		}
		else {
			h = (double)(g - b) / delta;        // between yellow & magenta
		}
	else
		if (g >= max)
			h = 2.0 + (double)(b - r) / delta;  // between cyan & yellow
		else
			h = 4.0 + (double)(r - g) / delta;  // between magenta & cyan

	h *= 60.0;                              // degrees

	if (h < 0.0)
		h += 360.0;

	return RET_HSV(h, s, v);
}

RGB_Color Color::HSVtoRGB(HSV_Color in) {
	double h, s, v;
	double r, g, b;

	h = (double)in.h;
	s = (double)in.s / 100.0;
	v = (double)in.v / 100.0;

	double      hh, p, q, t, ff;
	long        i;

	if (s <= 0.0) {       // < is bogus, just shuts up warnings
		r = v;
		g = v;
		b = v;
		return RET_RGB(r, g, b);
	}
	hh = h;
	if (hh >= 360.0) hh = 0.0;
	hh /= 60.0;
	i = (long)hh;
	ff = hh - i;
	p = (double)v * (1.0 - (double)s);
	q = (double)v * (1.0 - ((double)s * ff));
	t = (double)v * (1.0 - ((double)s * (1.0 - ff)));

	switch (i) {
	case 0:
		r = v;
		g = t;
		b = p;
		break;
	case 1:
		r = q;
		g = v;
		b = p;
		break;
	case 2:
		r = p;
		g = v;
		b = t;
		break;

	case 3:
		r = p;
		g = q;
		b = v;
		break;
	case 4:
		r = t;
		g = p;
		b = v;
		break;
	case 5:
	default:
		r = v;
		g = p;
		b = q;
		break;
	}
	return RET_RGB(r, g, b);
}
