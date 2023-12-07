#include "light.h"
#include <iostream>
#include <time.h>
#include <sstream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace game {

    Light::Light(float s_pwr, glm::vec3 col, const std::string name, const Resource* geometry, const Resource* material, const Resource* texture) : SceneNode(name, geometry, material, texture) {
        light_color_ = col;
        spec_pwr_ = s_pwr;
        orbit_angle_ = 0;
    }


    Light::~Light() {
    }

    void Light::Update(float delta_time) {
        Orbit(delta_time * 0.5);
    }

} // namespace game
