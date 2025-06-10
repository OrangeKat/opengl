#pragma once
#include <GL/glew.h>

GLuint loadShader(const char *path, GLenum type);
GLuint createProgram(const char *vertPath, const char *fragPath);
