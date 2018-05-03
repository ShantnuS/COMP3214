#include "RGL.h"

// namespace HELP
// {

void print(glm::vec2 x)
{
    printf("[%f %f]\n", x[0], x[1]);
}
void print(glm::vec3 x)
{
    printf("[%f %f %f]\n", x[0], x[1], x[2]);
}

void print(glm::vec4 x)
{
    printf("[%f %f %f %f]\n", x[0], x[1], x[2], x[3]);
}

void print(glm::mat4 x)
{
    printf("[[%f %f %f %f]\n[%f %f %f %f]\n[%f %f %f %f]\n[%f %f %f %f]]\n",
           x[0][0], x[0][1], x[0][2], x[0][3],
           x[1][0], x[1][1], x[1][2], x[1][3],
           x[2][0], x[2][1], x[2][2], x[2][3],
           x[3][0], x[3][1], x[3][2], x[3][3]);
}

void print(glm::mat3 x)
{
    printf("[[%f %f %f]\n[%f %f %f]\n[%f %f %f]]\n",
           x[0][0], x[0][1], x[0][2],
           x[1][0], x[1][1], x[1][2],
           x[2][0], x[2][1], x[2][2]);
}

void print(glm::mat2 x)
{
    printf("[[%f %f]\n[%f %f]]\n",
           x[0][0], x[0][1],
           x[1][0], x[1][1]);
}

void print(const char *x)
{
    printf("%s\n", x);
}
void Print(const char *x)
{
    printf("%s ", x);
}

void print(float x)
{
    printf("%f\n", x);
}
void Print(float x)
{
    printf("%f ", x);
}
void print(int x)
{
    printf("%d\n", x);
}

// } //End namespace Help.
