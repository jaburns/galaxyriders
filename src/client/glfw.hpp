#pragma once

#include <SDL.h>
#ifdef _WIN32
#   include <GL/glew.h>
#elif __APPLE__
#   define GLFW_INCLUDE_GLCOREARB
#else
#   error "Unhandled platform"
#endif
#include <gl\glew.h>
#include <SDL_opengl.h>