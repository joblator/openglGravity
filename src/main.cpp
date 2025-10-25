#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <vector>
#include <glm/vec2.hpp>
#include <glm/geometric.hpp>

using namespace std;
using namespace glm;

GLFWwindow* StartGLFW();
int screenWidth = 800;
int screenHeight = 1000;
struct Ball

{   
    // 0 = X , 1 = Y
    vec2 pos;
    float radius;
    vec2 accelration ,velocity;
    float mass;
    vector<vector<float>> trail;
    float maxSpeed;


    Ball(vec2 Position,float r,vec2 accel,vec2 vel,float m,vector<vector<float>> myTrail){
        radius = r;
        pos = Position;
        accelration = accel;
        velocity = vel;
        mass = m;
        trail = myTrail;
        maxSpeed = 0;
        
    }
    void drawBall(int res){ 
        glBegin(GL_TRIANGLE_FAN);
        glColor3f(0.0f, 0.4f, 0.8f); //rgb values between 1 and 0
        glVertex2f(pos[0], pos[1]); // center of circle 

        for (int i = 0; i <= res; i++) {
             // draws a triangle from the center of the circle each time at a diffrent angle 
            float angle = 2.0f * 3.141592653589f * (static_cast<float>(i) / res);
            float x1 = pos[0] + cos(angle) * radius;
            float y1 = pos[1] + sin(angle) * radius;
            glVertex2f(x1, y1);
        }
        glEnd();
    }
    void clampBall(){
        if (pos[0] < radius){
            pos[0] = radius;
            velocity[0] *= -1.0f;

        }
        if (pos[0] > screenWidth - radius){
            pos[0] = screenWidth - radius;
            velocity[0] *= -1.0f;
        }
        if (pos[1] < radius){
            pos[1] = radius;
            velocity[1] *= -1.0f;
            
        }
        if (pos[1] > screenHeight - radius){
            pos[1] = screenHeight - radius;
            velocity[1] *= -1.0f;
        }
    }




    void accelerate(float x ,float y){
        velocity[0] += x;
        velocity[1] += y;
        if (maxSpeed < velocity.length()){
            maxSpeed = velocity.length();
        }
    }
    void move(){
        pos[0] += velocity[0];
        pos[1] += velocity[1];
    }
    
    

    void collisionOther(Ball& other){
        vec2 originalVelA =velocity;
        vec2 originalVelB = other.velocity;
        float dist = distance(pos,other.pos);
        if(dist < radius + other.radius){
            float overLap = (other.radius + radius)  - dist;
            float massSum = mass + other.mass;
            vec2 ImpactLine = normalize(other.pos - pos);
            vec2 deltaVel =(other.velocity - velocity);
            float numerator = dot(deltaVel,ImpactLine);
            float denominator = massSum;
            //calculating vA(this)
            vec2 addVelA = 2 * other.mass *  numerator / denominator * ImpactLine;
            velocity = originalVelA + addVelA;
            //calculating vB(other)
            vec2 addVelB = -2* mass * numerator / denominator * ImpactLine;
            other.velocity = originalVelB + addVelB;
        pos -=  ImpactLine * 0.5f * overLap;
        other.pos +=  ImpactLine * 0.5f * overLap;
    
        }
    }


    void gravityForce(Ball& other){
        float G = 6.67f * pow(10,-14);
        float deltaY = other.pos[1] - pos[1];
        float deltaX = other.pos[0] - pos[0];
        float dist = distance(pos,other.pos);
        cout << "distance: "<< dist << "\n";
        if (dist > 100.f) {dist = 100.f;}


        vec2 direction = {deltaX / dist,deltaY/dist};//noramlizing the vector
        float Gforce = (G * mass * other.mass ) / (dist * dist);
        cout << "Gforce: "<< Gforce << "\n";
        float accel = Gforce / mass;
        float xAccel = 0.01f * direction[0] * accel;
        float yAccel = 0.01f * direction[1] * accel;
        cout << "directionY: "<< direction[1] << "\n";

        cout << "xAccel: "<< xAccel << "\n";
        cout << "yAccel "<< yAccel << "\n";

        accelerate(xAccel,yAccel);
          
    }
    void updateTrail(){
        trail.push_back({pos[0],pos[1]});
        if (trail.size() > 500){
            trail.erase(trail.begin());
        }
    }  
    void trailDraw(){
        glBegin(GL_LINE_STRIP);
        glColor3f(0.0f, 0.0f, 1.0f);
        for (auto& trailDot: trail){
            glVertex2f(trailDot[0],trailDot[1]);
        }
        glEnd();
    }
    void lerp(){
        float param =velocity.length();
        float maxParam = maxSpeed;
        float t = param / maxParam;
        if(t > 1.0f) t = 1.0f;  // clamp to 1

        float r = t;
        float g = 0.0f;
        float b = 1.0f - t;

        glColor3f(r, g, b);
    }
};


int main() {
    GLFWwindow* window = StartGLFW();
    if (!window) return -1;

    glfwMakeContextCurrent(window);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }

    // Set up orthographic projection
    //makes the bottom left be 0,0 and top right be the screen height and width
    // if i dont use this the values are set between 1 and -1 and i need to manualy covert them to that
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, screenWidth, 0, screenHeight, -1, 1);  // origin at bottom-left
    glMatrixMode(GL_MODELVIEW);

    float G = 6.67e-14f;
    float M = 1e17f;
    float orbitRadius = 300.0f;

    float centerX = screenWidth / 2.0f;  // 720 
    float centerY = screenHeight / 2.0f; // 1280

    //ball(pos,radius,accelration,velocity,mass,trail)
    Ball sun = Ball({centerX, centerY}, 30.0f, {0.0f, 0.0f}, {0.0f, 0.0f}, M,{{centerX, centerY}});

    // Planets positioned at 4 directions with tangential velocity for orbit
    Ball planetRight = Ball({centerX + orbitRadius, centerY}, 15.0f, {0.0f, 0.0f}, {0.0f, 5.0f}, 1e14f,{{centerX + orbitRadius, centerY}});
    Ball planetTop = Ball({centerX, centerY + orbitRadius}, 15.0f, {0.0f, 0.0f}, {-5.0f, 0.0f}, 1e14f,{{centerX, centerY + orbitRadius}});
    Ball planetLeft = Ball({centerX - orbitRadius, centerY}, 15.0f, {0.0f, 0.0f}, {0.0f, -5.0f}, 1e14f,{{centerX - orbitRadius, centerY}});
    Ball planetBottom = Ball({centerX, centerY - orbitRadius}, 15.0f, {0.0f, 0.0f}, {5.0f, 0.0f}, 1e14f,{{centerX, centerY - orbitRadius}});

    vector<Ball> balls = {sun, planetRight, planetTop, planetLeft, planetBottom};

    

    int res = 100;
    // the physics loop 
    while (!glfwWindowShouldClose(window)) {
        float Kenergy = 0;
        glClear(GL_COLOR_BUFFER_BIT);//clears the color buffer (screen) 
        glLoadIdentity();
        for (auto& ball: balls){

            ball.accelration[0] = 0.0f;
            ball.accelration[1] = 0.0f;
            for(auto& ball2 : balls){
                if(&ball == &ball2){continue;}
                ball.gravityForce(ball2);
                ball.collisionOther(ball2);
            }
            Kenergy += 0.5f * ball.mass *(ball.velocity[0] * ball.velocity[0] + ball.velocity[1] * ball.velocity[1]);
            ball.move();
            ball.clampBall();
            ball.updateTrail();
            ball.drawBall(res);
            ball.trailDraw();
        }
        cout << "energy: " << Kenergy << "\n";
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

GLFWwindow* StartGLFW() {
    if (!glfwInit()) {
        cout << "Failed to initialize GLFW" << endl;
        return nullptr;
    }

    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Circle Example", NULL, NULL);
    if (!window) {
        cout << "Failed to create window" << endl;
        glfwTerminate();
        return nullptr;
    }

    return window;
}