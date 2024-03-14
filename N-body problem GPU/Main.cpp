

//#include "Shader.h"
#include "Planet.h"
#include "ODE.h"
#include "Kernel.h"
#include "N-Bodies.h"
#include <random>

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

auto Init() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    auto window = glfwCreateWindow(1920, 1080, "Triangle", NULL, NULL);
    if (!window) {
        std::cout << "Failed to create window" << std::endl;
        glfwTerminate();
    }
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cout << "Failed to initialize GLEW" << std::endl;
    }
    int height, width;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glfwSetKeyCallback(window, key_callback);
    return window;
}

double getRandomNumber(double const& min, double const& max)
{
    std::random_device random_device;
    std::mt19937 generator(random_device());
    std::uniform_real_distribution<> distribution(min, max);
    return distribution(generator);
}

int main()
{
    auto window = Init();
    int width = 1080;
    int height = 1920;
    glfwGetWindowSize(window, &width, &height);
    Shader myShader(".\\Shaders\\vertex.v", ".\\Shaders\\fragment.frag");

    std::vector<Planet> planets;
    auto tempVec = glm::vec3((float)glm::cos(glm::radians((float)36 * 0)), (float)glm::sin(glm::radians((float)36 * 0)), 0.0f);
    auto tempVel = glm::vec3(0.0f, 0.0f, 0.0f);
    float x = 0;
    float y = 0;
    float radius = 0;
    float omega = 50.0f;
    int counter = 0;
    float v = 0;
    while (counter != 800) {
        x = getRandomNumber(-1.0f, 1.0f);
        y = getRandomNumber( -1.0f, 1.0f);
        radius = pow(x, 2)*2 + pow(y, 2);
        if (radius < 1.0f) {
            tempVec = glm::vec3(x - 0.75f, y, 0.0f);
            v = omega * sqrt(radius);
            if ((x > 0 && y > 0) || (x < 0 && y > 0)) tempVel = glm::vec3(v * cos(atan(-2*x / y)), v * sin(atan(-2*x / y)), 0.0f);
            else tempVel = glm::vec3(-v * cos(atan(-2*x / y)), -v * sin(atan(-2*x / y)), 0.0f);
            planets.push_back(Planet(0.6f, 0.125f / 10, tempVec, tempVel));
            ++counter;
        }
    }
    counter = 0;
    while (counter != 800) {
        x = getRandomNumber(-1.0f, 1.0f);
        y = getRandomNumber(-1.0f, 1.0f);
        radius = pow(x, 2) + pow(y, 2)*2;
        if (radius < 1.0f) {
            tempVec = glm::vec3(x + 0.75f, y, 0.0f);
            v = omega * sqrt(radius);
            if ((x > 0 && y > 0) || (x < 0 && y > 0)) tempVel = glm::vec3(v * cos(atan(- x / (y*2))),v * sin(atan(- x / (y*2))), 0.0f);
            else tempVel = glm::vec3(-v * cos(atan(- x / (y*2))), -v * sin(atan(- x / (y*2))), 0.0f);
            planets.push_back(Planet(0.6f, 0.125f / 10, tempVec, tempVel));
            ++counter;
        }
    }
    planets.push_back(Planet(0.6 * 50, 0.125f / 4, glm::vec3(-0.75f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f)));
    planets.push_back(Planet(0.6 * 50, 0.125f / 4, glm::vec3(0.75f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f)));
    NBodies nPlanets(planets);
    Kernel kernel(nPlanets, "./Kernels/kernel.cl");
    auto size = planets.size();
    double startImpulse = nPlanets.GetImpulse();
    double stopImpulse = 0.0;
    std::cout << "Size: " << size << std::endl;
    auto Time = glfwGetTime();
    auto startUpdateTime = glfwGetTime();
    while (!glfwWindowShouldClose(window) && glfwGetTime() < 600){
        glfwPollEvents();

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        Time = glfwGetTime();
        nPlanets.Draw(myShader);

        startUpdateTime = glfwGetTime();
        nPlanets.Update(kernel);

        glfwSwapBuffers(window);
    }
    stopImpulse = nPlanets.GetImpulse();
    std::cout << glfwGetTime() << std::endl;
    std::cout << "dImpulse: " << (stopImpulse - startImpulse) / startImpulse << std::endl;
    glfwTerminate();
    return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}