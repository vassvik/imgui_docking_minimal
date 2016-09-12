#define IMGUI_DISABLE_INCLUDE_IMCONFIG_H

#include <imgui.h>
//#include "imgui_impl_glfw.h"
#include "imgui_impl_glfw_gl3.h"
#include "imgui_dock.h"
#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>    // extension loading
#include <GLFW/glfw3.h>

#include "mathGL.h"

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error %d: %s\n", error, description);
}

GLFWwindow* window;
int resx = 1280;
int resy = 720;
float  prevx = -1, prevy = -1; // to track how much the mouse moved between frames
float cam_x = -2.0, cam_y = -2.0; // world coordinates of lower-left corner of the window
float cam_height = 4.0;
float cam_width = cam_height*resx/float(resy); 


// buffer object stuffs
GLuint cube_program;
GLuint VertexArrayID;

GLuint vertexbuffer;
GLuint elementbuffer;
int num_indices;

GLuint sizebuffer;
GLuint positionbuffer;
GLuint rotatebuffer;
GLuint startbuffer;
GLuint stopbuffer;
GLuint indexbuffer;


// Framebuffer stuff
GLuint framebuffer;
GLuint texture;
GLuint depthbuffer;

GLuint quad_vertexbuffer;

int textureX = resx;
int textureY = resy;
bool updateTexture = false;

GLuint quad_program;



char *readFile(const char *filename);
void CompileShader(const char * file_path, GLuint ShaderID);
GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path);

void init_GLFW_GLEW();
void init_GL();
void init_ImGUI();

void draw_cube();
void cube_GUI();
int main_menu_GUI();
void do_GUI();

int main(int, char**) {
    init_GLFW_GLEW();
    init_GL();
    init_ImGUI();

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        
        glfwGetFramebufferSize(window, &resx, &resy);
        ImGui_ImplGlfwGL3_NewFrame();

        draw_cube();
        do_GUI();

        // Rendering
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, resx, resy);
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

void init_GLFW_GLEW() {
    // Setup window
    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) {   
        fprintf(stderr, "Failed to initialize GLFW window\n"); fflush(stderr);
        exit(-1);
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(resx, resy, "ImGui OpenGL3 Docking", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create window\n"); fflush(stderr);
        exit(-2);
    }
    glfwMakeContextCurrent(window);

    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n"); fflush(stderr);
        exit(-3);
    }

    glfwSwapInterval(1);
}

void init_GL() {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    /////////////////////////
    // Create and bind VBO //
    /////////////////////////
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    ////////////////////////////////////////////////////////////////////
    // Create program for drawing cube, create VBOs and copy the data //
    ////////////////////////////////////////////////////////////////////
    cube_program = LoadShaders("vertex_shader.vs", "fragment_shader.fs");

    // Actually a cylinder with 20 triangles
    const int n = 10;
    num_indices = 2*n+2;

    GLfloat cubeVertices[n*2*3];
    GLubyte cubeIndices[2*n+2];
    for (int i = 0; i < n; i++) {
        float x = cos(2*PI*i/float(n));
        float y = sin(2*PI*i/float(n));
        cubeVertices[2*3*i + 0] = -1.0;
        cubeVertices[2*3*i + 1] = x;
        cubeVertices[2*3*i + 2] = y;
        cubeVertices[2*3*i + 3] = 1.0;
        cubeVertices[2*3*i + 4] = x;
        cubeVertices[2*3*i + 5] = y;

        cubeIndices[2*i+0] = 2*i + 0;
        cubeIndices[2*i+1] = 2*i + 1;

    }
    cubeIndices[2*n+0] = 0;
    cubeIndices[2*n+1] = 1;


    /*
    const GLfloat cubeVertices[] = {
       -1.0, -1.0,  1.0,
        1.0, -1.0,  1.0,
       -1.0,  1.0,  1.0,
        1.0,  1.0,  1.0,
       -1.0, -1.0, -1.0,
        1.0, -1.0, -1.0,
       -1.0,  1.0, -1.0,
        1.0,  1.0, -1.0,
    };

    const GLubyte cubeIndices[] = {
        0, 1, 2, 3, 7, 1, 5, 4, 7, 6, 2, 4, 0, 1
    };
    */

    const GLfloat instanceSizes[] = {
        1.0, 0.1, 0.1, 
        1.0, 0.2, 0.2, 
        1.0, 0.3, 0.3,
        1.0, 0.2, 0.2
    };

    const GLfloat instancePositions[] {
        0.0, 0.0, 0.0,
        sqrt(3), 0.0, 0.0,
        0.0, sqrt(3), 0.0,
        sqrt(3), sqrt(3), 0.0
    };

    const GLfloat instanceRotate[] {
        45.0, 135.0, 135.0, 45.0
    };


    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glGenBuffers(1, &sizebuffer);
    glBindBuffer(GL_ARRAY_BUFFER, sizebuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(instanceSizes), instanceSizes, GL_STATIC_DRAW);
    glGenBuffers(1, &positionbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, positionbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(instancePositions), instancePositions, GL_STATIC_DRAW);
    glGenBuffers(1, &rotatebuffer);
    glBindBuffer(GL_ARRAY_BUFFER, rotatebuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(instanceRotate), instanceRotate, GL_STATIC_DRAW);
    

    const GLfloat instanceStart[] = {
        0.7, 0.0, 0.0, 0.0,
        0.2, 0.0, 0.0, 0.0,
        0.0, 0.5, 0.0, 0.0,
        0.0, 0.2, 0.4, 0.6
    };

    const GLfloat instanceStop[] = {
        1.0, 0.0, 0.0, 0.0,
        0.5, 0.0, 0.0, 0.0,
        0.3, 1.0, 0.0, 0.0,
        0.1, 0.3, 0.5, 0.8
    };

    const GLfloat instanceIndex[] {
        0.0, -1.0, -1.0, -1.0,
        1.0, -1.0, -1.0, -1.0,
        0.0, 2.0, -1.0, -1.0,
        0.0, 3.0, 4.0, 5.0
    };

    glGenBuffers(1, &startbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, startbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(instanceStart), instanceStart, GL_STATIC_DRAW);
    glGenBuffers(1, &stopbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, stopbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(instanceStop), instanceStop, GL_STATIC_DRAW);
    glGenBuffers(1, &indexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, indexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(instanceIndex), instanceIndex, GL_STATIC_DRAW);

    glGenBuffers(1, &elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,  sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);


    ///////////////////////////////////////////////////////////////////////////
    // Create program for drawing the texture, create VBOs and copy the data //
    ///////////////////////////////////////////////////////////////////////////
    quad_program = LoadShaders("Passthrough.vs", "SimpleTexture.fs");

    const GLfloat g_quad_vertex_buffer_data[] = {
       -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
       -1.0f,  1.0f, 0.0f,
       -1.0f,  1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        1.0f,  1.0f, 0.0f,
    };

    glGenBuffers(1, &quad_vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);


    ////////////////////////////////////////////////////////////////////////
    // Create and bind framebuffer, attach a depth buffer to it           //
    // Create the texture to render to, and attach it to the framebuffer  //
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
}


void init_ImGUI() {
    ///////////////////////////////////////////////////////////
    // Style setup for ImGui. Colors copied from Stylepicker //
    ///////////////////////////////////////////////////////////
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

    ///////////////////////////////////////
    // Load docks from imgui_dock.layout //
    ///////////////////////////////////////
    ImGui::LoadDock();
}

void draw_cube() {
    const GLfloat instanceStart[] = {
        0.7, 0.0, 0.0, 0.0,
        0.2, 0.0, 0.0, 0.0,
        0.0, 0.5, 0.0, 0.0,
        0.0, 0.2, 0.4, 0.6
    };

    const GLfloat instanceStop[] = {
        1.0, 0.0, 0.0, 0.0,
        0.5, 0.0, 0.0, 0.0,
        0.3, 1.0, 0.0, 0.0,
        0.1, 0.3, 0.5, 0.8
    };

    const GLfloat instanceIndex[] {
        0.0, -1.0, -1.0, -1.0,
        1.0, -1.0, -1.0, -1.0,
        0.0, 2.0, -1.0, -1.0,
        0.0, 3.0, 4.0, 5.0
    };

    glBindBuffer(GL_ARRAY_BUFFER, startbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(instanceStart), 0, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(instanceStart), instanceStart);

    glBindBuffer(GL_ARRAY_BUFFER, stopbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(instanceStop), 0, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(instanceStop), instanceStop);

    glBindBuffer(GL_ARRAY_BUFFER, indexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(instanceIndex), 0, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(instanceIndex), instanceIndex);

    //////////////////////////////////////////////////////////////////////
    // Draw a cube to a framebuffer texture using orthogonal projection //
    // with camera looking along the negative Z axis                    //
    // The cube rotates around a random axis                            //
    //////////////////////////////////////////////////////////////////////
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureX, textureY, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glViewport(0,0,textureX,textureY);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(cube_program);

    /////////////////////////////////////////////////////////////////
    // View matrix is identity matrix for 2D,                      //
    // camera movement put in orthogonal projection matrix instead //
    /////////////////////////////////////////////////////////////////
    mat4 View = mat4(1.0f);
    mat4 Projection = ortho(cam_x, cam_x + cam_width, cam_y, cam_y + cam_height, -10, 10);
    //mat4 View = view(vec3(1,0,0), vec3(0,1,0), vec3(0,0,-1), vec3(cam_x,cam_y,6));
    //mat4 Projection = projection(60, resx/float(resy), 0.01, 10.0);

    ////////////////////////////////
    // same RNG seed every frame  //
    // "minimal standard" LCG RNG //
    ////////////////////////////////
    unsigned int IBM = 12321231;
    float rr = 60 + ((IBM *= 16807)/float((unsigned int)(-1)))*60;   // angular velocity, between 60 and 120 degrees per second
    float rr1 = (IBM *= 16807)/float((unsigned int)(-1))*2 + 1;      // x-axis, between -1 and 1
    float rr2 = (IBM *= 16807)/float((unsigned int)(-1))*2 + 1;      // y-axis, between -1 and 1
    float rr3 = (IBM *= 16807)/float((unsigned int)(-1))*2 + 1;      // z-axis, between -1 and 1
    mat4 Model = rotate(vec3(rr1,rr2,rr3), glfwGetTime()*rr*1);

    mat4 MVP = Projection*View*Model;
    glUniformMatrix4fv(glGetUniformLocation(cube_program, "MVP"), 1, GL_FALSE, &MVP.M[0][0]); 

    // Draw!
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void*)0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, sizebuffer);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,0,(void*)0);

    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, positionbuffer);
    glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,0,(void*)0);

    glEnableVertexAttribArray(3);
    glBindBuffer(GL_ARRAY_BUFFER, rotatebuffer);
    glVertexAttribPointer(3,1,GL_FLOAT,GL_FALSE,0,(void*)0);

    glEnableVertexAttribArray(4);
    glBindBuffer(GL_ARRAY_BUFFER, startbuffer);
    glVertexAttribPointer(4,4,GL_FLOAT,GL_FALSE,0,(void*)0);

    glEnableVertexAttribArray(5);
    glBindBuffer(GL_ARRAY_BUFFER, stopbuffer);
    glVertexAttribPointer(5,4,GL_FLOAT,GL_FALSE,0,(void*)0);

    glEnableVertexAttribArray(6);
    glBindBuffer(GL_ARRAY_BUFFER, indexbuffer);
    glVertexAttribPointer(6,4,GL_FLOAT,GL_FALSE,0,(void*)0);

    glVertexAttribDivisor(0, 0);
    glVertexAttribDivisor(1, 1);
    glVertexAttribDivisor(2, 1);
    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);

    //glDrawElements(GL_TRIANGLE_STRIP, 14, GL_UNSIGNED_BYTE, (void*)0);
    glDrawElementsInstanced(GL_TRIANGLE_STRIP, num_indices, GL_UNSIGNED_BYTE, (void*)0, 4);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
    glDisableVertexAttribArray(4);
    glDisableVertexAttribArray(5);
    glDisableVertexAttribArray(6);
}

void cube_GUI() {
    /////////////////////////////////////////////////////
    // Get available size of the current window (dock) //
    // and adjust the texture size if it has changed   //
    // keep cam_height constant                        //
    /////////////////////////////////////////////////////
    ImVec2 size = ImGui::GetContentRegionAvail();
    if (textureX != size.x || textureY != size.y) {
        textureX = size.x;
        textureY = size.y;
        cam_width = cam_height*textureX/float(textureY);
    }

    ImGuiIO& io = ImGui::GetIO();
    static ImVec2 imagePos = ImVec2(textureX/2, textureY/2); // If mouse cursor is outside the screen, use center of image as zoom point
    ImVec2 cursorPos = ImGui::GetCursorScreenPos();          // "cursor" is where imgui will draw the image
    ImVec2 mousePos = io.MousePos;

    //////////////////////////////////////////////////////
    // Draw the image/texture, filling the whole window //
    //////////////////////////////////////////////////////
    ImGui::Image((ImTextureID)texture, size, ImVec2(0, 0), ImVec2(1, -1));

    bool isHovered = ImGui::IsItemHovered();
    bool isClicked = ImGui::IsMouseClicked(0, false);
    bool isFocused = ImGui::IsWindowFocused();

    ////////////////////////////////////////////////////////////////////////////////////
    // So we can move the camera while the mouse cursor is outside the docking window //
    // but the button is not released yet                                             //
    ////////////////////////////////////////////////////////////////////////////////////
    static bool clickedWhileHovered = false;
    if (isHovered && isClicked) {
        clickedWhileHovered = true;
    }

    /////////////////////////////////////////////////////////////////////////////
    // Only do input if the window containing the image is in focus (activaed) //
    /////////////////////////////////////////////////////////////////////////////
    if (isFocused) {
        if (clickedWhileHovered) {
            clickedWhileHovered = io.MouseDown[0];

            if (io.MouseDown[0]) {
                ImVec2 mouseDelta = io.MouseDelta;
                cam_x -= mouseDelta.x/size.x*cam_width;
                cam_y += mouseDelta.y/size.y*cam_height;
            }
        }

        float speed = 3; // 3 units per second
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL)) speed *=  0.1; // slow
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT))   speed *= 10.0;  // fast

        float deltaMoveForward = speed*(glfwGetKey(window, GLFW_KEY_W) - glfwGetKey(window, GLFW_KEY_S));
        float deltaMoveRight   = speed*(glfwGetKey(window, GLFW_KEY_D) - glfwGetKey(window, GLFW_KEY_A));
        float deltaMoveUp      = speed*(glfwGetKey(window, GLFW_KEY_E) - glfwGetKey(window, GLFW_KEY_Q));
        
        // move camera...
        float dt = io.DeltaTime;
        cam_x += dt*deltaMoveRight;
        cam_y += dt*deltaMoveUp;

        // Zoom...
        if (isHovered) {
            imagePos = ImVec2(mousePos.x - cursorPos.x, mousePos.y - cursorPos.y);
        }

        ///////////////////////////////////////////////////////////
        // Zoom by keeping the mouse cursor at the same location //
        // in world coordinates before and after zooming         //
        ///////////////////////////////////////////////////////////
        float zoomFactor = powf(0.95f,dt*deltaMoveForward);
        float xend = cam_x + imagePos.x*(cam_width)/textureX;
        float yend = cam_y + (textureY - imagePos.y)*cam_height/textureY;   
        cam_x = (1.0-zoomFactor)*xend + zoomFactor*cam_x;
        cam_y = (1.0-zoomFactor)*yend + zoomFactor*cam_y;

        cam_width *= zoomFactor;
        cam_height *= zoomFactor;
    }
}

int main_menu_GUI() {
    //////////////////////
    // Placeholder menu //
    //////////////////////
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
    
    return menu_height;
}

void do_GUI() {
    static bool show_scene1 = true;
    static bool show_scene2 = true;
    static bool show_scene3 = true;
    static bool show_scene4 = true;
    static bool show_scene5 = true;
    static bool show_scene6 = true;

    int menu_height = main_menu_GUI();

    if (ImGui::GetIO().DisplaySize.y > 0) {
        ////////////////////////////////////////////////////
        // Setup root docking window                      //
        // taking into account menu height and status bar //
        ////////////////////////////////////////////////////
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
        ImGui::Print(); // print docking information
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
        cube_GUI();
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
}

char *readFile(const char *filename) {
    /////////////////////////////////////////////////////////////
    // Read content of "filename" and return it as a c-string. //
    /////////////////////////////////////////////////////////////
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

GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path) {
    printf("Creating shaders\n");
    GLuint VertexShaderID   = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    CompileShader(vertex_file_path, VertexShaderID);
    CompileShader(fragment_file_path, FragmentShaderID);


    printf("Create and linking program\n");
    GLuint program = glCreateProgram();
    glAttachShader(program, VertexShaderID);
    glAttachShader(program, FragmentShaderID);
    glLinkProgram(program);

    // Check the program
    GLint Result = GL_FALSE;
    int InfoLogLength;

    glGetProgramiv(program, GL_LINK_STATUS, &Result);
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &InfoLogLength);

    if ( InfoLogLength > 0 ){
        GLchar ProgramErrorMessage[9999];
        glGetProgramInfoLog(program, InfoLogLength, NULL, &ProgramErrorMessage[0]);
        printf("%s\n", &ProgramErrorMessage[0]); fflush(stdout);
    }

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

    return program;
}
