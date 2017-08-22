#include <iostream>
#include <string>

extern int compute_sdfs_main();
extern int generate_wire_meshes_main();

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
    run_task("Computing signed distance fields for sprites", compute_sdfs_main);
    run_task("Generating wire meshes", generate_wire_meshes_main);
    return 0;
}