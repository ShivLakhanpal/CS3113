/*
 CS-UY 3113 - Intro to Game Design
 Shiv Lakhanpal
 svl238@nyu.edu
 Homework 1 - Simple 2D Scene
 Professor Ivan Safrin
 */


#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "ShaderProgram.h"
#include "Matrix.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif


SDL_Window* displayWindow;


GLuint LoadTexture(const char *img_path){
    int width, height, item;
    unsigned char* img = stbi_load(img_path, &width, &height, &item, STBI_rgb_alpha);
    
    if (img == NULL){
        std::cout << "Cannot show image. Please make sure the directory is the correct one!\n";
    }
    
    GLuint imgTexture;
    glGenTextures(1, &imgTexture);
    glBindTexture(GL_TEXTURE_2D, imgTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    stbi_image_free(img);
    return imgTexture;
}

int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Homework 1 - Shiv Lakhanpal", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
    glewInit();
#endif
    
    
    //Setup
    
    glViewport(0, 0, 640, 360);
    
    ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
    
    
    GLint bartTexture = LoadTexture(RESOURCE_FOLDER"bart.jpg"); //First Picture -- Homer
    GLint albertTexture = LoadTexture(RESOURCE_FOLDER"albert.png"); //Second Picture -- Einstein
    GLint ghostTexture = LoadTexture(RESOURCE_FOLDER"ghost.jpg"); //Third Picture -- Ghost
    
    
    Matrix projectionMatrix;
    
    Matrix modelViewMatrix; //Bart mVM
    modelViewMatrix.Rotate(45.0 * (3.1415926 / 180.));
    
    Matrix modelViewMatrix2; //Einstein mVM
    modelViewMatrix2.Translate(-2.0, 0.0, 0.0);
    
    Matrix modelViewMatrix3; //Ghost mVM
    modelViewMatrix3.Translate(2.0, 0.0, 0.0);
    
    projectionMatrix.SetOrthoProjection(-3.55f, 3.55f, -2.0f, 2.0f, -1.0f, 1.0f);
    
    
    
    
    glEnable(GL_BLEND);
    
    
    
    SDL_Event event;
    bool done = false;
    
    while (!done) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                done = true;
            }
        }
        
        
        glUseProgram(program.programID);
        
        program.SetModelviewMatrix(modelViewMatrix);
        program.SetProjectionMatrix(projectionMatrix);
        
        glClear(GL_COLOR_BUFFER_BIT);
        
        //Bart Simpson
        
        glBindTexture(GL_TEXTURE_2D, bartTexture);
        
        
        float bart_vertices[] = {-0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f};
        glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, bart_vertices);
        glEnableVertexAttribArray(program.positionAttribute);
        
        float bart_texCoords[] = {0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f};
        glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, bart_texCoords);
        glEnableVertexAttribArray(program.texCoordAttribute);
        
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        glDisableVertexAttribArray(program.positionAttribute);
        glDisableVertexAttribArray(program.texCoordAttribute);
        
        
        
        //Albert Einstein
        
        
        glBindTexture(GL_TEXTURE_2D, albertTexture);
        
        
        program.SetModelviewMatrix(modelViewMatrix2);
        
        
        
        float albert_vertices[] = {1.25f, 1.25f, -1.25f, 1.25f, -1.25f, -1.25f, -1.25f, -1.25f, 1.25f, -1.25f, 1.25f, 1.25f};
        glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, albert_vertices);
        glEnableVertexAttribArray(program.positionAttribute);
        
        float albert_texCoords[] = {1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f};
        glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, albert_texCoords);
        glEnableVertexAttribArray(program.texCoordAttribute);
        
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        glDisableVertexAttribArray(program.positionAttribute);
        glDisableVertexAttribArray(program.texCoordAttribute);
        
        
        
        
        //Ghost
        
        
        glBindTexture(GL_TEXTURE_2D, ghostTexture);
        
        
        program.SetModelviewMatrix(modelViewMatrix3);
        
        
        float ghost_vertices[] = {1.25f, 1.25f, -1.25f, 1.25f, -1.25f, -1.25f, -1.25f, -1.25f, 1.25f, -1.25f, 1.25f, 1.25f};
        glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, ghost_vertices);
        glEnableVertexAttribArray(program.positionAttribute);
        
        float ghost_texCoords[] = {1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f};
        glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, ghost_texCoords);
        glEnableVertexAttribArray(program.texCoordAttribute);
        
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        glDisableVertexAttribArray(program.positionAttribute);
        glDisableVertexAttribArray(program.texCoordAttribute);
        
        
        
        
        SDL_GL_SwapWindow(displayWindow);
        
    }
    
    SDL_Quit();
    return 0;
}