#version 300 es

precision mediump float;

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D baseColorMap;
void main() 
{
  vec4 v = texture(baseColorMap, TexCoords);
  FragColor = v;
}
