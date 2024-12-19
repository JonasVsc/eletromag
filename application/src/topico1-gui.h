#pragma once

#include "layer.h"

#include <imgui.h>
#include <backends/imgui_impl_wgpu.h>
#include <backends/imgui_impl_glfw.h>

#include "transform.h"

class SceneTopico1;

class Topico1GUI : public Layer
{

public:
    SceneTopico1& mScene;

public:

    Topico1GUI(SceneTopico1& scene);

    ~Topico1GUI();

    void onAttach();

    void onUpdate(WGPURenderPassEncoder renderPass);
    
};
