#version 430 core
uniform sampler2D textureMap;
in vec2 textureCoordinate; 
in vec3 norm_final; 
in vec3 light_vector; 
in vec3 cam_vector; 

out vec4 color;
//Ambient reflectence coefficient = (0.25 , 0.25 , 0.25 , 1.0) Ambient light color = (0.3 , 0.3 , 0.3 , 1.0)
//Specular reflectence coefficient = (1.0 , 1.0 , 1.0 , 1.0) Specular light color = (1.0 , 1.0 , 1.0 , 1.0)
//Specular exponent = 100
//Diffuse reflectence coefficient = (1.0 , 1.0 , 1.0 , 1.0) Diffuse light color = (1.0 , 1.0 , 1.0 , 1.0)

void main() {
    vec4 textureColor = texture(textureMap, textureCoordinate);
    vec4 ambientCoef =  vec4(0.25f , 0.25f , 0.25f , 1.0f);
    vec4 lightColorAmbient = vec4(0.3f , 0.3f , 0.3f , 1.0f);
    vec4 specCoef = vec4(1.0f , 1.0f , 1.0f , 1.0f);
    vec4 lightColorSpec = vec4(1.0f , 1.0f , 1.0f , 1.0f);
    int specExp = 100;
    vec4 difCoef = vec4(1.0f , 1.0f , 1.0f , 1.0f);
    vec4 lightColorDif = vec4(1.0f , 1.0f , 1.0f , 1.0f);

    vec3 half_vector = normalize(light_vector + cam_vector);

    float cosAlpha = clamp(dot(norm_final, half_vector), 0.0f, 1.0f);
    float cosTheta = clamp(dot(norm_final, light_vector), 0.0f, 1.0f);

    vec4 ambientColor = lightColorAmbient * ambientCoef;
    vec4 diffuseColor = lightColorDif * difCoef * cosTheta;
    vec4 specularColor = lightColorSpec * specCoef * pow(cosAlpha,specExp);

    vec3 completeShaders = vec3(ambientColor + diffuseColor + specularColor);
    vec3 newCol = vec3(textureColor.x,textureColor.y,textureColor.z) ;
    color = vec4(clamp( newCol*completeShaders , 0.0f, 1.0f), 1.0f);
    //color = vec4(norm_final,1);                                       //DEBUG
}

