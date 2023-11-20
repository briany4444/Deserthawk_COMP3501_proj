#ifndef UI_H_
#define UI_H_

#define NUM_OBJECTIVES 4

#include "scene_node.h"

namespace game {

    // Class that manages one object in a scene 
    class Ui : public SceneNode{

    public:
        
        Ui(const std::string name, const Resource* wallGeometry, const Resource* material);
        ~Ui();

        //update the number of collected objectives
        void IncrementCollected(void);


    private:
        int num_collected_;

    

    }; // class Ui

} // namespace game

#endif // UI_H_
