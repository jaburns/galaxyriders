#include "core.hpp"

#include <cstdlib>
#include <cstdio>
#include <glm/gtc/matrix_transform.hpp>
#include "gl.hpp"
#include "palette.hpp"
#include "audio/audio_player.hpp"

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

Core::Core(bool fullscreen)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0) 
        exit(EXIT_FAILURE);

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

    if (fullscreen)
    {
    #ifdef __APPLE__
        SDL_DisplayMode displayMode;
        SDL_GetCurrentDisplayMode(0, &displayMode);

        m_window_width = displayMode.w+2;
        m_window_height = displayMode.h+2;

        window_flags |= SDL_WINDOW_BORDERLESS;
    #else
        window_flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    #endif
    }
    else
    {
        window_flags |= SDL_WINDOW_MAXIMIZED;
    }

    m_window = SDL_CreateWindow(
        "Galaxy Riders",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        m_window_width, m_window_height,
        window_flags
    );

    if (! fullscreen)
        SDL_SetWindowResizable(m_window, SDL_TRUE);

    m_context = SDL_GL_CreateContext(m_window);

    #ifdef __APPLE__
        SDL_SysWMinfo win_info;
        SDL_GetWindowWMInfo(m_window, &win_info);
        put_window_over_menu_bar(win_info.info.cocoa.window);
    #else
        glewExperimental = GL_TRUE;
        const auto glewInitResult = glewInit();

        if (glewInitResult != GLEW_OK) 
        {
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

    m_input_state.player_sources.push_back({});

    LOG(SDL_NumJoysticks(), "joysticks were found.");
    m_controller = nullptr;
    for (int i = 0; i < SDL_NumJoysticks(); ++i) 
    {
        if (! SDL_IsGameController(i)) continue;

        m_controller = SDL_GameControllerOpen(i);

        if (m_controller)
        {
            LOG("Using game controller: ", SDL_GameControllerName(m_controller));
            m_input_state.player_sources.push_back({});
            break;
        }
    }
}

void Core::handle_key_event(SDL_Keycode keycode, bool press)
{
    if (press)
        m_keys_down.insert(keycode);
    else
        m_keys_down.erase(keycode);

    m_input_state.player_sources[0].left  = m_keys_down.count(SDLK_a);
    m_input_state.player_sources[0].right = m_keys_down.count(SDLK_d);
    m_input_state.player_sources[0].up    = m_keys_down.count(SDLK_w);
    m_input_state.player_sources[0].down  = m_keys_down.count(SDLK_s);
    m_input_state.player_sources[0].join  = m_keys_down.count(SDLK_SPACE);
    m_input_state.pressing_shift = m_keys_down.count(SDLK_LSHIFT);
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

        switch (event.type) 
        {
            case SDL_QUIT:
                still_running = false;
                break;

            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) 
                {
                    m_window_width = event.window.data1;
                    m_window_height = event.window.data2;
                    glViewport(0, 0, m_window_width, m_window_height);
                }
                break;

            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE) 
                    still_running = false;

                handle_key_event(event.key.keysym.sym, true);
                break;

            case SDL_KEYUP:
                handle_key_event(event.key.keysym.sym, false);
                break;

            case SDL_MOUSEMOTION:
                handle_mouse_motion(event.motion);
                break;

            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT)
                    m_input_state.mouse_click = true;
                else if (event.button.button == SDL_BUTTON_RIGHT)
                    m_input_state.right_click = true;
                break;

            case SDL_MOUSEBUTTONUP:
                if (event.button.button == SDL_BUTTON_LEFT)
                    m_input_state.mouse_click = false;
                else if (event.button.button == SDL_BUTTON_RIGHT)
                    m_input_state.right_click = false;
                break;

            case SDL_MOUSEWHEEL:
                m_input_state.mouse_scroll.x += event.wheel.x; 
                m_input_state.mouse_scroll.y += event.wheel.y; 
                break;
        }
    }

    if (m_controller)
    {
        int16_t axis = SDL_GameControllerGetAxis(m_controller, SDL_CONTROLLER_AXIS_LEFTX);
        m_input_state.player_sources[1].join = SDL_GameControllerGetButton(m_controller, SDL_CONTROLLER_BUTTON_START);
        m_input_state.player_sources[1].left = axis < -5000;
        m_input_state.player_sources[1].right = axis > 5000;
        m_input_state.player_sources[1].up = SDL_GameControllerGetButton(m_controller, SDL_CONTROLLER_BUTTON_A);
        m_input_state.player_sources[1].down = SDL_GameControllerGetButton(m_controller, SDL_CONTROLLER_BUTTON_X);
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
    if (m_controller)
        SDL_GameControllerClose(m_controller);

    SDL_GL_DeleteContext(m_context);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}
