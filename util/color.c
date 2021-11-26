#include <assert.h>
#include <math.h>
#include <stdlib.h>

#include "util/color.h"
#define NaN (0.0f / 0.0f)

static const float angle = 60.0f;

static float max(float a, float b, float c) {
    return a > b ?
        (a > c ? a : c) :
        (b > c ? b : c);
}

static float min(float a, float b, float c) {
    return a < b ?
        (a < c ? a : c) :
        (b < c ? b : c);
}

static float rgbmax(struct RGB rgb) {
    return max(rgb.r, rgb.g, rgb.b);
}

static float rgbmin(struct RGB rgb) {
    return min(rgb.r, rgb.g, rgb.b);
}

static float hue(float value, float chroma, float r, float g, float b) {
    if (chroma == 0.0f) return 0.0f;

    if (g == value) return angle * (2 + (b - r) / chroma);
    if (b == value) return angle * (4 + (r - g) / chroma);

    assert(r == value);
    return angle * (g - b) / chroma;
}

struct HSV rgbtohsv(struct RGB rgb) {
    const float value = rgbmax(rgb);
    const float chroma = value - rgbmin(rgb);
    const float h = hue(value, chroma, rgb.r, rgb.g, rgb.b);
    const float saturation = value == 0.0f ? 0.0f : chroma / value;

    struct HSV hsv = {
        .h = h,
        .s = saturation,
        .v = value
    };
    return hsv;
}

/* see https://en.wikipedia.org/wiki/HSL_and_HSV#HSV_to_RGB_alternative */
static float f(float n, struct HSV hsv) {
    const float k = fmodf(n + hsv.h / angle, 6.0);
    return hsv.v - hsv.v * hsv.s * max(0.0f, 0.0f, min(k, 4.0f - k, 1.0f));
}

struct RGB hsvtorgb(struct HSV hsv) {
    struct RGB rgb = {
        .r = f(5.0f, hsv),
        .g = f(3.0f, hsv), 
        .b = f(1.0f, hsv)
    };
    return rgb;
}

