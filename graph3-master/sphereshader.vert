#version 430 core

layout (location = 0) in vec3 aPos;   // the position variable has attribute position 0
layout (location = 1) in vec2 coord;

uniform mat4 mod_proj_view;
uniform mat4 MV;
uniform mat4 norm_trans;
uniform float heightFactor;
uniform vec3 camera_pos;
uniform vec3 light_pos;
uniform int width;
uniform int height;
uniform int h_width;
uniform int h_height;
uniform sampler2D textureMap;
uniform sampler2D heightMap;
uniform vec3 center;
uniform float offset;
// We need these in fragment shader
out vec2 textureCoordinate; 
out vec3 vertexNormal; 
out vec3 vertexToLightVec; 
out vec3 vertexToCamVec; 


void main() 
{
    // We have to find the current positions texture coordinate and then we need to find the color of this point
    // As i understood interpolation done in gpu itself
    vec3 position = aPos; // Current position comes from input aPos

    float x = coord.x/125.0f; 
    float y = coord.y/250.0f;
    float h_x = coord.x/125.0f;
    float h_y = coord.y/250.0f;
    x = x - offset;
    h_x = h_x - offset;        
    textureCoordinate = vec2(x,y);
    vec2 heightCoordinate = vec2(h_x,h_y);
    position.x = position.x - offset; 

    vertexNormal = normalize(vec3((position.x),(position.y),(position.z))); 
        
    vec4 height_vec = texture(heightMap, textureCoordinate);
    vec3 temp = (float(height_vec.x) *vertexNormal*heightFactor)+position;
    position = temp;

    vertexNormal = vec3(norm_trans*vec4(vertexNormal,1));
    
    vertexToCamVec = normalize(vec3(MV * (vec4(vec3(camera_pos) - position, 0))));
    vertexToLightVec = normalize(vec3(MV * vec4(vec3(light_pos.x, light_pos.y, light_pos.z) - position, 0)));
    gl_Position = mod_proj_view * vec4(position, 1.0);
    
    
}
