#pragma once
#ifndef _PLANET_H
#define _PLANET_H

#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define CL_HPP_TARGET_OPENCL_VERSION  120

#include<vector>
#include<iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"

typedef std::vector<float> state_type;

class Planet {
public:
	Planet(float mass, float radius, glm::vec3 center, glm::vec3 velocity);
	Planet(Planet const& other);
	Planet operator= (Planet const& other);
	double GetImpulse() const;
	~Planet();
	friend void Update(std::vector<Planet>& planets, Shader const& shader);
	friend auto LambdaUpdateAcceleration(const state_type& x, std::vector<Planet>& others, size_t num);
	friend class Kernel;
	friend class NBodies;
private:
	float mass_;
	float radius_;
	glm::vec3 center_;
	glm::vec3 velocity_;
	glm::vec3 acceleration_;
	GLuint VAO_;
	state_type Solve(std::vector<Planet>& others, size_t num);
	void UpdateAcceleration(std::vector<Planet>& others);
	void UpdateCenter(std::vector<Planet>& others, size_t num);
	void Draw(Shader const& shader) const;
	void BufferInit();
};
#endif _PLANET_H