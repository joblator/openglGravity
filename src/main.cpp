#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>

using namespace std;

GLFWwindow* StartGLFW();
int screenWidth = 800;
int screenHeight = 1000;
struct Ball
{
    float x,y;
    float radius;
    float accelration ,velocity;


//TODO: change the pos vel and accel to vectors
    Ball(float xPos,float yPos ,float r,float accel[2],float vel){
        x = xPos;
        y = yPos;
        radius = r;
        accelration[0] = accel[0];
        accelration[1] = accel[1];
        velocity = vel;
    }
    void drawBall(int res){ 
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x, y);  // center of circle

        for (int i = 0; i <= res; i++) {
             // draws a triangle from the center of the circle each time at a diffrent angle 
            float angle = 2.0f * 3.141592653589f * (static_cast<float>(i) / res);
            float x1 = x + cos(angle) * radius;
            float y1 = y + sin(angle) * radius;
            glVertex2f(x1, y1);
        }
        glEnd();
    }
    void clampBall(){
        if (x < radius){
            x = radius;

        }
        if (x > screenWidth - radius){
            x = screenWidth - radius;
        }
        if (y < radius){
            y = radius;
            velocity *= -0.7f;

        }
        if (y > screenHeight - radius){
            y = screenHeight - radius;
        }
    }




    void moveBall(){
        accelration -= 0.0001f;
        velocity += accelration;
        y += velocity;
    }
    
    

    void collisionOther(Ball other){
        if(abs(y - other.y) < 2 * radius){
            if( y > other.y){
                float temp  = velocity;
                y = other.y + (2 * radius);
                velocity = 0.9f * velocity + 0.75f * other.velocity;
                other.velocity =  0.9f * other.velocity + 0.75f * temp;
            }
            else{
                float temp  = velocity;
                other.y = y - (2 * radius);
                velocity = 0.9f * velocity + 0.75f * other.velocity;
                other.velocity =  0.9f * other.velocity + 0.75f * temp;
            }     
        }
    }


    void gravityForce(Ball other){
        
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
    //ball(xpos,ypos,radius,accelration,velocity)
    Ball ball = Ball(screenHeight/2.f,screenWidth/2.f,50.0f,{0.0f,0.0f},0.0f);
    Ball ball1 = Ball(screenHeight/2.f ,0,50.0f,{0.0f,0.0f},5.0f);

    int res = 100;
    // the physics loop 
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);//clears the color buffer (screen) 
        glLoadIdentity();

        glColor3f(1.0f, 1.0f, 1.0f);  //rgb values between 0 and 1  
        ball.moveBall();
        ball1.moveBall();
        ball.clampBall();
        ball1.clampBall();
        ball.collisionOther(ball1);
        ball.drawBall(res);
        ball1.drawBall(res);

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