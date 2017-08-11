#include "core.hpp"

#include <unordered_set>
#include <cstdlib>
#include <cstdio>
#include "gl.hpp"
#include "palette.hpp"

static SDL_Window* s_window;
static SDL_GLContext s_context;

static std::unordered_set<SDL_Keycode> keys_down;
static InputState state;

int Core::g_window_width;
int Core::g_window_height;
bool Core::g_should_close_window = false;

void Core::init()
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

static void handle_key_event(SDL_Keycode keycode, bool press)
{
    if (press) {
        keys_down.insert(keycode);
    } else {
        keys_down.erase(keycode);
    }

    state.shared.left  = keys_down.count(SDLK_LEFT);
    state.shared.right = keys_down.count(SDLK_RIGHT);
    state.shared.up    = keys_down.count(SDLK_UP);
    state.shared.down  = keys_down.count(SDLK_DOWN);
    state.debug_pause  = keys_down.count(SDLK_p);
    state.debug_step   = keys_down.count(SDLK_PERIOD);
}

static void handle_mouse_motion(SDL_MouseMotionEvent event)
{
    state.mouse_pos = glm::vec2(
        2.0f * event.x / static_cast<float>(Core::g_window_width) - 1.0f,
        1.0f - 2.0f * event.y /static_cast<float>(Core::g_window_height) 
    );
}

void Core::flip_frame_and_poll_events()
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
                handle_key_event(event.key.keysym.sym, true);
                break;

            case SDL_KEYUP:
                handle_key_event(event.key.keysym.sym, false);
                break;

            case SDL_MOUSEMOTION:
                handle_mouse_motion(event.motion);
                break;

            case SDL_MOUSEBUTTONDOWN:
                state.mouse_click = true;
                break;

            case SDL_MOUSEBUTTONUP:
                state.mouse_click = false;
                break;
        }
    }
}

InputState Core::read_input_state()
{
    return state;
}

glm::vec3 Core::get_mouse_ray(const glm::vec2& mouse_pos, const glm::mat4x4& projection, const glm::mat4x4& view)
{
    glm::vec4 ray_clip = glm::vec4(mouse_pos, -1.0f, 1.0f);

    glm::vec4 ray_eye = glm::inverse(projection) * ray_clip;
    ray_eye.z = -1.0f;
    ray_eye.w =  0.0f;

    glm::vec4 ray_world = glm::inverse(view) * ray_eye;
    ray_world.w = 0.0f;
    ray_world = glm::normalize(ray_world);

    return ray_world;
}

void Core::deinit()
{
	SDL_GL_DeleteContext(s_context);
	SDL_DestroyWindow(s_window);
	SDL_Quit();
}