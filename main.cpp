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
const int ScreenWidth = 1080;

glm::vec3 camera_pos = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 camera_front = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 camera_up = glm::vec3(0.0f, 1.0f, 0.0f);
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

    //texture but i don`t need it lol
    // int Twidth, Theight, nrChannels;
    // unsigned char* Tdata = stbi_load("textures/Dot8x8.png", &Twidth, &Theight, &nrChannels, 0);
    // unsigned int textureDot;
    // glGenTextures(1, &textureDot);
    // glBindTexture(GL_TEXTURE_2D, textureDot);
    // if(Tdata){
    //     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Twidth, Theight, 0, GL_RGB, GL_UNSIGNED_BYTE, Tdata);
        
    // }
    // else{
    //     cout<<"Texture load error\n";
    // }
    // stbi_image_free(Tdata);



    float vertices_Cube[] = {
        -1.0f, -1.0f, -1.0f,   
        1.0f, -1.0f,  -1.0f,   
        1.0f, 1.0f,   -1.0f,   
        -1.0f, 1.0f,  -1.0f,   

        -1.0f, -1.0f, 1.0f,    
        1.0f, -1.0f,  1.0f,   
        1.0f, 1.0f,   1.0f,    
        -1.0f, 1.0f,  1.0f
         
    };
    int indexes_Cube[] = {
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

    unsigned int VAO_Cube, VBO_Cube, EBO_Cube;
    glGenVertexArrays(1,&VAO_Cube);
    glGenBuffers(1,&VBO_Cube);
    glGenBuffers(1,&EBO_Cube);
    glBindVertexArray(VAO_Cube);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO_Cube);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_Cube), vertices_Cube, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_Cube);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexes_Cube), indexes_Cube, GL_STATIC_DRAW);
    

    ifstream file("sphere.txt");
    int sphere_vert_size,sphere_idx_size;
    file>>sphere_vert_size>>sphere_idx_size;
    vector<float> vertices_Sphere(sphere_vert_size*3);
    vector<int> indexes_Sphere(sphere_idx_size); 
    // float vertices_Sphere[sphere_vert_size*3];
    // int indexes_Sphere[sphere_idx_size]; 
    for(int i=0;i<sphere_vert_size*3;i+=3){
        file>>vertices_Sphere[i]>>vertices_Sphere[i+1]>>vertices_Sphere[i+2];
    }
    for(int i=0;i<sphere_idx_size;i+=3){
        file>>indexes_Sphere[i]>>indexes_Sphere[i+1]>>indexes_Sphere[i+2];
    }


    unsigned int VAO_Sph, VBO_Sph, EBO_Sph;
    glGenVertexArrays(1,&VAO_Sph);
    glGenBuffers(1,&VBO_Sph);
    glBindVertexArray(VAO_Sph);
    glGenBuffers(1, &EBO_Sph);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO_Sph);
    glBufferData(GL_ARRAY_BUFFER, vertices_Sphere.size()*sizeof(float), vertices_Sphere.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_Sph);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexes_Sphere.size()*sizeof(int), indexes_Sphere.data(), GL_STATIC_DRAW);
    
   
    std::srand(std::time(nullptr));
    set<int> triangle_pts;
    for(;triangle_pts.size()<6;){
        int r = std::rand();
        r=r%sphere_vert_size;
        // cout<<r<<'\n';
        triangle_pts.insert(r);
    }
    float vertices_triagnle[18];
    int temporary_kostil=0;
    for(auto u:triangle_pts){
        
        vertices_triagnle[temporary_kostil] = vertices_Sphere[3*u];
        vertices_triagnle[temporary_kostil+1] = vertices_Sphere[3*u+1];
        vertices_triagnle[temporary_kostil+2] = vertices_Sphere[3*u+2];
        temporary_kostil+=3;
    }
    unsigned int VAO_Tri, VBO_Tri;
    glGenVertexArrays(1,&VAO_Tri);
    glGenBuffers(1,&VBO_Tri);
    glBindVertexArray(VAO_Tri);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO_Tri);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_triagnle), vertices_triagnle, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    


    float dot_points[12];
    glm::vec3 AB1 = glm::vec3(vertices_triagnle[0]-vertices_triagnle[3],vertices_triagnle[1]-vertices_triagnle[4],vertices_triagnle[2]-vertices_triagnle[5]);
    glm::vec3 AC1 = glm::vec3(vertices_triagnle[0]-vertices_triagnle[6],vertices_triagnle[1]-vertices_triagnle[7],vertices_triagnle[2]-vertices_triagnle[8]);
    glm::vec3 Normal1= glm::cross(glm::vec3(AB1), glm::vec3(AC1));
    glm::vec3 sph_center = glm::vec3(3.0f, 0.0f, -2.0f);
    dot_points[0] = sph_center.x+Normal1.x*(-5);
    dot_points[1] = sph_center.y+Normal1.y*(-5);
    dot_points[2] = sph_center.z+Normal1.z*(-5);
    dot_points[3] = sph_center.x+Normal1.x*(5);
    dot_points[4] = sph_center.y+Normal1.y*(5);
    dot_points[5] = sph_center.z+Normal1.z*(5);



    glm::vec3 AB2 = glm::vec3(vertices_triagnle[0+9]-vertices_triagnle[3+9],vertices_triagnle[1+9]-vertices_triagnle[4+9],vertices_triagnle[2+9]-vertices_triagnle[5+9]);
    glm::vec3 AC2 = glm::vec3(vertices_triagnle[0+9]-vertices_triagnle[6+9],vertices_triagnle[1+9]-vertices_triagnle[7+9],vertices_triagnle[2+9]-vertices_triagnle[8+9]);
    glm::vec3 Normal2= glm::cross(glm::vec3(AB2), glm::vec3(AC2));
    dot_points[6] = sph_center.x+Normal2.x*(-5);
    dot_points[7] = sph_center.y+Normal2.y*(-5);
    dot_points[8] = sph_center.z+Normal2.z*(-5);
    dot_points[9] = sph_center.x+Normal2.x*(5);
    dot_points[10] = sph_center.y+Normal2.y*(5);
    dot_points[11] = sph_center.z+Normal2.z*(5);


    unsigned int VAO_line, VBO_line;
    glGenVertexArrays(1,&VAO_line);
    glGenBuffers(1,&VBO_line);
    glBindVertexArray(VAO_line);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO_line);
    glBufferData(GL_ARRAY_BUFFER, sizeof(dot_points),dot_points, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
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
    
    vector<Model> models_cubes(5);
    models_cubes[0].m = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.0f));
    models_cubes[0].color = glm::vec3(1.0f, 1.0f, 1.0f);
    models_cubes[1].m = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.5f, -2.0f));
    models_cubes[1].m = glm::scale(models_cubes[1].m, glm::vec3(0.7f, 1.0f, 0.7f));
    models_cubes[1].color = glm::vec3(1.0f, 1.0f, 1.0f);
    models_cubes[2].m = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 3.3f, -2.0f));
    models_cubes[2].m = glm::scale(models_cubes[2].m, glm::vec3(0.85f, 0.85f, 0.85f));
    models_cubes[2].color = glm::vec3(227.0f/255.0f, 103.0f/255.0f, 9.0f/255.0f);
    models_cubes[3].m = glm::translate(glm::mat4(1.0f),glm::vec3(0.4f, 3.61f, -1.3f));
    models_cubes[3].m = glm::scale(models_cubes[3].m,glm::vec3(0.2f, 0.2f, 0.2f));
    models_cubes[3].color = glm::vec3(0.0f, 0.0f, 0.0f);
    models_cubes[4].m = glm::translate(glm::mat4(1.0f),glm::vec3(-0.4f, 3.61f, -1.3f));
    models_cubes[4].m = glm::scale(models_cubes[4].m,glm::vec3(0.2f, 0.2f, 0.2f));
    models_cubes[4].color = glm::vec3(0.0f, 0.0f, 0.0f);
    
    models_cubes[0].m = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 10.0f, 10.0f));
    models_cubes[0].m = glm::translate(models_cubes[0].m, glm::vec3(0.5f, 0.0f, 3.0f));
   
    // models_cubes[0].color = glm::vec3(1.0f, 1.0f, 1.0f);




    vector<Model> models_spheres(1);
    models_spheres[0].m = glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, 0.0f, -2.0f));
    // models_spheres[0].m = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    models_spheres[0].color = glm::vec3(0.0f, 2.0f, 1.0f);
    glUseProgram(shaderProgram);
    int uni_MVP = glGetUniformLocation(shaderProgram, "MVP");
    int uni_model = glGetUniformLocation(shaderProgram, "model");
    int uni_color = glGetUniformLocation(shaderProgram, "VertexCol");
    glm::mat4 MVP;
    glm::mat4 view;
    glm::mat4 proj;
    glm::mat4 VP;
    auto Draw_Cubes = [&](glm::mat4 &VP){
        glBindVertexArray(VAO_Cube);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_Cube);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_Cube);
        for(auto [model, color]:models_cubes){
            MVP = VP*model;

            glUniformMatrix4fv(uni_model, 1, GL_FALSE, &model[0][0]);
            glUniformMatrix4fv(uni_MVP, 1, GL_FALSE, &MVP[0][0]);
            glUniform3fv(uni_color, 1, &color[0]);
            glDrawElements(GL_TRIANGLES, sizeof(indexes_Cube)/sizeof(int), GL_UNSIGNED_INT, 0);
        }
    };
     auto Draw_Spheres = [&](glm::mat4 &VP){
        glBindVertexArray(VAO_Sph);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_Sph);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_Sph);
        for(auto [model, color]:models_spheres){

            MVP = VP*model;
            glUniformMatrix4fv(uni_MVP, 1, GL_FALSE, &MVP[0][0]);
            glUniformMatrix4fv(uni_model, 1, GL_FALSE, &model[0][0]);
            glUniform3fv(uni_color, 1, &color[0]);
            // glDrawArrays(GL_POINTS, 0, sizeof(vertices_Sphere)/3/sizeof(float));
            glDrawElements(GL_TRIANGLES, indexes_Sphere.size(), GL_UNSIGNED_INT, 0);
        }
    };
    auto Draw_12_30Triangle = [&](glm::mat4 &VP){
        glBindVertexArray(VAO_Tri);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_Tri);
        for(auto [model, color]:models_spheres){

            MVP = VP*model;
            glUniformMatrix4fv(uni_MVP, 1, GL_FALSE, &MVP[0][0]);
            glUniformMatrix4fv(uni_model, 1, GL_FALSE, &model[0][0]);
            glUniform3fv(uni_color, 1, &color[0]);
            glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices_triagnle)/3/sizeof(float));
        }
    };
    auto Draw_12_30line = [&](glm::mat4 &VP){
        glBindVertexArray(VAO_line);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_line);
        for(auto [model, color]:models_spheres){
            model = glm::mat4(1.0f);
            color = glm::vec3(1.0f, 0.1f, 0.1f);
            MVP = VP*model;
            glUniformMatrix4fv(uni_MVP, 1, GL_FALSE, &MVP[0][0]);
            glUniformMatrix4fv(uni_model, 1, GL_FALSE, &model[0][0]);
            glUniform3fv(uni_color, 1, &color[0]);
            glDrawArrays(GL_LINES, 0, sizeof(dot_points)/3/sizeof(float));
        }
    };
    // cout<< indexes_Sphere.size()<<'\n';
    // ===== цикл =====
    // glClearColor(1.0f, 230.0f/255.0f, 181.0f/255.0f, 0.0f);
    // glClearColor(244.0f/255.0f,213.0f/255.0f,187.0f/255.0f, 0.0f);
    while(!glfwWindowShouldClose(w)){

        // float current_Frame = glfwGetTime
        if(glfwGetKey(w, GLFW_KEY_ESCAPE)== GLFW_PRESS){
            glfwSetWindowShouldClose(w, true);
        }
        const float camera_speed = 0.02f;
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
        

        // glClearColor(1.0f, 1, 1, 1);
        glPointSize(1.0f);
        glLineWidth(3.0f);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        view = glm::lookAt(camera_pos, camera_pos+camera_front, camera_up);
        proj = glm::perspective(glm::radians(70.0f), (float)ScreenHeight/ScreenWidth,  0.1f, 70.0f);
        VP = proj*view;
        // Draw_12_30Triangle(VP);
        // Draw_12_30line(VP);
        Draw_Spheres(VP);
        Draw_Cubes(VP);
        glfwSwapBuffers(w);
        glfwPollEvents();
    
    }
}
