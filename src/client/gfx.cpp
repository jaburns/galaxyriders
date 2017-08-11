#include "gfx.hpp"

#include <cstdlib>
#include <cstdio>
#include "palette.hpp"

#include "readinput.hpp" // Only needed for as long as we're hacking the input directly from here across to the readinput module.

static SDL_Window* s_window;
static SDL_GLContext s_context;

int Gfx::g_window_width;
int Gfx::g_window_height;
bool Gfx::g_should_close_window = false;

void Gfx::init()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        exit(EXIT_FAILURE);
    }

    g_window_width = 1280;
    g_window_height = 720;

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1); // MSAA
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); 
    SDL_GL_SetSwapInterval(1); // 1: vsync, 0: fast

    s_window = SDL_CreateWindow(
        "Galaxy Riders", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        g_window_width, g_window_height,
        SDL_WINDOW_OPENGL
    );

    SDL_SetWindowResizable(s_window, SDL_TRUE);

    s_context = SDL_GL_CreateContext(s_window);

#ifdef _WIN32
    glewExperimental = GL_TRUE;
    const auto glewInitResult = glewInit();
    if (glewInitResult != GLEW_OK) {
        printf("ERROR: %s\n", glewGetErrorString(glewInitResult));
        exit(EXIT_FAILURE);
    }
#endif

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);
    glClearColor(Palette::COLOR_SPACE.r, Palette::COLOR_SPACE.g, Palette::COLOR_SPACE.b, 1.0f);
    glLineWidth(1.0f);
}

void Gfx::flip_and_poll_events()
{
    SDL_GL_SwapWindow(s_window);

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                g_should_close_window = true;
                break;

            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                    g_window_width = event.window.data1;
                    g_window_height = event.window.data2;
                    glViewport(0, 0, g_window_width, g_window_height);
                }
                break;

            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    g_should_close_window = true;
                }
                Input::GLOBAL_key_callback(event.key.keysym.sym, true);
                break;

            case SDL_KEYUP:
                Input::GLOBAL_key_callback(event.key.keysym.sym, false);
                break;
        }
    }
}

void Gfx::deinit()
{
	SDL_GL_DeleteContext(s_context);
	SDL_DestroyWindow(s_window);
	SDL_Quit();
}