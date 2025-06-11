#include "fire_shader.hh"
#include <iostream>
#include <fstream>
#include <sstream>

FireShader::FireShader()
    : shaderProgram(0), VAO(0), VBO(0), EBO(0), intensity(1.5f), speed(1.0f),
      octaves(6), scale(3.0f), noise_type(0) {}

FireShader::~FireShader() { cleanup(); }

// Helper function to load shader from file
std::string loadShaderFromFile(const char* filePath) {
    std::string shaderCode;
    std::ifstream shaderFile;
    shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    
    try {
        shaderFile.open(filePath);
        std::stringstream shaderStream;
        shaderStream << shaderFile.rdbuf();
        shaderFile.close();
        shaderCode = shaderStream.str();
    } catch (std::ifstream::failure& e) {
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " 
                  << filePath << std::endl;
    }
    return shaderCode;
}

bool FireShader::compileShader(GLuint shader, const char *source,
                               const char *type) {
  glShaderSource(shader, 1, &source, NULL);
  glCompileShader(shader);

  int success;
  char infoLog[512];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    std::cerr << type << " shader compilation failed: " << infoLog << std::endl;
    return false;
  }
  return true;
}

bool FireShader::linkProgram() {
  glLinkProgram(shaderProgram);

  int success;
  char infoLog[512];
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    std::cerr << "Shader program linking failed: " << infoLog << std::endl;
    return false;
  }
  return true;
}

void FireShader::getUniformLocations() {
  u_time_loc = glGetUniformLocation(shaderProgram, "u_time");
  u_intensity_loc = glGetUniformLocation(shaderProgram, "u_intensity");
  u_speed_loc = glGetUniformLocation(shaderProgram, "u_speed");
  u_octaves_loc = glGetUniformLocation(shaderProgram, "u_octaves");
  u_scale_loc = glGetUniformLocation(shaderProgram, "u_scale");
  u_noise_type_loc = glGetUniformLocation(shaderProgram, "u_noise_type");
}

void FireShader::setupGeometry() {
  // Setup geometry (fullscreen quad)
  float vertices[] = {// positions          // texture coords
                      1.0f, 1.0f,  0.0f, 1.0f,  1.0f,  1.0f, -1.0f,
                      0.0f, 1.0f,  0.0f, -1.0f, -1.0f, 0.0f, 0.0f,
                      0.0f, -1.0f, 1.0f, 0.0f,  0.0f,  1.0f};

  unsigned int indices[] = {0, 1, 3, 1, 2, 3};

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  // Position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  // Texture coordinate attribute
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
}

bool FireShader::initialize() {
  // Load shaders from files
  std::string vertexCode = loadShaderFromFile("shaders/fire_vertex.glsl");
  std::string fragmentCode = loadShaderFromFile("shaders/fire_fragment.glsl");
  
  const char* vertexShaderSource = vertexCode.c_str();
  const char* fragmentShaderSource = fragmentCode.c_str();

  // Create and compile vertex shader
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  if (!compileShader(vertexShader, vertexShaderSource, "Vertex")) {
    glDeleteShader(vertexShader);
    return false;
  }

  // Create and compile fragment shader
  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  if (!compileShader(fragmentShader, fragmentShaderSource, "Fragment")) {
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return false;
  }

  // Create shader program
  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);

  if (!linkProgram()) {
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glDeleteProgram(shaderProgram);
    return false;
  }

  // Clean up shaders
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  // Get uniform locations and setup geometry
  getUniformLocations();
  setupGeometry();

  return true;
}

void FireShader::render(float currentTime) {
  glUseProgram(shaderProgram);

  // Update uniforms
  glUniform1f(u_time_loc, currentTime);
  glUniform1f(u_intensity_loc, intensity);
  glUniform1f(u_speed_loc, speed);
  glUniform1i(u_octaves_loc, octaves);
  glUniform1f(u_scale_loc, scale);
  glUniform1i(u_noise_type_loc, noise_type);

  // Render quad
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void FireShader::toggleNoiseType() {
  noise_type = 1 - noise_type; // Toggle between 0 and 1
}

void FireShader::cleanup() {
  if (VAO) {
    glDeleteVertexArrays(1, &VAO);
    VAO = 0;
  }
  if (VBO) {
    glDeleteBuffers(1, &VBO);
    VBO = 0;
  }
  if (EBO) {
    glDeleteBuffers(1, &EBO);
    EBO = 0;
  }
  if (shaderProgram) {
    glDeleteProgram(shaderProgram);
    shaderProgram = 0;
  }
}

// Parameter setters
void FireShader::setIntensity(float value) { intensity = value; }

void FireShader::setSpeed(float value) { speed = value; }

void FireShader::setOctaves(int value) { octaves = value; }

void FireShader::setScale(float value) { scale = value; }

// Parameter getters
float FireShader::getIntensity() const { return intensity; }

float FireShader::getSpeed() const { return speed; }

int FireShader::getOctaves() const { return octaves; }

float FireShader::getScale() const { return scale; }

int FireShader::getNoiseType() const { return noise_type; }
