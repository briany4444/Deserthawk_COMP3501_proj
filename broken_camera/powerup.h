#ifndef POWERUP_H_
#define POWERUP_H_

#include <string>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#define GLM_FORCE_RADIANS
#include <glm/gtc/quaternion.hpp>

#include "resource.h"
#include "scene_node.h"

namespace game {

    // Abstraction of an asteroid
    class Powerup : public SceneNode {

    public:
        // Create asteroid from given resources
        Powerup(const std::string name, const Resource* geometry, const Resource* material);

        // Destructor
        ~Powerup();

    }; // class Powerup

} // namespace game

#endif // POWERUP_H_
