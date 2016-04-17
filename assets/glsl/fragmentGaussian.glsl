#version 330

// /////////////////////////
// Structures
// /////////////////////////

struct Light
{
	vec4 ambient;
	vec3 attenuation;
	vec4 diffuse;
	vec3 direction;
	vec3 position;
	float range;
	vec4 specular;
	float strength;
};

struct Point
{
	vec4 clipPosition;
	vec4 color;
	vec3 normal;
	vec2 texCoord;
	vec3 worldPosition;
};

// /////////////////////////
// Variables
// /////////////////////////

in Point point;

uniform bool horizontal;
uniform sampler2D sampler;
uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

layout(location = 0) out vec4 color;

// /////////////////////////
// Shader
// /////////////////////////

void main()
{
    vec2 texelSize = 1.0 / textureSize(sampler, 0);

    vec3 color3 = texture(sampler, point.texCoord).rgb * weight[0];

    if (horizontal)
    {
        for (int index = 1; index < 5; index++)
        {
            color3 += texture(sampler, point.texCoord + vec2(texelSize.x * index, 0.0)).rgb * weight[index];
            color3 += texture(sampler, point.texCoord - vec2(texelSize.x * index, 0.0)).rgb * weight[index];
        }
    }
    else
    {
        for (int index = 1; index < 5; index++)
        {
            color3 += texture(sampler, point.texCoord + vec2(0.0, texelSize.y * index)).rgb * weight[index];
            color3 += texture(sampler, point.texCoord - vec2(0.0, texelSize.y * index)).rgb * weight[index];
        }
    }
    color = vec4(color3, 1.0);
}
