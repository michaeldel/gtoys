#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "util/color.h"

static const float EPSILON = 0.000001f;

static const struct RGB BLACK = { .r = 0.0f, .g = 0.0f, .b = 0.0f };
static const struct RGB WHITE = { .r = 1.0f, .g = 1.0f, .b = 1.0f };

static const struct RGB RED   = { .r = 1.0f, .g = 0.0f, .b = 0.0f };
static const struct RGB GREEN = { .r = 0.0f, .g = 1.0f, .b = 0.0f };
static const struct RGB BLUE  = { .r = 0.0f, .g = 0.0f, .b = 1.0f };

static bool feq(float a, float b) {
    return fabs(a - b) < EPSILON;
}

static bool eqrgb(struct RGB a, struct RGB b) {
    return feq(a.r, b.r) && feq(a.g, b.g) && feq(a.b, b.b);
}

static bool eqhsv(struct HSV a, struct HSV b) {
    return feq(a.h, b.h) && feq(a.s, b.s) && feq(a.v, b.v);
}

static struct RGB randrgb(void) {
    struct RGB rgb = {
        .r = 1.0f * rand() / RAND_MAX,
        .g = 1.0f * rand() / RAND_MAX,
        .b = 1.0f * rand() / RAND_MAX
    };
    return rgb;
}

bool test_black(void) {
    struct HSV hsv = rgbtohsv(BLACK);
    if (!(hsv.v == 0)) return false;

    return true;
}

bool test_white(void) {
    struct HSV hsv = rgbtohsv(WHITE);
    struct HSV expected = { .h = 0.0f, .s = 0.0f, .v = 1.0f };

    if (!eqhsv(hsv, expected)) return false;

    return true;
}

bool test_red(void) {
    struct HSV hsv = rgbtohsv(RED);
    struct HSV expected = { .h = 0.0f, .s = 1.0f, .v = 1.0f };
    if (!eqhsv(hsv, expected)) return false;

    return true;
}

bool test_green(void) {
    struct HSV hsv = rgbtohsv(GREEN);
    struct HSV expected = { .h = 120.0f, .s = 1.0f, .v = 1.0f };
    if (!eqhsv(hsv, expected)) return false;

    return true;
}

bool test_blue(void) {
    struct HSV hsv = rgbtohsv(BLUE);
    struct HSV expected = { .h = 240.0f, .s = 1.0f, .v = 1.0f };
    if (!eqhsv(hsv, expected)) return false;

    return true;
}

bool test_convert_back_common(void) {
    const struct RGB rgbs[] = { BLACK, WHITE, RED, GREEN, BLUE };

    for (size_t i = 0; i < sizeof rgbs / sizeof rgbs[0]; i++) {
        if (!eqrgb(hsvtorgb(rgbtohsv(rgbs[i])), rgbs[i])) return false;
    }
    return true;
}

bool test_convert_back_random(void) {
    const unsigned int n = 1000;

    for (unsigned int i = 0; i < n; i++) {
        struct RGB before = randrgb();
        struct RGB after = hsvtorgb(rgbtohsv(before));

        if (!eqrgb(before, after)) return false;
    }

    return true;
}

bool test_zero_value_always_black(void) {
    const unsigned int n = 1000;

    for (unsigned int i = 0; i < n; i++) {
        struct HSV hsv = {
            .h = 1.0f * rand() / RAND_MAX,
            .s = 0.0f,
            .v = 0.0f
        };
        struct RGB rgb = hsvtorgb(hsv);
        if (!eqrgb(rgb, BLACK)) return false;
    }

    return true;
}

int main(void) {
    srand(time(NULL));

    bool (* testcases[])() = {
        test_black,
        test_white,
        test_red,
        test_green,
        test_blue,
        test_convert_back_common,
        test_convert_back_random,
        test_zero_value_always_black,
    };

    bool failed = false;

    for (size_t i = 0; i < sizeof testcases / sizeof testcases[0]; i++)
        if (testcases[i]()) putchar('.'); else { putchar('F'); failed = true; };
    putchar('\n');

    return failed ? EXIT_FAILURE : EXIT_SUCCESS;
}
