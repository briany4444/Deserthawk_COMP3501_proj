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
    std::cout << speed_ << std::endl;
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

    glm::quat rotation = glm::angleAxis(angle, GetUp()); 
    orientation_ = rotation * orientation_;
    orientation_ = glm::normalize(orientation_);
}


void Player::Roll(float angle){

    glm::quat rotation = glm::angleAxis(angle, GetForward()); 
    orientation_ = rotation * orientation_; // update the orientation
    orientation_ = glm::normalize(orientation_);
}


void Player::SetView(glm::vec3 position, glm::vec3 look_at, glm::vec3 up){

    // Store initial forward and side vectors
    // See slide in "Player control" for details
    // probably will not be much used (directly setting view a rare occurrence in games)
    forward_ = look_at - position;
    forward_ = -glm::normalize(forward_);
    side_ = glm::cross(up, forward_);
    side_ = glm::normalize(side_);

    // Reset orientation and position of Player
    position_ = position;
    orientation_ = glm::quat();
}


void Player::SetProjection(GLfloat fov, GLfloat near, GLfloat far, GLfloat w, GLfloat h){

    // Set projection based on field-of-view
    float top = tan((fov/2.0)*(glm::pi<float>()/180.0))*near;
    float right = top * w/h;
    projection_matrix_ = glm::frustum(-right, right, -top, top, near, far);
}


void Player::SetupShader(GLuint program){

    // Update view matrix
    SetupViewMatrix();

    // Set view matrix in shader
    GLint view_mat = glGetUniformLocation(program, "view_mat");
    glUniformMatrix4fv(view_mat, 1, GL_FALSE, glm::value_ptr(view_matrix_));
    
    // Set projection matrix in shader
    GLint projection_mat = glGetUniformLocation(program, "projection_mat");
    glUniformMatrix4fv(projection_mat, 1, GL_FALSE, glm::value_ptr(projection_matrix_));
}


void Player::SetupViewMatrix(void){

    //view_matrix_ = glm::lookAt(position, look_at, up);

    // Get current vectors of coordinate system
    // [side, up, forward]
    // See slide in "Player control" for details
    glm::vec3 current_forward = orientation_ * forward_;
    glm::vec3 current_side = orientation_ * side_;
    glm::vec3 current_up = glm::cross(current_forward, current_side);
    current_up = glm::normalize(current_up);

    // Initialize the view matrix as an identity matrix
    view_matrix_ = glm::mat4(1.0); 

    // Copy vectors to matrix
    // Add vectors to rows, not columns of the matrix, so that we get
    // the inverse transformation
    // Note that in glm, the reference for matrix entries is of the form
    // matrix[column][row]
    view_matrix_[0][0] = current_side[0]; // First row
    view_matrix_[1][0] = current_side[1];
    view_matrix_[2][0] = current_side[2];
    view_matrix_[0][1] = current_up[0]; // Second row
    view_matrix_[1][1] = current_up[1];
    view_matrix_[2][1] = current_up[2];
    view_matrix_[0][2] = current_forward[0]; // Third row
    view_matrix_[1][2] = current_forward[1];
    view_matrix_[2][2] = current_forward[2];

    // Create translation to Player position
    glm::mat4 trans = glm::translate(glm::mat4(1.0), -position_);

    // Combine translation and view matrix in proper order
    view_matrix_ *= trans;
}

} // namespace game
