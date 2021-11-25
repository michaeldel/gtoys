#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <cairo.h>
#include <cairo-xlib.h>
#include <pango/pangocairo.h>
#include <X11/Xlib.h>

#define WIDTH 128
#define HEIGHT 128

void die(const char * message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(EXIT_FAILURE);
}

unsigned long parsecolor(const char * string) {
    if (string[0] != '#') die("invalid color format"); /* TODO: stricter check */
    return strtol(&string[1], NULL, 16);
}

int main(int argc, char * argv[]) {
    if (argc != 2) die("argument missing"); 
    const unsigned long color = parsecolor(argv[1]);

    Display * display = XOpenDisplay(NULL);
    if (display == NULL) die("failed to open display");

    const int screen = DefaultScreen(display);
    const Window window = XCreateSimpleWindow(
        display,
        RootWindow(display, screen),
        0, 0, WIDTH, HEIGHT,
        0, BlackPixel(display, screen), color
    );
    XSelectInput(display, window, ExposureMask | KeyPressMask);
    XMapWindow(display, window);

    Atom WM_DELETE = XInternAtom(display, "WM_DELETE_WINDOW", false);
    XSetWMProtocols(display, window, &WM_DELETE, 1);

    cairo_surface_t * surface = cairo_xlib_surface_create(
        display, window, DefaultVisual(display, screen), WIDTH, HEIGHT
    );
    cairo_t * ctx = cairo_create(surface);

    PangoLayout * layout = pango_cairo_create_layout(ctx);
    PangoFontDescription * fontdesc = pango_font_description_from_string("monospace");

    pango_layout_set_text(layout, argv[1], -1);
    pango_layout_set_font_description(layout, fontdesc);
    pango_font_description_free(fontdesc);

    int layoutwidth, layoutheight;
    pango_layout_get_size(layout, &layoutwidth, &layoutheight);
    cairo_move_to(
        ctx,
        (double) layoutwidth / (2.0 * PANGO_SCALE),
        (double) layoutheight / (2.0 * PANGO_SCALE)
    );

    cairo_xlib_surface_set_size(surface, WIDTH, HEIGHT);

    bool running = true;

    while (running) {
        XEvent event;
        XNextEvent(display, &event);

        switch(event.type) {
        case Expose:
            pango_cairo_show_layout(ctx, layout);
            break;
        case ClientMessage:
            if ((Atom) event.xclient.data.l[0] == WM_DELETE) running = false;
            break;
        }
    }

    cairo_destroy(ctx);
    cairo_surface_destroy(surface);

    XDestroyWindow(display, window);
    XCloseDisplay(display);

    return EXIT_SUCCESS;
}
