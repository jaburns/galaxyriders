#include <iostream>
#include <thread> // Needed only for TCPServer test
#include <chrono>
#include <cstring>
#include <cstdint>
#include "core.hpp"
#include "state.hpp"
#include "net_game.hpp"
#include "renderer/game.hpp"
#include "../shared/config.hpp"
#include "../shared/world.hpp"

void main_net()
{
    GameRenderer renderer;
    ClientState new_state;
    NetGame net_game;
    World received_world;

    new_state.player_id = net_game.connect("localhost", Config::DEFAULT_PORT);

    auto receive_world = std::chrono::high_resolution_clock::now();
    auto last_receive_world = std::chrono::high_resolution_clock::now();
    auto millis_per_tick = 100.0f;
    auto last_state = new_state;

    do {
        const auto input = Core::read_input_state().player;

        if (net_game.update(input, received_world)) {
            last_state = new_state;
            new_state.step_with_world(received_world, input);

            last_receive_world = receive_world;
            receive_world = std::chrono::high_resolution_clock::now();
            millis_per_tick = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(receive_world - last_receive_world).count());
        }

        const auto this_frame = std::chrono::high_resolution_clock::now();
        const auto millis_since_update = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(this_frame - receive_world).count());

        renderer.render(last_state.lerp_to(new_state, millis_since_update / millis_per_tick));
    } 
    while (Core::flip_frame_and_poll_events());
}

void main_local()
{
    GameRenderer renderer;

    auto current_time = std::chrono::high_resolution_clock::now();
    auto accumulator = 0.0f;

    // TODO Properly handle adding players and animation states to world in local play mode
    ClientState new_state;
    new_state.player_id = 0;
    new_state.world.players[0];
    new_state.player_anims[0];
    auto last_state = new_state;

    do {
        const auto new_time = std::chrono::high_resolution_clock::now();
        const std::chrono::duration<float> diff = new_time - current_time;
        current_time = new_time;
        accumulator += diff.count() * 1000.0f;

        while (accumulator >= Config::MILLIS_PER_TICK) {
            last_state = new_state;
            new_state.step(Core::read_input_state());
            accumulator -= Config::MILLIS_PER_TICK;
        }

        renderer.render(last_state.lerp_to(new_state, accumulator / Config::MILLIS_PER_TICK));
    } 
    while (Core::flip_frame_and_poll_events());
}

static std::string tcp_server_request_handler(const std::string& message)
{
    std::cout << message << std::endl;
    return "HTTP/1.1 204 No Content\n\n";
//  return "HTTP/1.1 200 OK\nContent-Length: 5\n\nHello";
}

static void tcp_server_test()
{
    TCPServer server(3000);
    std::cout << "Listening on 3000..." << std::endl;

    while (true) {
        server.listen(tcp_server_request_handler);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

int common_main(std::vector<std::string> args)
{
    tcp_server_test();
    return 0;

    Core::init();

    if (args.size() > 0 && args[0] == "net") {
        main_net();
    } else {
        main_local();
    }

    Core::deinit();

    return 0;
}

#if _WIN32 && !_DEBUG

#pragma comment(linker, "/SUBSYSTEM:windows")
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

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
