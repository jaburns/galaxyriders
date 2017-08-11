#pragma once

class NoCopy 
{
public:
    NoCopy(NoCopy const&) =delete;
    NoCopy& operator=(NoCopy const&) =delete;
    NoCopy() {}
};