#include "orb.h"

namespace game {

    Orb::Orb(const std::string name, const Resource* geometry, const Resource* material, const Resource* tex) : SceneNode(name, geometry, material, tex) {
        collidable_ = true;
        radius_ = 30.0f;
        type_ = "Orb";
    }


    Orb::~Orb() {
    }


    // rotates the orb's rings
    void Orb::Update(float delta_time) {
        glm::vec3 axis[3] = {glm::vec3(1, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1)};
        for (int i = 0; i < children_.size(); i++) {
            glm::quat q = glm::normalize(glm::angleAxis(delta_time, axis[i]));
            children_[i]->Rotate(q);
        }

        SceneNode::Update(delta_time);
    }



} // namespace game
