#version 130

in vec2 POSITION;

void main()
{
	gl_Position = vec4(POSITION, 0., 1.);
}
