#ifndef COLOR_H
#define COLOR_H

struct RGB {
   float r; 
   float g; 
   float b; 
};

struct HSV {
   float h; 
   float s; 
   float v; 
};

struct HSV rgbtohsv(struct RGB rgb);
struct RGB hsvtorgb(struct HSV HSV);

#endif
