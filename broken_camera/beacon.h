#ifndef BEACON_H_
#define BEACON_H_

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
    class Beacon : public SceneNode {

        public:
            // Create asteroid from given resources
            Beacon(const std::string name, const Resource *geometry, const Resource *material);

            // Destructor
            ~Beacon();

    }; // class Beacon

} // namespace game

#endif // BEACON_H_
