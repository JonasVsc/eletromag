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
    Application& app = Application::get();
    GLFWwindow* window = static_cast<GLFWwindow*>(app.getWindow().getNativeWindow());
     // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::GetIO();

    // style
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4 customColor = ImVec4(0.1f, 0.5f, 0.8f, 1.0f); // Azul
    style.Colors[ImGuiCol_Button] = customColor;
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.2f, 0.6f, 0.9f, 1.0f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.0f, 0.4f, 0.7f, 1.0f);
    style.WindowPadding = ImVec2(10.0f, 10.0f);  
    style.FramePadding = ImVec2(5.0f, 5.0f);     
    style.ItemSpacing  = ImVec2(8.0f, 4.0f);     
    style.ScrollbarSize = 15.0f;                 
    style.WindowRounding = 5.0f;    // Bordas arredondadas das janelas
    style.FrameRounding = 4.0f;     // Bordas dos elementos
    style.PopupRounding = 3.0f;     // Bordas de popups
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 10));
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF("C:/Dev/eletromag/application/resources/RobotoMono-VariableFont_wght.ttf", 16.0f);


    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOther(window, true);
    ImGui_ImplWGPU_InitInfo init_info;
    init_info.Device = mDevice;
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

void ImGuiLayer::onUpdate()
{
    ImGui_ImplWGPU_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // ELECTRON
    static float electronMass = 0.0f;
    static float electronVelocity = 0.0f;
    static float electronDirection[3] = {0.0f, 0.0f, 1.0f};
    static float electronPosition[3] = {0.0f, 0.0f, 0.0f}; 
    static float electronRotation[3] = {0.0f, -90.0f, 0.0f}; 
    static float electronScale[3] = {1.0f, 1.0f, 1.0f}; 

    // MAGNECTIC FIELD
    static float fieldIntensity = 0.0f;
    static float fieldDirection[3] = {1.0f, 0.0f, 0.0f};
    static float fieldPosition[3] = {0.0f, 0.0f, 0.0f};
    static float fieldRotation[3] = {0.0f, 90.0f, 0.0f};
    static float fieldScale[3] = {1.0f, 1.0f, 1.0f}; 

    // MAGNECTIC STRENGTH
    static float fmStrength = 0.0f;
    static float fmDirection[3] = {0.0f, 1.0f, 0.0f};
    static float fmPosition[3] = {0.0f, 0.0f, 0.0f}; 
    static float fmRotation[3] = {0.0f, -90.0f, 0.0f}; 
    static float fmScale[3] = {1.0f, 1.0f, 1.0f}; 

    ImGui::SetNextWindowSize(ImVec2(300, 460)); 
    ImGui::SetNextWindowPos(ImVec2(0, 0));  
    ImGui::Begin("Propriedades");                                
    static bool isEletron = false;
    ImGui::Text("Carga Elétrica: ");
    ImGui::Separator();
    if (ImGui::Button(isEletron ? "Elétron" : "Próton"))
        isEletron = !isEletron;
    ImGui::DragFloat3("##Direção1", electronDirection);
    ImGui::Spacing();
    glm::mat4 electronModel(1.0f);
    electronModel = glm::translate(electronModel, glm::vec3(electronPosition[0], electronPosition[1], electronPosition[2]));
    electronModel = glm::rotate(electronModel, glm::radians(electronRotation[0]), glm::vec3(1.0f, 0.0f, 0.0f));
    electronModel = glm::rotate(electronModel, glm::radians(electronRotation[1]), glm::vec3(0.0f, 1.0f, 0.0f));
    electronModel = glm::rotate(electronModel, glm::radians(electronRotation[2]), glm::vec3(0.0f, 0.0f, 1.0f));
    electronModel = glm::scale(electronModel, glm::vec3(electronScale[0], electronScale[1], electronScale[2]));

    ImGui::Text("Campo Magnético: ");
    ImGui::Separator();
    ImGui::DragFloat3("##Direção2", fieldDirection);
    ImGui::Spacing();
    glm::mat4 fieldModel(1.0f);
    fieldModel = glm::translate(fieldModel, glm::vec3(fieldPosition[0], fieldPosition[1], fieldPosition[2]));
    fieldModel = glm::rotate(fieldModel, glm::radians(fieldRotation[0]), glm::vec3(1.0f, 0.0f, 0.0f));
    fieldModel = glm::rotate(fieldModel, glm::radians(fieldRotation[1]), glm::vec3(0.0f, 1.0f, 0.0f));
    fieldModel = glm::rotate(fieldModel, glm::radians(fieldRotation[2]), glm::vec3(0.0f, 0.0f, 1.0f));
    fieldModel = glm::scale(fieldModel, glm::vec3(fieldScale[0], fieldScale[1], fieldScale[2]));

    ImGui::Text("Força Magnética: ");
    ImGui::Separator();
    ImGui::DragFloat3("##Direção3", fmDirection);
    ImGui::Spacing();
    glm::mat4 fmModel(1.0f);
    fmModel = glm::translate(fmModel, glm::vec3(fmPosition[0], fmPosition[1], fmPosition[2]));
    fmModel = glm::rotate(fmModel, glm::radians(fmRotation[0]), glm::vec3(1.0f, 0.0f, 0.0f));
    fmModel = glm::rotate(fmModel, glm::radians(fmRotation[1]), glm::vec3(0.0f, 1.0f, 0.0f));
    fmModel = glm::rotate(fmModel, glm::radians(fmRotation[2]), glm::vec3(0.0f, 0.0f, 1.0f));
    fmModel = glm::scale(fmModel, glm::vec3(fmScale[0], fmScale[1], fmScale[2]));

    glm::vec3 electronPos = glm::vec3(electronPosition[0], electronPosition[1], electronPosition[2]);
    glm::vec3 electronTarget = glm::vec3(electronDirection[0], electronDirection[1], electronDirection[2]);
    electronModel = rotateToTarget(electronModel, electronPos, electronTarget);

    glm::vec3 fieldPos = glm::vec3(fieldPosition[0], fieldPosition[1], fieldPosition[2]);
    glm::vec3 fieldTarget = glm::vec3(fieldDirection[0], fieldDirection[1], fieldDirection[2]);
    fieldModel = rotateToTarget(fieldModel, fieldPos, fieldTarget);

    glm::vec3 fmPos = glm::vec3(fmPosition[0], fmPosition[1], fmPosition[2]);
    glm::vec3 fmTarget = glm::vec3(fmDirection[0], fmDirection[1], fmDirection[2]);
    fmModel = rotateToTarget(fmModel, fmPos, fmTarget);

    wgpuQueueWriteBuffer(mQueue, mUniformElectronBuffer, offsetof(MyUniforms, modelMatrix), &electronModel, sizeof(glm::mat4));
    wgpuQueueWriteBuffer(mQueue, mUniformElectronBuffer, offsetof(MyUniforms, direction), &electronTarget, sizeof(glm::vec3));

    wgpuQueueWriteBuffer(mQueue, mUniformFieldBuffer, offsetof(MyUniforms, modelMatrix), &fieldModel, sizeof(glm::mat4));
    wgpuQueueWriteBuffer(mQueue, mUniformFieldBuffer, offsetof(MyUniforms, direction), &fieldTarget, sizeof(glm::vec3));

    wgpuQueueWriteBuffer(mQueue, mUniformFMBuffer, offsetof(MyUniforms, modelMatrix), &fmModel, sizeof(glm::mat4));
    wgpuQueueWriteBuffer(mQueue, mUniformFMBuffer, offsetof(MyUniforms, direction), &fmTarget, sizeof(glm::vec3));

    if(ImGui::Button("Calcular Carga Elétrica", ImVec2(ImGui::GetContentRegionAvail().x, 35.0f)))
    {
        electronTarget = glm::vec3(glm::normalize(glm::cross(fieldTarget, fmTarget)));
        if(isEletron)
            electronTarget *= -1;
        electronModel = rotateToTarget(electronModel, electronPos, electronTarget);

        electronDirection[0] = electronTarget.x;
        electronDirection[1] = electronTarget.y;
        electronDirection[2] = electronTarget.z;

        wgpuQueueWriteBuffer(mQueue, mUniformElectronBuffer, offsetof(MyUniforms, modelMatrix), &electronModel, sizeof(glm::mat4));
        wgpuQueueWriteBuffer(mQueue, mUniformElectronBuffer, offsetof(MyUniforms, direction), &electronTarget, sizeof(glm::vec3));
    }
    if(ImGui::Button("Calcular Campo Magnético", ImVec2(ImGui::GetContentRegionAvail().x, 35.0f)))
    {
        if(isEletron)
            electronTarget *= -1;
        fieldTarget = glm::vec3(glm::normalize(glm::cross(fmTarget, electronTarget)));
        fieldModel = rotateToTarget(fieldModel, fieldPos, fieldTarget);

        fieldDirection[0] = fieldTarget.x;
        fieldDirection[1] = fieldTarget.y;
        fieldDirection[2] = fieldTarget.z;

        wgpuQueueWriteBuffer(mQueue, mUniformFieldBuffer, offsetof(MyUniforms, modelMatrix), &fieldModel, sizeof(glm::mat4));
        wgpuQueueWriteBuffer(mQueue, mUniformFieldBuffer, offsetof(MyUniforms, direction), &fieldTarget, sizeof(glm::vec3));
    }
    if(ImGui::Button("Calcular Força Magnética", ImVec2(ImGui::GetContentRegionAvail().x, 35.0f)))
    {
        if(isEletron)
            electronTarget *= -1;
        fmTarget = glm::vec3(glm::normalize(glm::cross(electronTarget, fieldTarget)));
        fmModel = rotateToTarget(fmModel, fmPos, fmTarget);

        fmDirection[0] = fmTarget.x;
        fmDirection[1] = fmTarget.y;
        fmDirection[2] = fmTarget.z;

        wgpuQueueWriteBuffer(mQueue, mUniformFMBuffer, offsetof(MyUniforms, modelMatrix), &fmModel, sizeof(glm::mat4));
        wgpuQueueWriteBuffer(mQueue, mUniformFMBuffer, offsetof(MyUniforms, direction), &fmTarget, sizeof(glm::vec3));
    }
    ImGui::End();

    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), renderPass);
}