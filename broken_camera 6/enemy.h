#ifndef ENEMY_H_
#define ENEMY_H_

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

namespace game {

    class Enemy : public SceneNode {

    public:
        Enemy(Player *c, const std::string name, const Resource* geometry, const Resource* material);

        ~Enemy();

        void Update(float);

    protected:
        float speed_;
        Player *player_;

    }; 

} // namespace game

#endif 

