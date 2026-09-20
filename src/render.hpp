#pragma once

//#include <SDL3_ttf/SDL_ttf>

#include<glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

namespace render{

    namespace draw2D{

    class shader{
    public:
    GLuint vbo;
    GLuint vao;  

    void init(std::string &vertsh, std::string &fragsh);
    //initiate , compile and link shader

    void use(){
        glUseProgram(ID);
    };
    void setBool(const std::string &name, bool value) const{         
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value); 
    };

    void setInt(const std::string &name, int value) const{ 
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value); 
    };

    void setFloat(const std::string &name, float value) const{ 
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value); 
    };

    void cleanup(){
        glDeleteProgram(ID);
    };

    private:

        unsigned int ID;

        void checkCompileErrors(unsigned int shader,const std::string& type);
    };

};



    namespace draw3D{

    };
}

void render::draw2D::shader::init(std::string &vertsh, std::string &fragsh){

    //open files
    std::ifstream vert(vertsh);
    std::ifstream frag(fragsh);  
    
    if (!vert)
    {
        std::cout<<"can not open file" << vertsh <<std::endl;
    }
    
    if (!frag)
    {
        std::cout<<"can not open file" << fragsh <<std::endl;
    }
    
    // ensure ifstream objects can throw exceptions:
    vert.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    frag.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    try 
    {
        std::stringstream vShaderStream, fShaderStream;

        // read file's buffer contents into streams
        vShaderStream << vert.rdbuf();
        fShaderStream << frag.rdbuf();	

        // close file handlers
        vert.close();
        frag.close();

        // convert stream into string
        vertsh   = vShaderStream.str();
        fragsh = fShaderStream.str();	

    }
    catch(std::ifstream::failure e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }

    const char* vShaderCode = vertsh.c_str();
    const char* fShaderCode = fragsh.c_str();

    // 2. compile shaders
    GLuint vertex, fragment;

    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");

    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");

    // shader Program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");

    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);    
}

void render::draw2D::shader::checkCompileErrors(unsigned int shader,const std::string& type){

        int success;
        char infoLog[1024];

        if (type != "PROGRAM")
        {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

        if (!success){
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success){
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << std::endl;
            }
        }
}