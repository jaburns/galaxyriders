#pragma once
#include "global.hpp"

class NoCopy 
{
public:
    NoCopy(NoCopy const&) =delete;
    NoCopy& operator=(NoCopy const&) =delete;
    NoCopy() {}
};