// ─────────────────────────────────────────────────────────────────────────────
// main.cpp  –  3D Art Gallery  (OpenGL 3.3 Core Profile)
//
//  Controls:
//    W A S D     – move
//    Mouse       – look
//    Scroll      – zoom
//    ESC         – quit
// ─────────────────────────────────────────────────────────────────────────────

#include <glad/glad.h>          // OpenGL function loader  (include before GLFW)
#include <GLFW/glfw3.h>         // Window / input

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "Room.h"
#include "Model.h"

#include <iostream>
#include <string>

// ─────────────────────────────────────────────────────────────────────────────
// Window constants
// ─────────────────────────────────────────────────────────────────────────────
constexpr unsigned int SCR_W = 1280;
constexpr unsigned int SCR_H =  720;
const char* WINDOW_TITLE = "3D Art Gallery";

// ─────────────────────────────────────────────────────────────────────────────
// Gallery room dimensions (parametric)
// ─────────────────────────────────────────────────────────────────────────────
constexpr float ROOM_WIDTH  = 12.0f;
constexpr float ROOM_HEIGHT =  6.0f;
constexpr float ROOM_DEPTH  = 16.0f;

// ─────────────────────────────────────────────────────────────────────────────
// Global state (accessed in GLFW callbacks)
// ─────────────────────────────────────────────────────────────────────────────
Camera camera(glm::vec3(0.0f, 1.7f, 6.0f));  // start near entrance

bool  firstMouse    = true;
float lastMouseX    = SCR_W * 0.5f;
float lastMouseY    = SCR_H * 0.5f;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

// ─────────────────────────────────────────────────────────────────────────────
// Callbacks
// ─────────────────────────────────────────────────────────────────────────────
void framebuffer_size_callback(GLFWwindow*, int w, int h)
{
    glViewport(0, 0, w, h);
}

void mouse_callback(GLFWwindow*, double xpos, double ypos)
{
    if(firstMouse)
    {
        lastMouseX = (float)xpos;
        lastMouseY = (float)ypos;
        firstMouse = false;
    }
    float xOff = (float)xpos - lastMouseX;
    float yOff = lastMouseY - (float)ypos;  // invert Y: screen Y grows downward
    lastMouseX = (float)xpos;
    lastMouseY = (float)ypos;

    camera.ProcessMouseMovement(xOff, yOff);
}

void scroll_callback(GLFWwindow*, double /*xoffset*/, double yoffset)
{
    camera.ProcessMouseScroll((float)yoffset);
}

// ─────────────────────────────────────────────────────────────────────────────
// Process held-down keyboard keys each frame
// ─────────────────────────────────────────────────────────────────────────────
void processInput(GLFWwindow* window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(CAM_FORWARD,  deltaTime);
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(CAM_BACKWARD, deltaTime);
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(CAM_LEFT,     deltaTime);
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(CAM_RIGHT,    deltaTime);
}

// ─────────────────────────────────────────────────────────────────────────────
// Upload lighting uniforms to a shader (both room and model use the same set)
// ─────────────────────────────────────────────────────────────────────────────
static void setLightingUniforms(Shader& shader,
                                const glm::vec3& lightPos,
                                const glm::vec3& lightColor,
                                const glm::vec3& viewPos)
{
    shader.setVec3("lightPos",   lightPos);
    shader.setVec3("lightColor", lightColor);
    shader.setVec3("viewPos",    viewPos);
}

// ─────────────────────────────────────────────────────────────────────────────
// main
// ─────────────────────────────────────────────────────────────────────────────
int main()
{
    // ── GLFW init ─────────────────────────────────────────────────────────────
    if(!glfwInit()) {
        std::cerr << "GLFW init failed\n";
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);   // 4× MSAA

    GLFWwindow* window = glfwCreateWindow(SCR_W, SCR_H, WINDOW_TITLE,
                                          nullptr, nullptr);
    if(!window) {
        std::cerr << "Window creation failed\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Register callbacks
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // Capture mouse (FPS look)
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // ── GLAD loader ───────────────────────────────────────────────────────────
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "GLAD init failed\n";
        return -1;
    }
    std::cout << "OpenGL " << glGetString(GL_VERSION) << "\n";

    // ── OpenGL global state ───────────────────────────────────────────────────
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);  // MSAA
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // ── Camera boundaries (match room; small margin prevents clipping) ────────
    float margin = 0.5f;
    camera.SetBoundaries(
        glm::vec2(-ROOM_WIDTH*0.5f + margin, -ROOM_DEPTH*0.5f + margin),
        glm::vec2( ROOM_WIDTH*0.5f - margin,  ROOM_DEPTH*0.5f - margin),
        1.7f   // eye height (metres)
    );

    // ── Shaders ───────────────────────────────────────────────────────────────
    Shader roomShader ("shaders/room.vert",  "shaders/room.frag");
    Shader modelShader("shaders/model.vert", "shaders/model.frag");

    // ── Room geometry ─────────────────────────────────────────────────────────
    Room room(ROOM_WIDTH, ROOM_HEIGHT, ROOM_DEPTH);

    // Textures – make sure these files exist in assets/textures/
    // If a file is missing, a magenta placeholder is used automatically.
    room.LoadTextures(
        "assets/textures/floor.jpg",
        "assets/textures/wall.jpg",
        "assets/textures/ceiling.jpg"
    );
    room.LoadPaintings(
        "assets/textures/painting1.jpg",
        "assets/textures/painting2.jpg",
        "assets/textures/painting3.jpg"
    );

    // Stand texture (wood/marble)
    unsigned int standTexID = Texture::Load("assets/textures/stand.jpg");

    // ── 3D model (sculpture) ──────────────────────────────────────────────────
    // Place your .obj file in assets/models/
    // The model will be shown even if missing – just no mesh will render.
    std::string modelPath = "assets/models/sculpture.obj";
    Model sculpture(modelPath);

    // ── Lighting ──────────────────────────────────────────────────────────────
    // Ceiling light centred in the room
    glm::vec3 lightPos  (0.0f, ROOM_HEIGHT - 0.3f, 0.0f);
    glm::vec3 lightColor(1.0f, 0.95f, 0.88f);   // warm white

    // ── Animation state ───────────────────────────────────────────────────────
    float sculptureAngle = 0.0f;           // rotation angle (degrees)
    float rotationSpeed  = 45.0f;          // degrees per second

    // Sculpture placement: centred on stand (stand top = Y=1.0)
    glm::vec3 sculpturePos(0.0f, 1.05f, 0.0f);

    // ── Render loop ───────────────────────────────────────────────────────────
    while(!glfwWindowShouldClose(window))
    {
        // ── Delta time ────────────────────────────────────────────────────────
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // ── Input ─────────────────────────────────────────────────────────────
        processInput(window);

        // ── Update animation: rotate sculpture around Y ───────────────────────
        sculptureAngle += rotationSpeed * deltaTime;
        if(sculptureAngle >= 360.0f)
            sculptureAngle -= 360.0f;

        // ── Clear ─────────────────────────────────────────────────────────────
        glClearColor(0.08f, 0.07f, 0.10f, 1.0f);   // dark background
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ── Matrices ──────────────────────────────────────────────────────────
        glm::mat4 view = camera.GetViewMatrix();

        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        float aspect = (h > 0) ? (float)w / (float)h : 1.0f;
        glm::mat4 projection = glm::perspective(
            glm::radians(camera.Zoom), aspect, 0.1f, 100.0f);

        // ─────────────────────────────────────────────────────────────────────
        // Draw Room (floor, ceiling, walls, paintings, stand)
        // ─────────────────────────────────────────────────────────────────────
        roomShader.use();
        roomShader.setMat4("view",       view);
        roomShader.setMat4("projection", projection);
        setLightingUniforms(roomShader, lightPos, lightColor, camera.Position);

        // Room surfaces
        room.Draw(roomShader);

        // Painting quads (no tiling, full image)
        room.DrawPaintings(roomShader);

        // Stand cube
        room.DrawStand(roomShader, standTexID);

        // ─────────────────────────────────────────────────────────────────────
        // Draw Sculpture (Assimp model)
        // ─────────────────────────────────────────────────────────────────────
        if(sculpture.IsLoaded())
        {
            modelShader.use();
            modelShader.setMat4("view",       view);
            modelShader.setMat4("projection", projection);
            setLightingUniforms(modelShader, lightPos, lightColor, camera.Position);

            // Fallback colour when model has no texture (bronze-ish)
            modelShader.setVec3("objectColor", glm::vec3(0.72f, 0.45f, 0.20f));
            modelShader.setFloat("texTiling",  1.0f);

            // Build model matrix:
            //   1. Scale to fit on stand (adjust if model is too large/small)
            //   2. Rotate around Y
            //   3. Translate to stand top
            glm::mat4 modelMat = glm::mat4(1.0f);
            modelMat = glm::translate(modelMat, sculpturePos);
            modelMat = glm::rotate(modelMat,
                                   glm::radians(sculptureAngle),
                                   glm::vec3(0.0f, 1.0f, 0.0f));
            modelMat = glm::scale(modelMat, glm::vec3(0.6f));  // ← adjust scale

            modelShader.setMat4("model", modelMat);
            sculpture.Draw(modelShader);
        }

        // ── Swap & poll ───────────────────────────────────────────────────────
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
