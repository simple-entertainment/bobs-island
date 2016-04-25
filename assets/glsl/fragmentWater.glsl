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

float applyWave1(vec3 position, float time)
{
	float amplitude = 0.4;
	float waveLengthX = 0.5;
	float waveLengthY = 0.2;
	float waveTimeX = time * 0.05;
	float waveTimeY = time * 0.13;

	return sin(position.x / waveLengthX + waveTimeX) * cos(position.z / waveLengthY + waveTimeY) * amplitude;
}

float applyWave2(vec3 position, float time)
{
	float amplitude = 2.7;
	float waveLengthX = -1.32;
	float waveLengthY = 1.75;
	float waveTimeX = time * 1.75;
	float waveTimeY = time * 0.9;

	return sin(position.x / waveLengthX + waveTimeX) * cos(position.z / waveLengthY + waveTimeY) * amplitude;
}

float applyWave3(vec3 position, float time)
{
	float amplitude = 1.3;
	float waveLengthX = -11.29;
	float waveLengthY = -1.234;
	float waveTimeX = time * 1.4;
	float waveTimeY = time * -0.63;

	return sin(position.x / waveLengthX + waveTimeX) * cos(position.z / waveLengthY + waveTimeY) * amplitude;
}

float applyWave4(vec3 position, float time)
{
	float amplitude = 0.72;
	float waveLengthX = 2.23;
	float waveLengthY = -3.35;
	float waveTimeX = time * 0.021;
	float waveTimeY = time * 0.056;

	return sin(position.x / waveLengthX + waveTimeX) * cos(position.z / waveLengthY + waveTimeY) * amplitude;
}

float applyWave5(vec3 position, float time)
{
	float amplitude = 1.74;
	float waveLengthX = -1.015;
	float waveLengthY = 0.937;
	float waveTimeX = time * 0.4;
	float waveTimeY = time * 0.7;

	return sin(position.x / waveLengthX + waveTimeX) * cos(position.z / waveLengthY + waveTimeY) * amplitude;
}

float applyWave6(vec3 position, float time)
{
	float amplitude = 2.31;
	float waveLengthX = -1.23;
	float waveLengthY = 3.24;
	float waveTimeX = time * 1.42;
	float waveTimeY = time * 2.7;

	return sin(position.x / waveLengthX + waveTimeX) * cos(position.z / waveLengthY + waveTimeY) * amplitude;
}

float applyWaves(vec3 position, float time)
{
	return applyWave1(position, time) +
	       applyWave2(position, time) +
	       applyWave3(position, time) +
	       applyWave4(position, time) +
	       applyWave5(position, time) +
	       applyWave6(position, time);
}

// /////////////////////////
// Variables
// /////////////////////////

in Point point;

uniform vec3 cameraPosition;
uniform Light theSunLight;
uniform float time = 0.0;

layout(location = 0) out vec4 color;
layout(location = 1) out vec4 color2;

// /////////////////////////
// Shader
// /////////////////////////

void main()
{
	float delta = 0.0001;

	float wave = applyWaves(point.worldPosition, time);

	vec3 northPosition = point.worldPosition;
	northPosition.z -= delta;
	northPosition.y = applyWaves(northPosition, time);
	vec3 northEdge = vec3(0.0, northPosition.y - wave, -delta);
	northEdge = normalize(northEdge);

	vec3 eastPosition = point.worldPosition;
	eastPosition.x += delta;
	eastPosition.y = applyWaves(eastPosition, time);
	vec3 eastEdge = vec3(delta, eastPosition.y - wave, 0.0);
	eastEdge = normalize(eastEdge);

	vec3 southPosition = point.worldPosition;
	southPosition.z += delta;
	southPosition.y = applyWaves(southPosition, time);
	vec3 southEdge = vec3(0.0, southPosition.y - wave, delta);
	southEdge = normalize(southEdge);

	vec3 westPosition = point.worldPosition;
	westPosition.x -= delta;
	westPosition.y = applyWaves(westPosition, time);
	vec3 westEdge = vec3(-delta, westPosition.y - wave, 0.0);
	westEdge = normalize(westEdge);

	vec3 waveNormal = normalize(cross(northEdge, westEdge) +
	                            cross(westEdge, southEdge) +
	                            cross(southEdge, eastEdge) +
	                            cross(eastEdge, northEdge));

	Point modifiedPoint = point;
	modifiedPoint.normal = waveNormal;

	color = applyDirectionalLight(modifiedPoint, theSunLight, cameraPosition);
	color.a = 1.0;

	color2 = vec4(0.0, 0.0, 0.0, 1.0);
}
