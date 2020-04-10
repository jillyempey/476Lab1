//
// sueda - geometry edits Z. Wood
// 3/16
//

#include <iostream>
#include "Particle.h"
#include "GLSL.h"
#include "MatrixStack.h"
#include "Program.h"
#include "Texture.h"


float randFloat(float l, float h)
{
	float r = rand() / (float) RAND_MAX;
	return (1.0f - r) * l + r * h;
}

void Particle::load(float x1, float y1, float z1)
{
	
    // Random initialization
    rebirth(0.0f, x1, y1, z1);
}

// all particles born at the origin
void Particle::rebirth(float t, float x1, float y1, float z1)
{
	charge = randFloat(0.0f, 1.0f) < 0.5f ? -1.0f : 1.0f;
	m = 1.0f;
	d = randFloat(0.0f, 0.2f);
	x.x = randFloat(x1 + -2.f, x1 + 2.f);;
	x.y = y1;
	x.z = randFloat(z1 + -2.f, z1 + 2.f);
    v.x = randFloat(-0.1f, 0.1f);
    v.y = randFloat(-0.1f, 0.1f);
    v.z = randFloat(-0.1f, 0.1f);
	lifespan = randFloat(5.f, 15.f);
	tEnd = t + lifespan;

	scale = randFloat(1.4f, 2.6f);
	color.r = 1;
	color.g = 0.55294117;
	color.b = 0.69607843;
	color.a = 1.0f;
}

void Particle::update(float t, float h, const vec3 &g, const bool *keyToggles, float x1, float y1, float z1)
{
	if (t > tEnd)
	{
		rebirth(t, x1, y1, z1);
	}
	// very simple update
	x += h * v;
	color.a = (tEnd - t) / lifespan;
    v.y -= 0.005;
}
