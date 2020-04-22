#version 330 core 

in vec3 fragNor;
in vec3 lightDir;
in vec3 v;
uniform float shine;
uniform vec3 MatAmb;
uniform vec3 MatDif;
uniform vec3 MatSpec;

out vec4 color;

float bin(float a){
    for(float i = 0.0; i < 1; i += .1){
        if(a < i){
            return i - .1;
        }
    }
    return .9;
}

vec4 binColor(vec4 incolor){
    return vec4(bin(incolor.x), bin(incolor.y), bin(incolor.z), incolor.w);
}
void main()
{
	vec3 normal = normalize(fragNor);
    vec4 tempColor;
    // Map normal in the range [-1, 1] to color in range [0, 1];
    vec3 light = normalize(lightDir);
    vec3 H = normalize(light + normalize(v));
    tempColor = vec4(MatDif*max(0, dot(normal, light)), 1.0) + vec4(MatSpec*pow(max(0, dot(normal, H)), shine),1.0) + vec4(MatAmb, 1.0);
    color = binColor(tempColor);
}