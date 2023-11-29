#include "racetrack.h"

namespace game {

    RaceTrack::RaceTrack() {
    }


    RaceTrack::~RaceTrack() {
        for (int i = 0; i < beacons_.size(); i++) {
            delete beacons_[i];
        }
    }

    std::string RaceTrack::GetNextName() {
        return beacons_[0]->GetName();
    }

    // returns true if player got to the last beacon
    bool RaceTrack::UpdateRaceTrack() {

        Beacon* next_beacon = beacons_[0];
        if (next_beacon != target_beacon_) {
            Beacon* next_beacon = beacons_[0];
            target_beacon_->SetPosition(next_beacon->GetPosition());
            beacons_.erase(beacons_.begin());
            delete next_beacon;
            return false;
        }

        return true;
    }


    std::vector<Beacon*> RaceTrack::InitBeacons(ResourceManager resman_) {

        for (int i = 0; i < 4; i++) {
            std::stringstream ss;
            ss << i;
            std::string index = ss.str();
            std::string name = "beacon_" + index;

            // Create beacon instance
            Beacon* beac = InitBeaconInstance(resman_, name, "Beacon", "ObjectMaterial");
            beac->SetPosition(glm::vec3(0, 0, 800 - (i + 1) * 40));
            beac->SetOrientation(glm::normalize(glm::angleAxis(glm::pi<float>() * ((float)rand() / RAND_MAX), glm::vec3(((float)rand() / RAND_MAX), ((float)rand() / RAND_MAX), ((float)rand() / RAND_MAX)))));
            beacons_.push_back(beac);
        }

        // init the target Beacon
        target_beacon_ = InitBeaconInstance(resman_, "targetBeacon", "Target", "ObjectMaterial");
        target_beacon_->SetPosition(glm::vec3(0, 0, 795));
        beacons_.push_back(target_beacon_);

        return beacons_;
    }

    Beacon* RaceTrack::InitBeaconInstance(ResourceManager resman_, std::string entity_name, std::string object_name, std::string material_name) {
        Resource* geom = resman_.GetResource(object_name);
        Resource* mat = resman_.GetResource(material_name);

        // Create beacon instance
        Beacon* b = new Beacon(entity_name, geom, mat);
        return b;
    }


} // namespace gam