#include "beacon.h"

namespace game {

    Beacon::Beacon(const std::string name, const Resource* geometry, const Resource* material) : SceneNode(name, geometry, material) {
        collidable_ = true;
        radius_ = 1.0f;
        type_ = "Beacon";
    }


    Beacon::~Beacon() {
    }



} // namespace game
