#include <iostream>
#include <string>

extern int compute_sdfs_main();
extern int generate_wire_meshes_main();
static void change_to_root_dir();

static void run_task(std::string message, int (*task)())
{
    std::cout << message << "...";
    std::cout.flush();

    int result = task();
    if (result != 0) {
        std::cout << "Error" << std::endl;
        exit(result);
    }

    std::cout << "Done!" << std::endl;
}

int main(int argc, char **argv)
{
    change_to_root_dir();
    run_task("Generating wire meshes", generate_wire_meshes_main);
    run_task("Computing signed distance fields for sprites", compute_sdfs_main);
    return 0;
}

static std::string match_before(const std::string& source, const std::string& find)
{
    const auto index = source.rfind(find, source.length());
    return source.substr(0, index);
}

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>

    static void change_to_root_dir()
    {
        char path_buffer[MAX_PATH];
        GetModuleFileNameA(NULL, path_buffer, MAX_PATH);
        std::string exe_path(path_buffer);
        SetCurrentDirectoryA(match_before(exe_path, "\\x64\\").c_str());
    }
#else
    #include <unistd.h>
    #include <cstdint>
    #ifdef __APPLE__
        #include <mach-o/dyld.h>
    #endif

    static void change_to_root_dir()
    {
        char path_buffer[1024];

        #ifdef __APPLE__
            uint32_t path_len = 1024;
            _NSGetExecutablePath(path_buffer, &path_len);
        #else
            int32_t path_len = readlink("/proc/self/exe", path_buffer, sizeof(path_buffer) - 1);
            if (path_len > 0) {
                path_buffer[path_len] = 0;
            }
        #endif

        if (path_len < 1) {
            std::cout << "Error getting executable path." << std::endl;
            exit(1);
        }

        std::string exe_path(path_buffer);
        chdir(match_before(exe_path, "/tools/").c_str());
    }
#endif
