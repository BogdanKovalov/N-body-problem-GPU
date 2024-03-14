#pragma once

#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define CL_HPP_TARGET_OPENCL_VERSION  120
#include <Cl/cl2.hpp>
#include <fstream>

#include "Planet.h"
#include "N-Bodies.h"

class Kernel {
public:
	Kernel();
	Kernel(std::vector<Planet> const& planets, std::string fileName);
	Kernel(NBodies const& bodies, std::string fileName);
	void operator() (std::vector<Planet> const& planets);
	friend class NBodies;
private:
	cl::Program program_;
	cl::CommandQueue queue_;
	cl::Buffer accelerations_;
	cl::Buffer velocities_;
	cl::Buffer centers_;
	cl::Buffer masses_;
};