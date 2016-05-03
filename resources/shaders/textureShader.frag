#version 440 core
in vec2 TexCoord;

out vec4 color;

// Texture samplers
uniform sampler2D ourTexture;

void main()
{
	//color = texture(ourTexture, TexCoord);//Old
	vec4 texColor = texture(ourTexture, TexCoord);
    if(texColor.a < 0.1)
        discard;
    color = texColor;

}