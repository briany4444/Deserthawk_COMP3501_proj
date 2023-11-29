#include "thorn.h"
#include <iostream>

namespace game {

    Thorn::Thorn(const std::string name, const Resource* geometry, const Resource* material) : SceneNode(name, geometry, material) {
 
    }


    Thorn::~Thorn() {

    }


} // namespace game
