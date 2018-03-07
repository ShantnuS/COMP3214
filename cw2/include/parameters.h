#ifndef PARAMETERS_H_INCLUDED
#define PARAMETERS_H_INCLUDED

typedef struct Lighting
{
    bool LightAtInfinity;
    glm::vec4 LightPosition;
    glm::vec4 LightVector;
    glm::vec4 LightAmbient; //Phong's model
    glm::vec4 LightDiffuse;
    glm::vec4 LightSpecular;
    int LightSpecularity;
} Lighting;

enum Shading {Fixed, Flat, Gouraud};

typedef struct Values
{
    Uint32 DefaultColour;
    Uint32 ObjectColour;
//
    bool BackFaceCulling;
    bool ZBuffer;
    float *zBuffer; // Pointer to z-buffer
    int Size;
    float MaxRange;
//
    glm::mat4 Transform; // Global Transform.
    int Height;
    int Width;
    glm::vec4 CameraPosition;
    glm::vec4 CameraLookAtPoint;
    glm::vec4 CameraUpVector;
    glm::vec4 CameraVector;
    bool CameraAtInfinity;
    int LightCount;
    //~ std::vector<Lighting> Lights;
    Shading ShadeType;
    bool WithZbuffer;
    float lod; // level of detail
    float timestep;
} Values;

#endif // PARAMETERS_H_INCLUDED
