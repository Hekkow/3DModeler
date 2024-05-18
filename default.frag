#version 330 core
out vec4 FragColor;

in vec3 color;
in vec2 texCoord;
uniform sampler2D tex0;
uniform vec4 edgeColor;
uniform bool isEdge;
uniform bool highlighted;
void main()
{
	if (highlighted) {
		FragColor = vec4(1, 0, 0, 1);
	}
	else if (isEdge) {
		FragColor = vec4(0, 0, 1, 1);
	}
	else {
		//FragColor = vec4(0, 0, 1, 1);
		FragColor = texture(tex0, texCoord);
	}
   
}