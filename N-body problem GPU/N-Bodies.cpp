#include "N-Bodies.h"

NBodies::NBodies() : N(0), accelerations_(NULL), centers_(NULL), velocities_(NULL), masses_(NULL), VAO_(0) {};

NBodies::NBodies(std::vector<Planet> const& planets) : accelerations_(new cl_float3[planets.size()]), centers_(new cl_float3[planets.size()]),
velocities_(new cl_float3[planets.size()]), masses_(new cl_float[planets.size()]), N(planets.size()), VAO_(planets[0].VAO_) {
	auto size = planets.size();
	for (size_t i = 0; i < size; ++i) {
		centers_[i] = { planets[i].center_.x, planets[i].center_.y, planets[i].center_.z };
		velocities_[i] = { planets[i].velocity_.x, planets[i].velocity_.y, planets[i].velocity_.z };
		accelerations_[i] = { planets[i].acceleration_.x, planets[i].acceleration_.y, planets[i].acceleration_.z };
		masses_[i] = { planets[i].mass_ };
	}
}

void NBodies::LoadBuffers(Kernel& kernel) {
	int error;
	error = cl::enqueueWriteBuffer(kernel.accelerations_, CL_TRUE, 0, N * sizeof(cl_float3), accelerations_);
	if (error != 0) std::cout << "ERORR::NBODIES::ERROR IN WRITING ACCELERATIONS: " <<  error << std::endl;

	error = cl::enqueueWriteBuffer(kernel.velocities_, CL_TRUE, 0, N * sizeof(cl_float3), velocities_);
	if (error != 0) std::cout << "ERORR::NBODIES::ERROR IN WRITING VELOCITIES:" << error << std::endl;

	error = cl::enqueueWriteBuffer(kernel.centers_, CL_TRUE, 0, N * sizeof(cl_float3), centers_);
	if (error != 0) std::cout << "ERORR::NBODIES::ERROR IN WRITING CENTERS: "<< error << std::endl;
}

void NBodies::ReadBuffers(Kernel& kernel) {
	int error;
	error = cl::enqueueReadBuffer(kernel.accelerations_, CL_TRUE, 0, N * sizeof(cl_float3), accelerations_);
	if (error != 0) std::cout << "ERROR::NBODIES::ERROR IN READING ACCELERATIONS: " << error << std::endl;

	error = cl::enqueueReadBuffer(kernel.velocities_, CL_TRUE, 0, N * sizeof(cl_float3), velocities_);
	if (error != 0) std::cout << "ERROR::NBODIES::ERROR IN READING VELOCITIES: " << error << std::endl;

	error = cl::enqueueReadBuffer(kernel.centers_, CL_TRUE, 0, N * sizeof(cl_float3), centers_);
	if (error != 0) std::cout << "ERROR::NBODIES::ERROR IN READING CENTERS: " << error << std::endl;
}

void NBodies::Draw(Shader const& shader) const {
	float width = 1080.0f;
	float height = 1920.0f;

	shader.Use();
	GLint transformLoc = glGetUniformLocation(shader.Program, "transform");
	GLint colorTransformLoc = glGetUniformLocation(shader.Program, "colorTransform");
	float length = 0;
	glBindVertexArray(VAO_); 
	glm::mat4 transform = glm::mat4(1.0f);
	for (size_t i = 0; i < N; ++i) {
		length = sqrt(pow(velocities_[i].x, 2) + pow(velocities_[i].y, 2))/100;
		transform = glm::scale(transform, glm::vec3(width / height, 1.0f, 1.0f));
		transform = glm::translate(transform, glm::vec3(width / height * centers_[i].x, width / height * centers_[i].y, centers_[i].z));

		for (int i = 0; i < 10; ++i) {
			transform = glm::rotate(transform, glm::radians(36.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniform1f(colorTransformLoc, length);
			glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
			glDrawArrays(GL_TRIANGLES, 0, 3);
		}
		transform = glm::mat4(1.0f);
	}
}

void NBodies::Update(Kernel & kernel) {
	LoadBuffers(kernel);

	int error = 0;
	cl::EnqueueArgs eargs(kernel.queue_, cl::NDRange(N));

	cl::KernelFunctor<cl_int, cl::Buffer, cl::Buffer, cl::Buffer, cl::Buffer> functor(kernel.program_, "test", &error);
	if (error != 0) std::cout << "Functor: " << error << std::endl;
	functor(eargs, N, kernel.accelerations_, kernel.centers_, kernel.velocities_, kernel.masses_);
	if (error != 0) std::cout << "NBODIES::ERROR::UPDATE::CALLING FUNCTOR: " << error << std::endl;

	ReadBuffers(kernel);
}

float NBodies::GetImpulse() const {
	float impulse = 0;
	for (int i = 0; i < N; ++i) {
		impulse += masses_[i] * sqrt(pow(velocities_[i].x, 2) + pow(velocities_[i].y, 2));
	}
	return impulse;
}

NBodies::~NBodies() {
	delete[] accelerations_;
	delete[] velocities_;
	delete[] centers_;
	delete[] masses_;
}