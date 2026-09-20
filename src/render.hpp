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
    GLuint getProgram() const{
    return ID;
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

void render::draw2D::shader::saveProgramBinary(GLuint program, const std::string& filename){
    GLint numFormats = 0;
    glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &numFormats);

    std::cout << "Program binary formats: "<< numFormats << std::endl;

    if (numFormats == 0) {
        std::cout << "Program binaries are NOT supported!" << std::endl;
        return;
    }

    GLint linked = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);

    if (!linked) {
        std::cout << "Cannot save binary: program is not linked!"<< std::endl;
        return;
    }

    GLint length = 0;
    glGetProgramiv(program, GL_PROGRAM_BINARY_LENGTH, &length);

    std::cout << "Binary length: "<< length << std::endl;

    if (length <= 0) {
        std::cout << "Invalid binary length!" << std::endl;
        return;
    }

    std::vector<GLubyte> binary(length);

    GLenum format = 0;

    glGetProgramBinary(
        program,
        length,
        nullptr,
        &format,
        binary.data()
    );

    GLenum error = glGetError();

    if (error != GL_NO_ERROR) {
        std::cout << "glGetProgramBinary error: 0x"<< std::hex << error << std::dec << std::endl;
        return;
    }

    std::ofstream out(filename, std::ios::binary);

    if (!out) {
        std::cout << "Could not open cache file!" << std::endl;
        return;
    }

    out.write(
        reinterpret_cast<char*>(&format),
        sizeof(format)
    );

    out.write(
        reinterpret_cast<char*>(binary.data()),
        length
    );

    std::cout << "Shader binary saved. Format: 0x" << std::hex << format << std::dec << std::endl;
}

GLuint render::draw2D::shader::loadProgramBinary(const std::string& filename){

    ID = glCreateProgram();

    std::ifstream in(filename, std::ios::binary);
    if (!in) {
        std::cout << "Cache file does not exist." << std::endl;
        return 0;
    }
    GLenum format = 0;

    in.read(
        reinterpret_cast<char*>(&format),
        sizeof(format)
    );

    if (!in) {
        std::cout << "Failed to read binary format." << std::endl;
        return 0;
    }

    auto first = std::istreambuf_iterator<char>{in};
    auto last = std::istreambuf_iterator<char>{};

    std::vector<GLubyte> binary(first,last);

    if (binary.empty()) {
        std::cout << "Binary cache is empty."<< std::endl;
        return 0;
    }

    std::cout << "Loading binary..."
              << "\nFormat: 0x"
              << std::hex << format
              << std::dec
              << "\nSize: "
              << binary.size()
              << std::endl;

    glProgramBinary(
        ID,
        format,
        binary.data(),
        static_cast<GLsizei>(binary.size())
    );

    GLenum error = glGetError();

    if (error != GL_NO_ERROR) {
        std::cout << "glProgramBinary error: 0x"<< std::hex << error << std::dec << std::endl;

        glDeleteProgram(ID);
        return 0;
    }

    GLint linked = GL_FALSE;

    glGetProgramiv(
        ID,
        GL_LINK_STATUS,
        &linked
    );

    if (!linked) {

        char log[2048];
        GLsizei logLength = 0;

        glGetProgramInfoLog(
            ID,
            sizeof(log),
            &logLength,
            log
        );

        std::cout << "Program binary rejected:\n" << log << std::endl;

        glDeleteProgram(ID);
        return 0;
    }

    std::cout << "Program binary loaded successfully!" << std::endl;

    return ID;
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