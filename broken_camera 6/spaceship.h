#ifndef SPACESHIP_H_
#define SPACESHIP_H_

#include <string>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#define GLM_FORCE_RADIANS
#include <glm/gtc/quaternion.hpp>

#include "resource.h"
#include "scene_node.h"
#include "enemy.h"

namespace game {

    // Abstraction of an asteroid
    class Spaceship : public Enemy {

    public:
        // Create asteroid from given resources
        Spaceship(Player *c, const std::string name, const Resource* geometry, const Resource* material);

        // Destructor
        ~Spaceship();

        // Update geometry configuration
        void Update(float);

    }; 

} // namespace game

#endif // Spaceship_H_

