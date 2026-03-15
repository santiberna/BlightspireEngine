
// The OPTIMIZE macro causes conflicts when compiling STB Image on WSL
#if BB_DEVELOPMENT
#undef __OPTIMIZE__
#endif

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
