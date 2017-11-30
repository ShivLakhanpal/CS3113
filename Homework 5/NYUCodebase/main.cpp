
/*
 CS-UY 3113 - Intro to Game Design
 Shiv Lakhanpal
 svl238@nyu.edu
 Homework 5 - Space Invaders with Noise
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
#include <SDL_mixer.h>
#include <vector>
#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif



SDL_Window* displayWindow;


//Implemented Struct -- Vector3 and coordsText
struct Vector3 {
    
    float x;
    float y;
    float z;
    
    Vector3(float xPos, float yPos, float zPos) : x(xPos), y(yPos), z(zPos) {}
};

//Coordinates -- Texture
struct coordsText {
    
    float top;
    float down;
    float left;
    float right;

    
coordsText (float top, float down, float left, float right) : top(top), down(down), left(left), right(right) {}
};

//Entity Class
class Entity {
public:

   
    void Draw(ShaderProgram& program) {
        if (!present) return;
        Matrix modelviewMatrix;
        
        modelviewMatrix.Identity();
        modelviewMatrix.Translate(objPos.x, objPos.y, objPos.z);
        
        program.SetModelviewMatrix(modelviewMatrix);
        
        glBindTexture(GL_TEXTURE_2D, textureID);
        
        //Vertice Coordinates
        float vertices[] = { -wd, -ht, wd, -ht, wd, ht, -wd, -ht, wd, ht, -wd, ht };
        
        glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
        glEnableVertexAttribArray(program.positionAttribute);
        
        //Texture Coordinates
        float texCoords[] =
        {cT.left, cT.down, cT.right, cT.down, cT.right, cT.top, cT.left, cT.down, cT.right, cT.top, cT.left, cT.top};
        
        glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
        glEnableVertexAttribArray(program.texCoordAttribute);
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    
    //Update
    void UpdateObjects(float elapsed)
    {
        objPos.x += velocity * cos(direction*3.141592/180) * elapsed;
        objPos.y += velocity * sin(direction*3.141592/180) * elapsed;
    }
    //Collision/Render
    bool ObjectCollision(const Entity& object)
    {
        if (!object.present || !present)
            return false;
        else if (!(objPos.x + wd <= object.objPos.x - object.wd || objPos.x - wd >= object.objPos.x + object.wd || objPos.y + ht <= object.objPos.y - object.ht || objPos.y - ht >= object.objPos.y + object.ht)){
            return true;
}
        else
            return false;
    }
    
    Vector3 objPos;
    GLuint textureID;
    

    //Speed and Direction
    float velocity = 0;
    float direction = 0;
    coordsText cT;
    //If Present
    bool present = true;
    //Size and Ratio
    float size;
    float ratio;
    //Measurements
    float wd;
    float ht;
    //Entity -- What objects will refer to
    Entity(GLuint textureID = 0.0f, float size = 1.0f, float ratio = 1.0f, Vector3 objPos = Vector3(0.0f,0.0f,0.0f),coordsText font = coordsText(0.0f, 0.0f, 0.0f, 0.0f)): textureID(textureID), size(size), ratio(ratio), objPos(objPos), cT(font)
    
    //Wd and Ht Measurements
    {wd = size*ratio*0.5f;ht = size*0.5f;}
    
};


//Monospaced Font Rendering -- From Slides 9/27
void DrawText(ShaderProgram *program, int fontTexture, std::string text, float size, float spacing, float x, float y) {
    float texture_size = 1.0 / 16.0f;
    std::vector<float> vertexData;
    std::vector<float> texCoordData;
    
    for (int i = 0; i < text.size(); i++) {
        int spriteIndex = (int)text[i];
        float texture_x = (float)(spriteIndex % 16) / 16.0f;
        float texture_y = (float)(spriteIndex / 16) / 16.0f;
        
        vertexData.insert(vertexData.end(), {
            ((size + spacing) * i) + (-0.5f * size), 0.5f * size,
            ((size + spacing) * i) + (-0.5f * size), -0.5f * size,
            ((size + spacing) * i) + (0.5f * size), 0.5f * size,
            ((size + spacing) * i) + (0.5f * size), -0.5f * size,
            ((size + spacing) * i) + (0.5f * size), 0.5f * size,
            ((size + spacing) * i) + (-0.5f * size), -0.5f * size,
        });
        texCoordData.insert(texCoordData.end(), {
            texture_x, texture_y,
            texture_x, texture_y + texture_size,
            texture_x + texture_size, texture_y,
            texture_x + texture_size, texture_y + texture_size,
            texture_x + texture_size, texture_y,
            texture_x, texture_y + texture_size,
        });
    }
    glBindTexture(GL_TEXTURE_2D, fontTexture);
    
    Matrix modelviewMatrix;
    
    modelviewMatrix.Identity();
    modelviewMatrix.Translate(x, y, 0);
    
    program->SetModelviewMatrix(modelviewMatrix);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data());
    glEnableVertexAttribArray(program->texCoordAttribute);
    glDrawArrays(GL_TRIANGLES, 0, text.size()*6);
}



//Load Texture -- Load image/If image is not shown

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


//STATES
enum GameState {TITLE_SCREEN, GAME_LEVEL};

int main(int argc, char *argv[])
{

    
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Homework 5: Space Invaders with Noise - Shiv Lakhanpal", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
    glewInit();
#endif
    

    //Pre-loop Setup
    glViewport(0, 0, 640, 360);
    ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
    //GLint backgroundTexture = LoadTexture(RESOURCE_FOLDER"whitebg.png"); //Third Picture -- Ball
    //GLuint fullSpriteSheet = LoadTexture(RESOURCE_FOLDER"spriteSheetSpaceInvaders.png");
    GLuint SpriteShooter = LoadTexture(RESOURCE_FOLDER"sprite_shooter.png");
    GLuint SpriteAlien = LoadTexture(RESOURCE_FOLDER"sprite_alien.png");
    GLuint SpriteAmmo = LoadTexture(RESOURCE_FOLDER"sprite_ammo.png");
    GLuint fontMonoSpacedSheet = LoadTexture(RESOURCE_FOLDER"text2.png");  //16,16  512,512
    
    Matrix projectionMatrix;
    projectionMatrix.SetOrthoProjection(-3.55f, 3.55f, -2.0f, 2.0f, -1.0f, 1.0f);
    Matrix modelviewMatrix;
    
    
    //Shooter -- Player that User is controlling
    Entity Shooter(SpriteShooter, 0.6f, 1.1084f, Vector3(2.5f, -1.5f, 0.0f),  coordsText(0.0f,0.8679f,0.0f, 1.025f));
    
    //Aliens -- Shooter must destroy them
    Entity Aliens[4][6];
    for (int a = 0; a < 4; a++){
        for (int b = 0; b < 6; b++){
            Aliens[a][b] = Entity(SpriteAlien, 0.4f, 1.1084, Vector3(-2.5f+b, 0.5f+a*0.5f, 1.0f), coordsText(0.0, 0.8679f, 0.0, 1.0f));
            Aliens[a][b].velocity = 1.0f;
            Aliens[a][b].direction = 180.0f;
        }
    }
    
    //Bullets -- What Shooter is using to destroy Aliens
    Entity Ammo[5];
    for (int i = 0; i < 5; i++)
    {
        Ammo[i] = Entity(SpriteAmmo, 0.2f, 0.2321f, Vector3(5.0f, 5.0f, 0.0f), coordsText(0.8773f, 1.0f, 0.0f, 0.3522));
    }


    GameState begin = TITLE_SCREEN;
    
    glUseProgram(program.programID);
    
    
    float lastFrameTicks = 0.0f;
    float initialBullet = 0;
    int tracking = 0;
    bool playing = false;
    SDL_Event event;
    bool done = false;
    
    //Default Format
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    
    //Loading Music
    Mix_Music *music;
    music = Mix_LoadMUS("MainMusic.wav");
    Mix_PlayMusic(music,-1);
    
    //Hit Detection Sound
    Mix_Chunk *EnemyHit;
    EnemyHit = Mix_LoadWAV("EnemyHit.wav");
    
    //Shooting Sound
    Mix_Chunk *ShootingNoise;
    ShootingNoise = Mix_LoadWAV("ShootingNoise.wav");
    
    while (!done) {
        
        
        float ticks = (float)SDL_GetTicks() / 1000.0f;
        float elapsed = ticks - lastFrameTicks;
        lastFrameTicks = ticks;
        initialBullet += elapsed;
        
        
        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        
        while (SDL_PollEvent(&event)) {
            
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                
                done = true;
                
            }
            
            //SPACEBAR TO BEGIN GAME
            if (begin == TITLE_SCREEN && keys[SDL_SCANCODE_SPACE]){
                begin = GAME_LEVEL;
                std::cout << "You have begun attacking the ALIENS! Do your best to Destroy Them!\n";
            }
            
            else if
                //If game is at Main Level, Spacebar is pressed, and initial bullet has been shot
                (begin == GAME_LEVEL && keys[SDL_SCANCODE_SPACE] && initialBullet >= 0.5){
                tracking++;
                
                    Mix_PlayChannel(-1, ShootingNoise, 0);
                    while (tracking >= 5.0){
                    tracking = 0.0f;
                        
                }
                
                Ammo[tracking].present = true;
                    

                //Shot Measurements
                Ammo[tracking].objPos.x = Shooter.objPos.x;
                Ammo[tracking].objPos.y = Shooter.objPos.y + Shooter.ht;
                //Shot Speed and Direction
                Ammo[tracking].velocity = 1.5f;
                Ammo[tracking].direction = 90.0f;
                
            }
            
        }
        glClear(GL_COLOR_BUFFER_BIT);
        
        program.SetProjectionMatrix(projectionMatrix);
        program.SetModelviewMatrix(modelviewMatrix);
        
        //Screen is at MAIN MENU
        if (begin == TITLE_SCREEN)
        {
            DrawText(&program, fontMonoSpacedSheet, "WELCOME TO SPACE INVADERS", 0.33f, -0.05f, -3.3f, 1.0f);
            DrawText(&program, fontMonoSpacedSheet, "Press Space to Start", 0.3f, 0.0f, -3.0f, 0.0f);
            DrawText(&program, fontMonoSpacedSheet, "Instructions:A=LEFT,D=RIGHT,SPACEBAR=SHOOT", 0.21f, -0.05f, -3.4f, -0.75f);
            DrawText(&program, fontMonoSpacedSheet, "HAVE FUN AND BE SAFE! This is you-->", 0.2f, -0.05f, -3.4f, -1.5f);
            
        }
        
        //If the Game has begun and is in Main Level
        else if (begin == GAME_LEVEL){
            //DRAW ALIENS AND UPDATE MOVEMENT
            for (int i = 0; i < 3; i++){
                for (int j = 0; j < 6; j++){
                    Aliens[i][j].UpdateObjects(elapsed);
                    Aliens[i][j].Draw(program);
                }
            }
        }
            //If A is pressed and shooter's position is greater than 5
            if (keys[SDL_SCANCODE_A] && Shooter.objPos.x >= -5){
                Shooter.objPos.x -= 1.5 * elapsed;
                
            }
            
            //If D is pressed and shooter's position is less than 5
            else if (keys[SDL_SCANCODE_D] && Shooter.objPos.x <= 5){
                Shooter.objPos.x += 1.5 * elapsed;
            }
            
            Shooter.Draw(program);
            //Continues to shoot ammo
            for (int i = 0; i < 5; i++){
                Ammo[i].UpdateObjects(elapsed);
                Ammo[i].Draw(program);
            }
            
            
          //MOVEMENT
    float alienMovement = 0.0f;
            
            for (int a = 0; a < 3; a++){
                for (int b = 0; b < 6; b++){
                    for (int k = 0; k < 6; k++)
                    {   //Ammo Collides with ALIENS
                        if (Aliens[a][b].ObjectCollision(Ammo[k])){
                            Mix_PlayChannel(-1, EnemyHit, 0);
                            std::cout << "GOOD SHOT!\n";
                            Aliens[a][b].present = false;
                            Ammo[k].present = false;
                        }
                        
                        
                    
                    }
                    
                    //Aliens are Present
                    if (Aliens[a][b].present == true)
                        playing = true;
                    
                    //ALIEN MOVEMENT
                    if (Aliens[0][0].objPos.x <= -3.1f) //sets flag for when to move right
                        alienMovement = 2.0f;
                    //std::cout << "OH NO THEY ARE GETTING CLOSER!\n";
                    if (Aliens[0][5].objPos.x >= 3.1f) //sets flag for when to move left
                        alienMovement = 1.0f;
                    //Move to the left
                    if (alienMovement == 2.0f){
                        //std::cout << "OH NO THEY ARE GETTING CLOSER!\n";
                        Aliens[a][b].direction = 0.0f;
                        //Go down 0.03
                        Aliens[a][b].objPos.y -= 0.03f;
                    }
                    //Move to the right
                    if (alienMovement == 1.0f){
                        //std::cout << "OH NO THEY ARE GETTING CLOSER!\n";
                        Aliens[a][b].direction = 180.0f;
                        //Go down 0.03
                        Aliens[a][b].objPos.y -= 0.03f;
                    }
                    

                }


            }
        
        

        
        glDisableVertexAttribArray(program.positionAttribute);
        glDisableVertexAttribArray(program.texCoordAttribute);
        
        SDL_GL_SwapWindow(displayWindow);
    }
    
    //Cleaning Up
    Mix_FreeChunk(ShootingNoise);
    Mix_FreeMusic(music);
    
    SDL_Quit();
    return 0;
}