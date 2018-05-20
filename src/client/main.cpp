#include <iostream>
#include <thread> // Needed only for TCPServer test
#include <chrono>
#include <cstring>
#include <cstdint>

#include "core.hpp"
#include "client_state.hpp"
#include "net_game.hpp"
#include "renderer/game_renderer.hpp"
#include "../shared/config.hpp"
#include "../shared/world_state.hpp"

#ifdef _DEBUG
    #include "../editor/editor.hpp"
#endif

void main_net()
{
    Core core;
    GameRenderer renderer;
    ClientState new_state;
    NetGame net_game;
    WorldState received_world;

    new_state.player_id = net_game.connect("localhost", Config::DEFAULT_PORT);

    auto receive_world = std::chrono::high_resolution_clock::now();
    auto last_receive_world = std::chrono::high_resolution_clock::now();
    auto millis_per_tick = 100.0f;
    auto last_state = new_state;

    do {
        const auto input = core.read_input_state().player;

        if (net_game.update(input, received_world)) {
            last_state = new_state;
            new_state.step_with_world(received_world, input);

            last_receive_world = receive_world;
            receive_world = std::chrono::high_resolution_clock::now();
            millis_per_tick = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(receive_world - last_receive_world).count());
        }

        const auto this_frame = std::chrono::high_resolution_clock::now();
        const auto millis_since_update = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(this_frame - receive_world).count());

        renderer.render(last_state.lerp_to(new_state, millis_since_update / millis_per_tick), core.get_core_view(), EditorState::NO_EDITOR);
    }
    while (core.flip_frame_and_poll_events());
}

void main_local()
{
    Core core;
    GameRenderer renderer;

    #ifdef _DEBUG
        Editor editor;
    #else 
        const auto& editor_state = EditorState::NO_EDITOR;
    #endif

    auto current_time = std::chrono::high_resolution_clock::now();
    auto accumulator = 0.0f;

    // TODO Properly handle adding players and animation states to world in local play mode
    ClientState new_state;
    new_state.player_id = 0;
    new_state.world.players[0];
    new_state.player_anims[0];
    auto last_state = new_state;

    bool editor_paused = false;

    do {
        const auto new_time = std::chrono::high_resolution_clock::now();
        const std::chrono::duration<float> diff = new_time - current_time;
        current_time = new_time;
        accumulator += diff.count() * 1000.0f;

        const auto core_view = core.get_core_view();
        const auto input_state = core.read_input_state();

        if (editor_paused) {
            accumulator = Config::MILLIS_PER_TICK;
        }
        else while (accumulator >= Config::MILLIS_PER_TICK) {
            last_state = new_state;
            new_state.step(input_state, false);
            accumulator -= Config::MILLIS_PER_TICK;
        }

        #ifdef _DEBUG
            const auto editor_state = editor.update(new_state, input_state, core_view);
            editor_paused = editor_state.paused;
        #endif

        renderer.render(last_state.lerp_to(new_state, accumulator / Config::MILLIS_PER_TICK), core_view, editor_state);
    }
    while (core.flip_frame_and_poll_events());
}

int common_main(std::vector<std::string> args)
{
    if (args.size() > 0 && args[0] == "net") {
        main_net();
    } else {
        main_local();
    }

    return 0;
}

#if _WIN32 && !_DEBUG

#pragma comment(linker, "/SUBSYSTEM:windows")
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <shellapi.h>

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)
{
    int argc;
    PWSTR *wargv = CommandLineToArgvW(GetCommandLineW(), &argc);
    std::vector<std::string> args;

    for (auto i = 1; i < argc; ++i) {
        std::wstring wide_arg(wargv[i]);
        std::string out_str;
        out_str.resize(wide_arg.length());
        size_t chars_converted = 0;
        wcstombs_s(&chars_converted, &out_str[0], wide_arg.length() + 1, wide_arg.c_str(), wide_arg.length());
        args.push_back(out_str);
    }

    int result = common_main(args);

    LocalFree(wargv);

    return result;
}

#else

int main(int argc, char **argv)
{
    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) {
        args.push_back(std::string(argv[i]));
    }

    return common_main(args);
}

#endif
