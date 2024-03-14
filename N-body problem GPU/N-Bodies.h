#pragma once
#ifndef _N_BODIES_H
#define _N_BODIES_H

#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define CL_HPP_TARGET_OPENCL_VERSION  120
#include <CL/cl2.hpp>

#include "Planet.h"
#include "Kernel.h"

class NBodies {
public:
	NBodies();
	NBodies(std::vector<Planet> const& planets);
	void LoadBuffers(Kernel& kernel);
	void ReadBuffers(Kernel& kernel);
	void Update(Kernel& kernel);
	void Draw(Shader const& shader) const;
	float GetImpulse() const;
	friend class Kernel;
	~NBodies();
private:
	cl_float3* centers_;
	cl_float3* accelerations_;
	cl_float3* velocities_;
	cl_float* masses_;
	GLuint VAO_;
	size_t N;
};

#endif _N_BODIES_H