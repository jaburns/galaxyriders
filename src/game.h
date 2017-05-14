#ifndef __GAME_H
#define __GAME_H

#include "world.h"
#include "input.h"

namespace Game 
{
    World init();
    World step(const World& world, const Input& input);
};

#endif//__GAME_H