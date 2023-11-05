#ifndef RACETRACK_H_
#define RACETRACK_H_

#include <string>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#define GLM_FORCE_RADIANS
#include <glm/gtc/quaternion.hpp>
#include <sstream>

#include "resource.h"
#include "beacon.h"
#include "resource_manager.h"
#include "scene_node.h"

namespace game {

    // Abstraction of an asteroid
    class RaceTrack {

    public:
        // Create asteroid from given resources
        RaceTrack();

        // Destructor
        ~RaceTrack();

        // update the racetrack and target Beacon Pos
        bool UpdateRaceTrack();

        // gets the name of the next becaon
        std::string GetNextName();

        // init beacons
        std::vector<Beacon*> InitBeacons(ResourceManager);
        Beacon* InitBeaconInstance(ResourceManager, std::string entity_name, std::string object_name, std::string material_name);

    private:
        std::vector<Beacon*> beacons_;
        Beacon* target_beacon_;

    }; // class RaceTrack

} // namespace game

#endif // RACETRACK_H_