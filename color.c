#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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

struct winstate {
    Display * display;
    int screen;
    Window  window;
    Atom WM_DELETE;
};

struct winstate initwinstate(unsigned long color) {
    Display * display = XOpenDisplay(NULL);
    if (display == NULL) die("failed to open display");

    const int screen = DefaultScreen(display);
    const Window window = XCreateSimpleWindow(
        display,
        RootWindow(display, screen),
        0, 0, WIDTH, HEIGHT,
        0, BlackPixel(display, screen), color
    );
    XSelectInput(display, window, StructureNotifyMask | ExposureMask | KeyPressMask);
    XMapWindow(display, window);

    Atom WM_DELETE = XInternAtom(display, "WM_DELETE_WINDOW", false);
    XSetWMProtocols(display, window, &WM_DELETE, 1);

    struct winstate ws = {
        .display = display,
        .screen = screen,
        .window = window,
        .WM_DELETE = WM_DELETE,
    };
    return ws;
}

void destroywinstate(struct winstate ws) {
    XDestroyWindow(ws.display, ws.window);
    XCloseDisplay(ws.display);
}

struct drawstate {
    cairo_surface_t * surface;
    PangoFontDescription * fontdesc;
    const char * text;
};

struct drawstate initdrawstate(struct winstate ws, const char * text) {
    struct drawstate ds = {
        .surface = cairo_xlib_surface_create(
            ws.display, ws.window, DefaultVisual(ws.display, ws.screen), WIDTH, HEIGHT
        ),
       .fontdesc = pango_font_description_from_string("monospace"),
       .text = text
    };
    return ds;
}

void destroydrawstate(struct drawstate ds) {
    pango_font_description_free(ds.fontdesc);
    cairo_surface_destroy(ds.surface);
}

void draw(struct drawstate ds) {
    cairo_t * ctx = cairo_create(ds.surface);

    /* TODO: should layout be moved into drawstate ? */
    PangoLayout * layout = pango_cairo_create_layout(ctx);
    pango_layout_set_font_description(layout, ds.fontdesc);
    pango_layout_set_text(layout, ds.text, -1);

    const int surfacewidth = cairo_xlib_surface_get_width(ds.surface);
    const int surfaceheight = cairo_xlib_surface_get_height(ds.surface);

    int layoutwidth, layoutheight;
    pango_layout_get_size(layout, &layoutwidth, &layoutheight);
    cairo_move_to(
        ctx,
        (surfacewidth - (double) layoutwidth / PANGO_SCALE) / 2.0,
        (surfaceheight - (double) layoutheight / PANGO_SCALE) / 2.0
    );
    pango_cairo_show_layout(ctx, layout);

    cairo_move_to(ctx, 50, 25);
    cairo_arc(ctx, 25, 25, 25, 0, 2 * M_PI);
    cairo_stroke(ctx);

    cairo_destroy(ctx);
}

void resize(struct drawstate ds, int width, int height) {
    cairo_xlib_surface_set_size(ds.surface, width, height);
}

void mainloop(struct winstate ws, struct drawstate ds) {
    for (;;) {
        XEvent event;
        XNextEvent(ws.display, &event);

        switch(event.type) {
        case Expose: draw(ds); break;
        case ConfigureNotify:
            resize(ds, event.xconfigure.width, event.xconfigure.height);
            break;
        case ClientMessage:
            if ((Atom) event.xclient.data.l[0] == ws.WM_DELETE) return;
            break;
        }
    }
}

int main(int argc, char * argv[]) {
    if (argc != 2) die("argument missing"); 
    const unsigned long color = parsecolor(argv[1]);

    struct winstate ws = initwinstate(color);
    struct drawstate ds = initdrawstate(ws, argv[1]);
    mainloop(ws, ds);

    destroydrawstate(ds);
    destroywinstate(ws);

    return EXIT_SUCCESS;
}
