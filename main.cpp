#include <iostream>
#include <string>

#include "src/core.hpp"
#include "src/input.hpp"
#include "src/render.hpp"
#include "src/dev_tool.hpp"
#include "src/settings.hpp"

std::string VERTEX_SHADER = "shader/vert.sh";
std::string FRAGMENT_SHADER = "shader/frag.sh";

int main(int argc, char* argv[]){

    render::draw2D::shader sh; 
    core g_core;

    input::action kin;
    cfg::Settings engs;

    engs.load("settings.toml");

    int height = engs.get<int>("window.height");
    int width = engs.get<int>("window.width");

    static bool vsync = 0;

    std::string title = engs.get<std::string>("window.title","HELLO WINDOW");

    g_core.window_get_title(title);
    g_core.window_get_resolution(height,width);
    g_core.window_get_flags(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    //g_core.load_font("font/IBMPlexMono-Regular,ttf", 14 );
    g_core.create_and_display_window();
    g_core.load_GPU_tools();

    float vertices[] = {
         0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
         0.0f,  0.5f, 0.0f   // top 
    };

    //some stupid vao,vbo setup
    glGenVertexArrays(1, &sh.vao);
    glGenBuffers(1, &sh.vbo);

    glBindVertexArray(sh.vao);
    glBindBuffer(GL_ARRAY_BUFFER, sh.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) , vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(sh.vao);

    GLint program = 0;
    std::string cache = "cache/shader.cache";

    bool changed = sh.shaderChanged(cache,VERTEX_SHADER,FRAGMENT_SHADER);

    std::cout << "Shader changed: " << changed << std::endl;

    program = sh.loadProgramBinary(cache);

    std::cout << "Loaded program: " << program << std::endl;

    if (!changed && program != 0) {

    std::cout << "SHADER LOADED FROM CACHE SUCCESSFULLY" << std::endl;

    } else {

    sh.init(VERTEX_SHADER, FRAGMENT_SHADER);
    program = sh.getProgram();                   //gets shader program id 
    sh.saveProgramBinary(program, cache);

    std::cout << "Shader cache created successfully" << std::endl;
    }

    dev_tool::dux_init(g_core.win,g_core.gl_context);

    uint64_t last_time = SDL_GetTicks();

    float dt = 0.00f;

    while(g_core.window_isopen()){  

        uint64_t now = SDL_GetTicks();

        dt = (now - last_time)/1000.0f;
        last_time = now;

        if (!g_core.is_window_min())
        {

        //1.keyboard input 
        kin.get_key_input(g_core);

        input::move_state ms = kin.get_input();                //movestate is being updated by get input function                                

        if (ms.forward)  std::cout << "forward key is pressed"  << std::endl;
        if (ms.backward) std::cout << "backward key is pressed" << std::endl;
        if (ms.left)     std::cout << "left key is pressed"     << std::endl;
        if (ms.right)    std::cout << "right key is pressed"    << std::endl;

        //2.start rendering debug window
        dev_tool::dux_render();

        //3. dev tool windows
        dev_tool::enable_vsync(vsync);
        dev_tool::display_stat(dt);  

        //4.data is updating
        

        //5.engine rendering
        sh.use();         //compile shader and use
        g_core.render(); 
        // render the triangle
        glDrawArrays(GL_TRIANGLES, 0, 3);   

        dev_tool::dux_render_end();

        SDL_GL_SwapWindow(g_core.win);                  //just clear buffer every frame

        //std::cout << "dt = "<< dt << "FPS = " << 1.00f/dt << std::flush;

        }

    if (!vsync){
        
        float delay = 25.0f - dt * 1000.0f;
        if (delay > 0.0f)
            SDL_Delay(static_cast<Uint32>(delay));                 //temporary capping to 40 fps
    }                                            
    }

    glDeleteVertexArrays(1, &sh.vao);
    glDeleteBuffers(1, &sh.vbo);

    //glViewport(0,0,width,height);

    sh.cleanup();

    dev_tool::dux_dinit();

    return 0;
}