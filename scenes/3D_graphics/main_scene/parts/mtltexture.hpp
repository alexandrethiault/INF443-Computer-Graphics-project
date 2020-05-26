#pragma once

#include "main/scene_base/base.hpp"

#ifdef MAIN_SCENE

struct mtltexture {
    float Ns = -1, Ni = -1, d = -1;
    vcl::vec3 Ka, Kd, Ks, Ke;
    int illum = -1;
    std::string name, map_Kd = "None";
};

#endif
