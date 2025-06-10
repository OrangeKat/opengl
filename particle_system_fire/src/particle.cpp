#include "particle.hh"
#include <cmath>
#include <cstdlib>

void initParticle(Particle &p) {
  p.active = true;
  p.maxLife = 0.5f + (rand() % 100) / 100.0f * 1.0f; // 1.5-3.5 seconds
  p.life = p.maxLife;

  // Start from base of fire with slight spread
  float baseSpread = 2.0f;
  p.position = glm::vec3(((rand() % 100) / 100.0f - 0.5f) * baseSpread,
                         -1.3f + (rand() % 100) /
                                     500.0f, // Slight height variation at base
                         ((rand() % 100) / 100.0f - 0.5f) * baseSpread);

  // Initial upward velocity with randomness
  float upwardForce = 1.2f + (rand() % 100) / 200.0f;
  p.velocity =
      glm::vec3(((rand() % 100) / 100.0f - 0.5f) * 0.3f, // Horizontal spread
                upwardForce,                             // Strong upward motion
                ((rand() % 100) / 100.0f - 0.5f) * 0.3f);

  p.acceleration = glm::vec3(0.0f, -0.5f, 0.0f); // Gravity + buoyancy

  p.initialSize = 0.08f + (rand() % 100) / 1000.0f;
  p.size = p.initialSize;

  // Temperature affects initial color (hotter = more white/yellow)
  p.temperature = 0.8f + (rand() % 100) / 500.0f;

  // Start with hot colors
  float r = 1.0f;
  float g = 0.3f + p.temperature * 0.7f;
  float b = p.temperature > 0.9f ? 0.2f : 0.0f;
  p.color = glm::vec4(r, g, b, 1.0f);

  p.turbulence = (rand() % 100) / 100.0f;
}

glm::vec3 getWindForce(const glm::vec3 &pos, float time) {
  // Simulate rising hot air and wind turbulence
  float windX = sin(time * 2.0f + pos.y * 3.0f) * 0.2f;
  float windZ = cos(time * 1.5f + pos.y * 2.0f) * 0.15f;
  float risingAir = (pos.y + 1.0f) * 0.3f; // Stronger rising air higher up

  return glm::vec3(windX, risingAir, windZ);
}

void updateParticle(Particle &p, float dt) {
  if (!p.active || p.life <= 0.0f) {
    initParticle(p);
    return;
  }

  p.life -= dt;
  float lifeRatio = p.life / p.maxLife;

  // Apply wind and turbulence
  static float globalTime = 0.0f;
  globalTime += dt;

  glm::vec3 windForce = getWindForce(p.position, globalTime);
  glm::vec3 turbulenceForce =
      glm::vec3(sin(globalTime * 5.0f + p.turbulence * 10.0f) * 0.1f, 0.0f,
                cos(globalTime * 4.0f + p.turbulence * 8.0f) * 0.1f);

  // Total acceleration includes gravity, buoyancy, wind, and turbulence
  p.acceleration = glm::vec3(0.0f, -0.2f, 0.0f) + windForce + turbulenceForce;

  // Update physics
  p.velocity += p.acceleration * dt;
  p.position += p.velocity * dt;

  // Size grows as particle rises and cools
  p.size = p.initialSize * (1.0f + (1.0f - lifeRatio) * 2.0f);

  // Temperature decreases over time
  p.temperature = lifeRatio * 0.9f + 0.1f;

  // Color transition: White/Yellow -> Orange -> Red -> Dark Red -> Transparent
  float r, g, b, a;

  if (lifeRatio > 0.7f) {
    // Hot phase: white/yellow
    r = 1.0f;
    g = 0.8f + p.temperature * 0.2f;
    b = p.temperature > 0.8f ? 0.4f : 0.0f;
  } else if (lifeRatio > 0.4f) {
    // Orange phase
    r = 1.0f;
    g = 0.4f + (lifeRatio - 0.4f) / 0.3f * 0.4f;
    b = (lifeRatio - 0.4f) / 0.3f * 0.1f;
  } else if (lifeRatio > 0.2f) {
    // Red phase
    r = 0.8f + (lifeRatio - 0.2f) / 0.2f * 0.2f;
    g = (lifeRatio - 0.2f) / 0.2f * 0.3f;
    b = 0.0f;
  } else {
    // Dark red/smoke phase
    float fadeRatio = lifeRatio / 0.2f;
    r = 0.3f * fadeRatio;
    g = 0.1f * fadeRatio;
    b = 0.1f * fadeRatio;
  }

  // Alpha fades out at the end
  a = lifeRatio < 0.3f ? lifeRatio / 0.3f : 1.0f;
  a *= 0.8f; // Overall transparency for blending

  p.color = glm::vec4(r, g, b, a);
}
