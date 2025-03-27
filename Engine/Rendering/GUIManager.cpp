#include "GUIManager.h"
#include "../Core/Timer.h"

#include "../ECS/Components/TransformComponent.h"
#include "../ECS/Components/CameraComponent.h"
#include "../ECS/Components/LightComponent.h"
#include "../ECS/Components/MaterialComponent.h"

GUIManager::GUIManager(Registry &registry, HWND windowHandle, Timer &timer)
    : registry(registry), windowHandle(windowHandle), timer(timer)
{
}

GUIManager::~GUIManager()
{
    Shutdown();
}

bool GUIManager::Initialize(ID3D11Device *device, ID3D11DeviceContext *context)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    if (!ImGui_ImplWin32_Init(windowHandle))
        return false;
    if (!ImGui_ImplDX11_Init(device, context))
        return false;

    return true;
}

void GUIManager::NewFrame()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ShowMainMenuBar();

    if (showDemoWindow)
    {
        ImGui::ShowDemoWindow(&showDemoWindow);
    }
}

void GUIManager::ShowStatsWindow()
{
    ImGui::SetNextWindowSizeConstraints(
        ImVec2(200, 100),
        ImVec2(FLT_MAX, FLT_MAX),
        [](ImGuiSizeCallbackData *data)
        {
            ImVec2 size = ImGui::GetWindowSize();
            ImGui::SetWindowSize(ImVec2(size.x, ImGui::GetWindowHeight()));
        });

    ImGui::Begin("Engine Stats", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Text("FPS: %d", timer.GetFPS());
    ImGui::Separator();

    auto mainCamera = FindMainCameraEntity();
    if (mainCamera != INVALID_ENTITY)
    {
        if (auto *camera = registry.GetComponent<CameraComponent>(mainCamera))
        {
            if (auto *transform = registry.GetComponent<TransformComponent>(mainCamera))
            {
                ImGui::Text("Camera Position: (%.2f, %.2f, %.2f)",
                            transform->position.x, transform->position.y, transform->position.z);
                ImGui::Text("Camera Look Direction: (%.2f, %.2f, %.2f)",
                            camera->lookDirection.x, camera->lookDirection.y, camera->lookDirection.z);
                ImGui::SliderFloat("FOV", &camera->fov, 0.5f, 2.0f);
                ImGui::SliderFloat("Move Speed", &camera->moveSpeed, 1.0f, 10.0f);
            }
        }
    }

    ImGui::Separator();
    ImGui::Text("Render Settings");
    ImGui::Checkbox("Wireframe Mode", &isWireframeEnabled);

    ImGui::Separator();
    ImGui::Text("Entities: %zu", registry.GetEntityCount());

    ImGui::End();
}

void GUIManager::ShowMainMenuBar()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Exit", "Alt+F4"))
            {
                PostQuitMessage(0);
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View"))
        {
            ImGui::MenuItem("Wireframe Mode", NULL, &isWireframeEnabled);
            ImGui::MenuItem("Demo Window", NULL, &showDemoWindow);
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Help"))
        {
            if (ImGui::MenuItem("About"))
            {
                // set a flag to show an about dialog in the next frame
                // this will be implemented later
            }
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

void GUIManager::ShowEntityInspector()
{
    if (ImGui::Begin("Entity Inspector", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        static EntityID selectedEntity = INVALID_ENTITY;

        ImGui::Text("Entities:");
        ImGui::Separator();

        // go through each new entity type separately

        auto entities = registry.GetEntitiesWith<TransformComponent>();
        auto cameraEntities = registry.GetEntitiesWith<CameraComponent>();
        auto directionalLightEntities = registry.GetEntitiesWith<DirectionalLightComponent>();
        auto pointLightEntities = registry.GetEntitiesWith<PointLightComponent>();
        auto spotLightEntities = registry.GetEntitiesWith<SpotLightComponent>();

        for (auto entity : cameraEntities)
        {
            std::string label = "Camera " + std::to_string(entity);
            if (ImGui::Selectable(label.c_str(), selectedEntity == entity))
            {
                selectedEntity = entity;
            }
        }

        for (auto entity : directionalLightEntities)
        {
            std::string label = "Directional Light " + std::to_string(entity);
            if (ImGui::Selectable(label.c_str(), selectedEntity == entity))
            {
                selectedEntity = entity;
            }
        }

        for (auto entity : pointLightEntities)
        {
            std::string label = "Point Light " + std::to_string(entity);
            if (ImGui::Selectable(label.c_str(), selectedEntity == entity))
            {
                selectedEntity = entity;
            }
        }

        for (auto entity : spotLightEntities)
        {
            std::string label = "Spot Light " + std::to_string(entity);
            if (ImGui::Selectable(label.c_str(), selectedEntity == entity))
            {
                selectedEntity = entity;
            }
        }

        // add any other entities (such as the meshes)
        // should just create a mesh transform component for each mesh entity tbh
        for (auto entity : entities)
        {
            if (registry.GetComponent<CameraComponent>(entity) ||
                registry.GetComponent<DirectionalLightComponent>(entity) ||
                registry.GetComponent<PointLightComponent>(entity) ||
                registry.GetComponent<SpotLightComponent>(entity))
            {
                continue;
            }

            std::string label = "Entity " + std::to_string(entity);
            if (ImGui::Selectable(label.c_str(), selectedEntity == entity))
            {
                selectedEntity = entity;
            }
        }

        // --

        ImGui::Separator();

        if (selectedEntity != INVALID_ENTITY)
        {
            ImGui::Text("Selected Entity: %u", selectedEntity);

            // set a transform tab if the entity has a transform component
            // - the primitive meshes
            if (auto *transform = registry.GetComponent<TransformComponent>(selectedEntity))
            {
                if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    ImGui::DragFloat3("Position", &transform->position.x, 0.1f);
                    ImGui::DragFloat3("Rotation", &transform->rotation.x, 0.01f);
                    ImGui::DragFloat3("Scale", &transform->scale.x, 0.1f, 0.1f, 10.0f);
                }
            }

            // set a camera tab if the entity has a camera component
            // note that we updated the camera component to host more properties,
            // so now those can be changed via the camera tab rather than the transform tab
            // - camera
            if (auto *camera = registry.GetComponent<CameraComponent>(selectedEntity))
            {
                if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    ImGui::DragFloat3("Position", &camera->position.x, 0.1f);
                    ImGui::DragFloat3("Look Direction", &camera->lookDirection.x, 0.01f);
                    ImGui::SliderFloat("FOV", &camera->fov, 0.1f, DirectX::XM_PI);
                    ImGui::SliderFloat("Near Plane", &camera->nearPlane, 0.01f, 1.0f);
                    ImGui::SliderFloat("Far Plane", &camera->farPlane, 10.0f, 1000.0f);
                    ImGui::SliderFloat("Move Speed", &camera->moveSpeed, 1.0f, 20.0f);
                }
            }

            // instead of a tab for a light entity with overloaded components
            // we can just set a light tab for each light type
            // - directional light
            if (auto *light = registry.GetComponent<DirectionalLightComponent>(selectedEntity))
            {
                if (ImGui::CollapsingHeader("Directional Light", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    ImGui::ColorEdit3("Ambient", &light->ambientColor.x);
                    ImGui::ColorEdit3("Diffuse", &light->diffuseColor.x);
                    ImGui::DragFloat3("Direction", &light->direction.x, 0.01f, -1.0f, 1.0f);
                    ImGui::SliderFloat("Intensity", &light->intensity, 0.0f, 5.0f);
                }
            }

            if (auto *light = registry.GetComponent<PointLightComponent>(selectedEntity))
            {
                if (ImGui::CollapsingHeader("Point Light", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    ImGui::ColorEdit3("Ambient", &light->ambientColor.x);
                    ImGui::ColorEdit3("Diffuse", &light->diffuseColor.x);
                    ImGui::DragFloat3("Position", &light->position.x, 0.1f);
                    ImGui::SliderFloat("Range", &light->range, 1.0f, 200.0f);
                    ImGui::SliderFloat("Intensity", &light->intensity, 0.0f, 5.0f);
                }
            }

            if (auto *light = registry.GetComponent<SpotLightComponent>(selectedEntity))
            {
                if (ImGui::CollapsingHeader("Spot Light", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    ImGui::ColorEdit3("Ambient", &light->ambientColor.x);
                    ImGui::ColorEdit3("Diffuse", &light->diffuseColor.x);
                    ImGui::DragFloat3("Position", &light->position.x, 0.1f);
                    ImGui::DragFloat3("Direction", &light->direction.x, 0.01f, -1.0f, 1.0f);
                    ImGui::SliderFloat("Range", &light->range, 1.0f, 200.0f);
                    ImGui::SliderFloat("Inner Cone Angle", &light->innerConeAngle, 0.0f, DirectX::XM_PIDIV2);
                    ImGui::SliderFloat("Outer Cone Angle", &light->outerConeAngle, 0.0f, DirectX::XM_PIDIV2);
                    ImGui::SliderFloat("Intensity", &light->intensity, 0.0f, 5.0f);
                }
            }
        }
    }
    ImGui::End();
}

void GUIManager::ShowDemoWindow(bool *open)
{
    if (*open)
        ImGui::ShowDemoWindow(open);
}

void GUIManager::Render()
{
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void GUIManager::Shutdown()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

EntityID GUIManager::FindMainCameraEntity() const
{
    auto cameraEntities = registry.GetEntitiesWith<CameraComponent>();
    return cameraEntities.empty() ? INVALID_ENTITY : cameraEntities[0];
}