#ifndef CORE_HPP
#define CORE_HPP

#include <string>
#include <glad.h>

#include <SDL3/SDL.h>      //include primary graphics api library
#include <SDL3/SDL_main.h> //include standard graphics api library

#include <SDL3_ttf/SDL_ttf.h>

class core{

    private:
    
    bool win_is_open = true; 

    static int screen_height;        //window height
    static int screen_width;         //window width  

    static std::string window_title;

    Uint32 window_flags;

    public:
    SDL_Window* win;
    SDL_GLContext gl_context;

    core();                    //initiate SDL context
    ~core();                   //destroy SDL context data

   std::string window_get_title(std::string title);            //get window title

   int window_get_resolution(int x,int y);               //get window resolution
   void window_get_flags(Uint32 flags);            //get window flag 
   void create_and_display_window();

   void load_GPU_tools();

   void render();                                        //render the scene according to updated info 

   bool window_isopen();                                   //command to start window
   bool window_quit();                                     //command to denied permission to run a window
   bool is_window_min();                                   //check if window is minimized 
    
};

int core::screen_height = 0;
int core::screen_width = 0;

std::string core::window_title = "";

core::core(){
    
    win = nullptr;
    gl_context = nullptr;

    if(!SDL_Init(SDL_INIT_VIDEO)){
        std::cout << "SDL INIT FAILED PLEASE CONSIDER DOUBLE CHECK [WARNING - 1] " << SDL_GetError() << std::endl;
    }else{
        std::cout << "SDL INIT SUCCESSFUL [COPY - 1]" << std::endl;
    }


    //setting up opengl version and profile

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

}

void core::load_GPU_tools(){

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)){
    std::cout << "GLAD FAILED TO INTIALIZE [WARNING - 4]" << std::endl;
    }else{
        std::cout << "GLAD INITIALIZATION SUCCESSFUL [COPY - 4]" << std::endl;
    }

    std::cout << "OpenGL: " << glGetString(GL_VERSION) << std::endl;

}

std::string core::window_get_title(std::string title){

   window_title = title;

   return this->window_title; 

}

int core::window_get_resolution(int x,int y){

    screen_height = x;
    screen_width  = y;

    return this->screen_height,this->screen_width;

}

bool core::window_isopen(){
    return win_is_open;
}

bool core::window_quit(){

    win_is_open = false;
    return win_is_open;

}

bool core::is_window_min(){

    return (SDL_GetWindowFlags(win) & SDL_WINDOW_MINIMIZED) != 0;

}

void core::create_and_display_window(){

    std::cout<< "creating a "<< this->screen_height <<" X "<<this->screen_width<<" window named "<< window_title.c_str() <<std::endl;
    
    win = SDL_CreateWindow(window_title.c_str(),
    screen_width,screen_height,
    window_flags); 

   if(win == nullptr){
        std::cout << "WINDOW CREATION FAILED [WARNING - 2] " << SDL_GetError() << std::endl;
   } else{
        std::cout << "WINDOW CREATION SUCCESSFUL [COPY - 2]" << std::endl;
   }

   gl_context = SDL_GL_CreateContext(win);


   if (gl_context == nullptr){
        std::cout << "OPENGL CONTEXT CREATION FAILED [WARNING - 3] " << SDL_GetError() << std::endl;
    }else{
        std::cout << "OPENGL CONTEXT CREATION SUCCESSFUL [COPY - 3]" << std::endl;
    }

    SDL_GL_MakeCurrent(win,gl_context);
    
}

void core::window_get_flags(Uint32 flags){
    window_flags = flags;
}

void core::render(){
    
    glClearColor(1.0f/2, 1.0f/2, 1.0f/2, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

}

core::~core(){

    SDL_DestroyWindow(win);
    SDL_GL_DestroyContext(gl_context);

    SDL_Quit();
}

#endif //CORE_HPP