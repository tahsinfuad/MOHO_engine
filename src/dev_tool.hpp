#pragma once

#include <iostream>
#include "SDL3/SDL.h"

#include "src/core.hpp"

#include "imgui-master/imgui.h"
#include "imgui-master/imgui_impl_sdl3.h"
#include "imgui-master/imgui_impl_opengl3.h"

namespace dev_tool{

    void dux_init(SDL_Window *win,SDL_GLContext &gl_context){
        IMGUI_CHECKVERSION();  
        ImGui::CreateContext();

        ImGui_ImplSDL3_InitForOpenGL( win , gl_context );
        ImGui_ImplOpenGL3_Init("#version 330");

    }

    bool enable_vsync(bool& vsync){

        ImGui::Begin("VSYNC status           ",nullptr,ImGuiWindowFlags_AlwaysAutoResize);

        if (ImGui::Checkbox("VSYNC         ", &vsync)){

        SDL_GL_SetSwapInterval(vsync ? 1 : 0);
        if (vsync){
        std::cout << "VSYNC enabled" << std::endl;
        }else{
        std::cout << "VSYNC disabled" << std::endl;
        }
    }
    
    ImGui::End();
    return vsync;

    }

    void display_stat(float dt){

    static float timer = 0.0f;
    static float frameTime = 0.0f;
    static int frames = 0;

    static float fps = 0.0f;
    static float avgDT = 0.0f;

    timer += dt;
    frameTime += dt;
    frames++;

    if (timer >= 2.5f){
    avgDT = frameTime / frames;
    fps = 1.0f / avgDT;

    timer = 0.0f;
    frameTime = 0.0f;
    frames = 0;
    }

    ImGui::Begin("GAME STATS");

    ImGui::Text("Delta time = %.2f ms", avgDT * 1000.0f);
    ImGui::Text("FPS = %.1f", fps);

    ImGui::End();

    }
    void dux_render(){
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
    }

    void dux_render_end(){
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void dux_dinit(){
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
    }
}