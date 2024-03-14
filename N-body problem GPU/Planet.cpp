#include "Planet.h"
static float const G = 6.67f;
Planet::Planet(float mass, float radius, glm::vec3 center, glm::vec3 velocity) : mass_(mass), radius_(radius),
center_(center), velocity_(velocity), acceleration_(glm::vec3(0.0f)) {
	BufferInit();
}

Planet::Planet(Planet const& other) {
	center_ = other.center_;
	velocity_ = other.velocity_;
	mass_ = other.mass_;
	radius_ = other.radius_;
	BufferInit();
}

Planet Planet::operator= (Planet const& other) {
	if (this != &other) {
		glDeleteVertexArrays(1, &VAO_);
		center_ = other.center_;
		velocity_ = other.velocity_;
		mass_ = other.mass_;
		radius_ = other.radius_;
		BufferInit();
	}
	return *this;
}

void Planet::Draw(Shader const& shader) const {
	float width = 1080.0f;
	float height = 1920.0f;

	shader.Use();
	GLint transformLoc = glGetUniformLocation(shader.Program, "transform");
	//GLint colorTransformLoc = glGetUniformLocation(shader.Program, "colorTransform");
	//auto length = glm::length(velocity_);
	//auto temp = glm::vec1(length);
	glBindVertexArray(VAO_);

	glm::mat4 transform = glm::mat4(1.0f);
	transform = glm::scale(transform, glm::vec3(width / height, 1.0f, 1.0f));
	transform = glm::translate(transform, glm::vec3(width / height * center_.x, width / height * center_.y, center_.z));

	for (int i = 0; i < 10; ++i) {
		transform = glm::rotate(transform, glm::radians(36.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		//glUniform1f(colorTransformLoc, length);
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}
}

void Update(std::vector<Planet>& planets, Shader const& shader) {
	auto size = planets.size();
	for (int i = 0; i < size; ++i) {
		planets[i].Draw(shader);
	}
	for (int i = 0; i < size; ++i) {
		planets[i].UpdateAcceleration(planets);
	}
	for (int i = 0; i < size; ++i) {
		planets[i].UpdateCenter(planets, i);
	}
}

void Planet::UpdateCenter(std::vector<Planet>& others, size_t num) {
	auto x0 = Solve(others, num);
	center_.x = x0[0];
	center_.y = x0[1];
	velocity_.x = x0[2];
	velocity_.y = x0[3];
	acceleration_.x = x0[4];
	acceleration_.y = x0[5];
}
void Planet::BufferInit() {
	GLfloat vertex[] = {
		0.0f, 0.0f,0.0f,
		radius_, 0.0f,0.0f,
		radius_ * glm::cos(glm::radians(36.0f)), radius_ * glm::sin(glm::radians(36.0f)), 0.0f
	};
	GLuint VBO;
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO_);
	glBindVertexArray(VAO_);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glDeleteBuffers(1, &VBO);
}


double Planet::GetImpulse() const {
	return mass_ * glm::length(velocity_);
}
Planet::~Planet() {
	glDeleteVertexArrays(1, &VAO_);
}