#pragma once

#include <glm/vec3.hpp>


struct InputState
{
    glm::vec3 look_dir;
    glm::vec3 movement;
    bool clicking; 
};