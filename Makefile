all:
	g++ -std=c++11 -Iimgui main.cpp imgui/{imgui,imgui_draw,imgui_demo,imgui_dock,imgui_impl_glfw}.cpp -lglfw3 -lopengl32 -lgdi32 -g
linux:
	g++ -std=c++11 -Iimgui main.cpp imgui/{imgui,imgui_draw,imgui_demo,imgui_dock,imgui_impl_glfw}.cpp -lglfw -lGL -g
