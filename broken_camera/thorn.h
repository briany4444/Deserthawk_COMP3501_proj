#ifndef THORN_H_
#define THORN_H_

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

    // Abstraction of a thorn
    class Thorn : public SceneNode {

    public:
        // Create thorn from given resources
        Thorn(const std::string name, const Resource* geometry, const Resource* material);

        // Destructor
        ~Thorn();

    }; // class Thorn

} // namespace game

#endif // THORN_H_
