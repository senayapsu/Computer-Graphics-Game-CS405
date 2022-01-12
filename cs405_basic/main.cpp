#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include<map>
#include<filesystem>
#include <iostream>
#include <vector>
#include <iomanip>
#include <string>

#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/common.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "includes/Window.hpp"
#include "includes/GameObj3D.hpp"
#include "includes/ShaderProgram.hpp"
#include "includes/Camera.hpp"
#include "includes/parametric-3d/Parametric3DShape.hpp"
#include "includes/CubeData.hpp"
#include "includes/collusion-helpers.hpp"
#include "includes/Scene.hpp"
#include "includes/Skybox.hpp"
#include "includes/TriangleSoup.hpp"
#include "includes/Shader.hpp"
#include "includes/shader.h"
#include "ft2build.h"
#include FT_FREETYPE_H

using namespace std;

// Global variables
int u_transform, u_pv, u_frame, u_light_pos, u_light_color;
int moveFront = 0, moveRight = 0;
float mouseX = 0, mouseY = 0;
float velocityY = 0;
int hero_lives = 100;
int donut_point = 0;
bool gameContinues = true;
bool game_won = false;
string str1, str2, str3;
//this vector is created to keep deleted donuts to push back when game starts again
vector<GameObj3D*> scene_donuts;

void keyCallback(GLFWwindow *_, int key, int scancode, int action, int mods)
{
    
    if (key == GLFW_KEY_W && action == GLFW_PRESS)
    {
        moveFront = 1;
    }
    else if (key == GLFW_KEY_W && action == GLFW_RELEASE)
    {
        moveFront = 0;
    }
    if (key == GLFW_KEY_S && action == GLFW_PRESS)
    {
        moveFront = -1;
    }
    else if (key == GLFW_KEY_S && action == GLFW_RELEASE)
    {
        moveFront = 0;
    }
    if (key == GLFW_KEY_D && action == GLFW_PRESS)
    {
        moveRight = 1;
    }
    else if (key == GLFW_KEY_D && action == GLFW_RELEASE)
    {
        moveRight = 0;
    }
    if (key == GLFW_KEY_A && action == GLFW_PRESS)
    {
        moveRight = -1;
    }
    else if (key == GLFW_KEY_A && action == GLFW_RELEASE)
    {
        moveRight = 0;
    }
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
    {
        velocityY = 3;
    }
    if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
    {
        str2 = "";
        str3 = "";
        moveRight = -1;
    }
    else if (key == GLFW_KEY_ENTER && action == GLFW_RELEASE)
    {
        moveRight = 0;
    }
}

static void cursorPositionCallback(GLFWwindow *_, double x, double y)
{
    mouseX = 2.0 * ((float)x / Window::width) - 1;
    mouseY = 2.0 * (1 - ((float)y / Window::height)) - 1;
}





//Text Rendering global definitions
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void RenderText(Shader2& shader, std::string text, float x, float y, float scale, glm::vec3 color);

// settings
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 1200;

/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
    unsigned int TextureID; // ID handle of the glyph texture
    glm::ivec2   Size;      // Size of glyph
    glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
    unsigned int Advance;   // Horizontal offset to advance to next glyph
};

map<GLchar, Character> Characters;
unsigned int VAO, VBO;
//End of text rendering global definitions

int main()
{
    // init window
    Window::init(1200, 1200, "my window");

    glfwSetKeyCallback(Window::window, keyCallback);
    glfwSetCursorPosCallback(Window::window, cursorPositionCallback);

    // init objects
    Model3D sphereModel = Parametric3DShape::generate(ParametricLine::halfCircle, 50, 50);
    Model3D cubeModel1(CubeData::positions, CubeData::normals, CubeData::uvs_floor(75,75), CubeData::indices);
    Model3D cubeModellava(CubeData::positions, CubeData::normals, CubeData::uvs_floor(75,75), CubeData::indices);
    Model3D cubeModel2(CubeData::positions, CubeData::normals, CubeData::uvs(1,1), CubeData::indices);
    Model3D donutModel = Parametric3DShape::generate(ParametricLine::circle, 30, 30);
    
    //HERO & FLOORS
    GameObj3D hero(sphereModel);
    hero.translate(0, 0.0, 10.0);
    hero.scale(0.3, 0.3, 0.3);
    hero.textureId = 2;
    scene.push_back(&hero);

    GameObj3D* water = new GameObj3D(cubeModel1);
    scene.push_back(water);
    water->translate(0, -2, 0);
    water->scale(1000, 1, 1000);
    water->textureId = 0;
    water->hasGravity = false;

    //second floor
    GameObj3D* floor2 = new GameObj3D(cubeModel1);
    floor2->translate(0, -1, 0);
    floor2->scale(10, 0.5, 100);
    floor2->textureId = 1;
    scene.push_back(floor2);

    //3rd floor
    GameObj3D* floor3 = new GameObj3D(cubeModel1);
    floor3->translate(30, -1, -100);
    floor3->scale(40, 0.5, 10);
    floor3->textureId = 1;
    scene.push_back(floor3);

    GameObj3D* donut = new GameObj3D(donutModel);
    donut->translate(0, 0.0, -10.0);
    donut->scale(0.2, 0.2, 0.2);
    donut->rotate(90, 0, 0);
    donut->textureId = 3;
    donut->hasGravity = false;
    donut->isCollid = false;
    scene.push_back(donut);

    GameObj3D* donut2 = new GameObj3D(donutModel);
    donut2->translate(-1, 0, -15.0);
    donut2->scale(0.2, 0.2, 0.2);
    donut2->rotate(90, 0, 0);
    donut2->textureId = 3;
    donut2->hasGravity = false;
    donut2->isCollid = false;
    scene.push_back(donut2);

    GameObj3D* donut3 = new GameObj3D(donutModel);
    donut3->translate(2, 0, -20.0);
    donut3->scale(0.2, 0.2, 0.2);
    donut3->rotate(90, 0, 0);
    donut3->textureId = 3;
    donut3->hasGravity = false;
    donut3->isCollid = false;
    scene.push_back(donut3);

    GameObj3D* donut4 = new GameObj3D(donutModel);
    donut4->translate(0, 0.0, -25.0);
    donut4->scale(0.2, 0.2, 0.2);
    donut4->rotate(90, 0, 0);
    donut4->textureId = 3;
    donut4->hasGravity = false;
    donut4->isCollid = false;
    scene.push_back(donut4);

    GameObj3D* donut5 = new GameObj3D(donutModel);
    donut5->translate(-3, 0, -30.0);
    donut5->scale(0.2, 0.2, 0.2);
    donut5->rotate(90, 0, 0);
    donut5->textureId = 3;
    donut5->hasGravity = false;
    donut5->isCollid = false;
    scene.push_back(donut5);

    GameObj3D* donut6 = new GameObj3D(donutModel);
    donut6->translate(0, 0.0, -50.0);
    donut6->scale(0.2, 0.2, 0.2);
    donut6->rotate(90, 0, 0);
    donut6->textureId = 3;
    donut6->hasGravity = false;
    donut6->isCollid = false;
    scene.push_back(donut6);

    GameObj3D* mine2 = new GameObj3D(cubeModel1);
    mine2->translate(5.0, 0.0, -10.0);
    mine2->scale(0.7, 0.7, 0.7);
    mine2->textureId = 5;
    scene.push_back(mine2);
    //bu cubeun uzerýne 1 donut
    GameObj3D* donutmine2 = new GameObj3D(donutModel);
    donutmine2->translate(5.0, 1.0, -10.0);
    donutmine2->scale(0.2, 0.2, 0.2);
    donutmine2->rotate(90, 0, 0);
    donutmine2->textureId = 3;
    donutmine2->hasGravity = false;
    donutmine2->isCollid = false;
    scene.push_back(donutmine2);

    GameObj3D* mine3 = new GameObj3D(cubeModel1);
    mine3->translate(-5.0, 0.0, -10.0);
    mine3->scale(0.7, 0.7, 0.7);
    mine3->textureId = 5;
    scene.push_back(mine3);
    //bu cubeun uzerýne 1 donut
    GameObj3D* donutmine3 = new GameObj3D(donutModel);
    donutmine3->translate(-5.0, 1.0, -10.0);
    donutmine3->scale(0.2, 0.2, 0.2);
    donutmine3->rotate(90, 0, 0);
    donutmine3->textureId = 3;
    donutmine3->hasGravity = false;
    donutmine3->isCollid = false;
    scene.push_back(donutmine3);


    GameObj3D* mine4 = new GameObj3D(cubeModellava);
    mine4->translate(-3.0, 0.0, -15.0);
    mine4->scale(1.0, 0.5, 0.3);
    mine4->textureId = 0;
    mine4->isCollid = false;
    mine4->hasGravity = false;
    scene.push_back(mine4);

    GameObj3D* mine5 = new GameObj3D(cubeModel1);
    mine5->translate(1.0, 0.0, -15.0);
    mine5->scale(0.3, 0.3, 0.3);
    mine5->textureId = 5;
    scene.push_back(mine5);

    GameObj3D* mine6 = new GameObj3D(cubeModel1);
    mine6->translate(0, 0.0, -20.0);
    mine6->scale(0.3, 0.3, 0.3);
    mine6->textureId = 5;
    scene.push_back(mine6);

    GameObj3D* mine7 = new GameObj3D(cubeModellava);
    mine7->translate(4.0, 0.0, -20.0);
    mine7->scale(1.0, 0.5, 0.3);
    mine7->textureId = 0;
    mine7->isCollid = false;
    mine7->hasGravity = false;
    scene.push_back(mine7);

    GameObj3D* mine8 = new GameObj3D(cubeModellava);
    mine8->translate(2.0, 0.0, -25.0);
    mine8->scale(1.0, 0.5, 0.3);
    mine8->textureId = 0;
    mine8->isCollid = false;
    mine8->hasGravity = false;
    scene.push_back(mine8);

    GameObj3D* mine9 = new GameObj3D(cubeModellava);
    mine9->translate(-2.0, 0.0, -25.0);
    mine9->scale(1.0, 0.5, 0.3);
    mine9->textureId = 0;
    mine9->hasGravity = false;
    mine9->isCollid = false;
    scene.push_back(mine9);

    GameObj3D* mine10 = new GameObj3D(cubeModel1);
    mine10->translate(-5.0, 0.0, -30.0);
    mine10->scale(0.3, 0.3, 0.3);
    mine10->textureId = 5;
    scene.push_back(mine10);

    GameObj3D* mine11 = new GameObj3D(cubeModellava);
    mine11->translate(-1.0, 0.0, -30.0);
    mine11->scale(1.0, 0.5, 0.3);
    mine11->textureId = 0;
    mine11->isCollid = false;
    mine11->hasGravity = false;
    scene.push_back(mine11);

    GameObj3D* mine12 = new GameObj3D(cubeModellava);
    mine12->translate(-2.0, 0.0, -35.0);
    mine12->scale(1.0, 0.5, 0.3);
    mine12->textureId = 0;
    mine12->isCollid = false;
    mine12->hasGravity = false;
    scene.push_back(mine12);

    GameObj3D* mine13 = new GameObj3D(cubeModellava);
    mine13->translate(0, 0.0, -35.0);
    mine13->scale(1.0, 0.5, 0.3);
    mine13->textureId = 0;
    mine13->hasGravity = false;
    mine13->isCollid = false;
    scene.push_back(mine13);

    GameObj3D* mine14 = new GameObj3D(cubeModel1);
    mine14->translate(0, 0.0, -40.0);
    mine14->scale(0.3, 0.3, 0.3);
    mine14->textureId = 5;
    scene.push_back(mine14);

    GameObj3D* mine16 = new GameObj3D(cubeModel1);
    mine16->translate(5.0, 0.0, -45.0);
    mine16->scale(0.3, 0.3, 0.3);
    mine16->textureId = 5;
    scene.push_back(mine16);

    GameObj3D* mine17 = new GameObj3D(cubeModel1);
    mine17->translate(-5.0, 0.0, -45.0);
    mine17->scale(0.3, 0.3, 0.3);
    mine17->textureId = 5;
    scene.push_back(mine17);

    GameObj3D* mine18 = new GameObj3D(cubeModel1);
    mine18->translate(-5.0, 0.0, -55.0);
    mine18->scale(0.7,0.7,0.7);
    mine18->textureId = 5;
    scene.push_back(mine18);
    //bu cubeun uzerýne 1 donut
    GameObj3D* donutmine18 = new GameObj3D(donutModel);
    donutmine18->translate(-5.0, 1.0, -55.0);
    donutmine18->scale(0.2, 0.2, 0.2);
    donutmine18->rotate(90, 0, 0);
    donutmine18->textureId = 3;
    donutmine18->hasGravity = false;
    donutmine18->isCollid = false;
    scene.push_back(donutmine18);

    GameObj3D* mine19 = new GameObj3D(cubeModel1);
    mine19->translate(3.0, 0.0, -55.0);
    mine19->scale(0.7, 0.7, 0.7);
    mine19->textureId = 5;
    scene.push_back(mine19);
    //bu cubeun uzerýne 1 donut
    GameObj3D* donutmine19 = new GameObj3D(donutModel);
    donutmine19->translate(3.0, 1.0, -55.0);
    donutmine19->scale(0.2, 0.2, 0.2);
    donutmine19->rotate(90, 0, 0);
    donutmine19->textureId = 3;
    donutmine19->hasGravity = false;
    donutmine19->isCollid = false;
    scene.push_back(donutmine19);

    GameObj3D* mine20 = new GameObj3D(cubeModellava);
    mine20->translate(-1.0, 0.0, -65.0);
    mine20->scale(1.0, 0.5, 0.3);
    mine20->textureId = 0;
    mine20->hasGravity = false;
    mine20->isCollid = false;
    scene.push_back(mine20);

    GameObj3D* donutmine20 = new GameObj3D(donutModel);
    donutmine20->translate(0.5, 0, -65.0);
    donutmine20->scale(0.2, 0.2, 0.2);
    donutmine20->rotate(90, 0, 0);
    donutmine20->textureId = 3;
    donutmine20->hasGravity = false;
    donutmine20->isCollid = false;
    scene.push_back(donutmine20);

    GameObj3D* mine21 = new GameObj3D(cubeModel1);
    mine21->translate(2.0, 0.0, -65.0);
    mine21->scale(0.3, 0.3, 0.3);
    mine21->textureId = 5;
    scene.push_back(mine21);

    GameObj3D* mine22 = new GameObj3D(cubeModellava);
    mine22->translate(-3.0, 0.0, -75.0);
    mine22->scale(1.0, 0.5, 0.3);
    mine22->textureId = 0;
    mine22->hasGravity = false;
    mine22->isCollid = false;
    scene.push_back(mine22);

    GameObj3D* donutmine22 = new GameObj3D(donutModel);
    donutmine22->translate(1, 0, -75.0);
    donutmine22->scale(0.2, 0.2, 0.2);
    donutmine22->rotate(90, 0, 0);
    donutmine22->textureId = 3;
    donutmine22->hasGravity = false;
    donutmine22->isCollid = false;
    scene.push_back(donutmine22);

    GameObj3D* mine23 = new GameObj3D(cubeModel1);
    mine23->translate(3.0, 0.0, -75.0);
    mine23->scale(0.3, 0.3, 0.3);
    mine23->textureId = 5;
    scene.push_back(mine23);

    GameObj3D* mine24 = new GameObj3D(cubeModel1);
    mine24->translate(0.0, 0.0, -80.0);
    mine24->scale(0.3, 0.3, 0.3);
    mine24->textureId = 5;
    scene.push_back(mine24);

    //3rd floor objects

    GameObj3D* rectangle2 = new GameObj3D(cubeModel1);
    rectangle2->translate(10, 0.0, -98.0);
    rectangle2->scale(0.7, 0.3, 6);
    rectangle2->textureId = 5;
    scene.push_back(rectangle2);

    GameObj3D* donutrectangle2 = new GameObj3D(donutModel);
    donutrectangle2->translate(15, 0.9, -98.0);
    donutrectangle2->scale(0.2, 0.2, 0.2);
    donutrectangle2->rotate(90, 90, 0);
    donutrectangle2->textureId = 3;
    donutrectangle2->hasGravity = false;
    donutrectangle2->isCollid = false;
    scene.push_back(donutrectangle2);

    GameObj3D* rectangle3 = new GameObj3D(cubeModel1);
    rectangle3->translate(15, 0.0, -98.0);
    rectangle3->scale(0.7, 0.3, 6);
    rectangle3->textureId = 5;
    scene.push_back(rectangle3);

    GameObj3D* donutrectangle3 = new GameObj3D(donutModel);
    donutrectangle3->translate(20, 0.9, -102.0);
    donutrectangle3->scale(0.2, 0.2, 0.2);
    donutrectangle3->rotate(90, 90, 0);
    donutrectangle3->textureId = 3;
    donutrectangle3->hasGravity = false;
    donutrectangle3->isCollid = false;
    scene.push_back(donutrectangle3);

    GameObj3D* rectangle4 = new GameObj3D(cubeModel1);
    rectangle4->translate(20, 0.0, -98.0);
    rectangle4->scale(0.7, 0.3, 6);
    rectangle4->textureId = 5;
    scene.push_back(rectangle4);

    GameObj3D* rectangle5 = new GameObj3D(cubeModel1);
    rectangle5->translate(25, 0.0, -98.0);
    rectangle5->scale(0.7, 0.3, 6);
    rectangle5->textureId = 5;
    scene.push_back(rectangle5);

    GameObj3D* finish_platform = new GameObj3D(cubeModel1);
    finish_platform->translate(50, 0.5, -98.0);
    finish_platform->scale(3, 0.1, 6);
    finish_platform->textureId = 5;
    finish_platform->hasGravity = false;
    scene.push_back(finish_platform);

    GameObj3D* key = new GameObj3D(donutModel);
    key->translate(50, 1.0, -98.0);
    key->scale(0.2, 0.2, 0.2);
    key->textureId = 2;
    key->rotate(90, 90, 0);
    key->hasGravity = false;
    key->isCollid = false;
    scene.push_back(key);

    // light
    glm::vec3 lightPos = glm::vec3(1.0, 1.0, 1.0);
    glm::vec3 lightPos3 = glm::vec3(0.0f, 2.0f, 2.0f);
    glm::vec3 lightColor = glm::vec3(2.0, 2.0, 2.0);

    //WATER 
    // time
    float time;

    // rotation
    glm::vec3 myRotationAxis;
    glm::mat4 rotMat(1.0f);

    Shader waterShader;
    GLuint waterID;
    GLint location_rotMat3;
    GLint light_pos3;
    GLint eye_pos3;
    GLint location_time1;

    waterShader.createShader("shader/waterShaderVert.glsl", "shader/waterShaderFrag.glsl");

    //water.readOBJ("lastv2/textures/plane2.obj");

    glm::mat4 waterMVP;
    waterID = glGetUniformLocation(waterShader.programID, "MVP");
    location_rotMat3 = glGetUniformLocation(waterShader.programID, "rotMat");
    light_pos3 = glGetUniformLocation(waterShader.programID, "lightPos");
    eye_pos3 = glGetUniformLocation(waterShader.programID, "eyePosition");
    location_time1 = glGetUniformLocation(waterShader.programID, "time");
    glUseProgram(waterShader.programID);
    //end of water
    
    //added for gamma correction
    float gamma = 2.2f;

    const vector<string> texture_files{"textures/lava.jpg", "textures/darkgrey.jpg", "textures/straight-gold.png", "textures/donuts.jpg", "textures/base-black.png",/*5 textures for cubes*/"textures/grey.jpg" };
    // load textures
    vector<unsigned int> textures = Textures::loadTextures(texture_files);

    // load skybox
    unsigned int skyboxVAO, skyboxVBO;
    initSkybox(skyboxVAO, skyboxVBO);
    vector<std::string> faces{
        "nebula/right.png",
        "nebula/left.png",
        "nebula/top.png",
        "nebula/bottom.png",
        "nebula/front.png",
        "nebula/back.png"};
    unsigned cubemapTexture = loadCubemap(faces);
    ShaderProgram skyboxShader("shader/skybox.vert", "shader/skybox.frag");
    skyboxShader.use();
    auto skybox_texture = glGetUniformLocation(skyboxShader.id, "skybox");
    auto u_pv_sky = glGetUniformLocation(skyboxShader.id, "u_pv");
    glUniform1i(skybox_texture, 0); // 0th unit

    // create shader
    ShaderProgram sp("shader/vertex.vert", "shader/frag.frag");
    sp.use();
    u_transform = glGetUniformLocation(sp.id, "u_transform");
    u_pv = glGetUniformLocation(sp.id, "u_pv");
    u_frame = glGetUniformLocation(sp.id, "u_frame");
    u_light_pos = glGetUniformLocation(sp.id, "u_light_pos");
    u_light_color = glGetUniformLocation(sp.id, "u_light_color");
    auto u_texture = glGetUniformLocation(sp.id, "u_texture");
    auto shadowMap = glGetUniformLocation(sp.id, "shadowMap");
    glUniform1i(glGetUniformLocation(sp.id, "gamma"), gamma);
    glUniform1i(u_texture, 0);
    glUniform1i(shadowMap, 1);
    glActiveTexture(GL_TEXTURE0); // active 0th unit
    glActiveTexture(GL_TEXTURE1); // active 1st unit
    
    unsigned a = 0;


    //ADDED FOR SHADOWS
    Shader2 simpleDepthShader("shader/shadow.vs", "shader/shadow.fr", "shader/shadow.gs");
    //
    // configure depth map FBO
    // -----------------------
    const unsigned int SHADOW_WIDTH = 1200, SHADOW_HEIGHT = 1200;
    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    // create depth cubemap texture
    unsigned int depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthMap);
    for (unsigned int i = 0; i < 6; ++i)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //added for water
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // define light positions
    float lightPos2[3] = { -2.0, 5.0, 13.5 };
    //

    
    //For text rendering 
    Shader2 shader("shader/text.vs", "shader/text.fr");
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(SCR_WIDTH), 0.0f, static_cast<float>(SCR_HEIGHT));
    shader.use();
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    // FreeType
    FT_Library ft;
    // All functions return a value different than 0 whenever an error occurred
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return -1;
    }

    // find path to font
    string font_name = "fonts/Zabdilus.ttf";
    if (font_name.empty())
    {
        std::cout << "ERROR::FREETYPE: Failed to load font_name" << std::endl;
        return -1;
    }

    // load font as face
    FT_Face face;
    if (FT_New_Face(ft, font_name.c_str(), 0, &face)) {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return -1;
    }
    else {
        // set size to load glyphs as
        FT_Set_Pixel_Sizes(face, 0, 48);

        // disable byte-alignment restriction
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // load first 128 characters of ASCII set
        for (unsigned char c = 0; c < 128; c++)
        {
            // Load character glyph 
            if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            {
                std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
                continue;
            }
            // generate texture
            unsigned int texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
            );
            // set texture options
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // now store character for later use
            Character character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                static_cast<unsigned int>(face->glyph->advance.x)
            };
            Characters.insert(std::pair<char, Character>(c, character));
        }
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    // destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    // configure VAO/VBO for texture quads
    // -----------------------------------
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    //Text rendering end
    
    // game loop
    while (!Window::isClosed())
    {

        // update player and camera
        hero.moveFront(moveFront * 0.2);
        hero.rotate(hero.rotation().x, hero.rotation().y - moveRight * 0.5f, hero.rotation().z);

        if (velocityY > 0)
        {
            hero.moveUp(velocityY);
        }

        Camera::position = hero.position() - hero.front() * 6.0f + hero.up() * 2.0f;

        Camera::front = hero.front() + glm::vec3(0, mouseY, 0);
        Camera::up = glm::cross(Camera::front, hero.right());
        Camera::front = glm::rotateY(Camera::front, -mouseX);

        //added for shadows 
        float near_plane = 1.0f;
        float far_plane = 120.0f;
        glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, near_plane, far_plane);
        std::vector<glm::mat4> shadowTransforms;
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos3, lightPos3 + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos3, lightPos3 + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos3, lightPos3 + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos3, lightPos3 + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos3, lightPos3 + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos3, lightPos3 + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        simpleDepthShader.use();
        for (unsigned int i = 0; i < 6; ++i)
            simpleDepthShader.setMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
        simpleDepthShader.setFloat("far_plane", far_plane);
        simpleDepthShader.setVec3("lightPos", lightPos3);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
         
        // update uniforms
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        sp.use();
        glUniformMatrix4fv(u_pv, 1, GL_FALSE, glm::value_ptr(Camera::getProjMatrix() * Camera::getViewMatrix()));
        glUniform3fv(glGetUniformLocation(sp.id, "viewPos"), 1, glm::value_ptr(Camera::position));
        glUniform1i(u_frame, 1);
        glUniform3fv(u_light_pos, 1, glm::value_ptr(lightPos3));
        glUniform3fv(u_light_color, 1, glm::value_ptr(lightColor));


        //colllision with donut or lavas
        for (int i = 0; i < scene.size(); i++) {
            GameObj3D* obj = scene[i];
            if (hero.id != obj->id && obj->id != water->id) {

                //if reaches to the key at the end, finish the game and start again
                if (intersect(hero, *obj) && obj->model3d == &donutModel && obj->textureId == 2) {
                    scene_donuts.push_back(obj);
                    DeleteFromScene(obj->id);
                    gameContinues = false;
                    game_won = true;
                    str2 = "YOU WON THE GAME!";
                    str3 = "Press ENTER to continue";
                    donut_point = 0;
                    //donutlarý geri getir
                    for (std::vector<GameObj3D*>::iterator t = scene_donuts.begin(); t != scene_donuts.end(); ++t)
                    {
                        // get the object
                        const int i = t - scene_donuts.begin();

                        // gravity logic
                        GameObj3D* donut_obj = scene_donuts[i];
                        scene.push_back(donut_obj);    
                    }
                    scene_donuts.clear();
                    gameContinues = true;
                    game_won = false;
                    hero_lives = 100;
                    hero.front() = glm::vec3(0, 0, -1);
                    hero.transformInfo.rotation = glm::vec3(0);
                    hero.rotate(hero.rotation().x, hero.rotation().y, hero.rotation().z);
                    hero.translate(0, 0.0, 10.0);
                    Camera::front = hero.front();
                    break;
                }
                else if (intersect(hero, *obj) && obj->model3d == &donutModel) {
                    scene_donuts.push_back(obj);
                    DeleteFromScene(obj->id);
                    donut_point += 1;
                    cout << "Items collected: " << donut_point << endl;
                }
                else if (intersect(hero, *obj) && obj->model3d == &cubeModellava) {
                    hero_lives -= 1;
                }
            }
        }

        //Eger lavlara duserse öluyor
        if (hero.position().y > -0.7 && hero.position().y <= -0.6)
        {
            hero_lives -= 30;
            hero.front() = glm::vec3(0, 0, -1);
            hero.transformInfo.rotation = glm::vec3(0);
            hero.rotate(hero.rotation().x, hero.rotation().y, hero.rotation().z);
            hero.translate(0, 0.0, 10.0);
            Camera::front = hero.front();

        }
        //eðer canlar sýfýr olduysa oyun biter 
        if (hero_lives <= 0)
        {
            gameContinues = false;
        }
        //oyun kaybedildiðinde baþtan baþlar
        if (gameContinues == false && game_won == false)
        {
            //print game over
            str2 = "GAME OVER";
            str3 = "Press ENTER to continue";
            donut_point = 0;

            //donutlarý geri getir
            for (std::vector<GameObj3D*>::iterator t = scene_donuts.begin(); t != scene_donuts.end(); ++t)
            {
                // get the object
                const int i = t - scene_donuts.begin();

                // gravity logic
                GameObj3D* donut_obj = scene_donuts[i];
                scene.push_back(donut_obj);
            }
            scene_donuts.clear();
            hero_lives = 100;
            gameContinues = true;
            hero.front() = glm::vec3(0, 0, -1);
            hero.transformInfo.rotation = glm::vec3(0);
            hero.rotate(hero.rotation().x, hero.rotation().y, hero.rotation().z);
            hero.translate(0, 0.0, 10.0);
            Camera::front = hero.front();
        }

        // scene draw
        for (std::vector<GameObj3D *>::iterator t = scene.begin(); t != scene.end(); ++t)
        {
            // get the object
            const int i = t - scene.begin();

            // gravity logic
            GameObj3D *object = scene[i];
            if (object->hasGravity)
            {
                object->moveUp(-0.2);
            }

            // for jump logic
            velocityY -= 0.2;

            if (i == 1 || object->model3d == &cubeModellava)
            {
                // draw water
                time = (float)glfwGetTime();
                glm::mat4 projection = Camera::getProjMatrix();
                waterMVP = Camera::getMVPMatrix(object->getTransform(), projection);
                glUseProgram(waterShader.programID);
                glUniformMatrix4fv(waterID, 1, GL_FALSE, &waterMVP[0][0]);
                glUniform3fv(light_pos3, 1, lightPos2);
                glUniform1f(location_time1, time);
                glUniform3fv(eye_pos3, 1, glm::value_ptr(Camera::position));
                //glUniformMatrix4fv(location_rotMat3, 1, GL_FALSE, &rotMat[0][0]);
                glUniformMatrix4fv(location_rotMat3, 1, GL_FALSE, glm::value_ptr(object->getTransform()));
                object->draw();
            }
            else
            {
                // draw the object
                sp.use();
                glUniformMatrix4fv(u_transform, 1, GL_FALSE, glm::value_ptr(object->getTransform()));
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, textures[object->textureId]);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, depthMap);
                object->draw();
            }
            
        }

        // draw skybox
        glDepthFunc(GL_LEQUAL); // change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.use();
        glUniformMatrix4fv(u_pv_sky, 1, GL_FALSE, glm::value_ptr(Camera::getProjMatrix() * glm::mat4(glm::mat3(Camera::getViewMatrix()))));
        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default

        glUseProgram(0);

        str1 = "Lives: " + to_string(hero_lives) + "  Items collected: " + to_string(donut_point);
        RenderText(shader, str1, 25.0f, 1000.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
        RenderText(shader, str2, 520.0f, 570.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
        RenderText(shader, str3, 470.0f, 520.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
        // update the scene
        Window::refresh();
   
    }//whielýn sonu
 
    Window::terminate(); 

    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// render line of text
// -------------------
void RenderText(Shader2& shader, std::string text, float x, float y, float scale, glm::vec3 color)
{
    // activate corresponding render state	
    shader.use();
    glUniform3f(glGetUniformLocation(shader.ID, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
