#include "orb.h"

namespace game {

    Orb::Orb(const std::string name, const Resource* geometry, const Resource* material, const Resource* tex) : SceneNode(name, geometry, material, tex) {
        collidable_ = true;
        radius_ = 0.5f;
        type_ = "Orb";
    }


    Orb::~Orb() {
    }



} // namespace game
