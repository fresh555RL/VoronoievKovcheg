#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <cmath>

#include <cstdlib>
#include <ctime>
#include <set>

#define STB_IMAGE_IMPLEMENTATION  
#include "stb_image.h"

using namespace std;

const int ScreenHeight = 1920;
const int ScreenWidth = 1000;
const float FOV = 70.0;

glm::vec3 camera_pos = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 camera_front = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 World_up = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 camera_up;
glm::vec3 camera_right;
float pitch = 0;
float yaw = -90;
bool first_mouse = true;
float last_x = ScreenWidth/2;
float last_y = ScreenHeight/2;
struct Model{
    glm::mat4 m;
    glm::vec3 color;
};
struct XYZ{
    double x, y,z;
};
struct Vertex {
    float x,y,z;
    float r,g,b;
};
void mouse_callback(GLFWwindow* w, double xposin, double yposin){
    float xpos = xposin;
    float ypos = yposin;
    if(first_mouse){
        last_x = xpos;
        last_y = ypos;
        first_mouse = 0;
    }
    float xoff = xpos-last_x;
    float yoff = last_y-ypos;
    last_x= xpos;
    last_y = ypos;
    float sensivity = 0.1f;
    xoff*=sensivity;
    yoff*=sensivity;
    yaw+=xoff;
    pitch+=yoff;
    if(pitch>89.0f){
        pitch = 89.0f;
    }
    else if(pitch<-89.0f){
        pitch = -89.0f;
    }
    glm::vec3 front; 
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch)); 
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    camera_front = glm::normalize(front);

}

void shaderError(const unsigned int& Shader,const char* ShaderCode,int& success, char* infolog){    
    glShaderSource(Shader, 1, &ShaderCode, NULL);
    glCompileShader(Shader);
    glGetShaderiv(Shader, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(Shader, 512, NULL,infolog);
        cout<<"Error vshader\n"<<infolog<<"\n";
    };
}


void programError(const unsigned int& shaderProgram,const unsigned int& vertexShader, const unsigned int& fragmentShader,int& success, char* infolog ){
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success){
        glGetProgramInfoLog(shaderProgram, 512, NULL,infolog);
        cout<<"Error linkshader\n"<<infolog<<"\n";
    }
}
    





int main() {
    glfwInit();
    GLFWwindow* w = glfwCreateWindow(ScreenHeight, ScreenWidth, "tri", 0, 0);
    glfwMakeContextCurrent(w);
    gladLoadGL();
    glfwSetInputMode(w,GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(w, mouse_callback);
    

    const string PATH = "shaders/";
    string to_open = "object";
    
    string vertexcode;
    string fragmentcode;
    ifstream vShaderfile;
    ifstream fShaderfile;    
    


    vShaderfile.exceptions(ifstream::failbit | ifstream::badbit);
    fShaderfile.exceptions(ifstream::failbit | ifstream::badbit);
    try{
        vShaderfile.open(PATH + to_open + ".vert");
        fShaderfile.open(PATH + to_open + ".frag");
        stringstream vShaderStream, fShaderStream;
        vShaderStream << vShaderfile.rdbuf();
        fShaderStream << fShaderfile.rdbuf();
        vShaderfile.close();
        fShaderfile.close();
        vertexcode = vShaderStream.str();
        fragmentcode = fShaderStream.str();
    }
    catch(ifstream::failure e){
        cout<<"Padasidasdaso на борту\n";
    }
    const char* vShaderCode = vertexcode.c_str();
    const char* fShaderCode = fragmentcode.c_str();
    int success;
    char infolog[512];

    unsigned int vertexShader =glCreateShader(GL_VERTEX_SHADER);
    shaderError(vertexShader, vShaderCode, success, infolog);
    unsigned int fragmentShader =glCreateShader(GL_FRAGMENT_SHADER);
    shaderError(fragmentShader, fShaderCode, success, infolog);
    unsigned int shaderProgram = glCreateProgram();
    programError(shaderProgram, vertexShader, fragmentShader, success, infolog);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glUseProgram(shaderProgram);
    unsigned int VBO, VAO, EBO;
    vector<int> idxes;
    
    if(to_open=="object"){
        glEnable(GL_DEPTH_TEST);
        vector<float> positions;
        vector<float> normals;
        vector<int> idxnormals;
        
        string path_model = (string("models/") + "12221_Cat_v1_l3" +".obj");
        std::ifstream file(path_model);
        std::string line;
        glm::vec3 abc;
        glm::vec3 rgb;
        vector<int> indexV(4);
        vector<int> indexN(4);
        getline(file,line);
        getline(file,line);
        while (std::getline(file, line))
        {
            std::stringstream ss(line);
            std::string type;
            ss >> type;
            if(type=="o" || type=="vn"){
                continue;
            }
            if(type=="v"){
                rgb.x=1.0;
                rgb.y=1.0;
                rgb.z=1.0;
                ss>>abc.x>>abc.y>>abc.z;
                if(ss>>rgb.x>>rgb.y>>rgb.z){}
                
                positions.push_back(abc.x);
                positions.push_back(abc.y);
                positions.push_back(abc.z);
                positions.push_back(rgb.x);
                positions.push_back(rgb.y);
                positions.push_back(rgb.z);
                

            }
            if(type=="vn"){
                ss>>abc.x>>abc.y>>abc.z;
                normals.push_back(abc.x);
                normals.push_back(abc.y);
                normals.push_back(abc.z);
            }
            if(type=="f"){
                char trash;
                bool flag = 1;
                int countslash = 1;
                if(countslash==1){
                    ss>>indexV[0]>>trash>>indexN[0];if(flag){ss>>trash>>indexN[0];}
                    ss>>indexV[1]>>trash>>indexN[1];if(flag){ss>>trash>>indexN[0];}
                    ss>>indexV[2]>>trash>>indexN[2];if(flag){ss>>trash>>indexN[0];}
                    ss>>indexV[3]>>trash>>indexN[3];if(flag){ss>>trash>>indexN[0];}
                }
                else{
                    ss>>indexV[0]>>trash>>trash>>indexN[0];if(flag){ss>>trash>>trash>>indexN[0];}
                    ss>>indexV[1]>>trash>>trash>>indexN[1];if(flag){ss>>trash>>trash>>indexN[0];}
                    ss>>indexV[2]>>trash>>trash>>indexN[2];if(flag){ss>>trash>>trash>>indexN[0];}
                    ss>>indexV[3]>>trash>>trash>>indexN[3];if(flag){ss>>trash>>trash>>indexN[0];}
                }
                
                idxes.push_back(indexV[0]-1);
                idxes.push_back(indexV[1]-1);
                idxes.push_back(indexV[2]-1);
                idxes.push_back(indexV[2]-1);
                idxes.push_back(indexV[3]-1);
                idxes.push_back(indexV[0]-1);



                // idxnormals.push_back(glm::vec3(__0123.x-1,__0123.y-1,__0123.z-1));
                // idxnormals.push_back(glm::vec3(__0123.x-1,__0123.z-1,__0123.w-1));
            }
            
            
        }




        glGenBuffers(1, &VBO);
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &EBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER,positions.size()*sizeof(float),positions.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(0));
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float)));
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, idxes.size()*sizeof(int), idxes.data(), GL_STATIC_DRAW);

    }
    else{
        
        vector<float> coords = {
        -1.0, -1.0, 1.0,
        -1.0, 1.0, 1.0,
        1.0, 1.0, 1.0,
        1.0, -1.0, 1.0,
    };
    idxes = {
        0,1,2,
        2,3,0
    };
 
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(float)*coords.size(),coords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)(0));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, idxes.size()*sizeof(int), idxes.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    }


    int uni_MVP = glGetUniformLocation(shaderProgram, "MVP");
    glm::mat4 MVP;
    glm::mat4 view;
    glm::mat4 proj;
    glm::mat4 Model = glm::translate(glm::mat4(1.0f), glm::vec3(3.0f,0.0f,-2.0f));
    // Model = glm::scale(Model, glm::vec3(0.2f));
    // Model = glm::rotate(Model, glm::radians(150.0f), glm::vec3(0.0f,1.0f, 1.0f));





    int uni_aspect = glGetUniformLocation(shaderProgram, "Aspect");
    int uni_campos = glGetUniformLocation(shaderProgram, "Cam_pos");
    int uni_camright = glGetUniformLocation(shaderProgram, "Cam_right");
    int uni_camup = glGetUniformLocation(shaderProgram, "Cam_up");
    int uni_camfront = glGetUniformLocation(shaderProgram, "Cam_front");
    int uni_FOV = glGetUniformLocation(shaderProgram, "FOV");
    int uni_time = glGetUniformLocation(shaderProgram, "timenow");
    glUniform1f(uni_aspect, ((float)ScreenHeight/ScreenWidth));
    while(!glfwWindowShouldClose(w)){

        // float current_Frame = glfwGetTime
        if(glfwGetKey(w, GLFW_KEY_ESCAPE)== GLFW_PRESS){
            glfwSetWindowShouldClose(w, true);
        }
        const float camera_speed = 0.12f;
        if(glfwGetKey(w, GLFW_KEY_W)== GLFW_PRESS){
            camera_pos+=camera_speed*camera_front;
        }
        if(glfwGetKey(w, GLFW_KEY_S)== GLFW_PRESS){
            camera_pos-=camera_speed*camera_front;
        }

        if(glfwGetKey(w, GLFW_KEY_A)== GLFW_PRESS){
            camera_pos-=camera_speed*glm::normalize(glm::cross(camera_front,camera_up));
        }
        if(glfwGetKey(w, GLFW_KEY_D)== GLFW_PRESS){
            camera_pos+=camera_speed*glm::normalize(glm::cross(camera_front,camera_up));
        }

    

        if(glfwGetKey(w, GLFW_KEY_SPACE)== GLFW_PRESS){
            camera_pos+=camera_speed*camera_up;
        }
        if(glfwGetKey(w, GLFW_KEY_LEFT_SHIFT)== GLFW_PRESS){
            camera_pos-=camera_speed*camera_up;
        }
        

        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        camera_right = glm::normalize(glm::cross(camera_front,World_up));
        camera_up = glm::normalize(glm::cross(camera_right, camera_front));
        view = glm::lookAt(camera_pos, camera_pos+camera_front, camera_up);
        proj = glm::perspective(glm::radians(70.0f), (float)ScreenHeight/ScreenWidth,  0.1f, 70.0f);
        glm::mat4 MVP = proj*view*Model;
        glUniformMatrix4fv(uni_MVP,1, GL_FALSE,&MVP[0][0]);

        glUniform3fv(uni_camfront,1,&camera_front[0]);
        glUniform3fv(uni_camright,1,&camera_right[0]);
        glUniform3fv(uni_campos,1,&camera_pos[0]);
        glUniform3fv(uni_camup,1,&camera_up[0]);  
        glUniform1f(uni_FOV, FOV); 
        glUniform1f(uni_time, glfwGetTime());  
        
        glDrawElements(GL_TRIANGLES, idxes.size()*sizeof(int), GL_UNSIGNED_INT, 0);
        glfwSwapBuffers(w);
        glfwPollEvents();
    
    }
}
