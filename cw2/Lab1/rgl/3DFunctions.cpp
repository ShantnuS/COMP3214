#include <RGL.h>

bool BackFaceCulling(glm::vec4 a, glm::vec4 b, glm::vec4 c) {
    glm::vec4 p, q;
    glm::vec3 r, s;
    p = a - b;
    q = b - c;

    r = glm::vec3(p[0], p[1], p[2]);
    r = glm::vec3(q[0], q[1], q[2]);

    glm::vec3 nv = glm::normalize(glm::cross(r, s));

    glm::vec3 camera = World.CameraVector.xyz();

    camera = glm::normalize(camera);

    float between = glm::dot(nv, camera);

    if(between < 0.)
        return true;  // cull me
    else
        return false; // keep me
      }
