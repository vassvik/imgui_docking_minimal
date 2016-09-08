#define IMGUI_DISABLE_INCLUDE_IMCONFIG_H
#include <imgui.h>
#include "imgui_impl_glfw.h"
#include "imgui_dock.h"
#include <stdio.h>
#include <GLFW/glfw3.h>

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error %d: %s\n", error, description);
}


int resx = 1280;
int resy = 720;

int main(int, char**)
{
    // Setup window
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        return 1;
    GLFWwindow* window = glfwCreateWindow(1280, 720, "ImGui OpenGL2 example", NULL, NULL);
    glfwMakeContextCurrent(window);

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowPadding = ImVec2(4.0f, 4.0f);
    style.WindowRounding = 2.0f;
    style.FramePadding = ImVec2(3.0f, 3.0f);
    style.FrameRounding = 2.0f;
/*
    // Optional color stuff
    style.Colors[ImGuiCol_Text]                  = ImVec4(0.78f, 0.78f, 0.78f, 1.00f);
    style.Colors[ImGuiCol_TextDisabled]          = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    style.Colors[ImGuiCol_WindowBg]              = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    style.Colors[ImGuiCol_ChildWindowBg]         = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_PopupBg]               = ImVec4(0.20f, 0.20f, 0.20f, 0.82f);
    style.Colors[ImGuiCol_Border]                = ImVec4(0.70f, 0.70f, 0.70f, 0.65f);
    style.Colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_FrameBg]               = ImVec4(0.80f, 0.80f, 0.80f, 0.30f);
    style.Colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.90f, 0.80f, 0.80f, 0.40f);
    style.Colors[ImGuiCol_FrameBgActive]         = ImVec4(0.90f, 0.65f, 0.65f, 0.45f);
    style.Colors[ImGuiCol_TitleBg]               = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    style.Colors[ImGuiCol_TitleBgActive]         = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
    style.Colors[ImGuiCol_MenuBarBg]             = ImVec4(0.32f, 0.32f, 0.32f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.20f, 0.25f, 0.30f, 0.00f);
    style.Colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.47f, 0.47f, 0.47f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.40f, 0.40f, 0.80f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.09f, 0.09f, 0.38f, 1.00f);
    style.Colors[ImGuiCol_ComboBg]               = ImVec4(0.20f, 0.20f, 0.20f, 0.99f);
    style.Colors[ImGuiCol_CheckMark]             = ImVec4(0.90f, 0.90f, 0.90f, 0.50f);
    style.Colors[ImGuiCol_SliderGrab]            = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
    style.Colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.32f, 0.23f, 0.95f, 1.00f);
    style.Colors[ImGuiCol_Button]                = ImVec4(0.33f, 0.50f, 0.80f, 1.00f);
    style.Colors[ImGuiCol_ButtonHovered]         = ImVec4(0.48f, 0.65f, 0.99f, 1.00f);
    style.Colors[ImGuiCol_ButtonActive]          = ImVec4(0.12f, 0.12f, 0.24f, 1.00f);
    style.Colors[ImGuiCol_Header]                = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    style.Colors[ImGuiCol_HeaderHovered]         = ImVec4(0.20f, 0.20f, 0.42f, 1.00f);
    style.Colors[ImGuiCol_HeaderActive]          = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    style.Colors[ImGuiCol_Column]                = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    style.Colors[ImGuiCol_ColumnHovered]         = ImVec4(0.70f, 0.60f, 0.60f, 1.00f);
    style.Colors[ImGuiCol_ColumnActive]          = ImVec4(0.90f, 0.70f, 0.70f, 1.00f);
    style.Colors[ImGuiCol_ResizeGrip]            = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
    style.Colors[ImGuiCol_ResizeGripHovered]     = ImVec4(1.00f, 1.00f, 1.00f, 0.60f);
    style.Colors[ImGuiCol_ResizeGripActive]      = ImVec4(1.00f, 1.00f, 1.00f, 0.90f);
    style.Colors[ImGuiCol_CloseButton]           = ImVec4(0.64f, 0.00f, 0.18f, 1.00f);
    style.Colors[ImGuiCol_CloseButtonHovered]    = ImVec4(0.70f, 0.70f, 0.90f, 0.60f);
    style.Colors[ImGuiCol_CloseButtonActive]     = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);
    style.Colors[ImGuiCol_PlotLines]             = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_PlotLinesHovered]      = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram]         = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.00f, 0.00f, 1.00f, 0.35f);
    style.Colors[ImGuiCol_ModalWindowDarkening]  = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
*/
/*
    style.Colors[ImGuiCol_Tab]                   = ImVec4(0.32f, 0.32f, 0.32f, 1.00f);
    style.Colors[ImGuiCol_TabHovered]            = ImVec4(0.47f, 0.47f, 0.47f, 1.00f);
    style.Colors[ImGuiCol_TabActive]             = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
*/

    // Setup ImGui binding
    ImGui_ImplGlfw_Init(window, true);

    ImVec4 clear_color = ImColor(114, 144, 154);

    ImGui::LoadDock();

    static bool show_scene1 = true;
    static bool show_scene2 = true;
    static bool show_scene3 = true;
    static bool show_scene4 = true;
    static bool show_scene5 = true;
    static bool show_scene6 = true;

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        ImGui_ImplGlfw_NewFrame();

        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);


        // Draw Main menu inline. Keep track of it's height
        int menu_height = 0;
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("New")) {}
                if (ImGui::MenuItem("Open", "Ctrl+O")) {}
                if (ImGui::BeginMenu("Open Recent"))
                {
                    ImGui::MenuItem("fish_hat.c");
                    ImGui::MenuItem("fish_hat.inl");
                    ImGui::MenuItem("fish_hat.h");
                    if (ImGui::BeginMenu("More.."))
                    {
                        ImGui::MenuItem("Hello");
                        ImGui::MenuItem("Sailor");
                        ImGui::EndMenu();
                    }
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Disabled", false)) // Disabled
                {
                    IM_ASSERT(0);
                }
                if (ImGui::MenuItem("Checked", NULL, true)) {}
                if (ImGui::MenuItem("Quit", "Alt+F4")) {}
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Edit"))
            {
                if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
                if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
                ImGui::Separator();
                if (ImGui::MenuItem("Cut", "CTRL+X")) {}
                if (ImGui::MenuItem("Copy", "CTRL+C")) {}
                if (ImGui::MenuItem("Paste", "CTRL+V")) {}
                ImGui::EndMenu();
            }
            menu_height = ImGui::GetWindowSize().y;

            ImGui::EndMainMenuBar();
        }

        if (ImGui::GetIO().DisplaySize.y > 0) {
            // Setup root docking window
            auto pos = ImVec2(0, menu_height);
            auto size = ImGui::GetIO().DisplaySize;
            size.y -= pos.y;
            ImGui::RootDock(pos, ImVec2(size.x, size.y - 25.0f));

            // Draw status bar (no docking)
            ImGui::SetNextWindowSize(ImVec2(size.x, 25.0f), ImGuiSetCond_Always);
            ImGui::SetNextWindowPos(ImVec2(0, size.y - 6.0f), ImGuiSetCond_Always);
            ImGui::Begin("statusbar", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoResize);
            ImGui::Text("FPS: %f", ImGui::GetIO().Framerate);
            ImGui::End();
        }

        // Draw docking windows
        if (ImGui::BeginDock("Scene1", &show_scene1)) {   
            ImGui::Text("Text       Scene1!");
            ImGui::Print();
        }
        ImGui::EndDock();
        if (ImGui::BeginDock("Scene2", &show_scene2)) {
            ImGui::Text("Text       Scene2!");
        }
        ImGui::EndDock();
        if(ImGui::BeginDock("Scene3", &show_scene3)) {
           ImGui::Text("Text       Scene3!");
        }
        ImGui::EndDock();
        if(ImGui::BeginDock("Scene4", &show_scene4)) {
            ImGui::Text("Text       Scene4!");
        }
        ImGui::EndDock();
        if(ImGui::BeginDock("Scene5", &show_scene5)) {
            ImGui::Text("Text       Scene5!");
        }
        ImGui::EndDock();

        if (ImGui::BeginDock("Scene6", &show_scene6)) {
            ImGui::Text("Text       Scene6!");
        }
        ImGui::EndDock();


        // Rendering
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui::Render();
        glfwSwapBuffers(window);
    }

    ImGui::SaveDock();

    // Cleanup
    ImGui_ImplGlfw_Shutdown();
    glfwTerminate();

    return 0;
}
