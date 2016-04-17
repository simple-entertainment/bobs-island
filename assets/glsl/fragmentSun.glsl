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
// Functions
// /////////////////////////

vec4 applyDirectionalLight(Point point, Light light, vec3 cameraPosition)
{
	// Add the ambient term.
	vec4 color = point.color * light.ambient;

	float diffuseFactor = dot(-light.direction, point.normal);
	if(diffuseFactor > 0.0f)
	{
		// Add the diffuse term.
		color += diffuseFactor * point.color * light.diffuse;

		// Add the specular term.
		vec3 toCamera = normalize(cameraPosition - point.worldPosition);
        vec3 lightReflect = normalize(reflect(light.direction, point.normal));
        float specularFactor = dot(toCamera, lightReflect);

        specularFactor = pow(specularFactor, light.strength);
        if (specularFactor > 0.0f)
        {
            color += specularFactor * light.specular;
        }
	}

	return color;
}

// /////////////////////////
// Variables
// /////////////////////////

in Point point;

uniform vec3 cameraPosition;
uniform Light flashLight;
uniform Light theSunLight;
uniform sampler2D sampler;
uniform int samplerEnabled;

layout(location = 0) out vec4 color;
layout(location = 1) out vec4 color2;

// /////////////////////////
// Shader
// /////////////////////////

void main()
{
	if (samplerEnabled == 1)
	{
		color = texture2D(sampler, point.texCoord);
		return;
	}

	color = applyDirectionalLight(point, theSunLight, cameraPosition);

	// Apply bloom to sun
	color2 = color;
}
