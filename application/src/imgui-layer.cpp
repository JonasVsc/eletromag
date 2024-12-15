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
    ImGui_ImplWGPU_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    sceneGUI();
    mainGUI();
    fpsGUI();

    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), renderPass);
}

void ImGuiLayer::mainGUI()
{
    Scene* scene = Application::get().mCurrentScene;

    ImGui::SetNextWindowSize(ImVec2(300, 720), ImGuiCond_Once); 
    ImGui::SetNextWindowPos(ImVec2(0, 200));
    ImGui::SetNextWindowSizeConstraints(ImVec2(300, 720), ImVec2(300, 720));
    ImGui::Begin("Objetos");                                
    std::vector<const char*> objName;

    static float positions[3] = {0.0f, 0.0f, 0.0f};

    for(auto& obj : scene->mObjects)
    {

        auto objLabel = obj.getDebugName();
        if(ImGui::TreeNode(objLabel.c_str()))
        {


            auto Id = "##" + objLabel;
            ImGui::Text("Position:");
            ImGui::DragFloat3((Id + "Position").c_str(), positions);
            ImGui::Text("Rotation:");
            ImGui::DragFloat3(Id.c_str(), positions);
            ImGui::Text("Scale:");
            ImGui::DragFloat3(Id.c_str(), positions);
            ImGui::TreePop();

        }
    }

    
    ImGui::End();
}

void ImGuiLayer::sceneGUI()
{
    ImGui::SetNextWindowSize(ImVec2(300, 720), ImGuiCond_Once); 
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSizeConstraints(ImVec2(300, 200), ImVec2(300, 200));
    ImGui::Begin("Cena");

    if (ImGui::BeginCombo("##Scene", "Cena Teste")) {
        if (ImGui::Selectable("Cena 1")) {

        }
        if (ImGui::Selectable("Cena 2")) {

        }
        if (ImGui::Selectable("Cena 3")) {

        }
        ImGui::EndCombo();
    }
    ImGui::End();
}

void ImGuiLayer::fpsGUI()
{
    ImVec2 windowPos = ImVec2(ImGui::GetIO().DisplaySize.x - 100, 0);
    ImVec2 windowSize = ImVec2(100, 30); 

    ImGui::SetNextWindowPos(windowPos);
    ImGui::SetNextWindowSize(windowSize);
    ImGui::SetNextWindowBgAlpha(0.0f);  
    ImGui::Begin("FPS Window", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs);
    float fps = ImGui::GetIO().Framerate;
    ImGui::Text("FPS: %.f", fps);
    ImGui::End();
}

