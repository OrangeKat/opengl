#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class FireShader {
private:
  GLuint shaderProgram;
  GLuint VAO, VBO, EBO;

  // Uniform locations
  GLint u_time_loc;
  GLint u_intensity_loc;
  GLint u_speed_loc;
  GLint u_octaves_loc;
  GLint u_scale_loc;

  // Fire parameters
  float intensity;
  float speed;
  int octaves;
  float scale;

  // Helper methods
  bool compileShader(GLuint shader, const char *source, const char *type);
  bool linkProgram();
  void setupGeometry();
  void getUniformLocations();

public:
  FireShader();
  ~FireShader();

  bool initialize();
  void render(float currentTime);
  void cleanup();

  // Parameter setters
  void setIntensity(float value);
  void setSpeed(float value);
  void setOctaves(int value);
  void setScale(float value);

  // Parameter getters
  float getIntensity() const;
  float getSpeed() const;
  int getOctaves() const;
  float getScale() const;
};
