#version 430 core
uniform sampler2D textureMap;
uniform sampler2D heightMap;
in vec2 textureCoordinate; 
in vec3 vertexNormal; 
in vec3 vertexToLightVec; 
in vec3 vertexToCamVec; 
vec4 textureColor;

out vec4 color;
//Ambient reflectence coefficient = (0.25 , 0.25 , 0.25 , 1.0) Ambient light color = (0.3 , 0.3 , 0.3 , 1.0)
//Specular reflectence coefficient = (1.0 , 1.0 , 1.0 , 1.0) Specular light color = (1.0 , 1.0 , 1.0 , 1.0)
//Specular exponent = 100
//Diffuse reflectence coefficient = (1.0 , 1.0 , 1.0 , 1.0) Diffuse light color = (1.0 , 1.0 , 1.0 , 1.0)

void main() {
    
    vec4 ambientCoef =  vec4(0.25 , 0.25 , 0.25 , 1.0);
    vec4 lightColorAmbient = vec4(0.3 , 0.3 , 0.3 , 1.0);
    vec4 specCoef = vec4(1.0 , 1.0 , 1.0 , 1.0);
    vec4 lightColorSpec = vec4(1.0 , 1.0 , 1.0 , 1.0);
    int specExp = 100;
    vec4 difCoef = vec4(1.0 , 1.0 , 1.0 , 1.0);
    vec4 lightColorDif = vec4(1.0 , 1.0 , 1.0 , 1.0);
    vec4 textureColor = texture(textureMap, textureCoordinate);
    vec3 hVec = normalize(vertexToLightVec + vertexToCamVec);

    float cosAlpha = clamp(dot(vertexNormal, hVec), 0.0, 1.0);
    float cosTheta = clamp(dot(vertexNormal, vertexToLightVec), 0.0, 1.0);
    
    vec4 specularColor = lightColorSpec * specCoef * pow(cosAlpha,specExp);
    vec4 ambientColor = lightColorAmbient * ambientCoef;
    vec4 diffuseColor = lightColorDif * difCoef * cosTheta;

    vec3 completeShaders = vec3(ambientColor + diffuseColor + specularColor);
    vec3 newCol = vec3(textureColor.x,textureColor.y,textureColor.z) ;
    color = vec4(clamp( newCol*completeShaders , 0.0, 1.0), 1.0);
    //color = vec4(vertexNormal,1);
}

