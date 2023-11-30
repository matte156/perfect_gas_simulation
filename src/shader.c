#include <GL/glew.h>
#include "shader.h"
#include <stdio.h>
#include <stdlib.h>

int fileSize(FILE *fp)
{
    fseek(fp, 0L, SEEK_END);
    int size = ftell(fp);

    fseek(fp, 0L, SEEK_SET);

    return size;
}

char *fileToString(char *filePath, char *errorMessage)
{
    FILE *fp = fopen(filePath, "r");
    if(!fp)
    {
	char *errMsg = (char *)malloc(strlen("Failed to open ") + strnlen(filePath, 300));
	if(!errMsg)
	{
	    perror("Failed to allocate memory");
	    fprintf(stderr, "%s\n", errorMessage);
	    exit(1);
	}

	sprintf(errMsg, "Failed to open %.*s", 300, filePath);
	perror(errMsg);
	free(errMsg);
	fprintf(stderr, "%s\n", errorMessage);
    }
   
    int size = fileSize(fp);
    char *str = (char *)malloc(size);

    if(!str)
    {
	perror("Failed to allocate memory");
	fprintf(stderr, "%s\n", errorMessage);
	exit(1);
    }
    
    fread(str, 1, size, fp);

    return str;
}

void createShaderProgram(unsigned int *program, char *vertexShaderPath, char *fragmentShaderPath)
{
    char *vertexShaderSource, *fragmentShaderSource;
    unsigned int vertexShader, fragmentShader;
    int success;
    char infoLog[512];

    vertexShaderSource = fileToString(vertexShaderPath, "ERROR:SHADER:VERTEX:LOADING_SOURCE_FAILED");

    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vertexShader, 1, (const GLchar *const *) &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if(!success)
    {
	glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
	fprintf(stderr, "ERROR:SHADER:VERTEX:COMPILATION_FAILED\n%s\n", infoLog);
	exit(1);
    }
    free(vertexShaderSource);

    fragmentShaderSource = fileToString(fragmentShaderPath, "ERROR:SHADER:FRAGMENT:LOADING_SOURCE_FAILED");

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(fragmentShader, 1, (const GLchar *const *) &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if(!success)
    {
	glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
	fprintf(stderr, "ERROR:SHADER:FRAGMENT:COMPILATION_FAILED\n%s\n", infoLog);
	exit(1);
    }
    free(fragmentShaderSource);


    *program = glCreateProgram();
    
    glAttachShader(*program, vertexShader);
    glAttachShader(*program, fragmentShader);
    glLinkProgram(*program);

    glGetShaderiv(*program, GL_LINK_STATUS, &success);
    if(!success)
    {
	glGetShaderInfoLog(*program, 512, NULL, infoLog);
	fprintf(stderr, "ERROR:SHADER:PROGRAM:LINKING_FAILED\n%s\n", infoLog);
	exit(1);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}
