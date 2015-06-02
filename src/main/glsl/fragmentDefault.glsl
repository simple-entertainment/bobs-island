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

in Point point;

uniform vec3 cameraPosition;
uniform Light flashLight;
uniform Light theSunLight;
uniform sampler2D sampler;
uniform int samplerEnabled;

layout(location = 0) out vec4 color;

// /////////////////////////
// Shader
// /////////////////////////

void main()
{
	Point point2 = point;

	if (samplerEnabled == 1)
	{
		point2.color = texture2D(sampler, point2.texCoord);
	}

	Light theSunLight1 = theSunLight;
	float sunFactor = max(min(sin(theSunLight.position.y / 1000.0f) + 0.25f, 1.0f), 0.0f);
	theSunLight1.ambient *= max(sunFactor, 0.2f);
	theSunLight1.diffuse *= sunFactor;
	theSunLight1.specular *= sunFactor;

	// Grass shader.
	if (near(point.color.x, 0.0f) && near(point.color.y, 0.5f) && near(point.color.z, 0.0f))
	{
		point2.color.y = getRandomFloatZeroToOne(point.worldPosition.xz) * 0.5f + 0.25f;
	}

	// Ocean shader.
	if (near(point.color.x, 0.0f) && near(point.color.y, 0.4f) && near(point.color.z, 0.6f))
	{
		if (cameraPosition.y < 0.0f)
		{
			Light theSunLightUnderwater = theSunLight;
			theSunLightUnderwater.ambient = vec4(0.0f, 0.0f, 0.0f, 1.0f);
			theSunLightUnderwater.attenuation = vec3(0.5f, 0.00025f * -cameraPosition.y, 0.0f);
			color = applyPointLight(point, theSunLightUnderwater, cameraPosition * vec3(1.0f, -1.0f, 1.0f));

			color /= length(cameraPosition - point.worldPosition) * 0.05f;

			return;
		}
	}

	// Sand shader.
	if (near(point.color.x, 0.83f) && near(point.color.y, 0.65f) && near(point.color.z, 0.15f))
	{
		if (isGrain(point.worldPosition.xyz, 0.0f, 100.0f, 0.3f))
		{
			point2.color = vec4(0.72f, 0.44f, 0.04f, 1.0f);
		}
		if (isGrain(point.worldPosition.xyz, 1.0f, 100.0f, 0.3f))
		{
			point2.color = vec4(0.6f, 0.6f, 0.6f, 1.0f);
		}
	}

	// Sky shader.
	if (near(point.color.x, 0.0f) && near(point.color.y, 0.5f) && near(point.color.z, 0.75f))
	{
		point2.color.x = (1.0f - sunFactor) * 0.5f;
	}

	// Tree shader.
	if (near(point.color.x, 0.47f) && near(point.color.y, 0.24f) && near(point.color.z, 0.0f))
	{
		point2.color *= getRandomFloatZeroToOne(point.worldPosition.xy * 0.000001f) * 0.5f + 0.25f;
	}

	color = applyDirectionalLight(point2, theSunLight1, cameraPosition);

	color.y *= sunFactor;
	color.z *= sunFactor;

	color += applyPointLight(point2, flashLight, cameraPosition);
}
