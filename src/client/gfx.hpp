#pragma once

#ifdef __APPLE__
#   include <OpenGL/gl3.h>
#   include <SDL2/SDL.h>
#elif _WIN32
#   include <GL/glew.h>
#   include <SDL.h>
#else
#   error "Unsupported platform"
#endif

namespace Gfx 
{
    extern bool g_should_close_window;
    extern int g_window_width, g_window_height;

    void init();
    void flip_and_poll_events();
    void deinit();
}