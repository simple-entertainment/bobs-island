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

vec4 applyPointLight(Point point, Light light, vec3 cameraPosition)
{
	vec3 toLight = light.position - point.worldPosition;
	float distanceToLight = length(toLight);
	toLight /= distanceToLight;

	// If the point is out of range, do not light it.
	if(distanceToLight > light.range)
	{
		return vec4(0.0f, 0.0f, 0.0f, 1.0f);
	}

	// Add the ambient term.
	vec4 color = point.color * light.ambient;

	float diffuseFactor = dot(toLight, point.normal);
	if(diffuseFactor > 0.0f)
	{
		// Add the diffuse term.
		color += diffuseFactor * point.color * light.diffuse;

		// Add the specular term.
		vec3 toCamera = normalize(cameraPosition - point.worldPosition);
		vec3 lightReflect = normalize(reflect(-toLight, point.normal));
		float specularFactor = dot(toCamera, lightReflect);

		specularFactor = pow(specularFactor, light.strength);
		if (specularFactor > 0.0f)
		{
			color += specularFactor * light.specular;
		}
	}

	// Attenuate
	color /= dot(light.attenuation, vec3(1.0f, distanceToLight, distanceToLight * distanceToLight));
	
	return color;
}

vec4 applySpotLight(Point point, Light light, vec3 cameraPosition)
{
	vec3 toLight = normalize(light.position - point.worldPosition);

	return applyPointLight(point, light, cameraPosition) *
		pow(max(dot(-toLight, light.direction), 0.0f), light.strength);
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
	//color += applyPointLight(point, flashLight, cameraPosition);
	color.a = 1.0;

	color2 = vec4(0.0, 0.0, 0.0, 1.0);
}
