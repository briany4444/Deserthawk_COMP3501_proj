#include "spaceship.h"

namespace game {

    Spaceship::Spaceship(Camera *c, const std::string name, const Resource* geometry, const Resource* material) : SceneNode(name, geometry, material) {
        collidable_ = true;
        radius_ = 0.5f;
        speed_ = ENEMY_SPEED;
        player_ = c;
        type_ = "Spaceship";
    }


    Spaceship::~Spaceship() {
    }

    void Spaceship::Update(float delta_time) {

        // follow player
        glm::vec3 dir = glm::normalize(player_->GetPosition() - GetPosition());
        glm::vec3 new_pos = GetPosition() + speed_ * delta_time * dir;
        SetPosition(new_pos);

    }


} // namespace game
