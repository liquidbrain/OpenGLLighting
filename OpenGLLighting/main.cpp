/**
 * OpenGL lighting demo using the Phong reflection model.
 * Demo has two cubes, a larger cube that is the subject of our light source, and a smaller one that
 * is used as the light source.
 *
 * A Vertex Shader in OpenGL is a piece of C like code written to the GLSL specification which
 * influences the attributes of a vertex. Vertex shaders can be used to modify properties of the
 * vertex such as position, color, and texture coordinates.
 *
 * A Fragment Shader is similar to a Vertex Shader, but is used for calculating individual fragment
 * colors. This is where lighting and bump-mapping effects are performed.
 *
 * The shaders are compiled and then chained together into a Shader Program.
 *
 * The shader pipeline behaves as follows:
 * Vertex Shaders -> Geometry Shaders -> (Rasterizing Engine) -> Fragment Shaders.
 *
 * A Vertex Array Object (VAO) is an object which contains one or more buffer objects and is designed
 * to store the information for a complete rendered object. The shaders receive input data from our
 * VAO through a process of attribute binding, allowing us to perform the needed computations to provide
 * the desired results.
 *
 * A Vertex Buffer Object (VBO) holds information about vertices. VBOs can also store information
 * such as color, normals, texcoords, etc.
 *
 * Created 8/6/17.
 */

#include <iostream>

// GLEW: OpenGL Extension Wrangler
//#define GLEW_STATIC
#include <GL/glew.h>

// GLFW: A simple API for creating windows, contexts and surfaces, and receiving input and events
#include <GLFW/glfw3.h>

// GLM: OpenGL Math
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GLSLProgram.h"
#include "Camera.h"

GLFWwindow* InitGlfw();
void InitShaders();
void Render(GLFWwindow* window);
void HandleDirectionalKeys(GLFWwindow *window);
void GlfwErrorCallback(int error, const char* description);
void GlfwFramebufferResizeCallback(GLFWwindow *window, int width, int height);
void GlfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void GlfwMousePositionCallback(GLFWwindow* window, double xpos, double ypos);
void GlfwMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void GLfwMouseScrollWheelCallback(GLFWwindow* window, double xoffset, double yoffset);
void GlfwWindowRefreshCallback(GLFWwindow* window);

// Window dimensions when the application is started.
static const GLuint WIDTH = 1280;
static const GLuint HEIGHT = 960;

static const char* LIGHTING_VERTEX_SHADER_PATH =
        "/Users/john/Dev/OpenGL/LearnOpenGL/OpenGLLighting/OpenGLLighting/cube.vs";
static const char* LIGHTING_FRAGMENT_SHADER_PATH =
        "/Users/john/Dev/OpenGL/LearnOpenGL/OpenGLLighting/OpenGLLighting/cube.fs";
static const char* LAMP_VERTEX_SHADER_PATH =
        "/Users/john/Dev/OpenGL/LearnOpenGL/OpenGLLighting/OpenGLLighting/lamp.vs";
static const char* LAMP_FRAGMENT_SHADER_PATH =
        "/Users/john/Dev/OpenGL/LearnOpenGL/OpenGLLighting/OpenGLLighting/lamp.fs";

GLSLProgram lightingShader;
GLSLProgram lampShader;

GLuint cubeVAO;
GLuint lightVAO;
GLuint VBO;

Camera camera(glm::vec3(0.0f, 0.0f, 6.0f));

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

float lastX = 0.0f;
float lastY = 0.0f;
uint mouseCallbackNbr = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Position and normal data
float vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
};

int main(int argc, const char * argv[])
{
    GLFWwindow* window = InitGlfw();

    InitShaders();

    // Render the window's contents, then poll for events  (key pressed, mouse moved, etc.).
    while (!glfwWindowShouldClose(window)) {
        Render(window);
        HandleDirectionalKeys(window);
        glfwPollEvents();
    }

    // Deallocate all resources once they've outlived their purpose.
    glUseProgram(0);
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBO);

    // Terminate GLFW and clear its resources.
    glfwTerminate();

    return EXIT_SUCCESS;
}

void Render(GLFWwindow* window)
{
    float currentFrame = static_cast<float>(glfwGetTime()); // per-frame time logic
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Make the cube's shader part of the current rendering state.
    lightingShader.UseProgram();

    // Set the cube's color, the color and position of the light, and the camera's position.
    lightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
    lightingShader.setVec3("lightColor",  1.0f, 1.0f, 1.0f);
    lightingShader.setVec3("lightPos", lightPos);
    lightingShader.setVec3("viewPos", camera.Position);

    // View/Projection transformations.
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix();
    lightingShader.setMat4("projection", projection);
    lightingShader.setMat4("view", view);

    // World transformation.
    glm::mat4 model;
    lightingShader.setMat4("model", model);

    // Render the cube. For glDrawArrays(...):
    // - first argument specifies what kind of primitives to render.
    // - second argument specifies the start index
    // - third argument specifies the number of indices
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Make the lamp's shader part of the current rendering state.
    lampShader.UseProgram();

    // View/Projection transformations.
    lampShader.setMat4("projection", projection);
    lampShader.setMat4("view", view);

    // World transformations. Note that the lamp's cube is smaller than the main cube.
    model = glm::mat4();
    model = glm::translate(model, lightPos);
    model = glm::scale(model, glm::vec3(0.2f));
    lampShader.setMat4("model", model);

    // Render the lamp. For glDrawArrays(...):
    // - first argument specifies what kind of primitives to render.
    // - second argument specifies the start index
    // - third argument specifies the number of indices
    glBindVertexArray(lightVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glfwSwapBuffers(window);
}

/**
 * Creates and initializes a GLFW window and sets callback functions.
 */
GLFWwindow* InitGlfw()
{
    // Init GLFW.
    int glfwInitStatus = glfwInit();
    if (glfwInitStatus != GLFW_TRUE) {
        std::cerr << "GLFW initialization error" << std::endl;
    }

    // Set all the required options for GLFW.
#if 0
    // For Windows...
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
#endif
    // ... For macOS.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a GLFWwindow object that we can use for GLFW's functions.
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL Lighting", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    // Select the minimum number of monitor refreshes the driver wait should from the time glfwSwapBuffers()
    // was called before swapping the buffers.
    glfwSwapInterval(1);

    // Set the callback functions.
    glfwSetFramebufferSizeCallback(window, GlfwFramebufferResizeCallback);
    //glfwSetWindowRefreshCallback(window, GlfwWindowRefreshCallback);
    glfwSetKeyCallback(window, GlfwKeyCallback);
    glfwSetCursorPosCallback(window, GlfwMousePositionCallback);
    glfwSetMouseButtonCallback(window, GlfwMouseButtonCallback);
    glfwSetScrollCallback(window, GLfwMouseScrollWheelCallback);
    glfwSetErrorCallback(GlfwErrorCallback);

    // Tell GLFW to capture our mouse.
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Tell OpenGL to do depth comparisons and update the depth buffer.
    glEnable(GL_DEPTH_TEST);

    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions.
    glewExperimental = GL_TRUE;

    // Initialize GLEW to setup OpenGL function pointers and extension entry points.
    GLenum err = glewInit();
    if (err != GLEW_OK)	{
        std::cerr << "GLEW initialization error: " << glewGetErrorString(err) << std::endl;
    }

    return window;
}

void InitShaders()
{
    // Load shaders and create the GLSL programs for the cube.
    lightingShader.AddShaderFromFile(GL_VERTEX_SHADER, LIGHTING_VERTEX_SHADER_PATH);
    lightingShader.AddShaderFromFile(GL_FRAGMENT_SHADER, LIGHTING_FRAGMENT_SHADER_PATH);
    lightingShader.CreateAndLinkProgram();

    // Load shaders and create the GLSL programs for the lamp.
    lampShader.AddShaderFromFile(GL_VERTEX_SHADER, LAMP_VERTEX_SHADER_PATH);
    lampShader.AddShaderFromFile(GL_FRAGMENT_SHADER, LAMP_FRAGMENT_SHADER_PATH);
    lampShader.CreateAndLinkProgram();

    // Create cube's Vertex Array Object and bind to it.
    glGenVertexArrays(1, &cubeVAO);
    glBindVertexArray(cubeVAO);

    // Create the Vertex Buffer Object and bind to it.
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(
            0,                      // vertex attribute to configure
            3,                      // size of the vertex attribute (vec3 has 3 values)
            GL_FLOAT,               // data is GL_FLOAT
            GL_FALSE,               // don't normalize data
            6 * sizeof(float),      // stride between consecutive attributes: XYZ for position and normals
            (void*)0);              // position data offset is 0
    glEnableVertexAttribArray(0);

    // Normal attribute
    glVertexAttribPointer(
            1,                      // vertex attribute to configure
            3,                      // size of the vertex attribute (vec3 has 3 values)
            GL_FLOAT,               // data is GL_FLOAT
            GL_FALSE,               // don't normalize data
            6 * sizeof(float),      // stride between consecutive attributes: XYZ for position and normals
            (void*)(3 * sizeof(float)));    // normal data offset is 3
    glEnableVertexAttribArray(1);

    // Create the lamp's Vertex Array Object for the lamp and bind to it.
    // Note we're still bound to the VBO from above -- the vertices are the same.
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);

    // Position attribute
    glVertexAttribPointer(
            0,                      // vertex attribute to configure
            3,                      // size of the vertex attribute (vec3 has 3 values)
            GL_FLOAT,               // data is GL_FLOAT
            GL_FALSE,               // don't normalize data
            6 * sizeof(float),      // stride between consecutive attributes: XYZ for position and (unused) normals
            (void*)0);              // position data offset is 0
    glEnableVertexAttribArray(0);
}

/**
 * Handle direction keys.
 */
void HandleDirectionalKeys(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

void GlfwFramebufferResizeCallback(GLFWwindow* window, int width, int height)
{
    std::cout << "Resize" << std::endl;
    glViewport(0, 0, width, height);
}

void GlfwWindowRefreshCallback(GLFWwindow* window)
{
    std::cout << "Refresh" << std::endl;
}

/**
 * Called whenever a key is pressed via GLFW.
 * Note we use this method for key presses only; HandleDirectionalKeys(...) offers smoother scrolling
 * for directional key presses.
 */
void GlfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    else if (key == GLFW_KEY_X && action == GLFW_PRESS) {
        camera.ResetToPosition(glm::vec3(0.0f, 0.0f, 6.0f));
    }
}

/**
 * Called whenever the mouse moves.
 */
void GlfwMousePositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (mouseCallbackNbr > 1) {
        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos;   // reversed since y-coordinates go from bottom to top

        lastX = xpos;
        lastY = ypos;

        camera.ProcessMouseMovement(xoffset, yoffset);
    }
    else if (mouseCallbackNbr == 1) {
        lastX = xpos;
        lastY = ypos;

        ++mouseCallbackNbr;
    }
    else {  // mouseCallbackNbr == 0
        // Debounce the first mouse position; approximately half of the time we get an incorrect
        // xpos and ypos. This causes the camera to ends up in the wrong location once the mouse
        // is moved.
        ++mouseCallbackNbr;
    }
}

void GlfwMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    // TODO
}

/**
 * Called whenever the mouse scroll wheel is used.
 */
void GLfwMouseScrollWheelCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseZoom(yoffset);
}

void GlfwErrorCallback(int error, const char* description)
{
    std::cerr << "GLFW error: " << description << " error code: " << std::endl;
}
