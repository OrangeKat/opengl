#include "fire_shader.hh"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <iostream>

// Global fire shader pointer for callbacks
FireShader *g_fireShader = nullptr;

void printControls() {
  std::cout << "\n=== Procedural Fire Shader Demo ===" << std::endl;
  std::cout << "Fire Intensity: 1-4 keys (0.5x - 2.0x)" << std::endl;
  std::cout << "Fire Speed: Q-R keys (0.5x - 2.0x)" << std::endl;
  std::cout << "Noise Octaves: Z-C keys (3, 6, 8)" << std::endl;
  std::cout << "Fire Scale: A-D keys (2.0x - 4.0x)" << std::endl;
  std::cout << "Toggle Noise Type: N key" << std::endl;
  std::cout << "Reset to defaults: SPACE" << std::endl;
  std::cout << "Exit: ESC" << std::endl;
  std::cout << "================================\n" << std::endl;
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action,
                 int mods) {
  if (!g_fireShader)
    return;

  if (action == GLFW_PRESS || action == GLFW_REPEAT) {
    switch (key) {
    case GLFW_KEY_ESCAPE:
      glfwSetWindowShouldClose(window, true);
      break;

    // Fire Intensity
    case GLFW_KEY_1:
      g_fireShader->setIntensity(0.5f);
      std::cout << "Fire intensity: 0.5x" << std::endl;
      break;
    case GLFW_KEY_2:
      g_fireShader->setIntensity(1.0f);
      std::cout << "Fire intensity: 1.0x" << std::endl;
      break;
    case GLFW_KEY_3:
      g_fireShader->setIntensity(1.5f);
      std::cout << "Fire intensity: 1.5x" << std::endl;
      break;
    case GLFW_KEY_4:
      g_fireShader->setIntensity(2.0f);
      std::cout << "Fire intensity: 2.0x" << std::endl;
      break;

    // Fire Speed
    case GLFW_KEY_Q:
      g_fireShader->setSpeed(0.5f);
      std::cout << "Fire speed: 0.5x" << std::endl;
      break;
    case GLFW_KEY_W:
      g_fireShader->setSpeed(1.0f);
      std::cout << "Fire speed: 1.0x" << std::endl;
      break;
    case GLFW_KEY_E:
      g_fireShader->setSpeed(1.5f);
      std::cout << "Fire speed: 1.5x" << std::endl;
      break;
    case GLFW_KEY_R:
      g_fireShader->setSpeed(2.0f);
      std::cout << "Fire speed: 2.0x" << std::endl;
      break;

    // Noise Octaves
    case GLFW_KEY_Z:
      g_fireShader->setOctaves(3);
      std::cout << "Noise octaves: 3" << std::endl;
      break;
    case GLFW_KEY_X:
      g_fireShader->setOctaves(6);
      std::cout << "Noise octaves: 6" << std::endl;
      break;
    case GLFW_KEY_C:
      g_fireShader->setOctaves(8);
      std::cout << "Noise octaves: 8" << std::endl;
      break;

    // Fire Scale
    case GLFW_KEY_A:
      g_fireShader->setScale(2.0f);
      std::cout << "Fire scale: 2.0x" << std::endl;
      break;
    case GLFW_KEY_S:
      g_fireShader->setScale(3.0f);
      std::cout << "Fire scale: 3.0x" << std::endl;
      break;
    case GLFW_KEY_D:
      g_fireShader->setScale(4.0f);
      std::cout << "Fire scale: 4.0x" << std::endl;
      break;

    // Toggle noise type
    case GLFW_KEY_N:
      g_fireShader->toggleNoiseType();
      std::cout << "Noise type: " 
                << (g_fireShader->getNoiseType() == 0 ? "Simplex" : "Perlin")
                << std::endl;
    break;

    // Reset to defaults
    case GLFW_KEY_SPACE:
      g_fireShader->setIntensity(1.5f);
      g_fireShader->setSpeed(1.0f);
      g_fireShader->setOctaves(6);
      g_fireShader->setScale(3.0f);
      std::cout << "Reset to default values" << std::endl;
      break;
    }
  }
}

void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

void errorCallback(int error, const char *description) {
  std::cerr << "GLFW Error " << error << ": " << description << std::endl;
}

bool initializeOpenGL() {
  // Set error callback
  glfwSetErrorCallback(errorCallback);

  // Initialize GLFW
  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW" << std::endl;
    return false;
  }

  return true;
}

GLFWwindow *createWindow() {
  // Configure GLFW
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  // Create window
  GLFWwindow *window =
      glfwCreateWindow(800, 600, "Procedural Fire Shader", NULL, NULL);
  if (!window) {
    std::cerr << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return nullptr;
  }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  return window;
}

bool initializeGLEW() {
  // Initialize GLEW
  GLenum err = glewInit();
  if (err != GLEW_OK) {
    std::cerr << "Failed to initialize GLEW: " << glewGetErrorString(err)
              << std::endl;
    return false;
  }

  return true;
}

void setupOpenGLState() {
  // Enable blending for fire transparency
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Enable multisampling if available
  glEnable(GL_MULTISAMPLE);

  // Print OpenGL information
  std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
  std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION)
            << std::endl;
  std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
}

int main() {
  // Initialize OpenGL context
  if (!initializeOpenGL()) {
    return -1;
  }

  // Create window
  GLFWwindow *window = createWindow();
  if (!window) {
    return -1;
  }

  // Initialize GLEW
  if (!initializeGLEW()) {
    glfwTerminate();
    return -1;
  }

  // Setup OpenGL state
  setupOpenGLState();

  // Create and initialize fire shader
  FireShader fireShader;
  g_fireShader = &fireShader;

  if (!fireShader.initialize()) {
    std::cerr << "Failed to initialize fire shader" << std::endl;
    glfwTerminate();
    return -1;
  }

  // Setup callbacks
  glfwSetKeyCallback(window, keyCallback);
  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

  // Print controls
  printControls();

  // Main render loop
  double lastTime = glfwGetTime();
  int frameCount = 0;

  while (!glfwWindowShouldClose(window)) {
    // Calculate FPS
    double currentTime = glfwGetTime();
    frameCount++;
    if (currentTime - lastTime >= 1.0) {
      std::cout << "FPS: " << frameCount << std::endl;
      frameCount = 0;
      lastTime = currentTime;
    }

    // Get window size for viewport
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    // Clear screen
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Render fire
    fireShader.render(static_cast<float>(currentTime));

    // Swap buffers and poll events
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // Cleanup
  fireShader.cleanup();
  glfwTerminate();

  return 0;
}
