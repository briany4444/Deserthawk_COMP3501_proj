#pragma once
#ifndef LIGHT_H_
#define LIGHT_H_

#include <string>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#define GLM_FORCE_RADIANS
#include <glm/gtc/quaternion.hpp>

#include "resource.h"
#include "scene_node.h"
#include <vector>


namespace game {

    class Light : public SceneNode {

    public:
        // Create asteroid from given resources
        Light(float, glm::vec3, const std::string name, const Resource* geometry, const Resource* material, const Resource* texture);

        // Destructor
        ~Light();

        // Update geometry configuration
        void Update(float) override;
        inline float GetSpecPwr() { return spec_pwr_; }

    private:
        glm::vec3 light_color_;
        float spec_pwr_;

    }; // class Light

} // namespace game

#endif // LIGHT_H_
