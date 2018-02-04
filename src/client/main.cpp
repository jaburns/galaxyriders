#define _CRT_SECURE_NO_WARNINGS // For sprintf for now

#include <iostream>
#include <chrono>
#include <cstring>
#include <cstdint>
#include "core.hpp"
#include "state.hpp"
#include "renderer/game.hpp"
#include "../shared/network.hpp"
#include "../shared/config.hpp"

void main_net()
{
    GameRenderer renderer;
    UDPSocket socket;
    const auto send_address = UDPSocket::get_host_address("localhost", Config::DEFAULT_PORT);
    SocketAddress receive_address;
    uint8_t buffer[Config::MAX_PACKET_SIZE];

    std::cout << "Sending ack packet to server port " << Config::DEFAULT_PORT << std::endl;
    sprintf((char*)buffer, "This is packet");
    socket.send(send_address, buffer, static_cast<int>(strnlen((char*)buffer, Config::MAX_PACKET_SIZE)));

    ClientState last_state, new_state;

    auto receive_world = std::chrono::high_resolution_clock::now();
    auto last_receive_world = std::chrono::high_resolution_clock::now();
    auto millis_per_tick = 100.0f;

    do {
        int32_t message_len = 0;
        if (socket.receive(receive_address, buffer, Config::MAX_PACKET_SIZE, message_len)) {
            const auto input = Core::read_input_state().player;

            last_state = new_state;
            new_state.step_with_world(World(buffer, message_len), input);

            last_receive_world = receive_world;
            receive_world = std::chrono::high_resolution_clock::now();
            millis_per_tick = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(receive_world - last_receive_world).count());

            const auto input_buf = input.serialize();
            socket.send(send_address, input_buf.data(), input_buf.size());
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
    ClientState last_state, new_state;

    auto current_time = std::chrono::high_resolution_clock::now();
    auto accumulator = 0.0f;

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

int common_main(std::vector<std::string> args)
{
    Core::init();

    //if (args.size() > 0 && args[0] == "net") {
    if (true) {
        main_net();
    } else {
        main_local();
    }

    Core::deinit();

    return 0;
}


#if _WIN32 && !_DEBUG

#pragma comment(linker, "/SUBSYSTEM:windows")
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
