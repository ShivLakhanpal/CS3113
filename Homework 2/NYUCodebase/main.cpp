
/*
 CS-UY 3113 - Intro to Game Design
 Shiv Lakhanpal
 svl238@nyu.edu
 Homework 2 - Pong
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
    displayWindow = SDL_CreateWindow("Homework 2: Pong - Shiv Lakhanpal", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
    glewInit();
#endif
    
    
    //Preloop Setup
    
    glViewport(0, 0, 640, 360);
    ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
    
    GLint lpTexture = LoadTexture(RESOURCE_FOLDER"paddle2.png"); //First Picture -- LP
    GLint rpTexture = LoadTexture(RESOURCE_FOLDER"paddle2.png"); //Second Picture -- RP
    GLint ballTexture = LoadTexture(RESOURCE_FOLDER"ball2.png"); //Third Picture -- Ball
    GLint backgroundTexture = LoadTexture(RESOURCE_FOLDER"whitebg.png"); //Third Picture -- Ball
    GLint middleTexture = LoadTexture(RESOURCE_FOLDER"Middle.png"); //Fourth Picture -- Middle

    Matrix projectionMatrix;
    projectionMatrix.SetOrthoProjection(-3.55f, 3.55f, -2.0f, 2.0f, -1.0f, 1.0f);
    Matrix modelViewMatrix;
    Matrix left_paddle_matrix;
    Matrix right_paddle_matrix;
    Matrix ball_matrix;
    Matrix middle_matrix;
    

    glEnable(GL_BLEND);
    
    float lastFrameTicks = 0;
    //Boolean for top and right side of screen
    bool top = true;
    bool right = true;
    
    //Ball values
    float xPos_ball = 0.0f;
    float yPos_ball = 0.0f;
    float distance = 0.15f;
    float elapsed = 0.0f;
    float dirX_ball = 0.0f;
    float dirY_ball = 0.0f;
    
    //Paddle values
    float leftPaddle_Y = 0.0f;
    float rightPaddle_Y = 0.0f;
    

    SDL_Event event;
    bool done = false;
    //bool start_game = false;
    
    
    

    ball_matrix.Translate(dirX_ball, dirY_ball, 0);
    glUseProgram(program.programID);
    
    
    //float lastFrameTicks = 0.0;


    
    while (!done) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                done = true;
            }
            
        }
        
        
        
        program.SetModelviewMatrix(left_paddle_matrix);
        program.SetProjectionMatrix(projectionMatrix);
        
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glClear(GL_COLOR_BUFFER_BIT);
        
        
        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        
        //Left
        
        if(keys[SDL_SCANCODE_W]){
            if (leftPaddle_Y < 2.0f){
                
                leftPaddle_Y += distance;
                left_paddle_matrix.Translate(0.0f, distance, 0.0f);
                
                
                }
        }
        
        
        if(keys[SDL_SCANCODE_S]){
            if (leftPaddle_Y > -2.0f){
                
                leftPaddle_Y -= distance;
                left_paddle_matrix.Translate(0.0f, -distance, 0.0f);
                
            }
        }
        
        //Right
        
        
        if(keys[SDL_SCANCODE_UP]){
            if (rightPaddle_Y < 2.0f){
                
                rightPaddle_Y += distance;
                right_paddle_matrix.Translate(0.0f, distance, 0.0f);
                
            }
        }
        
        if(keys[SDL_SCANCODE_DOWN]){
            if (rightPaddle_Y > -2.0f){
                
                rightPaddle_Y -= distance;
                right_paddle_matrix.Translate(0.0f, -distance, 0.0f);
                
            }
        }
        
        float ticks = (float)SDL_GetTicks() / 1000.0f;
        elapsed = ticks - lastFrameTicks;
        lastFrameTicks = ticks;
        
        glUseProgram(program.programID);
        
        program.SetProjectionMatrix(projectionMatrix);
        program.SetModelviewMatrix(modelViewMatrix);
        
        //Left - Paddle Texture
        
        float texCoords[] = {0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f};
        
        program.SetModelviewMatrix(left_paddle_matrix);
        glBindTexture(GL_TEXTURE_2D, lpTexture);
        
        float leftVertices[] = { -3.5f, -0.5f, -3.4f, -0.5f, -3.4f, 0.5f, -3.4f, 0.5f, -3.5f, 0.5f, -3.5f, -0.5f  };
        glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, leftVertices);
        glEnableVertexAttribArray(program.positionAttribute);
        
        glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
        glEnableVertexAttribArray(program.texCoordAttribute);
        glBindTexture(GL_TEXTURE_2D, backgroundTexture);

        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        glDisableVertexAttribArray(program.positionAttribute);
        glDisableVertexAttribArray(program.texCoordAttribute);
        
        
        //Right - Paddle Texture
        
        program.SetModelviewMatrix(right_paddle_matrix);
        glBindTexture(GL_TEXTURE_2D, rpTexture);
        
        float rightVertices[] = { 3.4f, -0.5f, 3.5f, -0.5f, 3.5f, 0.5f, 3.5f, 0.5f, 3.4f, 0.5f, 3.4f, -0.5f };
        glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, rightVertices);
        glEnableVertexAttribArray(program.positionAttribute);
        
        glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
        glEnableVertexAttribArray(program.texCoordAttribute);
        glBindTexture(GL_TEXTURE_2D, backgroundTexture);

        
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        glDisableVertexAttribArray(program.positionAttribute);
        glDisableVertexAttribArray(program.texCoordAttribute);
        
        
        //Ball Texture
        
        program.SetModelviewMatrix(ball_matrix);
        glBindTexture(GL_TEXTURE_2D, ballTexture);
        
        float ballVertices[] = { -0.1f, -0.1f, 0.1f, -0.1f, 0.1f, 0.1f, 0.1f, 0.1f, -0.1f, 0.1f, -0.1f, -0.1f };
        glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, ballVertices);
        glEnableVertexAttribArray(program.positionAttribute);
        
        glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
        glEnableVertexAttribArray(program.texCoordAttribute);
        glBindTexture(GL_TEXTURE_2D, backgroundTexture);

        
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        glDisableVertexAttribArray(program.positionAttribute);
        glDisableVertexAttribArray(program.texCoordAttribute);
        
        dirX_ball = elapsed*1.5;
        dirY_ball = elapsed*1.5;
        
        
        //Middle Texture
        
        program.SetModelviewMatrix(middle_matrix);
        glBindTexture(GL_TEXTURE_2D, middleTexture);
        
        float middleVertices[] = { -0.15f, -2.05f, 0.15f, -2.05f, 0.15f, 2.05f, -0.15f, -2.05f, 0.15f, 2.05f, -0.15f, 2.05f };
        glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, middleVertices);
        glEnableVertexAttribArray(program.positionAttribute);
        
        glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
        glEnableVertexAttribArray(program.texCoordAttribute);
        
        
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        glDisableVertexAttribArray(program.positionAttribute);
        glDisableVertexAttribArray(program.texCoordAttribute);
        
        
        //Outcomes - Direction of ball
        
                 
        if (top == false) {
            dirY_ball *= -1.5f;
        }
    if (right == false) {
            dirX_ball *= -1.5f;
        }
        
        if (yPos_ball >= 1.5f) {
            dirY_ball *= -1.5f;
            top = false;
        }
        
        else if (yPos_ball <= -1.5f) {
            dirY_ball *= -1.5f;
            top = true;
        }
        
        
        //Paddle and Ball Collision Detection
        float right_sideT = rightPaddle_Y + 1.0f;
        float right_sideL = rightPaddle_Y - 1.0f;
        float left_sideT = leftPaddle_Y + 1.0f;
        float left_sideL = leftPaddle_Y - 1.0f;
        
        //If ball hits right paddle
        if (xPos_ball >= 3.2f) {
            if (yPos_ball <= right_sideT && yPos_ball >= right_sideL) {
                dirX_ball *= -1.0f;
                right = false;
                            std::cout << "Right Player Has Counter-Attacked!\n";
            }
        }
        
        //If ball hits left paddle
        else if (xPos_ball <= -3.2f) {
            if (yPos_ball <= left_sideT && yPos_ball >= left_sideL) {
                dirX_ball *= -1.0f;
                right = true;
                std::cout << "Left Player has Counter-Attacked!\n";
            }
            
        }
        
        if (xPos_ball >= 3.5f || xPos_ball <= -3.5f) {
            done = true;
        }
        
        ball_matrix.Translate(dirX_ball, dirY_ball, 0.0f);
        xPos_ball += dirX_ball;
        yPos_ball += dirY_ball;
        
        SDL_GL_SwapWindow(displayWindow);
        
    }
    
    SDL_Quit();
    return 0;
}