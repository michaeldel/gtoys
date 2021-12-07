#include <math.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>

#include <cairo.h>
#include <cairo-xlib.h>
#include <pango/pangocairo.h>
#include <X11/Xlib.h>

#define FPS 30

#define WIDTH 128
#define HEIGHT 128

#define MAX_BUBBLES 256

struct bubble {
    float x;
    float y;
};

static struct bubble bubbles[MAX_BUBBLES];
static size_t nbubbles = 0;

void die(const char * message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(EXIT_FAILURE);
}

struct winstate {
    Display * display;
    int screen;
    Window  window;
    Atom WM_DELETE;
};

struct winstate initwinstate(void) {
    Display * display = XOpenDisplay(NULL);
    if (display == NULL) die("failed to open display");

    const int screen = DefaultScreen(display);
    const Window window = XCreateSimpleWindow(
        display,
        RootWindow(display, screen),
        0, 0, WIDTH, HEIGHT,
        0, BlackPixel(display, screen), BlackPixel(display, screen)
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

void emitbubble(void) {
    bubbles[nbubbles].x = 1.0f * rand() / RAND_MAX;
    bubbles[nbubbles].y = 1.0f * rand() / RAND_MAX;
    nbubbles++;
}

struct drawstate {
    cairo_surface_t * surface;
    PangoFontDescription * fontdesc;
};

struct drawstate initdrawstate(struct winstate ws) {
    struct drawstate ds = {
        .surface = cairo_xlib_surface_create(
            ws.display, ws.window, DefaultVisual(ws.display, ws.screen), WIDTH, HEIGHT
        ),
       .fontdesc = pango_font_description_from_string("monospace"),
    };
    return ds;
}

void destroydrawstate(struct drawstate ds) {
    pango_font_description_free(ds.fontdesc);
    cairo_surface_destroy(ds.surface);
}

void draw(struct drawstate ds) {
    cairo_t * ctx = cairo_create(ds.surface);
    cairo_paint(ctx);
    cairo_set_source_rgb(ctx, 223.0 / 255, 249.0 / 255, 251.0 / 255);

    const int surfacewidth = cairo_xlib_surface_get_width(ds.surface);
    const int surfaceheight = cairo_xlib_surface_get_height(ds.surface);

    for (size_t i = 0; i < nbubbles; i++) {
        const struct bubble b = bubbles[i];
        cairo_arc(ctx, surfacewidth * b.x, surfaceheight * b.y, 25, 0, 2 * M_PI);
        cairo_stroke(ctx);
    }

    cairo_destroy(ctx);
}

void update(float dt) {
    const float speed = 0.7;

    for (size_t i = 0; i < nbubbles; i++) {
        bubbles[i].y -= dt * speed;
    }
}

void resize(struct drawstate ds, int width, int height) {
    cairo_xlib_surface_set_size(ds.surface, width, height);
}

void mainloop(struct winstate ws, struct drawstate ds) {
    emitbubble();
    emitbubble();
    emitbubble();
    emitbubble();
    emitbubble();
    emitbubble();
    emitbubble();
    emitbubble();
    emitbubble();
    emitbubble();

    clock_t prevt = clock();

    for (;;) {
        XEvent event;

        while (XPending(ws.display)) {
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

        clock_t t = clock();
        float dt = 1.0f * (t - prevt) / CLOCKS_PER_SEC;
        prevt = t;

        update(dt);
        draw(ds);
        
        const unsigned int mspf = 1000000 / FPS;
        if (dt < mspf) usleep(mspf - dt);
    }
}

int main(void) {
    struct winstate ws = initwinstate();
    struct drawstate ds = initdrawstate(ws);

    mainloop(ws, ds);

    destroydrawstate(ds);
    destroywinstate(ws);

    return EXIT_SUCCESS;
}
