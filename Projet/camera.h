#pragma once
#include <cmath>
#include "mat4.h"
#include "vec.h"

class Camera {
public:
    vec3 position;
    vec3 target;
    vec3 up;

    float azimuth;
    float elevation;
    float radius;

    bool freeMode;
    vec3 freePos;
    float yaw;
    float pitch;

    vec3 orbitTarget;

    Camera() {
        azimuth = 3.14159265f / 2.0f;
        elevation = 0.0f;
        radius = 15.0f;
        up = vec3(0.0f, 1.0f, 0.0f);
        
        freeMode = false;
        freePos = vec3(0.0f, 0.0f, 10.0f);
        yaw = -90.0f;
        pitch = 0.0f;
        
        orbitTarget = vec3(0.0f, -1.4f, -1.5f);
        
        Update();
    }

    void Update() {
        if (!freeMode) {
            float Y = orbitTarget.y + radius * sinf(elevation);
            float X = orbitTarget.x + radius * cosf(elevation) * cosf(azimuth);
            float Z = orbitTarget.z + radius * cosf(elevation) * sinf(azimuth);
            position = vec3(X, Y, Z);
            target = orbitTarget;
        } else {
            float pitchRad = pitch * 3.14159265f / 180.0f;
            float yawRad   = yaw   * 3.14159265f / 180.0f;
            
            vec3 front;
            front.x = cosf(pitchRad) * cosf(yawRad);
            front.y = sinf(pitchRad);
            front.z = cosf(pitchRad) * sinf(yawRad);
            front = normalize(front);

            position = freePos;
            target = add(freePos, front);
        }
    }

    Mat4 GetViewMatrix() {
        return makeLookAt(position, target, up);
    }
};
