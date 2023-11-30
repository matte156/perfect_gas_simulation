#version 330 core
#define PI 3.1415926535897932384626433832795

layout (location = 0) in vec3 aPos;
// layout (location = 1) in vec4 aColor;

out vec4 ourColor;


uniform float xOffset;
uniform vec3 color;
uniform vec2 parameters;
uniform float apotheme;

uniform float y;

void main()
{
//    gl_Position = vec4(aPos.xyz, 1.0f);
    float yScale = 50.0;

    gl_Position = vec4(aPos.xyz, 1.0f);

    // rotate cube by 45° degrees
    gl_Position *= mat4(
	cos(PI/4), sin(PI/4), 0, 0,
	-sin(PI/4), cos(PI/4), 0,0,
	0, 0, 1, 0,
	0, 0, 0, 1
    );

    // make rectangle fit all screen
    gl_Position *= mat4(
	1/apotheme, 0, 0, 0,
	0, 1/apotheme, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
    );

    
    gl_Position *= mat4(
	420.0/960.0, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
    );

    gl_Position *= mat4(
	1/200.0, 0, 0, 0,
	0, (parameters[1])/yScale, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
    );

    gl_Position += vec4(2*(1080.0 - 1920.0/2.0)/(1920.0), 0, 0, 0);



    gl_Position += vec4(10.0*parameters[0]*0.06, -1 + parameters[1]/yScale, 0, 0);


    ourColor = vec4(color.xyz, 1.0);
}
