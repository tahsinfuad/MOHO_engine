#pragma once

//#include <SDL3_ttf/SDL_ttf>

#include<glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <filesystem>

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

    void saveProgramBinary(GLuint program, const std::string& filename);
    GLuint loadProgramBinary(const std::string& filename);
    //load shader cache

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
    bool shaderChanged(const std::string& cacheFile, const std::string& vert, const std::string& frag) {

    if (!std::filesystem::exists(cacheFile)) return true; // no cache yet

    auto cacheTime = std::filesystem::last_write_time(cacheFile);
    if (std::filesystem::last_write_time(vert) > cacheTime) return true;
    if (std::filesystem::last_write_time(frag) > cacheTime) return true;
    return false;
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

    //check if gpu is compatible with my advance feature
    GLint numFormats = 0;
    glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &numFormats);

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

void render::draw2D::shader::saveProgramBinary(GLuint program, const std::string& filename) {

    GLint length = 0;
    glGetProgramiv(program, GL_PROGRAM_BINARY_LENGTH, &length);

    std::vector<GLubyte> binary(length);
    GLenum format = 0;
    glGetProgramBinary(program, length, nullptr, &format, binary.data());

    std::ofstream out(filename, std::ios::binary);
    out.write(reinterpret_cast<char*>(&format), sizeof(format)); // format first!
    out.write(reinterpret_cast<char*>(binary.data()), length);
}

GLuint render::draw2D::shader::loadProgramBinary(const std::string& filename){

    std::ifstream in(filename, std::ios::binary);
    if (!in) return 0; // no cache

    GLenum format;
    in.read(reinterpret_cast<char*>(&format), sizeof(format));

    std::vector<GLubyte> binary((std::istreambuf_iterator<char>(in)),
    std::istreambuf_iterator<char>());
    if (binary.empty()) return 0;

    GLuint program = glCreateProgram();
    glProgramBinary(program, format, binary.data(), binary.size());

    GLint linked = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked) {
        glDeleteProgram(program);
        return 0; 
    }
    return program;
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