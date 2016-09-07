all:
	g++ -std=c++11 -Iimgui -Iimgui_dock main.cpp imgui/{imgui,imgui_draw,imgui_demo,imgui_impl_glfw}.cpp imgui_dock/imgui_dock.cpp -lglfw3 -lopengl32 -lgdi32 -g
