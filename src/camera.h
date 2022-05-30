#pragma once

#include <Magpie/vec.h>
#include <Magpie/angle.h>

namespace Magpie {
    class Camera {
        public:
            Magpie::Vec3 pos   = Magpie::Vec3(0.0f, 0.0f,  0.0f);
            Magpie::Vec3 front = Magpie::Vec3(0.0f, 0.0f, -1.0f);
            Magpie::Vec3 up    = Magpie::Vec3(0.0f, 1.0f,  0.0f);
            
            float yaw = 0.0f;
            float pitch = 0.0f;
            
            void Rotate(int yawOffset, int pitchOffset) {
                yaw += yawOffset;
                pitch += pitchOffset;

                if(pitch > 89.99f) pitch =  89.99f;
                if(pitch < -89.99f) pitch = -89.99f;
                
                Magpie::Vec3 direction;
                direction.x = cos(Magpie::Radians(yaw)) * cos(Magpie::Radians(pitch));
                direction.y = sin(Magpie::Radians(pitch));
                direction.z = sin(Magpie::Radians(yaw)) * cos(Magpie::Radians(pitch));
                front = Magpie::Vector::Normalize(direction);
            }
    };
}