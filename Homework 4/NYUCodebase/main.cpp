
/*
 CS-UY 3113 - Intro to Game Design
 Shiv Lakhanpal
 svl238@nyu.edu
 Homework 4 - Simple Platform
 Professor Ivan Safrin
 */


/*EGG CATCHER -- Simple PLATFORM
 USE WASD KEYS TO MOVE
 Was unable to figure out how to complete it using tiledmap, so I used static entities instead
 
 I know it's not the best looking game, but hopefully I met all the requirements :)
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
#include <vector>
#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

SDL_Window* displayWindow;


// 60 FPS (1.0f/60.0f) (update sixty times a second)
#define FIXED_TIMESTEP 0.0166666f

//Main Sprite Sheet Class
class SheetSprite {
public:
    SheetSprite() {}
    SheetSprite(unsigned int texID, float u, float v, float width, float height, float size) : textureID(texID), u(u), v(v), width(width), height(height), size(size) {}
    
    float size;
    unsigned int textureID;
    float u;
    float v;
    float width;
    float height;
    
    
    //Draw Program
    void Draw(ShaderProgram* program) {
        
        Matrix modelviewMatrix;

        glBindTexture(GL_TEXTURE_2D, textureID);
        GLfloat texCoords[] = {
            u, v + height,
            u + width, v,
            u, v,
            u + width, v,
            u, v + height,
            u + width, v + height
        };
        
        glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
        glEnableVertexAttribArray(program->texCoordAttribute);
        
        float aspect = width / height;
        float vertices[] = {
            -0.5f * size * aspect, -0.5f * size,
            0.5f * size * aspect, 0.5f * size,
            -0.5f * size * aspect, 0.5f * size,
            0.5f * size * aspect, 0.5f * size,
            -0.5f * size * aspect, -0.5f * size ,
            0.5f * size * aspect, -0.5f * size
        };
        glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
        glEnableVertexAttribArray(program->positionAttribute);
        

        glDrawArrays(GL_TRIANGLES, 0, 6);

    }
};

//Vector Class
class Vector3 {
public:
    Vector3() {}
    Vector3(float x, float y, float z) : xPos(x), yPos(y), zPos(z) {}
    
    float xPos;
    float yPos;
    float zPos;
};

//Entity Class
enum EntityType {ENTITY_PLAYER, ENTITY_TILE, ENTITY_EGG};
class Entity {
public:
    Entity() {}
    
    Entity(float x, float y, float z, float xPosition, float yPosition, float zPosition, float xPos2, float yPos2, float zPos2, bool staticDynamic, SheetSprite mainSprite) : position(x,y,z), velocity(xPosition,yPosition,zPosition), acceleration(xPos2,yPos2,zPos2), isStatic(staticDynamic), sprite(mainSprite) {}
    
    SheetSprite sprite;
    
    Vector3 position;
    Vector3 size;
    Vector3 velocity;
    Vector3 acceleration;
    
    
    bool isStatic;
    EntityType entityType;
    
    bool collidedTop;
    bool collidedBottom;
    bool collidedLeft;
    bool collidedRight;
};

// Lotsa global game variables
std::vector<Entity> entities;

//Entity -- Object and Player
Entity hunter;
Entity egg;

//Sprite Sheets
SheetSprite mainSprite;
SheetSprite eggSprite;
SheetSprite gameSprite;

//Gravity Force
float g = 0.45f;

//
int eggCounter = 0;

GLuint LoadTexture(const char *img_path) {
    int width, height, item;
    unsigned char* img = stbi_load(img_path, &width, &height, &item, STBI_rgb_alpha);
    
    if (img == NULL) {
        std::cout << "Unable to load image. Make sure to path is correct\n";

    }
    
    GLuint imgTexture;
    glGenTextures(1, &imgTexture);
    glBindTexture(GL_TEXTURE_2D, imgTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    stbi_image_free(img);
    return imgTexture;
}

//Draw Text Function
void DrawText(ShaderProgram* program, int fontTexture, std::string text, float size, float spacing) {
    
    Matrix modelviewMatrix;
    
    float texture_size = 1.0 / 16.0f;
    std::vector<float> vertexData;
    std::vector<float> texCoordData;
    
    for (size_t i = 0; i < text.size(); i++) {
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
        
        glBindTexture(GL_TEXTURE_2D, fontTexture);
        glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
        
        texCoordData.insert(texCoordData.end(), {
            texture_x, texture_y,
            texture_x, texture_y + texture_size,
            texture_x + texture_size, texture_y,
            texture_x + texture_size, texture_y + texture_size,
            texture_x + texture_size, texture_y,
            texture_x, texture_y + texture_size,
        });
    }
    
    glUseProgram(program->programID);
    glBindTexture(GL_TEXTURE_2D, fontTexture);
    //vertexData
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
    glEnableVertexAttribArray(program->positionAttribute);
    //texCoordData
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data());
    glEnableVertexAttribArray(program->texCoordAttribute);
    //Draw Arrays
    glDrawArrays(GL_TRIANGLES, 0, text.size() * 6);
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

//Renders Gameplay and displays font
void Render(Matrix& modelviewMatrix, ShaderProgram& program) {
    
    
    // Coin text
    GLuint gameFont = LoadTexture(RESOURCE_FOLDER"text2.png");
    modelviewMatrix.Identity();
    modelviewMatrix.Scale(1.0f, 1.0f, 0.0f);
    modelviewMatrix.Translate(-3.50f, 1.9f, 0.0f);
    program.SetModelviewMatrix(modelviewMatrix);
    
    
    DrawText(&program, gameFont, "THERE ARE 5 EGGS ON 5 DIFFERENT LEVELS.", 0.15f, 0.0f);
    modelviewMatrix.Identity();
    modelviewMatrix.Scale(1.0f, 1.0f, 0.0f);
    modelviewMatrix.Translate(-3.50f, 1.7f, 0.0f);
    program.SetModelviewMatrix(modelviewMatrix);
    
    
    DrawText(&program, gameFont, "WASD to move. Move down for different levels.", 0.15f, 0.0f);
    modelviewMatrix.Identity();
    modelviewMatrix.Scale(1.0f, 1.0f, 0.0f);
    modelviewMatrix.Translate(-3.50f, 1.3f, 0.0f);
    program.SetModelviewMatrix(modelviewMatrix);
    
    
    DrawText(&program, gameFont, "COLLECT ALL THE EGGS: " + std::to_string(eggCounter) + "/5", 0.20f, 0.0f);
    
    //Puts it on the Screen
    modelviewMatrix.Identity();
    modelviewMatrix.Translate(hunter.position.xPos, hunter.position.yPos, hunter.position.zPos);
    program.SetModelviewMatrix(modelviewMatrix);
    hunter.sprite.Draw(&program);
    
    //Every Entity is Drawn and created
    for (size_t i = 0; i < entities.size(); i++) {
        modelviewMatrix.Identity();
        modelviewMatrix.Translate(entities[i].position.xPos, entities[i].position.yPos, entities[i].position.zPos);
        program.SetModelviewMatrix(modelviewMatrix);
        entities[i].sprite.Draw(&program);
    }
}

void CollidesWith(Entity& entity) {
    entity.collidedTop = false;
    entity.collidedBottom = false;
    entity.collidedLeft = false;
    entity.collidedRight = false;
}

void Update(float elapsed) {
    // Enable keyboard

    
    CollidesWith(hunter);
    
    //Collisions occuring with other objects
    for (size_t i = 0; i < entities.size(); i++) {
        if (hunter.position.xPos - mainSprite.width * 4 < entities[i].position.xPos + gameSprite.width * 2 &&
            hunter.position.xPos + mainSprite.width * 4 > entities[i].position.xPos - gameSprite.width * 2 &&
            hunter.position.yPos - mainSprite.height * 2 < entities[i].position.yPos + gameSprite.height * 2 &&
            hunter.position.yPos + mainSprite.height * 2 > entities[i].position.yPos - gameSprite.height * 2)
        {
            //These are the Static Collisions
            if (entities[i].isStatic) {
                //If hunter Yposition is greater than entities position
                if (hunter.position.yPos > entities[i].position.yPos){
                    hunter.collidedBottom = true;
                }
                //If hunter Yposition is less than entities position
                else if (hunter.position.yPos < entities[i].position.yPos){
                    hunter.collidedTop = true;
                }
                //If hunter Xposition is greater than entities position plus gamesprite width
                if (hunter.position.xPos > entities[i].position.xPos + gameSprite.width * 2){
                    hunter.collidedRight = true;
                }
                //If hunter Xposition is less than entities position plus gamesprite width
                else if (hunter.position.xPos < entities[i].position.xPos - gameSprite.width * 2){
                    hunter.collidedLeft = true;
                }
            }
            //If Hunter Grabs an egg -- Increment Egg Counter
            else if ((entities[i].entityType = ENTITY_EGG)) {
                std::swap(entities[i], entities[entities.size()-1]);
                entities.pop_back();
                eggCounter++;
            }
        }
    }
    
    
    //If collision occurs
    if ((hunter.collidedBottom && hunter.velocity.yPos < 0.0f) || (hunter.collidedTop && hunter.velocity.yPos > 0.0f))
        hunter.velocity.yPos = 0.0f;
    else {
        for (size_t i = 0; i < entities.size(); i++)
            entities[i].position.yPos -= hunter.velocity.yPos * elapsed;
        hunter.velocity.yPos -= g * elapsed;
    }

    //Activates Keyboard
    const Uint8* keys = SDL_GetKeyboardState(NULL);
    
    
    //The A key is used to move right
    if (keys[SDL_SCANCODE_A]) {
        if (!hunter.collidedRight) {
            for (size_t i = 0; i < entities.size(); i++)
                entities[i].position.xPos += hunter.velocity.xPos * elapsed;
        }
    }
    
    //The D key is used to move left
    else if (keys[SDL_SCANCODE_D]) {
        if (!hunter.collidedLeft) {
            for (size_t i = 0; i < entities.size(); i++)
                entities[i].position.xPos -= hunter.velocity.xPos * elapsed;
        }
    }
    //The W key is used to move upwards
    if (keys[SDL_SCANCODE_W] && hunter.collidedBottom == true)
        hunter.velocity.yPos = 1.0f;
    
    //The S key is used to move downwards
    else{
        if (keys[SDL_SCANCODE_S] && hunter.collidedBottom == false)
            hunter.velocity.yPos = -1.0f;
    }
}



int main(int argc, char *argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Homework 4: Simple Platform - Shiv Lakhanpal", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
    glewInit();
#endif
    
    //Default Setup
    glViewport(0, 0, 640, 360);
    
    ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
    
    Matrix projectionMatrix;
    projectionMatrix.SetOrthoProjection(-3.55f, 3.55f, -2.0f, 2.0f, -1.0f, 1.0f);
    Matrix modelviewMatrix;

    
    
    glUseProgram(program.programID);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    float lastFrameTicks = 0.0f;
    
    //LOAD THE TEXTURES
    GLuint eggSheet = LoadTexture(RESOURCE_FOLDER"gameTiles.png");
    GLuint playerTexture = LoadTexture(RESOURCE_FOLDER"hunterMain.png");
    GLuint tileTexture = LoadTexture(RESOURCE_FOLDER"gameTiles.png");
    
    
    //HUNTER
    mainSprite = SheetSprite(playerTexture, 386.0f / 1024.0f, -400.0f / 1024.0f, 151.0f / 1024.0f, 75.0f / 1024.0f, 0.2f);
    hunter = Entity(0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.5f, 0.0f, 0.0f, false, mainSprite);
    hunter.entityType = ENTITY_PLAYER;
    
    //EGG
    eggSprite = SheetSprite(eggSheet, 778.0f / 1024.0f, 557.0f / 1024.0f, 31.0f / 1024.0f, 30.0f / 1024.0f, 0.3f);
    
    
    //First Level Egg
    egg = Entity(3.0f, 0.0f, 0.0f, 2.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, false, eggSprite);
    entities.push_back(egg);
    
    
    //Second Level Egg
    egg = Entity(-3.0f, -3.0f, 0.0f, 2.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, false, eggSprite);
    entities.push_back(egg);
    
    
    //Third Level Egg
    egg = Entity(0.5f,  -5.5f, 0.0f, 2.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, false, eggSprite);
    entities.push_back(egg);
    
    
    //Fourth Level Egg
    egg = Entity(3.0f, -8.5f, 0.0f, 2.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, false, eggSprite);
    entities.push_back(egg);
    
    
    //Fifth Level Egg
    egg = Entity(-3.0f, -9.0f, 0.0f, 2.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, false, eggSprite);
    entities.push_back(egg);
    egg.entityType = ENTITY_EGG;
    
    //TILES
    
    
    //1st floor
    gameSprite = SheetSprite(tileTexture, -100.0f / 1024.0f, 78.0f / 1024.0f, 222.0f / 1024.0f, 39.0f / 1024.0f, 0.2f);
    entities.push_back(Entity(-3.0f, -2.0f, 0.0f, 2.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, true, gameSprite));
    entities.push_back(Entity(0.0f, -2.0f, 0.0f, 2.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, true, gameSprite));
    entities.push_back(Entity(-1.5f, -1.5f, 0.0f, 2.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, true, gameSprite));
    entities.push_back(Entity(1.5f, -1.5f, 0.0f, 2.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, true, gameSprite));
    entities.push_back(Entity(3.0f, -2.0f, 0.0f, 2.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, true, gameSprite));
    entities.push_back(Entity(3.5f, -1.0f, 0.0f, 2.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, true, gameSprite));
    

//2nd floor
    
    entities.push_back(Entity(-3.0f, -4.0f, 0.0f, 2.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, true, gameSprite));
    entities.push_back(Entity(0.0f, -4.0f, 0.0f, 2.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, true, gameSprite));
    entities.push_back(Entity(-1.5f, -3.5f, 0.0f, 2.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, true, gameSprite));
    entities.push_back(Entity(1.5f, -3.5f, 0.0f, 2.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, true, gameSprite));
    entities.push_back(Entity(3.0f, -4.0f, 0.0f, 2.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, true, gameSprite));
    entities.push_back(Entity(3.5f, -3.0f, 0.0f, 2.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, true, gameSprite));
    
    
//3rd floor
    entities.push_back(Entity(-3.0f, -6.0f, 0.0f, 2.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, true, gameSprite));
    entities.push_back(Entity(0.0f, -6.0f, 0.0f, 2.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, true, gameSprite));
    entities.push_back(Entity(-1.5f, -5.5f, 0.0f, 2.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, true, gameSprite));
    entities.push_back(Entity(1.5f, -5.5f, 0.0f, 2.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, true, gameSprite));
    entities.push_back(Entity(3.0f, -6.0f, 0.0f, 2.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, true, gameSprite));
    entities.push_back(Entity(3.5f, -5.0f, 0.0f, 2.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, true, gameSprite));

//4th floor
    entities.push_back(Entity(-3.0f, -8.0f, 0.0f, 2.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, true, gameSprite));
    entities.push_back(Entity(0.0f, -8.0f, 0.0f, 2.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, true, gameSprite));
    entities.push_back(Entity(-1.5f, -7.5f, 0.0f, 2.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, true, gameSprite));
    entities.push_back(Entity(1.5f, -7.5f, 0.0f, 2.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, true, gameSprite));
    entities.push_back(Entity(3.0f, -8.0f, 0.0f, 2.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, true, gameSprite));
    entities.push_back(Entity(3.5f, -7.0f, 0.0f, 2.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, true, gameSprite));
    
//5th floor
    entities.push_back(Entity(-3.0f, -10.0f, 0.0f, 2.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, true, gameSprite));
    entities.push_back(Entity(0.0f, -10.0f, 0.0f, 2.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, true, gameSprite));
    entities.push_back(Entity(-1.5f, -9.5f, 0.0f, 2.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, true, gameSprite));
    entities.push_back(Entity(1.5f, -9.5f, 0.0f, 2.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, true, gameSprite));
    entities.push_back(Entity(3.0f, -10.0f, 0.0f, 2.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, true, gameSprite));
    entities.push_back(Entity(3.5f, -9.0f, 0.0f, 2.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, true, gameSprite));
    
    
    SDL_Event event;
    bool done = false;
    while (!done) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                done = true;
            }
        }
        
        //Game Time
        float ticks = (float)SDL_GetTicks() / 1000.0f;
        float elapsed = ticks - lastFrameTicks;
        lastFrameTicks = ticks;
        float gameElapse = elapsed;
        
        
        //This gets the elapsed time
        if (gameElapse > FIXED_TIMESTEP) {
            gameElapse = FIXED_TIMESTEP;
        }
        while (gameElapse >= FIXED_TIMESTEP) {
            gameElapse -= FIXED_TIMESTEP;
            Update(FIXED_TIMESTEP);
        }
        
        //Updated Gameplay
        Update(gameElapse);
        
        // Drawing
        glClear(GL_COLOR_BUFFER_BIT);
        
        program.SetModelviewMatrix(modelviewMatrix);
        program.SetProjectionMatrix(projectionMatrix);
        
        
        Update(elapsed);
        Render(modelviewMatrix, program);
        
        SDL_GL_SwapWindow(displayWindow);

    }
    
    SDL_Quit();
    return 0;
}