#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Magpie {
    class Camera {
        public:
            glm::vec3 pos   = glm::vec3(0.0f, 0.0f,  0.0f);
            glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
            glm::vec3 up    = glm::vec3(0.0f, 1.0f,  0.0f);
            
            float yaw = 0.0f;
            float pitch = 0.0f;
            
            void Rotate(int yawOffset, int pitchOffset) {
                yaw += yawOffset;
                pitch += pitchOffset;

                if(pitch > 89.99f) pitch =  89.99f;
                if(pitch < -89.99f) pitch = -89.99f;
                
                glm::vec3 direction;
                direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
                direction.y = sin(glm::radians(pitch));
                direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
                front = glm::normalize(direction);
            }
    };
}