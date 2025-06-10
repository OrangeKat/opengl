#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "particle.hh"
#include "shader.hh"
#include <cstdlib>
#include <ctime>
#include <vector>

#define PARTICLE_COUNT 5000 // Increased for denser fire

int main() {
  srand((unsigned)time(0));
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  GLFWwindow *win = glfwCreateWindow(
      1200, 900, "Realistic Fire Particle System", nullptr, nullptr);
  glfwMakeContextCurrent(win);

  glewExperimental = GL_TRUE;
  glewInit();

  GLuint shader = createProgram("shaders/shader.vert", "shaders/shader.frag");

  std::vector<Particle> particles(PARTICLE_COUNT);
  for (auto &p : particles)
    initParticle(p);

  GLuint vao, vbo;
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, particles.size() * sizeof(Particle), nullptr,
               GL_DYNAMIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(Particle),
                        (void *)(offsetof(Particle, size)));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Particle),
                        (void *)(offsetof(Particle, color)));
  glEnableVertexAttribArray(2);

  // Enhanced blending for more realistic fire
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_PROGRAM_POINT_SIZE);
  glClearColor(0.02f, 0.02f, 0.05f, 1.0f); // Dark background

  glm::mat4 projection =
      glm::perspective(glm::radians(45.0f), 1200.f / 900.f, 0.1f, 100.f);
  glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, -0.5f, -4));

  double lastTime = glfwGetTime();

  while (!glfwWindowShouldClose(win)) {
    double currentTime = glfwGetTime();
    float deltaTime = float(currentTime - lastTime);
    lastTime = currentTime;

    glfwPollEvents();
    glClear(GL_COLOR_BUFFER_BIT);

    // Update particles with actual delta time for smooth animation
    for (auto &p : particles)
      updateParticle(p, deltaTime);

    glBufferSubData(GL_ARRAY_BUFFER, 0, particles.size() * sizeof(Particle),
                    particles.data());

    glUseProgram(shader);
    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE,
                       &projection[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE,
                       &view[0][0]);

    glBindVertexArray(vao);
    glDrawArrays(GL_POINTS, 0, PARTICLE_COUNT);

    glfwSwapBuffers(win);
  }

  glDeleteBuffers(1, &vbo);
  glDeleteVertexArrays(1, &vao);
  glfwDestroyWindow(win);
  glfwTerminate();
  return 0;
}
