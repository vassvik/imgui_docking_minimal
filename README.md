# imgui_docking_minimal

A minimal example combining `dear imgui` with LumixEngine's extension for docking windows (tiling windows, essentially). 

LumixEngine's code uses Lua for layout loading and saving. These have been removed and replaced with simpler functions that just read and write a plain textfile using fprintf and fscanf. These functions are not particularly impressive, but does its job at the moment. 

The main program itself is a modified version of opengl3_example from the imgui repo. It has a menubar, and has 6 docked windows that can me rearranged how you see fit. 

A cube (currently 4 cylinders) is rendered to a framebuffer texture, and that is drawn in one of the docks. This dock accepts mouse and keyboard input to move the camera while active/focused. 

A simple Makefile tested for Windows (MinGW64-w64) and Linux (Arch?) is included, and a simple layout file (that matches the code in main.cpp) is included, that will update after modification of the layout while running the program. 

## Dependencies:
 - GLFW3
 - GLEW
 - dear imgui (included from the imgui repo)
 - imgui_docking.{h,cpp} (included from the LumixEngine repo slightly modified)
