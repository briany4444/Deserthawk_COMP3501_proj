#ifndef SPACESHIP_H_
#define SPACESHIP_H_

#include <string>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#define GLM_FORCE_RADIANS
#include <glm/gtc/quaternion.hpp>

#define ENEMY_SPEED 2
#include "resource.h"
#include "scene_node.h"
#include "player.h"

namespace game {

    // Abstraction of an asteroid
    class Spaceship : public SceneNode {

    public:
        // Create asteroid from given resources
        Spaceship(Player *c, const std::string name, const Resource* geometry, const Resource* material);

        // Destructor
        ~Spaceship();

        // Update geometry configuration
        void Update(float);

    private:
        float speed_;
        Player *player_;

    }; // class Spaceship

} // namespace game

#endif // Spaceship_H_

