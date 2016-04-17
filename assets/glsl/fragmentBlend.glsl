#version 330

// /////////////////////////
// Structures
// /////////////////////////

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

uniform sampler2D bloom;
uniform float exposure = 1.0;
uniform float gamma = 2.0;
uniform sampler2D sampler;

layout(location = 0) out vec4 color;

// /////////////////////////
// Shader
// /////////////////////////

void main()
{
	vec3 hdrColor = texture(sampler, point.texCoord).rgb;
	vec3 bloomColor = texture(bloom, point.texCoord).rgb;

	 // Additive blending
	vec3 color3 = hdrColor + bloomColor;

	// Tone mapping
	color3 = vec3(1.0) - exp(-color3 * exposure);

	// Gamma correction
	color3 = pow(color3, vec3(1.0 / gamma));

	color = vec4(color3, 1.0f);
}
