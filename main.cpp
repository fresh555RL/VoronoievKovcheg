#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>
#define GLM_FORCE_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <set>


using namespace std;

const int ScreenHeight = 1920;
const int ScreenWidth = 1080;
const float FOV = 70.0;

glm::vec3 camera_pos = glm::vec3(0.0f, 3.0f, 0.0f);
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
    


glm::vec3 projDir(glm::vec3 rd, glm::vec3 n){
    return glm::normalize(rd - dot(rd,n)*n);
}

float opSmoothUnion( float a, float b, float k )
{
    k *= 4.0f;
    float h = max(k-abs(a-b),0.0f);
    return min(a, b) - h*h*0.25f/k;
}

float opSmoothSubtraction( float a, float b, float k )
{
    return -opSmoothUnion(a,-b,k);
}

float opSmoothIntersection( float a, float b, float k )
{
    return -opSmoothUnion(-a,-b,k);
}

float sdSphere(glm::vec3 p, glm::vec4 c){

    return glm::length(p-c.xyz())-c.w;
}
float sdTriPrism( glm::vec3 p, glm::vec2 h, glm::vec3 c )
{
  glm::vec3 q = abs(p-c);
  return max(q.z-h.y,max(q.x*0.866025f+p.y*0.5f,-p.y)-h.x*0.5f);
}

float sdCube(glm::vec3 p,glm::vec3 c, glm::vec3 b){
    glm::vec3 q = abs(p-c) - b;
    return glm::length(max(q,0.0f)) + min(max(q.x,max(q.y,q.z)),0.0f);
}
float sdTorus( glm::vec3 p, glm::vec2 t )
{
  glm::vec2 q = glm::vec2(glm::length(p.xz())-t.x,p.y);
  return glm::length(q)-t.y;
}
glm::vec2 cmp(glm::vec2 a, glm::vec2 b){
    return (a.x<b.x?a:b);
}
float sdRoundBox( glm::vec3 p, glm::vec3 b, float r )
{
  glm::vec3 q = abs(p) - b + r;
  return glm::length(max(q,0.0f)) + min(max(q.x,max(q.y,q.z)),0.0f) - r;
}
float sdCylinder( glm::vec3 p, glm::vec3 c )
{
  return glm::length(p.xz()-c.xy())-c.z;
}
float sdFlatWorld(glm::vec3 p){
    return p.y;
}



float bordermap0(glm::vec3 p){
    float d = 1e9;
    glm::vec3 BBox1 = glm::vec3(10.0f, 20.0f, 10.0f);
    float dBb = sdRoundBox(p, BBox1, 0.5f);
   
    return -dBb;
}
float map0(glm::vec3 p){
    float d = 1e9f;
    glm::vec2 Torus1 = glm::vec2(1.0f, 0.5f);
    glm::vec3 Cylinder1 = glm::vec3(3.0f,5.0f, 1.0f);
    glm::vec3 RBox1 = glm::vec3(10.0f, 1.0f, 10.0f);

    float dRb =  sdRoundBox(p, RBox1, 0.5f);
    float  dCy = sdCylinder(p, Cylinder1);
    float dirka = opSmoothSubtraction(dCy, dRb, 0.3f);
    d = min(d, dirka);

    return d;
}
glm::vec3 Calcnorm0(glm::vec3 p){
    const float Eps = 0.001f;
    return glm::normalize(glm::vec3(
        map0(p+glm::vec3(Eps,0.0f,0.0f))-map0(p-glm::vec3(Eps,0.0f,0.0f)),
        map0(p+glm::vec3(0.0f,Eps,0.0f))-map0(p-glm::vec3(0.0f,Eps,0.0f)),
        map0(p+glm::vec3(0.0f,0.0f,Eps))-map0(p-glm::vec3(0.0f,0.0f,Eps))
        )
    );
}
//--------------------------
float bordermap1(glm::vec3 p){
    return 1e5f;
}
float map1(glm::vec3 p){
    float d = 1e9f;
    glm::vec4 Sphere1 = glm::vec4(0.0, 0.0, 0.0, 2.0);
    float dSph = sdSphere(p,Sphere1);
    d = min(d, dSph);

    return d;
}
glm::vec3 Calcnorm1(glm::vec3 p){
    const float Eps = 0.001f;
    return glm::normalize(glm::vec3(
        map1(p+glm::vec3(Eps,0.0f,0.0f))-map1(p-glm::vec3(Eps,0.0f,0.0f)),
        map1(p+glm::vec3(0.0f,Eps,0.0f))-map1(p-glm::vec3(0.0f,Eps,0.0f)),
        map1(p+glm::vec3(0.0f,0.0f,Eps))-map1(p-glm::vec3(0.0f,0.0f,Eps))
        )
    );
}
//---------------------
float bordermap3(glm::vec3 p){
    return 1e5f;
}
float map3(glm::vec3 p){
    float d = 1e9f;
    glm::vec2 Torus = glm::vec2(2.0f, 1.3f);
    float dT = sdTorus(p+glm::vec3(-2.0f, 0.0f, 0.0f),Torus);
    d = min(d, dT);

    return d;
}
glm::vec3 Calcnorm3(glm::vec3 p){
    const float Eps = 0.008f;
    return glm::normalize(glm::vec3(
        map3(p+glm::vec3(Eps,0.0f,0.0f))-map3(p-glm::vec3(Eps,0.0f,0.0f)),
        map3(p+glm::vec3(0.0f,Eps,0.0f))-map3(p-glm::vec3(0.0f,Eps,0.0f)),
        map3(p+glm::vec3(0.0f,0.0f,Eps))-map3(p-glm::vec3(0.0f,0.0f,Eps))
        )
    );
}
//-------------------------
float bordermap4(glm::vec3 p){
    return 1e5f;
    
}
float map4(glm::vec3 p){
    
    float d = 1e9;
    float dG = sdFlatWorld(p);
    glm::vec4 Sphere1 = glm::vec4(5.0, 0.0, 3.0, 1.0);
    glm::vec4 Sphere2 = glm::vec4(-5.0, 0.0, 3.0, 1.0);
    glm::vec4 Sphere3 = glm::vec4(5.0, 0.0, -3.0, 1.0);
    glm::vec4 Sphere4 = glm::vec4(-5.0, 0.0, -3.0, 1.0);
    float dW = opSmoothSubtraction(sdSphere(p, Sphere2), dG, 0.2);
    dW = opSmoothUnion(dW, sdSphere(p,Sphere1), 0.2);
    dW = min(dW, sdSphere(p, Sphere3));
    dW = max(dW, -sdSphere(p, Sphere4));
    d = min(d, dW);
    return d;


}
glm::vec3 Calcnorm4(glm::vec3 p){
    const float Eps = 0.008f;
    return glm::normalize(glm::vec3(
        map4(p+glm::vec3(Eps,0.0f,0.0f))-map4(p-glm::vec3(Eps,0.0f,0.0f)),
        map4(p+glm::vec3(0.0f,Eps,0.0f))-map4(p-glm::vec3(0.0f,Eps,0.0f)),
        map4(p+glm::vec3(0.0f,0.0f,Eps))-map4(p-glm::vec3(0.0f,0.0f,Eps))
        )
    );
}
//---------------
float bordermap5(glm::vec3 p){
    return 1e5f;
    
}

float sdMobius(glm::vec3 p)
{
    float R = 1.5;
    float w = 1.0;
    float t = 0.2;

    float a = glm::atan(p.y, p.x);
    float r = glm::length(p.xy());

    float ca = cos(a*0.5);
    float sa = sin(a*0.5);

    glm::vec2 q = glm::vec2(r - R, p.z);

    glm::mat2 m = glm::mat2(ca,-sa,sa,ca);
    q = m * q;

    glm::vec2 d = abs(q) - glm::vec2(w, t);
    return min(max(d.x,d.y),0.0f) + glm::length(max(d,glm::vec2(0.0f)));
}
float map5(glm::vec3 p){
    
    float d =1e9;

    d = sdCylinder(p, glm::vec3(1.0f, 2.0f, 1.0f));

    return d;


}
glm::vec3 Calcnorm5(glm::vec3 p){
    const float Eps = 0.008f;
    return glm::normalize(glm::vec3(
        map5(p+glm::vec3(Eps,0.0f,0.0f))-map5(p-glm::vec3(Eps,0.0f,0.0f)),
        map5(p+glm::vec3(0.0f,Eps,0.0f))-map5(p-glm::vec3(0.0f,Eps,0.0f)),
        map5(p+glm::vec3(0.0f,0.0f,Eps))-map5(p-glm::vec3(0.0f,0.0f,Eps))
        )
    );
}
//---------------------------
float bordermap6(glm::vec3 p){
    return 1e5f;
    
}

float map6(glm::vec3 p){
    
    float d =1e9;

    d = sdMobius(p);

    return d;


}
glm::vec3 Calcnorm6(glm::vec3 p){
    const float Eps = 0.008f;
    return glm::normalize(glm::vec3(
        map6(p+glm::vec3(Eps,0.0f,0.0f))-map6(p-glm::vec3(Eps,0.0f,0.0f)),
        map6(p+glm::vec3(0.0f,Eps,0.0f))-map6(p-glm::vec3(0.0f,Eps,0.0f)),
        map6(p+glm::vec3(0.0f,0.0f,Eps))-map6(p-glm::vec3(0.0f,0.0f,Eps))
        )
    );
}
//---------------------------
float bordermaphub(int MODE, glm::vec3 p){
    if(MODE== 1 || MODE==2) return bordermap1(p);
    else if(MODE==3) return bordermap3(p);
    else if(MODE==4) return bordermap4(p);
    else if (MODE==5) return bordermap5(p);
    else if (MODE==6) return bordermap6(p);
    else return bordermap0(p);

}
float maphub(int MODE, glm::vec3 p){
    if(MODE==1 || MODE==2) return map1(p);
    else if(MODE==3) return map3(p);
    else if(MODE==4) return map4(p);
    else if (MODE==5) return map5(p);
    else if (MODE==6) return map6(p);
    else return map0(p);
} 
glm::vec3 Calcnormhub(int MODE, glm::vec3 p){
    if(MODE==1 || MODE==2) return Calcnorm1(p);
    else if(MODE==3) return Calcnorm3(p);
    else if(MODE==4) return Calcnorm4(p);
    else if (MODE==5) return Calcnorm5(p);
    else if (MODE==6) return Calcnorm6(p);
    else return Calcnorm0(p);
}
int main() {

    int idx_open;
    int MODE =777;
    cout<<"Write 0 for shader (recommended first)\nWrite 1 for scaner\n";
    cin>>idx_open;
    string idx_start = "\nWrite for visualisation:\n0 - dirka\n1 - sphere\n2 -sphere with plyama\n3 -donut\n4 -opuklosti\n5 - cylinder\n";
    cout<<idx_start;
    string idx_full = "6-Mobius\n7 -inf dimension\n8 - gyroid\n9- inf dimension\n10- boolean visualisation\n777-Schwarzes Loch\n\n123 - you can write something in shader123.frag in function map() and test it\n";
    if(idx_open==0) cout<<idx_full;
    cin>>MODE;
    const string PATH = "shaders/";
    vector<string> list_to_open = {"shader", "scaner"};
    string to_open = list_to_open[idx_open];
    
    const float scan_radius = 8.0f;

    glfwInit();
    GLFWwindow* w = glfwCreateWindow(ScreenHeight, ScreenWidth, "tri", 0, 0);
    glfwMakeContextCurrent(w);
    gladLoadGL();
    glfwSetInputMode(w,GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(w, mouse_callback);
    

    
    string vertexcode;
    string fragmentcode;
    ifstream vShaderfile;
    ifstream fShaderfile;    
    

    
    vShaderfile.exceptions(ifstream::failbit | ifstream::badbit);
    fShaderfile.exceptions(ifstream::failbit | ifstream::badbit);
    try{
        vShaderfile.open(PATH + to_open +".vert");
        fShaderfile.open(PATH + to_open + to_string(MODE) + ".frag");
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
    //not obj
        
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
    


    int uni_MVP = glGetUniformLocation(shaderProgram, "MVP");
    glm::mat4 MVP;
    glm::mat4 view;
    glm::mat4 proj;
    glm::mat4 Model = glm::translate(glm::mat4(1.0f), glm::vec3(3.0f,0.0f,-2.0f));
    // Model = glm::scale(Model, glm::vec3(0.2f));
    // Model = glm::rotate(Model, glm::radians(150.0f), glm::vec3(0.0f,1.0f, 1.0f));
    
    glm::vec3 dir = glm::vec3(1.0f, 0.0f, 0.0f);//for scaner
    glm::vec3 right;
    glm::vec3 normal = Calcnormhub(MODE,camera_pos);
    if(to_open=="scaner"){
        camera_pos-= normal*maphub(MODE,camera_pos); 
        dir = projDir(dir, normal);
    }

    
    


    
    int uni_aspect = glGetUniformLocation(shaderProgram, "Aspect");
    int uni_campos = glGetUniformLocation(shaderProgram, "Cam_pos");
    int uni_camright = glGetUniformLocation(shaderProgram, "Cam_right");
    int uni_camup = glGetUniformLocation(shaderProgram, "Cam_up");
    int uni_camfront = glGetUniformLocation(shaderProgram, "Cam_front");
    int uni_FOV = glGetUniformLocation(shaderProgram, "FOV");
    int uni_time = glGetUniformLocation(shaderProgram, "timenow");
    
    int uni_scan_forward = glGetUniformLocation(shaderProgram, "dForward");
    int uni_scan_right = glGetUniformLocation(shaderProgram, "dRight");
    int uni_scan_Radius = glGetUniformLocation(shaderProgram, "Radius");
    glUniform3fv(uni_scan_forward,1,&dir[0]);
    glUniform3fv(uni_scan_right,1,&right[0]);
    // glUniform3fv(,1,&camera_up[0]);  
    glUniform1f(uni_scan_Radius, scan_radius); 


    const float camera_speed = 0.12f;
    const float border_eps   = 0.18f;
    glm::vec3 camera_pos_copy;
    glUniform1f(uni_aspect, ((float)ScreenHeight/ScreenWidth));
    while(!glfwWindowShouldClose(w)){

        float current_Frame = glfwGetTime();
        if(to_open=="scaner"){
            camera_pos_copy = camera_pos;
            normal = Calcnormhub(MODE,camera_pos);
            dir = projDir(dir, normal);
            // float chatgpt = glm::step(0.95f, abs(normal.x));
            // chatgpt_help = glm::vec3(0.0f, 1.0f, 0.0f)*chatgpt-(glm::vec3(1.0f, 0.0f, 0.0f)*(1.0f-chatgpt));  
            // a = vec3(0.0, 1.0, 0.0);
            if(glfwGetKey(w, GLFW_KEY_W)== GLFW_PRESS){
                camera_pos_copy+=camera_speed*glm::normalize(glm::cross(dir,normal));
                if(bordermaphub(MODE,camera_pos_copy)>border_eps){
                    camera_pos = camera_pos_copy;
                }
            }
            if(glfwGetKey(w, GLFW_KEY_S)== GLFW_PRESS){
                camera_pos_copy-=camera_speed*glm::normalize(glm::cross(dir,normal));
                if(bordermaphub(MODE,camera_pos_copy)>border_eps){
                    camera_pos = camera_pos_copy;
                }
            }

            if(glfwGetKey(w, GLFW_KEY_A)== GLFW_PRESS){
                camera_pos_copy-=camera_speed*dir;
                if(bordermaphub(MODE,camera_pos_copy)>border_eps){
                    camera_pos = camera_pos_copy;
                }
            }
            if(glfwGetKey(w, GLFW_KEY_D)== GLFW_PRESS){
                camera_pos_copy+=camera_speed*dir;
                if(bordermaphub(MODE,camera_pos_copy)>border_eps){
                    camera_pos = camera_pos_copy;
                }
            }
            normal = Calcnormhub(MODE,camera_pos);
            dir = projDir(dir, normal);
            right = glm::normalize(glm::cross(dir, normal));
            dir = glm::normalize(glm::cross(normal,right));
            // cout<<"----\n";
            // cout<<right.x<<" "<<right.y<<" "<<right.z<<"\n";
            // cout<<dir.x<<" "<<dir.y<<" "<<dir.z<<"\n";
            // cout<<"----\n";
            // cout<<camera_pos.x<<" "<<camera_pos.y<<" "<<camera_pos.z<<"\n";
            // cout<<"----\n";


            glUniform3fv(uni_scan_forward,1,&dir[0]);
            glUniform3fv(uni_scan_right,1,&right[0]);
            // glUniform3fv(,1,&camera_up[0]);  
            glUniform1f(uni_scan_Radius, scan_radius); 
            

        }
        else{
            if(glfwGetKey(w, GLFW_KEY_ESCAPE)== GLFW_PRESS){
                glfwSetWindowShouldClose(w, true);
            }
            
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
                camera_pos+=camera_speed*World_up;
            }
            if(glfwGetKey(w, GLFW_KEY_LEFT_SHIFT)== GLFW_PRESS){
                camera_pos-=camera_speed*World_up;
            }
        camera_right = glm::normalize(glm::cross(camera_front,World_up));
        camera_up = glm::normalize(glm::cross(camera_right, camera_front));
        view = glm::lookAt(camera_pos, camera_pos+camera_front, camera_up);
        proj = glm::perspective(glm::radians(70.0f), (float)ScreenHeight/ScreenWidth,  0.1f, 70.0f);
        MVP = proj*view*Model;
        glUniformMatrix4fv(uni_MVP,1, GL_FALSE,&MVP[0][0]);
        glUniform3fv(uni_camfront,1,&camera_front[0]);
        glUniform3fv(uni_camright,1,&camera_right[0]);
        glUniform3fv(uni_camup,1,&camera_up[0]);  
        glUniform1f(uni_FOV, FOV); 
        glUniform1f(uni_time, glfwGetTime());  
        }

        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        
        
        


        glUniform3fv(uni_campos,1,&camera_pos[0]);
    
        glDrawElements(GL_TRIANGLES, idxes.size(), GL_UNSIGNED_INT, 0);
        glfwSwapBuffers(w);
        glfwPollEvents();
    
    }
}
