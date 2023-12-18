 #include "tree.h"
#include <iostream>

namespace game {

    Tree::Tree(const std::string name, const Resource* geometry, const Resource* material, float l, float rad, const Resource* t_geom, const Resource* tex) : SceneNode(name, geometry, material, tex) {
        radius_ = rad;
        length_ = l; 
        curr_state_t = counterclockwise;
        orbit_angle_ = 0;
        thorn_geom = t_geom;
    }


    Tree::~Tree() {
    }


    // creates child branches recursively
    void Tree::createBranches(const Resource* geom, const Resource* mat, int num_branches) {
        
        for (int i = 0; i < num_branches; i++) {

            // creates new child tree/branch
            Tree* child_tree = new Tree("branch", geom, mat, this->length_ / 2, this->radius_ / 2, thorn_geom, NULL);
            children_.push_back(child_tree);
            child_tree->SetParent(this);

            
            // chooses positions randomly
            float rand_angle = i * 2 * glm::pi<float>() / num_branches + glm::pi<float>() / 3;
            float rand_y = length_/4 + (fmod(std::rand(), length_ / 4));
            float rand_x = ((child_tree->GetLength() / 2) + radius_ / 2) * cos(rand_angle);
            float rand_z = ((child_tree->GetLength() / 2) + radius_ / 2) * sin(rand_angle);

            // sets child attributes
            child_tree->SetPosition(glm::vec3(rand_x, rand_y, rand_z));
            child_tree->SetScale(glm::vec3(0.3, length_/7, 0.3));
            glm::vec3 dir = glm::normalize(child_tree->GetPosition());
            child_tree->SetOrientation(glm::normalize(glm::angleAxis(-2 * glm::pi<float>() / 5 , glm::cross(dir, glm::vec3(0, 1, 0)))));
            child_tree->SetJointPos(glm::vec3(0, rand_y, 0));

            // recursively creates branches in children
            child_tree->createBranches(geom, mat, num_branches - 1);
            child_tree->createThorns(thorn_geom, mat, 3);
        }
    }

    // creates the thorns for the tree
    void Tree::createThorns(const Resource* geom, const Resource* mat, int num_thorns) {

        for (int i = 0; i < num_thorns; i++) {

            // creates new child tree/branch
            Thorn* child_tree = new Thorn("thorn", geom, mat);
            children_.push_back(child_tree);
            child_tree->SetParent(this);

            // chooses positions randomly
            float rand_angle = i * 2 * glm::pi<float>() / num_thorns;
            float rand_y = -length_/2 + fmod(std::rand(), length_);
            float rand_x = (radius_/2) * cos(rand_angle);
            float rand_z = (radius_/2) * sin(rand_angle);

            // sets child attributes
            child_tree->SetPosition(glm::vec3(rand_x, rand_y, rand_z));
            glm::vec3 dir = glm::normalize(child_tree->GetPosition());
            child_tree->SetOrientation(glm::angleAxis(-glm::pi<float>() / 2, glm::cross(dir, glm::vec3(0, 0, 1))));
            
        }
        

    }

 
    // increase/decrease angle depending on the "sway state" to some max/min angle
    void Tree::Update(float d) {

        if (orbit_angle_ < -glm::pi<float>() / 16) {
            curr_state_t = counterclockwise;
            orbit_angle_ = -glm::pi<float>() / 16;
        }
        else if (orbit_angle_ > glm::pi<float>() / 16) {
            curr_state_t = clockwise;
            orbit_angle_ = glm::pi<float>() / 16;

        }
        else if (curr_state_t == clockwise) {
            Orbit(-d/8);
        }
        else if (curr_state_t == counterclockwise){
            Orbit(d/8);
        }

        SceneNode::Update(d);
    }
    

} // namespace game
