#include "Kernel.h"

Kernel::Kernel() : accelerations_(NULL), centers_(NULL), masses_(NULL) {}
Kernel::Kernel(std::vector<Planet> const& planets, std::string fileName) : accelerations_(NULL), centers_(NULL), masses_(NULL) {
	std::vector<cl::Platform> platforms;
	std::vector<cl::Device> devices;
	cl::Platform::get(&platforms);
	platforms.front().getDevices(CL_DEVICE_TYPE_ALL, &devices);
	cl::Context context(devices[0]);

	std::ifstream file;
	std::string fileCode;
	file.exceptions(std::ifstream::failbit);
	try {
		file.open(fileName);
		std::stringstream ss;
		ss << file.rdbuf();
		file.close();
		fileCode = ss.str();
	}
	catch (std::ifstream::failure e) {
		std::cout << "ERROR::KERNEL::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	};

	cl_int error = 0;
	cl::Program program(context, fileCode, CL_TRUE, &error);
	if (error != 0) {
		std::cout << "ERROR::KERNEL::PROGRAM::ERROR IN CREATING PROGRAM: " << error << std::endl;
	}
	program_ = program;

	auto size = planets.size();
	cl_float3* accelerations = new cl_float3[size];
	cl_float3* centers = new cl_float3[size];
	cl_float* masses = new cl_float[size];
	cl_float3* velocities = new cl_float3[size];
	for (int i = 0; i < 100; ++i) {
		accelerations[i] = {planets[i].acceleration_.x,  planets[i].acceleration_.y,  planets[i].acceleration_.z };
		velocities[i] = { planets[i].velocity_.x,  planets[i].velocity_.y,  planets[i].velocity_.z };
		centers[i] = { planets[i].center_.x, planets[i].center_.y, planets[i].center_.z };
		masses[i] = { planets[i].mass_ };
	}
	
	cl::Buffer accelerationsBuf(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(cl_float3) * size, accelerations, &error);
	if (error != 0) {
		std::cout << "ERROR::KERNEL::BUFFER::ERROR IN CREATING BUFFER: " << error << std::endl;
	}
	accelerations_ = accelerationsBuf;

	cl::Buffer velocitiesBuf(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(cl_float3) * size, velocities, &error);
	if (error != 0) {
		std::cout << "ERROR::KERNEL::BUFFER::ERROR IN CREATING BUFFER: " << error << std::endl;
	}
	velocities_ = velocitiesBuf;

	cl::Buffer centersBuf(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(cl_float3) * size, centers, &error);
	if (error != 0) {
		std::cout << "ERROR::KERNEL::BUFFER::ERROR IN CREATING BUFFER: " << error << std::endl;
	}
	centers_ = accelerationsBuf;

	cl::Buffer massesBuf(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(cl_float) * size, masses, &error);
	if (error != 0) {
		std::cout << "ERROR::KERNEL::BUFFER::ERROR IN CREATING BUFFER: " << error << std::endl;
	}
	masses_ = massesBuf;

	cl::CommandQueue queue(context, devices[0]);
	queue_ = queue;

	error = cl::enqueueReadBuffer(masses_, CL_TRUE, 0, sizeof(cl_float),masses);
	std::cout << error << std::endl;
}

Kernel::Kernel(NBodies const& bodies, std::string fileName) {
	std::vector<cl::Platform> platforms;
	std::vector<cl::Device> devices;
	cl::Platform::get(&platforms);
	platforms.front().getDevices(CL_DEVICE_TYPE_ALL, &devices);
	cl::Context context(devices[0]);

	std::ifstream file;
	std::string fileCode;
	file.exceptions(std::ifstream::failbit);
	try {
		file.open(fileName);
		std::stringstream ss;
		ss << file.rdbuf();
		file.close();
		fileCode = ss.str();
	}
	catch (std::ifstream::failure e) {
		std::cout << "ERROR::KERNEL::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	};

	cl_int error = 0;
	cl::Program program(context, fileCode, CL_TRUE, &error);
	if (error != 0) {
		std::cout << "ERROR::KERNEL::PROGRAM::ERROR IN CREATING PROGRAM: " << error << std::endl;
	}
	program_ = program;

	cl::Buffer accelerationsBuf(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(cl_float3) * bodies.N, bodies.accelerations_, &error);
	if (error != 0) {
		std::cout << "ERROR::KERNEL::BUFFER::ERROR IN CREATING BUFFER: " << error << std::endl;
	}
	accelerations_ = accelerationsBuf;

	cl::Buffer velocitiesBuf(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(cl_float3) * bodies.N, bodies.velocities_, &error);
	if (error != 0) {
		std::cout << "ERROR::KERNEL::BUFFER::ERROR IN CREATING BUFFER: " << error << std::endl;
	}
	velocities_ = velocitiesBuf;

	cl::Buffer centersBuf(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(cl_float3) * bodies.N, bodies.centers_, &error);
	if (error != 0) {
		std::cout << "ERROR::KERNEL::BUFFER::ERROR IN CREATING BUFFER: " << error << std::endl;
	}
	centers_ = centersBuf;

	cl::Buffer massesBuf(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(cl_float) * bodies.N, bodies.masses_, &error);
	if (error != 0) {
		std::cout << "ERROR::KERNEL::BUFFER::ERROR IN CREATING BUFFER: " << error << std::endl;
	}
	masses_ = massesBuf;

	cl::CommandQueue queue(context, devices[0]);
	queue_ = queue;
}

void Kernel::operator() (std::vector<Planet> const& planets) {
	cl::EnqueueArgs enqueue(queue_, cl::NDRange(planets.size()));
	cl::KernelFunctor<cl_int> functor(program_, "test");
	functor(enqueue, planets.size());
}