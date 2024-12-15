#include "imgui-layer.h"

#include "application.h"

#include <imgui.h>
#include <backends/imgui_impl_wgpu.h>
#include <backends/imgui_impl_glfw.h>

ImGuiLayer::ImGuiLayer()
    : Layer("ImGuiLayer")
{
}

ImGuiLayer::~ImGuiLayer()
{

}

void ImGuiLayer::onAttach()
{
    std::cout << "ImGuiLayer Attached!" << '\n';

    Application& app = Application::get();
    Renderer2& renderer = app.getRenderer();
    GLFWwindow* window = static_cast<GLFWwindow*>(app.getWindow().getNativeWindow());

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::GetIO();

    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4 customColor = ImVec4(0.1f, 0.5f, 0.8f, 1.0f);
    style.Colors[ImGuiCol_Button] = customColor;
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.2f, 0.6f, 0.9f, 1.0f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.0f, 0.4f, 0.7f, 1.0f);
    style.WindowPadding = ImVec2(10.0f, 10.0f);  
    style.FramePadding = ImVec2(5.0f, 5.0f);     
    style.ItemSpacing  = ImVec2(8.0f, 4.0f);     
    style.ScrollbarSize = 15.0f;                 
    style.WindowRounding = 5.0f;    
    style.FrameRounding = 4.0f;     
    style.PopupRounding = 3.0f;     
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 10));
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF("C:/Dev/eletromag/application/resources/RobotoMono-VariableFont_wght.ttf", 16.0f);

    ImGui_ImplGlfw_InitForOther(window, true);
    ImGui_ImplWGPU_InitInfo init_info;
    init_info.Device = renderer.getDevice();
    init_info.NumFramesInFlight = 3;
    init_info.RenderTargetFormat = WGPUTextureFormat_BGRA8Unorm;
    init_info.DepthStencilFormat = WGPUTextureFormat_Depth24Plus;
    ImGui_ImplWGPU_Init(&init_info);
}

void ImGuiLayer::onDetach()
{
    // terminate gui
    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplWGPU_Shutdown();
}

void ImGuiLayer::onUpdate(WGPURenderPassEncoder renderPass)
{
    Scene* scene = Application::get().mCurrentScene;
    std::string sceneName = scene->getDebugName();
    const char* scene_c = sceneName.c_str();

    

    ImGui_ImplWGPU_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::Begin("Detalhes");                                
    ImGuiIO& io = ImGui::GetIO();
    ImGui::Text("Current Scene: %s", scene_c);
    ImGui::Text("Objetos:");
    std::vector<const char*> objName;
    for(auto& obj : scene->mObjects)
    {
        auto debugNameStr = obj.getDebugName();
        auto debugName = debugNameStr.c_str();
        ImGui::Text("%s", debugName);
    }
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    ImGui::End();

    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), renderPass);
}