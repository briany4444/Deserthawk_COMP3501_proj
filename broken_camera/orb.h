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
    class Orb : public SceneNode {

    public:
        // Create asteroid from given resources
        Orb(const std::string name, const Resource* geometry, const Resource* material, const Resource *tex);

        // Destructor
        ~Orb();
        void Update(float) override;

    }; // class Orb

} // namespace game

#endif // POWERUP_H_
