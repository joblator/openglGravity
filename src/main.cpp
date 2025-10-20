#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <vector>

using namespace std;

GLFWwindow* StartGLFW();
int screenWidth = 800;
int screenHeight = 1000;
struct Ball

{   
    // 0 = X , 1 = Y
    vector<float> pos;
    float radius;
    vector<float> accelration ,velocity;
    float mass;
    vector<vector<float>> trail;


    Ball(vector<float> Position,float r,vector<float> accel,vector<float> vel,float m,vector<vector<float>> myTrail){
        radius = r;
        pos = Position;
        accelration = accel;
        velocity = vel;
        mass = m;
        trail = myTrail;
        
    }
    void drawBall(int res){ 
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(pos[0], pos[1]);  // center of circle

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
            velocity[0] *= -0.1f;

        }
        if (pos[0] > screenWidth - radius){
            pos[0] = screenWidth - radius;
            velocity[0] *= -0.1f;
        }
        if (pos[1] < radius){
            pos[1] = radius;
            velocity[1] *= -0.1f;
            
        }
        if (pos[1] > screenHeight - radius){
            pos[1] = screenHeight - radius;
            velocity[1] *= -0.1f;
        }
    }




    void accelerate(float x ,float y){
        velocity[0] += x;
        velocity[1] += y;
    }
    void move(){
        pos[0] += velocity[0];
        pos[1] += velocity[1];
    }
    
    

    void collisionOther(Ball other){
        if(abs(pos[1] - other.pos[1] < 2 * radius)){
            if( pos[1] > other.pos[1]){
                float temp  = velocity[1];
                pos[1] = other.pos[1] + (2 * radius);
                velocity[1] = 0.9f * velocity[1] + 0.75f * other.velocity[1];
                other.velocity[1] =  0.9f * other.velocity[1] + 0.75f * temp;
            }
            else{
                float temp  = velocity[1];
                other.pos[1] = pos[1] - (2 * radius);
                velocity[1] = 0.9f * velocity[1] + 0.75f * other.velocity[1];
                other.velocity[1] =  0.9f * other.velocity[1] + 0.75f * temp;
            }     
        }
    }


    void gravityForce(Ball& other){
        float G = 6.67f * pow(10,-14);
        float deltaY = other.pos[1] - pos[1];
        float deltaX = other.pos[0] - pos[0];
        float distance = sqrt(pow(deltaX,2) + pow(deltaY,2));
        cout << "distance: "<< distance << "\n";
        if (distance > 100.f) {distance = 100.f;}


        vector<float> direction = {deltaX / distance,deltaY/distance};//scaling the vector
        float Gforce = (G * mass * other.mass ) / (distance * distance);
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

    // Calculate orbital velocity magnitude for circular orbit
    float velocityMag = sqrt(G * M / orbitRadius);
    //ball(xpos,ypos,radius,accelration,velocity,mass)
    Ball sun = Ball({centerX, centerY}, 30.0f, {0.0f, 0.0f}, {0.0f, 0.0f}, M,{{centerX, centerY}});

    // Planets positioned at 4 directions with tangential velocity for orbit
    Ball planetRight = Ball({centerX + orbitRadius, centerY}, 15.0f, {0.0f, 0.0f}, {0.0f, velocityMag}, 1e14f,{{centerX + orbitRadius, centerY}});
    Ball planetTop = Ball({centerX, centerY + orbitRadius}, 15.0f, {0.0f, 0.0f}, {-velocityMag, 0.0f}, 1e14f,{{centerX, centerY + orbitRadius}});
    Ball planetLeft = Ball({centerX - orbitRadius, centerY}, 15.0f, {0.0f, 0.0f}, {0.0f, -velocityMag}, 1e14f,{{centerX - orbitRadius, centerY}});
    Ball planetBottom = Ball({centerX, centerY - orbitRadius}, 15.0f, {0.0f, 0.0f}, {velocityMag, 0.0f}, 1e14f,{{centerX, centerY - orbitRadius}});

    vector<Ball> balls = {sun, planetRight, planetTop, planetLeft, planetBottom};

    

    int res = 100;
    // the physics loop 
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);//clears the color buffer (screen) 
        glLoadIdentity();
        for (auto& ball: balls){
            ball.accelration[0] = 0.0f;
            ball.accelration[1] = 0.0f;
            for(auto& ball2 : balls){
                if(&ball == &ball2){continue;}
                ball.gravityForce(ball2);
            }
            glColor3f(1.0f, 1.0f, 1.0f); //rgb values between 1 and 0
            ball.move();
            ball.updateTrail();
            ball.clampBall();
            ball.drawBall(res);
            ball.trailDraw();
        }
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