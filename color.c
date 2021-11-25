#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <X11/Xlib.h>

#define WIDTH 128
#define HEIGHT 128

void die(const char * message) {
    fputs(message, stderr);
    exit(EXIT_FAILURE);
}

int main(void) {
    Display * display = XOpenDisplay(NULL);
    if (display == NULL) die("failed to open display");

    const int screen = DefaultScreen(display);
    const Window window = XCreateSimpleWindow(
        display,
        RootWindow(display, screen),
        0, 0, WIDTH, HEIGHT,
        1, BlackPixel(display, screen), WhitePixel(display, screen)
    );
    XSelectInput(display, window, ExposureMask | KeyPressMask);
    XMapWindow(display, window);

    Atom WM_DELETE = XInternAtom(display, "WM_DELETE_WINDOW", false);
    XSetWMProtocols(display, window, &WM_DELETE, 1);

    bool running = true;

    while (running) {
        XEvent event;
        XNextEvent(display, &event);
        switch(event.type) {
        case ClientMessage:
            if ((Atom) event.xclient.data.l[0] == WM_DELETE) running = false;
            break;
        }
    }

    XDestroyWindow(display, window);
    XCloseDisplay(display);

    return EXIT_SUCCESS;
}
