#version 330 core

in vec2 Texcoord;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D tex;
out vec4 outColor;

void main()
{
	vec4 col = vec4(0.3,0.3,0.3,1.0);
	vec3 light = vec3(0.5,0.5,0.5);
	vec3 L = normalize(light - FragPos);
	vec4 Idiff = gl_FrontLightProduct[0].diffuse *  max(dot(Normal,L), 0.0); 
	Idiff = clamp(Idiff,0.0,1.0);
	outColor = Idiff * col;// * texture2D(tex,Texcoord);		
}

