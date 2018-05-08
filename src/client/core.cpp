#include "core.hpp"

#include <cstdlib>
#include <cstdio>
#include <glm/gtc/matrix_transform.hpp>
#include "gl.hpp"
#include "palette.hpp"

#ifdef _DEBUG
    #include <imgui.h>
    #include <imgui_impl_sdl_gl3.h>
#endif

#ifdef __APPLE__
    #include <SDL2/SDL_syswm.h>
    #include "osx/put_window_over_menu_bar.h"
#endif

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

CoreView::CoreView(glm::vec2 mouse_pos, int window_width, int window_height)
{
    m_mouse_pos = mouse_pos;
    m_perspective = glm::perspective(3.14159f / 3.0f, window_width / (float)window_height, 0.1f, 1024.0f);
}

glm::vec2 CoreView::get_mouse_world_pos(const glm::vec3& camera_pos) const
{
    const auto mouse_ray = get_mouse_ray(m_mouse_pos, m_perspective, get_view_matrix(camera_pos));
    const auto plane_normal = glm::vec3(0.0f, 0.0f, -1.0f);
    const auto plane_coord = glm::vec3(0.0f);
    const auto t = (glm::dot(plane_normal, plane_coord) - glm::dot(plane_normal, camera_pos)) / glm::dot(plane_normal, mouse_ray);
    return camera_pos + t * mouse_ray;
}

glm::mat4x4 CoreView::get_view_matrix(const glm::vec3& camera_pos) const
{
    return glm::translate(glm::lookAt(glm::vec3(0.0f), {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f, 0.0f}), -camera_pos);
}

glm::mat4x4 CoreView::get_perspective_matrix() const
{
    return m_perspective;
}

Core::Core() 
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        exit(EXIT_FAILURE);
    }

    m_window_width = 1280;
    m_window_height = 720;

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1); // MSAA
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetSwapInterval(1);

    Uint32 window_flags = SDL_WINDOW_OPENGL;

    #ifdef __APPLE__
        SDL_DisplayMode displayMode;
        SDL_GetCurrentDisplayMode(0, &displayMode);

        s_window_width = displayMode.w+2;
        s_window_height = displayMode.h+2;
        recompute_perspective();

        window_flags |= SDL_WINDOW_BORDERLESS;
    #else
        window_flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    #endif

    m_window = SDL_CreateWindow(
        "Galaxy Riders",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        m_window_width, m_window_height,
        window_flags
    );

    SDL_SetWindowResizable(m_window, SDL_TRUE);
    m_context = SDL_GL_CreateContext(m_window);

    #ifdef __APPLE__
        SDL_SysWMinfo win_info;
        SDL_GetWindowWMInfo(s_window, &win_info);
        put_window_over_menu_bar(win_info.info.cocoa.window);
    #else
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

    #if _DEBUG
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        ImGui_ImplSdlGL3_Init(m_window);
        ImGui::StyleColorsDark();
        ImGui_ImplSdlGL3_NewFrame(m_window);
    #endif
}

void Core::handle_key_event(SDL_Keycode keycode, bool press)
{
    if (press) {
        m_keys_down.insert(keycode);
    } else {
        m_keys_down.erase(keycode);
    }

    m_input_state.player.left    = m_keys_down.count(SDLK_LEFT);
    m_input_state.player.right   = m_keys_down.count(SDLK_RIGHT);
    m_input_state.player.up      = m_keys_down.count(SDLK_UP);
    m_input_state.player.down    = m_keys_down.count(SDLK_DOWN);
    m_input_state.editmode_toggle   = m_keys_down.count(SDLK_p);
    m_input_state.editmode_step     = m_keys_down.count(SDLK_PERIOD);
    m_input_state.editmode_zoom_in  = m_keys_down.count(SDLK_a);
    m_input_state.editmode_zoom_out = m_keys_down.count(SDLK_z);
    m_input_state.debug_toggle_wireframe = m_keys_down.count(SDLK_w);
}

void Core::handle_mouse_motion(SDL_MouseMotionEvent event)
{
    m_input_state.mouse_pos = glm::vec2(
        2.0f * event.x / static_cast<float>(m_window_width) - 1.0f,
        1.0f - 2.0f * event.y /static_cast<float>(m_window_height)
    );
}

bool Core::flip_frame_and_poll_events()
{
    bool still_running = true;

    #if _DEBUG
        ImGui::Render();
        ImGui_ImplSdlGL3_RenderDrawData(ImGui::GetDrawData());
    #endif

    SDL_GL_SwapWindow(m_window);

    #if _DEBUG
        ImGui_ImplSdlGL3_NewFrame(m_window);
    #endif

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        #if _DEBUG
            ImGui_ImplSdlGL3_ProcessEvent(&event);
        #endif

        switch (event.type) {
            case SDL_QUIT:
                still_running = false;
                break;

            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                    m_window_width = event.window.data1;
                    m_window_height = event.window.data2;
                    glViewport(0, 0, m_window_width, m_window_height);
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
                m_input_state.mouse_click = true;
                break;

            case SDL_MOUSEBUTTONUP:
                m_input_state.mouse_click = false;
                break;
        }
    }

    return still_running;
}

InputState Core::read_input_state() const
{
    return m_input_state;
}

CoreView Core::get_core_view() const
{
    return CoreView(m_input_state.mouse_pos, m_window_width, m_window_height);
}

Core::~Core()
{
    SDL_GL_DeleteContext(m_context);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}
