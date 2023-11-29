#include <stdexcept>
#include <iostream>
#include <fstream>
#define GLM_FORCE_RADIANS
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "scene_graph.h"

namespace game {

SceneGraph::SceneGraph(void){

    background_color_ = glm::vec3(0.0, 0.0, 0.0);
}


SceneGraph::~SceneGraph(){
}


void SceneGraph::SetBackgroundColor(glm::vec3 color){

    background_color_ = color;
}


glm::vec3 SceneGraph::GetBackgroundColor(void) const {

    return background_color_;
}
 

SceneNode *SceneGraph::CreateNode(std::string node_name, Resource *geometry, Resource *material){

    // Create scene node with the specified resources
    SceneNode *scn = new SceneNode(node_name, geometry, material);

    // Add node to the scene
    node_.push_back(scn);

    return scn;
}


void SceneGraph::AddNode(SceneNode *node){
    if (node->GetCollidable()) {
        collidable_nodes_.push_back(node);
    }
    node_.push_back(node);
}


SceneNode *SceneGraph::GetNode(std::string node_name) const {

    // Find node with the specified name
    for (int i = 0; i < node_.size(); i++){
        if (node_[i]->GetName() == node_name){
            return node_[i];
        }
    }
    return NULL;

}

void SceneGraph::RemoveCollidable(std::string node_name) {
    for (int i = 0; i < collidable_nodes_.size(); i++) {
        if (collidable_nodes_[i]->GetName() == node_name) {
            collidable_nodes_.erase(collidable_nodes_.begin() + i);
            RemoveNode(node_name);
            break;
        }
    }
}

// remove from main graph, this is private
void SceneGraph::RemoveNode(std::string node_name) {
    for (int i = 0; i < node_.size(); i++) {
        if (node_[i]->GetName() == node_name) {
            node_.erase(node_.begin() + i);
            break;
        }
    }
}


std::vector<SceneNode *>::const_iterator SceneGraph::begin() const {

    return node_.begin();
}


std::vector<SceneNode *>::const_iterator SceneGraph::end() const {

    return node_.end();
}


void SceneGraph::Draw(Camera *camera){

    // Clear background
    glClearColor(background_color_[0], 
                 background_color_[1],
                 background_color_[2], 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw all scene nodes
    for (int i = 0; i < node_.size(); i++){
        node_[i]->Draw(camera);
    }
}


void SceneGraph::Update(float delta_time){

    for (int i = 0; i < node_.size(); i++){
        node_[i]->Update(delta_time);
    }
}

} // namespace game
