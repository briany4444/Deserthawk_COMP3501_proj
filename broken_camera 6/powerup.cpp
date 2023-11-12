#include "powerup.h"

namespace game {

    Powerup::Powerup(const std::string name, const Resource* geometry, const Resource* material) : SceneNode(name, geometry, material) {
        collidable_ = true;
        radius_ = 0.5f;
        type_ = "Powerup";
        node_type_ = Obj_t.collectable;
    }


    Powerup::~Powerup() {
    }



} // namespace game
