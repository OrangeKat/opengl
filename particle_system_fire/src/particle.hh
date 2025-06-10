#pragma once
#include <glm/glm.hpp>

struct Particle {
  glm::vec3 position;
  glm::vec3 velocity;
  glm::vec3 acceleration;
  float life;
  float maxLife;
  float size;
  float initialSize;
  glm::vec4 color;
  float temperature;
  float turbulence;
  bool active;
};

void initParticle(Particle &p);
void updateParticle(Particle &p, float dt);
glm::vec3 getWindForce(const glm::vec3 &pos, float time);
