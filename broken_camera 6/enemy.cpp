#include "enemy.h"

namespace game {

    Enemy::Enemy(Player *c, const std::string name, const Resource* geometry, const Resource* material) : SceneNode(name, geometry, material) {
        player_ = c;
        node_type_ = Obj_t.enemy;
    }


    Enemy::~Enemy() {
    }

    void Enemy::Update(float delta_time) {

    }


} // namespace game
