#ifndef GRAPHICS_GFX_H
#define GRAPHICS_GFX_H
#include <exec/types.h>
typedef UBYTE *PLANEPTR;
struct BitMap { UWORD BytesPerRow; UWORD Rows; UBYTE Flags; UBYTE Depth; UWORD pad; PLANEPTR Planes[8]; };
struct Rectangle { WORD MinX, MinY, MaxX, MaxY; };
#endif
