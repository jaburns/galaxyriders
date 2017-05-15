#pragma once
#ifdef _WIN32
#   include <GL/glew.h>
#elif __APPLE__
#   define GLFW_INCLUDE_GLCOREARB
#endif
#include <GLFW/glfw3.h>