#version 330 core

in vec2 ex_texCoord;

out vec4 color;

uniform int selector;
uniform sampler2D texture0;

void main()
{   
    color = texture(texture0, ex_texCoord);
    
    if(color.a < 0.1)
        discard;
}