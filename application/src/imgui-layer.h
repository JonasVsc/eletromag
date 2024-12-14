#pragma once

#include "layer.h"

class ImGuiLayer : public Layer
{
public:
    ImGuiLayer();

    ~ImGuiLayer();

    void onAttach();

    void onDetach();

    void onUpdate();
    
};