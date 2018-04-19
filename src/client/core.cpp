#include "core.hpp"

#include <unordered_set>
#include <cstdlib>
#include <cstdio>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>
#include <imgui_impl_sdl_gl3.h>
#include "gl.hpp"
#include "palette.hpp"

static SDL_Window* s_window;
static SDL_GLContext s_context;
static int s_window_width;
static int s_window_height;
static glm::mat4x4 s_perspective_matrix;

static std::unordered_set<SDL_Keycode> keys_down;
static InputState state;

static void recompute_perspective()
{
    s_perspective_matrix = glm::perspective(3.14159f / 3.0f, s_window_width / (float)s_window_height, 0.1f, 1024.0f);
}

void Core::init()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        exit(EXIT_FAILURE);
    }

    s_window_width = 1280;
    s_window_height = 720;
    recompute_perspective();

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
        s_window_width, s_window_height,
        SDL_WINDOW_OPENGL
    );

//  SDL_SetWindowResizable(s_window, SDL_TRUE);
//  SDL_SetWindowFullscreen(s_window, SDL_WINDOW_FULLSCREEN_DESKTOP);

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    ImGui_ImplSdlGL3_Init(s_window);
    ImGui::StyleColorsDark();

    s_context = SDL_GL_CreateContext(s_window);

#ifndef __APPLE__
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

    ImGui_ImplSdlGL3_NewFrame(s_window);
}

static void handle_key_event(SDL_Keycode keycode, bool press)
{
    if (press) {
        keys_down.insert(keycode);
    } else {
        keys_down.erase(keycode);
    }

    state.player.left    = keys_down.count(SDLK_LEFT);
    state.player.right   = keys_down.count(SDLK_RIGHT);
    state.player.up      = keys_down.count(SDLK_UP);
    state.player.down    = keys_down.count(SDLK_DOWN);
    state.editmode_toggle   = keys_down.count(SDLK_p);
    state.editmode_step     = keys_down.count(SDLK_PERIOD);
    state.editmode_zoom_in  = keys_down.count(SDLK_a);
    state.editmode_zoom_out = keys_down.count(SDLK_z);
    state.debug_toggle_wireframe = keys_down.count(SDLK_w);
}

static void handle_mouse_motion(SDL_MouseMotionEvent event)
{
    state.mouse_pos = glm::vec2(
        2.0f * event.x / static_cast<float>(s_window_width) - 1.0f,
        1.0f - 2.0f * event.y /static_cast<float>(s_window_height)
    );
}

bool Core::flip_frame_and_poll_events()
{
    bool still_running = true;

    ImGui::Render();
    ImGui_ImplSdlGL3_RenderDrawData(ImGui::GetDrawData());

    SDL_GL_SwapWindow(s_window);

    ImGui_ImplSdlGL3_NewFrame(s_window);

    SDL_Event event;
    while (SDL_PollEvent(&event)) 
    {
        ImGui_ImplSdlGL3_ProcessEvent(&event);

        switch (event.type) {
            case SDL_QUIT:
                still_running = false;
                break;

            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                    s_window_width = event.window.data1;
                    s_window_height = event.window.data2;
                    recompute_perspective();
                    glViewport(0, 0, s_window_width, s_window_height);
                }
                break;

            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    still_running = false;
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

    return still_running;
}

InputState Core::read_input_state()
{
    return state;
}

static glm::vec3 get_mouse_ray(const glm::vec2& mouse_pos, const glm::mat4x4& projection, const glm::mat4x4& view)
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

const glm::mat4x4& Core::get_perspective_matrix()
{
    return s_perspective_matrix;
}

glm::mat4x4 Core::get_view_matrix(const glm::vec3& camera_pos)
{
    return glm::translate(glm::lookAt(glm::vec3(0.0f), {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f, 0.0f}), -camera_pos);
}

glm::vec2 Core::get_mouse_world_pos(const glm::vec3& camera_pos, const glm::vec2& mouse_pos)
{
    const auto mouse_ray = get_mouse_ray(Core::read_input_state().mouse_pos, Core::get_perspective_matrix(), Core::get_view_matrix(camera_pos));
    const auto plane_normal = glm::vec3(0.0f, 0.0f, -1.0f);
    const auto plane_coord = glm::vec3(0.0f);
    const auto t = (glm::dot(plane_normal, plane_coord) - glm::dot(plane_normal, camera_pos)) / glm::dot(plane_normal, mouse_ray);
    return camera_pos + t * mouse_ray;
}

void Core::deinit()
{
    SDL_GL_DeleteContext(s_context);
    SDL_DestroyWindow(s_window);
    SDL_Quit();
}
