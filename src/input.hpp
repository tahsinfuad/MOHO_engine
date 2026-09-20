#ifndef INPUT_HPP
#define INPUT_HPP

#include "src/core.hpp"

//#include "src/physics.hpp"
//#include "src/ux.hpp"

#include "imgui-master/imgui.h"
#include "imgui-master/imgui_impl_sdl3.h"
#include "imgui-master/imgui_impl_opengl3.h"

namespace input{

    struct move_state {
        bool forward  = false;
        bool backward = false;
        bool left     = false;
        bool right    = false;
    };

    class action{

    public:

    void get_key_input(core &c);           //get keyboard input 
    void get_mouse_input();                //get mouse input
    
    move_state get_input();       //this function will return bool value and user will get the bool value to define action using conditions

    private:

    SDL_Event k_event,m_event;

    };

    enum keymapping{

        //movement
        forward = SDL_SCANCODE_W,
        backward = SDL_SCANCODE_S,
        left = SDL_SCANCODE_A,
        right = SDL_SCANCODE_D,

        up_arrow = SDL_SCANCODE_UP,
        down_arrow = SDL_SCANCODE_DOWN,
        left_arrow = SDL_SCANCODE_LEFT,
        right_arrow = SDL_SCANCODE_RIGHT,

        //control
        pick = SDL_SCANCODE_F,
        drop = SDL_SCANCODE_G,
        jump = SDL_SCANCODE_PAUSE
    };
}

void input::action::get_key_input(core &c){

    while(SDL_PollEvent(&this->k_event)){

    ImGui_ImplSDL3_ProcessEvent(&this->k_event);

        if(k_event.type == SDL_EVENT_QUIT){
            std::cout << "WINDOW CLOSE EVENT REQUESTED" << std::endl;
            c.window_quit();
            std::cout << "WINDOW CLOSED SUCCESSFULLY" << std::endl;
        }
    }     
}

input::move_state input::action::get_input(){

    const bool* state = SDL_GetKeyboardState(nullptr);

    move_state m;

    m.forward  = state[input::keymapping::forward];
    m.backward = state[input::keymapping::backward];
    m.left     = state[input::keymapping::left];
    m.right    = state[input::keymapping::right];

    return m;
}

#endif //INPUT_HPP