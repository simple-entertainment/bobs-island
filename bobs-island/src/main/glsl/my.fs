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

float getRandomFloat(vec2 seed)
{
    return fract(sin(dot(seed.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

float getRandomFloatZeroToOne(vec2 seed)
{
	float random = getRandomFloat(seed);
	random = abs(random);
    return random - floor(random);
}

bool isGrain(vec3 position, float grainId, float granularity, float probability)
{
	vec2 seed = vec2(int((position.x + grainId) * granularity), int((position.z + grainId) * granularity));

    return getRandomFloatZeroToOne(seed) < probability;
}

bool near(float a, float b)
{
	return abs(a - b) < 0.0001f;
}

// /////////////////////////
// Variables
// /////////////////////////

in Point point1;

uniform vec3 cameraPosition;
uniform Light theSunLight;

out vec4 color;

// /////////////////////////
// Shader
// /////////////////////////

void main()
{
	Point point2 = point1;

	// Grass shader.
	if (near(point1.color.x, 0.0f) && near(point1.color.y, 0.5f) && near(point1.color.z, 0.0f))
	{
		point2.color.y = getRandomFloatZeroToOne(point1.worldPosition.xz) * 0.5f + 0.25f;
	}

	// Ocean shader.
	if (near(point1.color.x, 0.0f) && near(point1.color.y, 0.4f) && near(point1.color.z, 0.6f))
	{
		if (cameraPosition.y < 0.0f)
		{
			Light theSunLightUnderwater = theSunLight;
			theSunLightUnderwater.ambient = vec4(0.0f, 0.0f, 0.0f, 1.0f);
			theSunLightUnderwater.attenuation = vec3(0.5f, 0.00025f * -cameraPosition.y, 0.0f);
			color = applyPointLight(point1, theSunLightUnderwater, cameraPosition * vec3(1.0f, -1.0f, 1.0f));

			color /= length(cameraPosition - point1.worldPosition) * 0.05f;

			return;
		}
	}

	// Sand shader.
	if (near(point1.color.x, 0.83f) && near(point1.color.y, 0.65f) && near(point1.color.z, 0.15f))
	{
		if (isGrain(point1.worldPosition.xyz, 0.0f, 100.0f, 0.3f))
		{
			point2.color = vec4(0.72f, 0.44f, 0.04f, 1.0f);
		}
		if (isGrain(point1.worldPosition.xyz, 1.0f, 100.0f, 0.3f))
		{
			point2.color = vec4(0.6f, 0.6f, 0.6f, 1.0f);
		}
	}

	// Tree shader.
	if (near(point1.color.x, 0.47f) && near(point1.color.y, 0.24f) && near(point1.color.z, 0.0f))
	{
		point2.color *= getRandomFloatZeroToOne(point1.worldPosition.xy * 0.000001f) * 0.5f + 0.25f;
	}

	color = applyDirectionalLight(point2, theSunLight, cameraPosition);
}
