#version 330 core

in vec3 vertex_position;
in vec3 vertex_normal;
in vec2 vertex_uv;

uniform int u_frame;
uniform sampler2D u_texture;
uniform samplerCube shadowMap;
uniform vec3 u_light_pos;
uniform vec3 u_light_color;
uniform float gamma;
uniform vec3 viewPos;

out vec4 FragColor;

vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

float ShadowCalculation(vec3 fragPos)
{
    // get vector between fragment position and light position
    vec3 fragToLight = fragPos - u_light_pos;
    float currentDepth = length(fragToLight);
    
    float shadow = 0.0;
    float bias = 0.15;
    int samples = 20;
    float viewDistance = length(viewPos - fragPos);
    float diskRadius = (1.0 + (viewDistance / 120.0f)) / 25.0;
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(shadowMap, fragToLight + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= 120.0f;   // undo mapping [0;1]
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);    
        
    return shadow;
}

void main()
{
	// diff light
	vec3 norm = normalize(vertex_normal);
	vec3 lightDir = normalize(u_light_pos - vertex_position);  
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * u_light_color;
    
    // specular
    vec3 viewDir = normalize(viewPos - vertex_position);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(norm, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * u_light_color;

	// ambient light
	//vec3 ambientLight = vec3(0.7,0.7,0.7);
    vec3 ambient = 0.3 * u_light_color;

	// texture
	vec3 surfaceColor = texture(u_texture, vertex_uv).rgb;
	
    float shadow = ShadowCalculation(vertex_position);

	// final color
	vec3 color = surfaceColor * (ambient + (1.0 - shadow) * (diffuse + specular));

	FragColor = vec4(color, 1.0f);
}
