#version 430 core

layout (location = 0) in vec3 aPos;   // the position variable has attribute position 0

uniform mat4 mod_proj_view;
uniform mat4 mod_view;
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
uniform float offset;

// We need these in fragment shader
out vec2 textureCoordinate; 
out vec3 norm_final; 
out vec3 light_vector; 
out vec3 cam_vector; 

vec3 computeFaceNormal( in vec3 v1,  in vec3 v2 , in vec3 v3){
    vec3 norm = -normalize(cross(normalize(v1-v2),normalize(v3-v2)));
    return norm;
}

void compute_height(inout vec3 v){
    float h_x = 1.0f - (float(v.x) / float(width+1)); 
    float h_y = 1.0f - (float(v.z) / float(height+1)); 
    h_x = h_x - offset; 
    vec2 heightCoordinate = vec2(h_x,h_y);
    vec4 height_vec = texture(heightMap, heightCoordinate);
    v.y = float(height_vec.x) * heightFactor;
}

void main() 
{
    // We have to find the current positions texture coordinate and then we need to find the color of this point
    // As i understood interpolation done in gpu itself
    vec3 position = aPos; // Current position comes from input aPos
    position.x = position.x-offset;
    // Calculating coordinate
    float x = 1.0f - (float(position.x) / float(width+1));
    x = x - offset; 
    float y = 1.0f - (float(position.z) / float(height+1));
    float h_x = 1.0f - (float(position.x) / float(h_width+1));
    h_x = h_x - offset;
    float h_y = 1.0f - (float(position.z) / float(h_height+1));    
    textureCoordinate = vec2(x,y);
    vec2 heightCoordinate = vec2(h_x,h_y);

    // TextureColor of the calculated coordinate

    vec4 height_vec = texture(heightMap, heightCoordinate);
    position.y = float(height_vec.x) * heightFactor;
    // At this point we have to calculate other outputs 
    // for the normal inspect link below
    //To implement Phong shading, you will have to compute the normal vectors of the vertices at the vertex shader 
    //and define the normal vector as a varying variable so that the normal vectors will be interpolated by the rasterizer 
    //and passed to the fragment shader. The normal vector of a vertex is defined as the average of the 
    //normal vectors of the triangles that this vertex is adjacent to. 
    //You will have to query the heights of the neighboring vertices (hint: use texture look-up for this, too) 
    //to compute the normals of the adjacent triangles.
    gl_Position = mod_proj_view * vec4(position, 1.0);
    
    vec3 top = vec3(aPos.x, 0, aPos.z + 1);
    compute_height(top);
    vec3 bottom = vec3(aPos.x, 0, aPos.z - 1);
    compute_height(bottom);

    vec3 left = vec3(aPos.x-1, 0, aPos.z);
    compute_height(left);

    vec3 right = vec3(aPos.x+1, 0, aPos.z);
    compute_height(right);

    vec3 topright = vec3(aPos.x+1, 0, aPos.z+1);
    compute_height(topright);

    vec3 bottomleft = vec3(aPos.x-1, 0, aPos.z-1);
    compute_height(bottomleft);
    vec3 norm1 = vec3(0.0f,0.0f,0.0f);
    vec3 norm2 = vec3(0.0f,0.0f,0.0f);
    vec3 norm3 = vec3(0.0f,0.0f,0.0f);
    vec3 norm4 = vec3(0.0f,0.0f,0.0f);
    vec3 norm5 = vec3(0.0f,0.0f,0.0f);
    vec3 norm6 = vec3(0.0f,0.0f,0.0f);

    if(aPos.z == height-1 && aPos.x == 0) {
            //Top Left
        norm4 =computeFaceNormal(bottom,aPos,right);

    }
    else if(aPos.z == height-1 && aPos.x==width-1){
        //Top Right
        norm5 =computeFaceNormal(bottomleft,aPos,bottom);
        norm6 =computeFaceNormal(left,aPos,bottomleft);
    }
    else if(aPos.z == 0 && aPos.x== width-1) {
            // bottom right
        norm3 =computeFaceNormal(top,aPos,left);
    }
    else if(aPos.z == 0 && aPos.x== 0) {
        // bottom left
        norm1 = computeFaceNormal(topright,aPos,top);
        norm2 =computeFaceNormal(right,aPos,topright);
        
    }
    else if(aPos.z == 0){
        norm1 = computeFaceNormal(topright,aPos,top);
        norm2 = computeFaceNormal(right,aPos,topright);
        norm3 = computeFaceNormal(top,aPos,left);
        //bottom
    }
    else if(aPos.z == height-1){
        //top
        norm4 = computeFaceNormal(bottom,aPos,right);
        norm5 = computeFaceNormal(bottomleft,aPos,bottom);
        norm6 = computeFaceNormal(left,aPos,bottomleft);
    }
    else if(aPos.x == 0){
        //left
        norm1 = computeFaceNormal(topright,aPos,top);
        norm2 = computeFaceNormal(right,aPos,topright);     
        norm4 = computeFaceNormal(bottom,aPos,right);
    }
    else if(aPos.x == width-1){
        //right
        norm3 = computeFaceNormal(top,aPos,left);
        norm5 = computeFaceNormal(bottomleft,aPos,bottom);
        norm6 = computeFaceNormal(left,aPos,bottomleft);
    }
    else {        

        norm1 = computeFaceNormal(topright,aPos,top);
        norm2 = computeFaceNormal(right,aPos,topright);
        norm3 = computeFaceNormal(top,aPos,left);
        norm4 = computeFaceNormal(bottom,aPos,right);
        norm5 = computeFaceNormal(bottomleft,aPos,bottom);
        norm6 = computeFaceNormal(left,aPos,bottomleft);
    }
    

    vec3 totalNormals = (norm1 + norm2 + norm3 + norm4 + norm5 + norm6);
    norm_final = normalize(totalNormals);
    norm_final = vec3(norm_trans*vec4(norm_final,0));

    cam_vector = normalize(vec3(mod_view * (vec4(vec3(camera_pos) - position, 0))));
    light_vector = normalize(vec3(mod_view * vec4(vec3(light_pos.x, light_pos.y, light_pos.z) - position, 0)));
    
    
}