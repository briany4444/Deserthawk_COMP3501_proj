#include <stdexcept>
#define GLM_FORCE_RADIANS
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "player.h"

namespace game {

Player::Player(void){
}


Player::~Player(){
}



void Player::Update(double delta_time) {
    position_ += float(speed_ * delta_time) * GetForward();
    // player bounds
    if (position_.x < -530) {
        position_.x = -530;
    }
    else if (position_.x > 530) {
        position_.x = 530;
    }

    if (position_.z > 1330) {
        position_.z = 1330;
    } else if (position_.z > 1330) {
        position_.z = -250;
    }

    if (position_.y > 60) {
        position_.y = 60;
    }
    speed_ = 0;
    //update shape
    shape_->SetPosition(position_);
    shape_->SetOrientation(orientation_);
}


void Player::SetShape(SceneNode* shape)
{
    shape_ = shape;
}

glm::vec3 Player::GetPosition(void) const {

    return position_;
}


glm::quat Player::GetOrientation(void) const {

    return orientation_;
}




void Player::SetPosition(glm::vec3 position){

    position_ = position;
}


void Player::SetOrientation(glm::quat orientation){

    orientation_ = orientation;
}


void Player::Translate(glm::vec3 trans){

    position_ += trans;
}


void Player::Rotate(glm::quat rot){
    // apply this rotation to the Player's current orientation
    orientation_ = rot * orientation_;
    orientation_ = glm::normalize(orientation_);
}

void Player::Accelerate(float s) {
    if (speed_ + (s * accel_magnitude) <= max_speed_) {
        speed_ += s * accel_magnitude;
    } 
}

void Player::Decelerate(float s) {
    if (speed_ - s >= 0) {
        speed_ -= s;
    }
}

void Player::MoveForward() {
    speed_ = 10;
}

void Player::MoveBackward(){
    speed_ = -10;
}

void Player::MoveStop()
{
    max_speed_ = 0;
}

void Player::AddMaxSpeed(float s) {
    max_speed_ += s;
}


glm::vec3 Player::GetForward(void) const {

    glm::vec3 current_forward = orientation_ * forward_;
    return -current_forward; // Return -forward since the Player coordinate system points in the opposite direction
}


glm::vec3 Player::GetSide(void) const {
    glm::vec3 current_side = orientation_ * side_;
    return current_side;
}


glm::vec3 Player::GetUp(void) const {
    glm::vec3 current_up = glm::cross(-GetForward(), GetSide());
    return current_up;
}


void Player::Pitch(float angle){

    glm::quat rotation = glm::angleAxis(angle, GetSide()); 
    orientation_ = rotation * orientation_;
    orientation_ = glm::normalize(orientation_);
}


void Player::Yaw(float angle){

    glm::quat rotation = glm::angleAxis(angle, glm::cross(forward_, side_));
    orientation_ = rotation * orientation_;
    orientation_ = glm::normalize(orientation_);
}


void Player::Roll(float angle){

    glm::quat rotation = glm::angleAxis(angle, forward_); 
    orientation_ = rotation * orientation_; // update the orientation
    orientation_ = glm::normalize(orientation_);
}

void Player::Init(glm::vec3 position, glm::vec3 look_at, glm::vec3 up)
{
    forward_ = look_at - position;
    forward_ = -glm::normalize(forward_);
    side_ = glm::cross(up, forward_);
    side_ = glm::normalize(side_);

    // Reset orientation and position of camera
    position_ = position;
    orientation_ = glm::quat();
}


} // namespace game
