#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "shader.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define WIDTH 1920
#define HEIGHT 1080

#define PI 3.1415926536

#define ABS_F(x) (x > 0 ? x : -x)

void sphereCollision(float positions [][2], float velocities [][2], int indexA, int indexB)
{
    float n[2], un[2], ut[2], magnitude, normalVelocities[2], tangentVelocities[2];

    n[0] = positions[indexB][0] - positions[indexA][0];
    n[1] = positions[indexB][1] - positions[indexA][1];

    magnitude = sqrt(n[0]*n[0] + n[1]*n[1]);

    un[0] = n[0] / magnitude;
    un[1] = n[1] / magnitude;

    ut[0] = -un[1];
    ut[1] = un[0];

    normalVelocities[0] = velocities[indexA][0]*un[0] + velocities[indexA][1]*un[1];
    normalVelocities[1] = velocities[indexB][0]*un[0] + velocities[indexB][1]*un[1];

    tangentVelocities[0] = velocities[indexA][0]*ut[0] + velocities[indexA][1]*ut[1];
    tangentVelocities[1] = velocities[indexB][0]*ut[0] + velocities[indexB][1]*ut[1];

    velocities[indexA][0] = un[0]*normalVelocities[1] + ut[0]*tangentVelocities[0];
    velocities[indexA][1] = un[1]*normalVelocities[1] + ut[1]*tangentVelocities[0];

    velocities[indexB][0] = un[0]*normalVelocities[0] + ut[0]*tangentVelocities[1];
    velocities[indexB][1] = un[1]*normalVelocities[0] + ut[1]*tangentVelocities[1];

    positions[indexA][0] += - un[0] * (0.02 - magnitude)/2;
    positions[indexA][1] += - un[1] * (0.02 - magnitude)/2;
    positions[indexB][0] += un[0] * (0.02-magnitude)/2;
    positions[indexB][1] += un[1] * (0.02 - magnitude)/2;
}

struct polygon
{
    unsigned int VBO;
    unsigned int EBO;
    unsigned int VAO;
};

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	glfwSetWindowShouldClose(window, 1);
}

void identityMatrix(int n, float M[n][n])
{
    for(int i = 0; i < n; i++)
	for(int j = 0; j < n; j++)
	    M[i][j] = 0.0f;
    for(int i = 0; i < n; i++)
	M[i][i] = 1;
}

void computeTranslationMatrix(float M[4][4], float vec[3])
{
/*    for(int i = 0; i < 3; i++)
	M[i][3] = vec[i]; */
    M[0][3] = vec[0] * 1080.0/1920.0 - (WIDTH - HEIGHT)/(float)WIDTH;
    M[1][3] = vec[1];
    M[2][3] = vec[2];
}

void computeRotationMatrix(float M[4][4], double theta)
{
    M[0][0] = cos(theta);
    M[0][1] = sin(theta);

    M[1][0] = -sin(theta);
    M[1][1] = cos(theta);
}

void computeScaleMatrix(float M[4][4])
{
    M[0][0] = 1080.0/1920.0;
}

struct polygon createPolygon(float radius, int vertexNumber)
{
    struct polygon polygon;
    float sideAngle = (PI*2) / vertexNumber;
    float vertices[(vertexNumber + 1)*3];
    unsigned int indices[vertexNumber*3];

    vertices[0] = 0;
    vertices[1] = 0;
    vertices[2] = 0;

    for(int i = 0; i < vertexNumber; i++)
    {
	vertices[(i+1)*3] = cos(i*sideAngle)*radius;
	vertices[(i+1)*3 + 1] = sin(i*sideAngle)*radius;
	vertices[(i+1)*3 + 2] = 0;
    }
    for(int i = 0; i < vertexNumber; i++)
    {
	indices[i*3] = 0;
	indices[i*3 + 1] = (i % vertexNumber) + 1;
	indices[i*3 + 2] = ((i+1) % vertexNumber) + 1;
    }

    glGenVertexArrays(1, &(polygon.VAO));
    glGenBuffers(1, &(polygon.VBO));
    glGenBuffers(1, &(polygon.EBO));

    glBindVertexArray(polygon.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, polygon.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, polygon.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    return polygon;
}

void spawnBalls(float position[][2], float velocity[][2], float color[][3], float acceleration[][2], int n, float v)
{
    for(int i = 0; i < n; i++)
    {
	position[i][0] = 2*(float)rand()/(float)RAND_MAX - 1;
	position[i][1] = 2*(float)rand()/(float)RAND_MAX - 1;

	float theta = 10*(float)rand()/(float)RAND_MAX;
	velocity[i][0] = v*cos(theta);
	velocity[i][1] = v*sin(theta);

	color[i][0] = (float)rand()/(float)RAND_MAX;
	color[i][1] = (float)rand()/(float)RAND_MAX;
	color[i][2] = (float)rand()/(float)RAND_MAX;
	
	acceleration[i][0] = acceleration[i][1] = 0;

	printf("%f %f\n", position[i][0], position[i][1]);
    }
}

int compare(const void *a, const void *b)
{
    float result = *(float *)a - *(float *)b;
    
    if(result > 0)
	return 1;
    if(result < 0)
	return -1;
    else
	return 1;
}

void numberOfMoleculesByIntervalOfVelocities(float velocities[][2], int *result, int n, float interval)
{
    float buffer[n];

    for(int i = 0; i < n; i++)
    {
	buffer[i] = sqrt(powf(velocities[i][0], 2) + powf(velocities[i][1], 2));
    }

    qsort(buffer, n, sizeof(float), compare);

    int counter = 0;
    for(int i = 0; i < n; i++)
    {
	if(buffer[i] <= (counter + 1)*interval)
	    result[counter]++;
	else
	    counter ++;
    }
}

void matrixMultiplication(float ( *M)[4][4], float N[4][4])
{
    printf(" %p\n", M);
    float result[4][4];
    for(int i = 0; i < 4; i++)
    {
	for(int j = 0; j < 4; j++)
	{
	    result[i][j] = 0;
	}
    }

    for(int i = 0; i < 4; i++)
    {
	for(int j = 0; j < 4; j++)
	{
	    for(int k = 0; k < 4; k++)
	    {
		result[i][j] += (*M)[i][k]*N[k][j];
	    }
	}
    }

    for(int i = 0; i < 4; i++)
	for(int j = 0; j < 4; j++)
	    (*M)[i][j] = result[i][j];
}

void drawPlot(int data[], int n, unsigned int shaderProgram)
{
    float transMatrix[4][4], bufMatrix[4][4];
    identityMatrix(4, transMatrix);
    identityMatrix(4, bufMatrix);


//    transMatrix[0][0] = (1920.0 - 1080.0)/1920.0;
    
    struct polygon rectangle = createPolygon(1.0f, 4);
        float apotheme = 1.0*cos(PI/4);


    bufMatrix[0][0] = 1/apotheme;
    bufMatrix[1][1] = 1/apotheme;


    identityMatrix(4, bufMatrix);
    bufMatrix[0][3] = 2*1080/1920.0;

/*    unsigned int transformationMatrixPosition = glGetUniformLocation(shaderProgram,
		    "transformationMatrix");
    glUniformMatrix4fv(transformationMatrixPosition, 1, GL_FALSE, &transMatrix[0][0]);*/

    glUseProgram(shaderProgram);
    unsigned int apothemPosition = glGetUniformLocation(shaderProgram, "apotheme");
    glUniform1f(apothemPosition, apotheme);

    
    
    for(int i = 0; i < n; i++)
    {
	unsigned int colorPosition = glGetUniformLocation(shaderProgram, "color");
	glUniform3fv(colorPosition, 1, (float [3]){0.0f,i/200.0,0.5});

	unsigned int parametersPosition = glGetUniformLocation(shaderProgram, "parameters");

	glUniform2f(parametersPosition, i*0.01, data[i]);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rectangle.EBO);
	glBindVertexArray(rectangle.VAO);

	glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
    }
}

int main()
{
    int n_balls = 1000;

    float velocity[n_balls][2];

    float acceleration[n_balls][2];

    float position[n_balls][2];

    float colors[n_balls][3];

    spawnBalls(position, velocity, colors, acceleration, n_balls, 0.5);

    float transMatrix[4][4];

    identityMatrix(4, transMatrix);

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window;

    window = glfwCreateWindow(WIDTH, HEIGHT, "Prova", glfwGetPrimaryMonitor(), NULL);
    if(!window)
    {
	fprintf(stderr, "Failed to create window\n");
	glfwTerminate();
	exit(1);
    }

    glfwMakeContextCurrent(window);

    glViewport(0, 0, WIDTH, HEIGHT);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    /* initialize glew */
    GLenum err;
    if((err = glewInit()) != GLEW_OK)
    {
	fprintf(stderr, "Error initialize glew\n");
	fprintf(stderr, "%s\n", glewGetErrorString(err));
    }

    unsigned int shaderProgram, plotShaderProgram;
    createShaderProgram(&shaderProgram, "shader/1.vertex.glsl", "shader/1.fragment.glsl");
    createShaderProgram(&plotShaderProgram, "shader/plot.vertex.shader", "shader/1.fragment.glsl");

    double lastFrameTime = glfwGetTime();

    struct polygon hexagon = createPolygon(0.01f, 10);
    float apotheme = 0.01*cos(PI/10);

    while(!glfwWindowShouldClose(window))
    {
	processInput(window);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);	
	glClear(GL_COLOR_BUFFER_BIT);

	double time = glfwGetTime();
	double delta = time - lastFrameTime;

	lastFrameTime = time;

	for(int i = 0; i < n_balls; i++)
	{
	    velocity[i][0] += acceleration[i][0]*delta;
	    velocity[i][1] += acceleration[i][1]*delta;

	    position[i][0] += velocity[i][0]*delta;
	    position[i][1] += velocity[i][1]*delta;
	    

	    if(position[i][0] >= 1-apotheme)
	    {
		if(velocity[i][0] > 0)
		    velocity[i][0] *= -1;
		else if(velocity[i][0] == 0)
		    velocity[i][0] = -0.01;
		position[i][1] += ((position[i][0] - 1 + apotheme)*velocity[i][1])/velocity[i][0];
		position[i][0] = 1-apotheme;
	    }
	    if(position[i][0] <= -1+apotheme)
	    {
		if(velocity[i][0] < 0)
		    velocity[i][0] *= -1;
		else if(velocity[i][0] == 0)
		    velocity[i][0] = +0.01;

		position[i][1] += (-1+apotheme-position[i][0])*velocity[i][1]/velocity[i][0];

		position[i][0] = -1+apotheme;
	    }
	    if(position[i][1] >= 1-apotheme)
	    {
		if(velocity[i][1] > 0)
		    velocity[i][1] *= -1;
		else if(velocity[i][1] == 0)
		    velocity[i][1] = -0.01;
		position[i][0] += (position[i][1] -1 + apotheme)*velocity[i][0]/velocity[i][1];

		position[i][1] = 1-apotheme;
	    }

	    if(position[i][1] <= -1+apotheme)
	    {
		if(velocity[i][1] < 0)
		    velocity[i][1] *= -1;
		else if(velocity[i][1] == 0)
		    velocity[i][0] = 0.01;
		
		position[i][0] += (-position[i][1] -1 + apotheme)*velocity[i][0]/velocity[i][1];
		position[i][1] = -1+apotheme;
	    }
	}

	
	

	printf("FPS: %f\n", 1/delta);
	float kEnergy = 0;
	for(int i = 0; i < n_balls; i++)
	{
	    kEnergy += 0.5*powf(velocity[i][0], 2);
	    kEnergy += 0.5*powf(velocity[i][1], 2);
	}

	printf("Kinetic energy: %fJ\n", kEnergy);

	int res[200];
	for(int i = 0; i < 200; i++)
	    res[i] = 0;
	numberOfMoleculesByIntervalOfVelocities(velocity, res, n_balls, 0.01);


	drawPlot(res, 200, plotShaderProgram);

	if(time > 30.0)
	{
	    FILE *fp = fopen("output.csv", "w");
	    fprintf(fp, "v\n");

	    for(int i = 0; i < n_balls; i++)
	    {
		float v;
		
		v = sqrt(powf(velocity[i][0], 2) + powf(velocity[i][1], 2));
		fprintf(fp, "%f\n", v);
	    }
	    
	    fclose(fp);
	}

	glUseProgram(shaderProgram);
	for(int i = 0; i < n_balls; i++)
	{
//	    computeRotationMatrix(transMatrix, PI/6);
	    computeScaleMatrix(transMatrix);
	    computeTranslationMatrix(transMatrix, (float [3]){position[i][0], position[i][1], 0});
/*	    for(int j = 0; j < 4; j++)
		printf("%f %f %f %f\n", transMatrix[j][0], transMatrix[j][1], 
		    transMatrix[j][2], transMatrix[j][3]); */

	    unsigned int transformationMatrixPosition = glGetUniformLocation(shaderProgram,
		    "transformationMatrix");
	    glUniformMatrix4fv(transformationMatrixPosition, 1, GL_FALSE, &transMatrix[0][0]);

	    unsigned int colorPosition = glGetUniformLocation(shaderProgram, "color");
	    glUniform3fv(colorPosition, 1, colors[i]);

	    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, hexagon.EBO);
	    glBindVertexArray(hexagon.VAO);

	    glDrawElements(GL_TRIANGLES, 300, GL_UNSIGNED_INT, 0); 
	}

	glfwSwapBuffers(window);
	glfwPollEvents();

	while(glfwGetTime() < 2.0);

	float distance;
	/*	if(distance <= 2*apotheme)
	{
	    sphereCollision(position, velocity, 0, 1);
	} */

	for(int i = 0; i < n_balls - 1; i++)
	{
	    for(int j = i + 1; j < n_balls; j++)
	    {

		distance = powl(position[j][0] - position[i][0], 2) + powl(position[j][1] - position[i][1], 2);
		distance = sqrt(distance);
		distance = distance >= 0 ? distance : distance*-1;

		if(distance <= 0.02)
		    sphereCollision(position, velocity, i, j);
	    }
	}
    }
    glDeleteShader(shaderProgram);
    glDeleteShader(plotShaderProgram);
}
