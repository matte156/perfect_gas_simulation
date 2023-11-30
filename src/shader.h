#ifndef SHADER_H
#define SHADER_H

#include <stdlib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct
{
    unsigned int ID;
} shaderProgram_t;



void createShaderProgram(unsigned int *program, char *vertexShaderPath, char *fragmentShaderPath);

#endif
