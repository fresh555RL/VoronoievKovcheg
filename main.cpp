#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace std;

#define RESOURCES_PATH "resources/"

const int ScreenHeight = 800;
const int ScreenWidth = 600;

glm::vec3 camera_pos = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 camera_front = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 camera_up = glm::vec3(0.0f, 1.0f, 0.0f);
float pitch = 0;
float yaw = -90;
bool first_mouse = true;
float last_x = ScreenWidth/2;
float last_y = ScreenHeight/2;

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


int main() {
    glfwInit();
    GLFWwindow* w = glfwCreateWindow(ScreenHeight, ScreenWidth, "tri", 0, 0);
    glfwMakeContextCurrent(w);
    gladLoadGL();
    glfwSetInputMode(w,GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(w, mouse_callback);
    glEnable(GL_DEPTH_TEST);

    unsigned int VAO,VBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.0f,    0.2f, 1.0f, 0.2f,
        0.5f, 0.5f, 0.0f,   1.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.0f,   0.1f, 0.7f, 0.3f,

       -0.5f, -0.5f, 1.0f,   1.0f, 0.4f, 0.3f, //4
        0.5f, -0.5f, 1.0f,    0.7f, 0.4f, 0.6f, //5 
        0.5f, 0.5f, 1.0f,   0.0f, 0.5f, 0.5f, //6
        -0.5f, 0.5f, 1.0f,   0.2f, 0.0f, 0.5f // 7
    };
    int idxes[] = {
        0,1,2,
        0,3,2,
        7,3,2,
        6,2,1,
        5,1,0,
        4,0,3,
        7,6,2,
        6,5,1,
        5,4,0,
        7,4,3,
        7,6,5,
        4,5,7
        


    };



    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float),  (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float),  (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);


    unsigned int EBO;
    glGenBuffers  (1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idxes), idxes, GL_STATIC_DRAW);



    string vertexcode;
    string fragmentcode;
    ifstream vShaderfile;
    ifstream fShaderfile;    
    
    vShaderfile.exceptions(ifstream::failbit | ifstream::badbit);
    fShaderfile.exceptions(ifstream::failbit | ifstream::badbit);
    try{
        vShaderfile.open("shaders/shader.vert");
        fShaderfile.open("shaders/shader.frag");
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

    unsigned int vertexShader =glCreateShader(GL_VERTEX_SHADER);
    int success;
    char infolog[512];
    glShaderSource(vertexShader, 1, &vShaderCode, NULL);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(vertexShader, 512, NULL,infolog);
        cout<<"Error vshader\n"<<infolog<<"\n";
    };
    unsigned int fragmentShader =glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(fragmentShader, 512, NULL,infolog);
        cout<<"Error fshader\n"<<infolog<<"\n";
    };


    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success){
        glGetProgramInfoLog(shaderProgram, 512, NULL,infolog);
        cout<<"Error linkshader\n"<<infolog<<"\n";
    }



    

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    // ===== цикл =====
    while(!glfwWindowShouldClose(w)){

        // float current_Frame = glfwGetTime
        if(glfwGetKey(w, GLFW_KEY_ESCAPE)== GLFW_PRESS){
            glfwSetWindowShouldClose(w, true);
        }
        const float camera_speed = 0.05f;
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
        glBindVertexArray(VAO);
        glUseProgram(shaderProgram);
        int uni_loc = glGetUniformLocation(shaderProgram, "screen_aspect");
        glUniform1f(uni_loc, (float)ScreenHeight/ScreenWidth);

        int uni_angle = glGetUniformLocation(shaderProgram, "angle");
        glUniform1f(uni_angle, (float)glfwGetTime());
        
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.0f));
        // model *= glm::rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 view = glm::lookAt(camera_pos, camera_pos+camera_front, camera_up);
        glm::mat4 proj = glm::perspective(glm::radians(70.0f), (float)ScreenHeight/ScreenWidth,  0.1f, 70.0f);

        int uni_model = glGetUniformLocation(shaderProgram, "model");
        int uni_view = glGetUniformLocation(shaderProgram, "view");
        int uni_proj = glGetUniformLocation(shaderProgram, "proj");
        glUniformMatrix4fv(uni_model, 1, GL_FALSE, &model[0][0]);
        glUniformMatrix4fv(uni_view, 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(uni_proj, 1, GL_FALSE, &proj[0][0]);

        // glDrawArrays(GL_TRIANGLES,0,3);
        glDrawElements(GL_TRIANGLES, sizeof(idxes)/sizeof(int), GL_UNSIGNED_INT, 0);
        
        glfwSwapBuffers(w);
        glfwPollEvents();
    }
}
