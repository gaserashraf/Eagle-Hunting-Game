#include "entity.hpp"
#include "../deserialize-utils.hpp"
#include <glm/gtx/euler_angles.hpp>

namespace our {

    // This function computes and returns a matrix that represents this transform
    // Remember that the order of transformations is: Scaling, Rotation then Translation
    // HINT: to convert euler angles to a rotation matrix, you can use glm::yawPitchRoll
    glm::mat4 Transform::toMat4() const {
        //TODO: (Req 2) Write this function
        glm::mat4 Translation = glm::translate(                   
                glm::mat4(1.0f),                         
                position                     
            );
        glm::mat4 Scale = glm::scale(                   
                glm::mat4(1.0f),                         
                scale                    
            );
        // Translation[0][0]*=scale[0];
        // Translation[1][1]*=scale[1];
        // Translation[2][2]*=scale[2];

        glm::mat4 Rotation = glm::yawPitchRoll(rotation.y, rotation.x, rotation.z);
        return Translation*Rotation*Scale;
    }

     // Deserializes the entity data and components from a json object
    void Transform::deserialize(const nlohmann::json& data){
        position = data.value("position", position);
        rotation = glm::radians(data.value("rotation", glm::degrees(rotation)));
        scale    = data.value("scale", scale);
    }

}