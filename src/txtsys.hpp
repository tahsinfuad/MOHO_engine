#pragma once

#include <SDL3_ttf/SDL_ttf.h>
#include <glad.h>

namespace txtsys{

    class text{
        private:

        TTF_Font* f = nullptr;
        SDL_Surface *textsur = nullptr;
        GLint font_tex = 0;

        public:

        text(){
        if (!TTF_Init()) {
        std::cerr << "TTF_Init failed: " << SDL_GetError() << std::endl;
        };
        }

        ~text(){
        if (textureID != 0) {
            glDeleteTextures(1, &font_tex);
            font_tex = 0;
        }

        TTF_CloseFont(fontL);
        TTF_Quit();
        };

        void load_font(const std::string& fontP,int size);
        void render_txt(const std::string& text, int xpos , int ypos , int height , SDL_Colour& col );
        //render text with various properties
    }
}

void load_font(const std::string& fontP,int size){

    f = TTF_OpenFont(const fontP.c_str(), size);

    if (!f) {
        std::cerr << "Font load error: " << SDL_GetError() << std::end;
    }


}
void txtsys::text::rende_text(const std::string& text, int xpos , int ypos , int height , int width , SDL_Colour& col ){

    textsur = TTF_RenderText_Solid(font, text.c_str() , &col);

    
}
