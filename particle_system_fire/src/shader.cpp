#include "shader.hh"
#include <fstream>
#include <iostream>
#include <sstream>

GLuint loadShader(const char *path, GLenum type) {
  std::ifstream file(path);
  std::stringstream ss;
  ss << file.rdbuf();
  std::string src = ss.str();
  const char *code = src.c_str();

  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &code, nullptr);
  glCompileShader(shader);

  GLint success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    char log[512];
    glGetShaderInfoLog(shader, 512, nullptr, log);
    std::cerr << "Shader compile error: " << log << std::endl;
  }

  return shader;
}

GLuint createProgram(const char *vertPath, const char *fragPath) {
  GLuint vs = loadShader(vertPath, GL_VERTEX_SHADER);
  GLuint fs = loadShader(fragPath, GL_FRAGMENT_SHADER);
  GLuint program = glCreateProgram();
  glAttachShader(program, vs);
  glAttachShader(program, fs);
  glLinkProgram(program);

  GLint success;
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success) {
    char log[512];
    glGetProgramInfoLog(program, 512, nullptr, log);
    std::cerr << "Shader link error: " << log << std::endl;
  }

  glDeleteShader(vs);
  glDeleteShader(fs);
  return program;
}
