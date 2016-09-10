#define IMGUI_DISABLE_INCLUDE_IMCONFIG_H
#include <imgui.h>
//#include "imgui_impl_glfw.h"
#include "imgui_impl_glfw_gl3.h"
#include "imgui_dock.h"
#include <stdio.h>
#include <GL/glew.h>    // extension loading
#include <GLFW/glfw3.h>

#include "mathGL.h"

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error %d: %s\n", error, description);
}


int resx = 1280;
int resy = 720;
double prevx = -1, prevy = -1; // to track how much the mouse moved between frames
double cam_x = -2.0, cam_y = -2.0; // world coordinates of lower-left corner of the window
double cam_height = 4.0;
double cam_width = cam_height*resx/double(resy); 


// buffer object stuffs
GLuint programID;
GLuint VertexArrayID;

GLuint vertexbuffer;
GLuint elementbuffer;

static const GLfloat cubeVertices[] = {
   -1.0, -1.0,  1.0,
    1.0, -1.0,  1.0,
   -1.0,  1.0,  1.0,
    1.0,  1.0,  1.0,
   -1.0, -1.0, -1.0,
    1.0, -1.0, -1.0,
   -1.0,  1.0, -1.0,
    1.0,  1.0, -1.0,
};

static const GLushort cubeIndices[] = {
    0, 1, 2, 3, 7, 1, 5, 4, 7, 6, 2, 4, 0, 1
};


GLuint framebuffer;
GLuint texture;
GLuint depthbuffer;

GLuint quad_VertexArrayID;
GLuint quad_vertexbuffer;

int textureX = resx;
int textureY = resy;
bool updateTexture = false;


static const GLfloat g_quad_vertex_buffer_data[] = {
    -1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    1.0f,  1.0f, 0.0f,
};

GLuint quad_program;


char *readFile(const char *filename) {
    // Read content of "filename" and return it as a c-string.
    printf("Reading %s\n", filename);
    FILE *f = fopen(filename, "rb");

    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);
    printf("Filesize = %d\n", int(fsize));

    char *string = (char*)malloc(fsize + 1);
    fread(string, fsize, 1, f);
    string[fsize] = '\0';
    fclose(f);

    return string;
}


void CompileShader(const char * file_path, GLuint ShaderID) {
    GLint Result = GL_FALSE;
    int InfoLogLength;

    char *ShaderCode = readFile(file_path);

    // Compile Shader
    printf("Compiling shader : %s\n", file_path);
    glShaderSource(ShaderID, 1, (const char**)&ShaderCode , NULL);
    glCompileShader(ShaderID);

    // Check Shader
    glGetShaderiv(ShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(ShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);

    if ( Result == GL_FALSE ){
        char ShaderErrorMessage[9999];
        glGetShaderInfoLog(ShaderID, InfoLogLength, NULL, ShaderErrorMessage);
        printf("%s", ShaderErrorMessage);
    }
}

GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path){
    printf("Creating shaders\n");
    GLuint VertexShaderID   = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    CompileShader(vertex_file_path, VertexShaderID);
    CompileShader(fragment_file_path, FragmentShaderID);


    printf("Create and linking program\n");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);

    // Check the program
    GLint Result = GL_FALSE;
    int InfoLogLength;

    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);

    if ( InfoLogLength > 0 ){
        GLchar ProgramErrorMessage[9999];
        glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
        printf("%s\n", &ProgramErrorMessage[0]); fflush(stdout);
    }

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);


    return ProgramID;
}


int main(int, char**)
{
    // Setup window
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        return 1;
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(1280, 720, "ImGui OpenGL2 example", NULL, NULL);
    glfwMakeContextCurrent(window);

    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        exit(-3);
    }

    /////////////////////////
    // Create and bind VBO //
    /////////////////////////
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    ////////////////////////////////////////////////////////////////////
    // Create program for drawing cube, create VBOs and copy the data //
    ////////////////////////////////////////////////////////////////////
    programID = LoadShaders( "vertex_shader.vs", "fragment_shader.fs" );

    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glGenBuffers(1, &elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,  sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);


    ///////////////////////////////////////////////////////////////////////////
    // Create program for drawing the texture, create VBOs and copy the data //
    ///////////////////////////////////////////////////////////////////////////
    quad_program = LoadShaders("Passthrough.vs", "SimpleTexture.fs");
    glGenBuffers(1, &quad_vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);


    ////////////////////////////////////////////////////////////////////////
    // Create and bind framebuffer, attach a depth buffer to it           //
    // Create the texture to render to, and attatch it to the framebuffer //
    ////////////////////////////////////////////////////////////////////////
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    glGenRenderbuffers(1, &depthbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, resx, resy);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthbuffer);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, resx, resy, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture, 0);
    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, DrawBuffers);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        printf("Error in setting up the framebuffer\n");
    }



    glfwSwapInterval(1);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);




    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowPadding = ImVec2(4.0f, 4.0f);
    style.WindowRounding = 2.0f;
    style.FramePadding = ImVec2(3.0f, 3.0f);
    style.FrameRounding = 2.0f;

    style.Colors[ImGuiCol_Text]                  = ImVec4(0.97f, 0.97f, 0.97f, 1.00f);
    style.Colors[ImGuiCol_TextDisabled]          = ImVec4(0.56f, 0.56f, 0.54f, 1.00f);
    style.Colors[ImGuiCol_WindowBg]              = ImVec4(0.15f, 0.16f, 0.13f, 1.00f);
    style.Colors[ImGuiCol_ChildWindowBg]         = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_PopupBg]               = ImVec4(0.24f, 0.24f, 0.23f, 1.00f);
    style.Colors[ImGuiCol_Border]                = ImVec4(0.70f, 0.70f, 0.70f, 0.65f);
    style.Colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_FrameBg]               = ImVec4(0.20f, 0.20f, 0.18f, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.24f, 0.24f, 0.23f, 1.00f);
    style.Colors[ImGuiCol_FrameBgActive]         = ImVec4(0.24f, 0.24f, 0.23f, 1.00f);
    style.Colors[ImGuiCol_TitleBg]               = ImVec4(0.27f, 0.27f, 0.54f, 0.83f);
    style.Colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.40f, 0.40f, 0.80f, 0.20f);
    style.Colors[ImGuiCol_TitleBgActive]         = ImVec4(0.32f, 0.32f, 0.63f, 0.87f);
    style.Colors[ImGuiCol_MenuBarBg]             = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.21f, 0.21f, 0.20f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
    style.Colors[ImGuiCol_ComboBg]               = ImVec4(0.20f, 0.20f, 0.20f, 0.99f);
    style.Colors[ImGuiCol_CheckMark]             = ImVec4(0.90f, 0.90f, 0.90f, 0.50f);
    style.Colors[ImGuiCol_SliderGrab]            = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
    style.Colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.80f, 0.50f, 0.50f, 1.00f);
    style.Colors[ImGuiCol_Button]                = ImVec4(0.32f, 0.32f, 0.32f, 1.00f);
    style.Colors[ImGuiCol_ButtonHovered]         = ImVec4(0.48f, 0.48f, 0.48f, 1.00f);
    style.Colors[ImGuiCol_ButtonActive]          = ImVec4(0.65f, 0.65f, 0.65f, 1.00f);
    style.Colors[ImGuiCol_Header]                = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
    style.Colors[ImGuiCol_HeaderHovered]         = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
    style.Colors[ImGuiCol_HeaderActive]          = ImVec4(0.53f, 0.53f, 0.87f, 0.80f);
    style.Colors[ImGuiCol_Column]                = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    style.Colors[ImGuiCol_ColumnHovered]         = ImVec4(0.70f, 0.60f, 0.60f, 1.00f);
    style.Colors[ImGuiCol_ColumnActive]          = ImVec4(0.90f, 0.70f, 0.70f, 1.00f);
    style.Colors[ImGuiCol_ResizeGrip]            = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
    style.Colors[ImGuiCol_ResizeGripHovered]     = ImVec4(1.00f, 1.00f, 1.00f, 0.60f);
    style.Colors[ImGuiCol_ResizeGripActive]      = ImVec4(1.00f, 1.00f, 1.00f, 0.90f);
    style.Colors[ImGuiCol_CloseButton]           = ImVec4(0.50f, 0.50f, 0.90f, 0.50f);
    style.Colors[ImGuiCol_CloseButtonHovered]    = ImVec4(0.70f, 0.70f, 0.90f, 0.60f);
    style.Colors[ImGuiCol_CloseButtonActive]     = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);
    style.Colors[ImGuiCol_PlotLines]             = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_PlotLinesHovered]      = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram]         = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.00f, 0.00f, 1.00f, 0.35f);
    style.Colors[ImGuiCol_ModalWindowDarkening]  = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);





    // Setup ImGui binding
    ImGui_ImplGlfwGL3_Init(window, true);

    ImVec4 clear_color = ImColor(0,0,0);

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
        ImGui_ImplGlfwGL3_NewFrame();

        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureX, textureY, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glViewport(0,0,textureX,textureY);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(programID);

        // View matrix is identity matrix for 2D, 
        // camera movement put in orthogonal projection matrix instead
        mat4 View = mat4(1.0f);
        mat4 Projection = ortho(cam_x, cam_x + cam_width, cam_y, cam_y + cam_height, -2, 2);

        // same RNG seed every frame
        // "minimal standard" LCG RNG
        unsigned int IBM = 12321231;
        double rr = 60 + ((IBM *= 16807)/double((unsigned int)(-1)))*60;   // rotation, between 60 and 120 degrees per second
        double rr1 = (IBM *= 16807)/double((unsigned int)(-1))*2 + 1;      // x-axis, between -1 and 1
        double rr2 = (IBM *= 16807)/double((unsigned int)(-1))*2 + 1;      // y-axis, between -1 and 1
        double rr3 = (IBM *= 16807)/double((unsigned int)(-1))*2 + 1;      // z-axis, between -1 and 1
        mat4 Model = rotate(vec3(rr1,rr2,rr3), glfwGetTime()*rr);
        mat4 MVP = Projection*View*Model;
        glUniformMatrix4fv(glGetUniformLocation(programID, "MVP"), 1, GL_FALSE, &MVP.M[0][0]); 

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void*)0);
        glDrawElements(GL_TRIANGLE_STRIP, sizeof(cubeIndices), GL_UNSIGNED_SHORT, (void*)0);
        glDisableVertexAttribArray(0);

        

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
        if (ImGui::BeginDock("Docks", &show_scene1)) {   
            ImGui::Print();
        }
        ImGui::EndDock();
        if (ImGui::BeginDock("Dummy1", &show_scene2)) {
            ImGui::Text("Placeholder!");
        }
        ImGui::EndDock();
        if(ImGui::BeginDock("Dummy2", &show_scene3)) {
           ImGui::Text("Placeholder!");
        }
        ImGui::EndDock();
        if(ImGui::BeginDock("Cube", &show_scene4, ImGuiWindowFlags_NoScrollbar)) {
            ImVec2 size = ImGui::GetContentRegionAvail();

            if (textureX != size.x || textureY != size.y) {
                
                textureX = size.x;
                textureY = size.y;

                cam_width = cam_height*textureX/float(textureY);
            }

            ImVec2 cursorPos = ImGui::GetCursorScreenPos();
            ImGui::Image((ImTextureID)texture, size, ImVec2(0, 0), ImVec2(1, -1));

            ImGuiIO& io = ImGui::GetIO();

            static bool clickedWhileHovered = false;
            bool isHovered = ImGui::IsItemHovered();

            if (isHovered && ImGui::IsMouseClicked(0, false)) {
                clickedWhileHovered = true;
            }

            static ImVec2 imagePos = ImVec2(textureX/2, textureY/2);

            if (ImGui::IsWindowFocused()) {
                if (clickedWhileHovered) {
                    clickedWhileHovered = io.MouseDown[0];

                    if (io.MouseDown[0]) {
                        ImVec2 mouseDelta = io.MouseDelta;
                        cam_x -= mouseDelta.x/size.x*cam_width;
                        cam_y += mouseDelta.y/size.y*cam_height;
                    }
                }

                double speed = 3; // 3 units per second
                if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL)) speed *=  0.1; // slow
                if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT))   speed *= 10.0;  // fast

                double deltaMoveForward = speed*(glfwGetKey(window, GLFW_KEY_W) - glfwGetKey(window, GLFW_KEY_S));
                double deltaMoveRight   = speed*(glfwGetKey(window, GLFW_KEY_D) - glfwGetKey(window, GLFW_KEY_A));
                double deltaMoveUp      = speed*(glfwGetKey(window, GLFW_KEY_E) - glfwGetKey(window, GLFW_KEY_Q));
                
                double dt = io.DeltaTime;
                // move camera...
                cam_x += dt*deltaMoveRight;
                cam_y += dt*deltaMoveUp;

                // Zoom...
                ImVec2 mousePos = io.MousePos;
                if (isHovered) {
                    imagePos = ImVec2(mousePos.x - cursorPos.x, mousePos.y - cursorPos.y);
                }
                
                double zoomFactor = pow(0.95,dt*deltaMoveForward);
                double xend = cam_x + imagePos.x*(cam_width)/textureX;           // convert screen position to world cooridnates
                double yend = cam_y + (textureY - imagePos.y)*cam_height/textureY;   
                cam_x = (1.0-zoomFactor)*xend + zoomFactor*cam_x;       // update lower left corner
                cam_y = (1.0-zoomFactor)*yend + zoomFactor*cam_y;

                cam_width *= zoomFactor;
                cam_height *= zoomFactor;
                
            }
        }
        ImGui::EndDock();
        if(ImGui::BeginDock("Dummy3", &show_scene5)) {
            ImGui::Text("Placeholder!");
        }
        ImGui::EndDock();

        if (ImGui::BeginDock("StyleEditor", &show_scene6)) {
            ImGui::ShowStyleEditor();
        }
        ImGui::EndDock();


        // Rendering
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui::Render();
        glfwSwapBuffers(window);
    }

    ImGui::SaveDock();

    // Cleanup
    ImGui_ImplGlfwGL3_Shutdown();
    glfwTerminate();

    return 0;
}
