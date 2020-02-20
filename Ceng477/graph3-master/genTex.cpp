#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include <jpeglib.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtx/rotate_vector.hpp"


using namespace glm;
using namespace std;


unsigned int ID = 0;
unsigned int texture ;
unsigned int heightmap ;
unsigned int VBO, EBO;
int screen_w = 1000;
int screen_h = 1000;
const GLFWvidmode* fullsize;
GLFWmonitor* primary_monitor; 
float yaw = -90.0f;
float pitch = 0.0f;
GLfloat camera_speed = 0.0f;
int width = 0;
int height = 0;
int h_width;
int h_height;
float offset = 0;
GLfloat heightFactor = 10.0f; 
int nrChannels = 0;   
vec3 camera_dir = vec3(0.0f, 0.0f, -1.0f);
vec3 camera_gaze = vec3(0.0f, 0.0f, 1.0f);
vec3 camera_v = vec3(0.0f, 1.0f, 0.0f);
vec3 camera_u = vec3(0.0f, 0.0f, 0.0f);
vec3 camera_pos = vec3(0.0f, 0.0f, 0.0f);
vec3 light_pos = vec3(0.0f, 0.0f, 0.0f);  
mat4 mod_proj_view = mat4(1.0f);
mat4 mod_view = mat4(1.0f);
mat4 norm_trans = mat4(1.0f);
float* vertexCol;
unsigned int VAO;
void resize_frame(GLFWwindow* window, int width, int height);


void set_uniforms(mat4 &mod_proj_view, mat4 &mod_view, mat4 &norm_trans){

    glUniformMatrix4fv(glGetUniformLocation(ID, "mod_proj_view"), 1, GL_FALSE, &mod_proj_view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(ID, "mod_view"), 1, GL_FALSE, &mod_view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(ID, "norm_trans"), 1, GL_FALSE, &norm_trans[0][0]);
    glUniform1f(glGetUniformLocation(ID, "heightFactor"), heightFactor);
    glUniform3fv(glGetUniformLocation(ID, "camera_pos"), 1, &camera_pos[0]); 
    glUniform3fv(glGetUniformLocation(ID, "light_pos"), 1, &light_pos[0]);
    glUniform1i(glGetUniformLocation(ID, "width"),width);
    glUniform1i(glGetUniformLocation(ID, "height"),height);
    glUniform1i(glGetUniformLocation(ID, "h_width"),h_width);
    glUniform1i(glGetUniformLocation(ID, "h_height"),h_height); 
    glUniform1i(glGetUniformLocation(ID, "heightMap"),0);   
	glUniform1i(glGetUniformLocation(ID, "textureMap"),1);
    glUniform1f(glGetUniformLocation(ID, "offset"),offset);
       
}
bool readDataFromFile(const string& fileName, string &data) {              // CODE FROM LAST YEAR'S TEMPLATE
    fstream myfile;

    myfile.open(fileName.c_str(), std::ios::in);

    if (myfile.is_open()) {
        string curLine;

        while (getline(myfile, curLine)){
            data += curLine;
            if (!myfile.eof())
                data += "\n";
        }

        myfile.close();
    }
    else{

        return false;
    }

    return true;
}

void readTexture(char* filename, int f ){                                 // CODE FROM LAST YEAR'S TEMPLATE  

    int temp_w , temp_h;
    unsigned char *raw_image = NULL;
    int bytes_per_pixel = 3;   /* or 1 for GRACYSCALE images */
    int color_space = JCS_RGB; /* or JCS_GRAYSCALE for grayscale images */

    /* these are standard libjpeg structures for reading(decompression) */
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    /* libjpeg data structure for storing one row, that is, scanline of an image */
    JSAMPROW row_pointer[1];

    FILE *infile = fopen( filename, "rb" );
    unsigned long location = 0;
    int i = 0, j = 0;

    if ( !infile )
    {
        printf("Error opening jpeg file %s\n!", filename );
        return;
    }
    printf("Texture filename = %s\n",filename);

    /* here we set up the standard libjpeg error handler */
    cinfo.err = jpeg_std_error( &jerr );
    /* setup decompression process and source, then read JPEG header */
    jpeg_create_decompress( &cinfo );
    /* this makes the library read from infile */
    jpeg_stdio_src( &cinfo, infile );
    /* reading the image header which contains image information */
    jpeg_read_header( &cinfo, TRUE );
    /* Start decompression jpeg here */
    jpeg_start_decompress( &cinfo );

    /* allocate memory to hold the uncompressed image */
    raw_image = (unsigned char*)malloc( cinfo.output_width*cinfo.output_height*cinfo.num_components );
    /* now actually read the jpeg into the raw buffer */
    row_pointer[0] = (unsigned char *)malloc( cinfo.output_width*cinfo.num_components );
    /* read one scan line at a time */
    while( cinfo.output_scanline < cinfo.image_height )
    {
        jpeg_read_scanlines( &cinfo, row_pointer, 1 );
        for( i=0; i<cinfo.image_width*cinfo.num_components;i++)
            raw_image[location++] = row_pointer[0][i];
    }

    temp_h = cinfo.image_height;
    temp_w = cinfo.image_width;
    if(f == 1){
        width = temp_w;
        height = temp_h;
    }
    else{
        h_width = temp_w;
        h_height = temp_h;        
    }

    if(f == 1){
        glActiveTexture(GL_TEXTURE0+1);
        glGenTextures(1,&texture);
        glBindTexture(GL_TEXTURE_2D, texture);        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, temp_w, temp_h, 0, GL_RGB, GL_UNSIGNED_BYTE, raw_image);        
    }
    else{
        glActiveTexture(GL_TEXTURE0);
        glGenTextures(1,&heightmap);
        glBindTexture(GL_TEXTURE_2D, heightmap);   
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, temp_w, temp_h, 0, GL_RGB, GL_UNSIGNED_BYTE, raw_image);             
    }



    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);    

    jpeg_finish_decompress( &cinfo );
    jpeg_destroy_decompress( &cinfo );
    free( row_pointer[0] );
    free( raw_image );
    fclose( infile );

}

void initialize_shader(){


    string v_code;
    string f_code;


    if (!readDataFromFile("shader.vert", v_code)) {
        cout << "Cannot find file name: shader.vert" << endl;
        exit(-1);
    }

    GLint lengthvert = v_code.length();
    const GLchar* vrt_shader = (const GLchar*) v_code.c_str();
     if (!readDataFromFile("shader.frag", f_code)) {
        cout << "Cannot find file name: hader.frag" << endl;
        exit(-1);
    }

    GLint lengthfrag = v_code.length();
    const GLchar* frg_shader = (const GLchar*) f_code.c_str();

    unsigned int vertex = 0;
    unsigned int fragment = 0;

    vertex = glCreateShader(GL_VERTEX_SHADER);                                          // VERTEX SHADER
    glShaderSource(vertex, 1, &vrt_shader, &lengthvert);
    glCompileShader(vertex);

                                                                                        // FRAGMENT SHADER
    fragment = glCreateShader(GL_FRAGMENT_SHADER);   
    glShaderSource(fragment, 1, &frg_shader, &lengthfrag);
    glCompileShader(fragment);
    
                                                                                        // LINK
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);

    glDeleteShader(vertex);
    glDeleteShader(fragment);    
}

void use_shader(){

    glUseProgram(ID);
}

void clear_all(){

    glClearDepth(1.0f);
    glClearStencil(0.0f);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);
    glClear(GL_STENCIL_BUFFER_BIT); 
}

mat4 camera_init(){
    camera_pos.x = width/2;
    camera_pos.y = width/10;
    camera_pos.z = -width/4; 
    mat4 view = mat4(1.0f);
    vec3 camera_target = camera_pos + camera_gaze*0.1f ; //near
    camera_u = (cross(camera_v,camera_gaze));
    view = lookAt(camera_pos, camera_target, camera_v);
    return view; 
}

mat4 camera_comp(){ 
    mat4 view = mat4(1.0f);
    camera_pos += camera_gaze * camera_speed;
    vec3 camera_target = camera_pos + camera_gaze*0.1f;
    camera_u = normalize(cross(camera_v,camera_gaze));
    view = lookAt(camera_pos, camera_target, camera_v);
    mat4 projection = mat4(1.0f);
    projection = perspective(45.0f, 1.0f, 0.1f, 1000.0f);
    mod_proj_view = projection*view;
    mod_view = view;   
    norm_trans = mat4(1.0f);
    norm_trans = inverseTranspose(view);  
    glUniformMatrix4fv(glGetUniformLocation(ID, "mod_proj_view"), 1, GL_FALSE, &mod_proj_view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(ID, "mod_view"), 1, GL_FALSE, &mod_view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(ID, "norm_trans"), 1, GL_FALSE, &norm_trans[0][0]);        
    glUniform3fv(glGetUniformLocation(ID, "camera_pos"), 1, &camera_pos[0]);
    return view; 
}

void light_init(){    
    light_pos.x = width/2;
    light_pos.y = 100;
    light_pos.z = height/2;
}

void vertice_conf(){
    int v_size = 6*(width+1)*(height+1);
    vertexCol = new float[v_size*3];
    int k = 0;
    for(int i=0; i<width+1; i++){
        for(int j=0; j<height+1; j++){
            vertexCol[k] = i;
            vertexCol[k+1] = 0;
            vertexCol[k+2] = j+1;

            k = k+3;
            vertexCol[k] = i;
            vertexCol[k+1] = 0;
            vertexCol[k+2] = j;

            k = k+3;
            vertexCol[k] = i+1;
            vertexCol[k+1] = 0;
            vertexCol[k+2] = j+1;
                                                                                    
            k = k+3;

            vertexCol[k] = i;
            vertexCol[k+1] = 0;
            vertexCol[k+2] = j;

            k = k+3;
            vertexCol[k] = i+1;
            vertexCol[k+1] = 0;
            vertexCol[k+2] = j+1;

            k = k+3;
            vertexCol[k] = i+1;
            vertexCol[k+1] = 0;
            vertexCol[k+2] = j;

            k = k+3;

        }
    }
    unsigned int* indices ;
    int i_size = width*height*2;
    indices = new unsigned int[i_size*3] ; // BURAYA BAKARLAR

    k = 0;

    for (int  i=0; i<width*height*6;i=i+6){
        indices[k] = i;
        indices[k+1] = i+1;
        indices[k+2] = i+2;
        indices[k+3] = i+3;
        indices[k+4] = i+4;
        indices[k+5] = i+5;
        k = k+6;        
    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*v_size*3, vertexCol, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*i_size*3, indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, (void*)0);
    glEnableVertexAttribArray(0);

}

void processInput(GLFWwindow *window ,int key, int scode, int act, int mods)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }
    if(glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS){
        //return to initial position
        camera_speed = 0.0f;
        camera_dir.x = 0.0f;
        camera_dir.y = 0.0f;
        camera_dir.z = -1.0f;
        yaw = -90.0f;
        pitch = 0.0f;
        offset = 0.0f;
        heightFactor = 10.0f;
        light_init();
        camera_init();     

    }   
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
        // pitch up
        camera_v = rotate(camera_v, -0.05f, camera_u);
        camera_gaze = rotate(camera_gaze, -0.05f, camera_u);    
    }    
        
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        // pitch down
        camera_v = rotate(camera_v, 0.05f, camera_u);
        camera_gaze = rotate(camera_gaze, 0.05f, camera_u);
    }   
        
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
        // yaw left
        camera_u = rotate(camera_u, 0.05f, camera_v);
        camera_gaze = rotate(camera_gaze, 0.05f, camera_v);
    }   

    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        // yaw right
        camera_u = rotate(camera_u, -0.05f, camera_v);
        camera_gaze = rotate(camera_gaze, -0.05f, camera_v);
    }   
        
    if(glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS){
        // speed change
        camera_speed += 0.01f;
    }   

    if(glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS){
        // speed change 
        camera_speed -= 0.01f;   
    }   

    if(glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS){
        // stop
        camera_speed = 0.0f;
    }

    if(glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS){
        // hf inc
        heightFactor += 0.5f;
        glUniform1f(glGetUniformLocation(ID, "heightFactor"), heightFactor);
    }       
    if(glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS){
        // hf dec
        heightFactor -= 0.5f;
        glUniform1f(glGetUniformLocation(ID, "heightFactor"), heightFactor);
    } 
    if(glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS){
        // hf inc
        light_pos.y += 5.0f;
        glUniform3fv(glGetUniformLocation(ID, "light_pos"), 1, &light_pos[0]);
    }       
    if(glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS){
        // hf dec
        light_pos.y -= 5.0f;
        glUniform3fv(glGetUniformLocation(ID, "light_pos"), 1, &light_pos[0]);
    } 

    if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS){
        // hf inc
        light_pos.z  += 5.0f;
        glUniform3fv(glGetUniformLocation(ID, "light_pos"), 1, &light_pos[0]);
    }       
    if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS){
        // hf dec
        light_pos.z -= 5.0f;
        glUniform3fv(glGetUniformLocation(ID, "light_pos"), 1, &light_pos[0]);
    } 

    if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS){
        // hf inc
        light_pos.x -= 5.0f;
        glUniform3fv(glGetUniformLocation(ID, "light_pos"), 1, &light_pos[0]);
    }       
    if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS){
        // hf dec
        light_pos.x += 5.0f;
        glUniform3fv(glGetUniformLocation(ID, "light_pos"), 1, &light_pos[0]);
    } 
        
    if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS){
        // hf inc
        //camera_pos.x += 1.0f;
        offset += 1.0f/width;
        glUniform1f(glGetUniformLocation(ID, "offset"),offset);
        //glUniform3fv(glGetUniformLocation(ID, "camera_pos"), 1, &camera_pos[0]);
    }       
    if(glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS){
        // hf dec
        //camera_pos.x += -1.0f;
        offset -= 1.0f/width;
        glUniform1f(glGetUniformLocation(ID, "offset"),offset);
        //glUniform3fv(glGetUniformLocation(ID, "camera_pos"), 1, &camera_pos[0]);      ??????????
    }     
    if(glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS){
        // hf dec
        screen_w = fullsize->width;
        screen_h = fullsize->height;
        glfwSetWindowMonitor(window, primary_monitor,0,0,screen_w, screen_h, fullsize->refreshRate);
    }     

}       


static void errorCallback(int error,
  const char * description) {
  fprintf(stderr, "Error: %s\n", description);
}


int main(int argc, char *argv[]){

    char* texture_file;
    char* heightmap_file;
    heightmap_file = argv[1];
    texture_file = argv[2];
    screen_w = 1000;
    screen_h = 1000;
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    GLFWwindow* window = glfwCreateWindow(screen_w, screen_h, "Render Display", NULL, NULL);
    if (window == NULL)
    {
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, processInput);        
    glewInit();
    glGetError();
    readTexture(heightmap_file,0);
    readTexture(texture_file,1);
    offset = 0.0f;
    mat4 view = mat4(1.0f);
    mat4 projection = mat4(1.0f);
    mod_proj_view = mat4(1.0f);
    mod_view = mat4(1.0f);
    light_init();
    view = camera_init();
    vertice_conf();
    initialize_shader();    
    projection = perspective(45.0f, 1.0f, 0.1f, 1000.0f);
    mod_proj_view = projection*view;
    mod_view = view;   
    norm_trans = mat4(1.0f);
    norm_trans = inverseTranspose(view);
    glfwSetFramebufferSizeCallback(window, resize_frame);    
    use_shader();
    set_uniforms(mod_proj_view,mod_view,norm_trans);
    glEnable(GL_DEPTH_TEST);

    primary_monitor = glfwGetPrimaryMonitor();
    fullsize = glfwGetVideoMode(primary_monitor);
    glfwGetWindowSize(window, &screen_w, &screen_w);
    int flag = 1;
    while (!glfwWindowShouldClose(window))
    {
        clear_all();       
        use_shader();

        glBindVertexArray(VAO);
        camera_comp();
        set_uniforms(mod_proj_view,mod_view,norm_trans);
        glDrawElements(GL_TRIANGLES, 6*width*height, GL_UNSIGNED_INT, 0);          
       
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glfwTerminate();
        
}

void resize_frame(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

