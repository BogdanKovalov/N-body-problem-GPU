#include "ODE.h"

#include <boost/numeric/odeint.hpp>

typedef boost::numeric::odeint::runge_kutta_dopri5<state_type> rkd5;
typedef boost::numeric::odeint::controlled_runge_kutta<rkd5> crk;

static float const G = 6.67f;
state_type Planet::Solve(std::vector<Planet>& others, size_t num) {
    state_type x0 = { center_.x, center_.y, velocity_.x, velocity_.y,acceleration_.x, acceleration_.y };
    auto System = [&others, &num](const state_type& x, state_type& dxdt, const double t) {
        dxdt[0] = x[2];
        dxdt[1] = x[3];
        dxdt[2] = x[4];
        dxdt[3] = x[5];
    };
    auto Observer = [](const state_type& x, const double t) {
        std::cout << x[4] << " " << x[5] << std::endl;
    };
    double t0 = 0.0;
    double t1 = 0.001;
    double dt = 0.00001;
    boost::numeric::odeint::integrate_adaptive(crk(), System, x0, t0, t1, dt);
    return x0;
}

void Planet::UpdateAcceleration(std::vector<Planet>& others) {
    auto size = others.size();
    float distance = 0;
    acceleration_ = glm::vec3(0.0f);
    for (int i = 0; i < size; ++i) {
        if (this != &others[i]) {
            distance = glm::length(center_ - others[i].center_);
            if (distance > 0.05)  acceleration_ += others[i].mass_ * (others[i].center_ - center_) / (float)pow(abs(distance), 3);
        }
    }
    acceleration_ *= G;
}