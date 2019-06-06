#ifndef _InputManager_H
#define _InputManager_H

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Standar GLFW for the management of inputs codes keyboards

enum InputCodes {
    kEscape = 27,
    Space = 32,
    Left = 37,
    Up = 38,
    Right = 39,
    Down = 40,
    a = 97,
    A = 65,
    b = 98,
    B = 66,
    c = 99,
    C = 67,
    d = 100,
    D = 68,
    e = 101,
    E = 69,
    f = 102,
    F = 70,
    g = 103,
    G = 71,
    h = 104,
    H = 72,
    i = 105,
    I = 73,
    j = 106,
    J = 74,
    k = 107,
    K = 75,
    l = 108,
    L = 76,
    m = 109,
    M = 77,
    n = 110,
    N = 78,
    o = 111,
    O = 79,
    p = 112,
    P = 80,
    q = 113,
    Q = 81,
    r = 114,
    R = 82,
    s = 115,
    S = 83,
    t = 116,
    T = 84,
    u = 117,
    U = 85,
    v = 118,
    V = 86,
    w = 119,
    W = 87,
    x = 120,
    X = 88,
    y = 121,
    Y = 89,
    z = 122,
    Z = 90,
    LShift = 340,
    Enter = 257,
    n0 = 48,
    n1 = 49,
    n2 = 50,
    n3 = 51,
    n4 = 52,
    n5 = 53,
    n6 = 54
};

enum MouseButtonIndex {
    LEFT = 0, RIGHT = 1, MIDDLE = 2
};

enum State {
    RELESED = 0, PRESSED = 1, REPEATED = 2
};

enum GameState {
    SPLASHSCREEN,
    MAINGAME,
    STARTMENU,
    TRANSITIONSCREEN,
    GAMEOVER,
    WINNER,
    CREDITS,
	EXIT
};

class InputManager {
public:
    glm::vec4 camera_look_at;
    int mapLoaded;
    float roty;
    float totalExplosion;

    InputManager() :
    lastMousePos(glm::ivec2(0.0)), deltax(0), deltay(0), scrollYoffset(
    0), cameraPos(glm::vec3(0.0f, 0.0f, 3.0f)), yaw(0), pitch(
    20), roll(0), distanceFromPlayer(10), initialMovement(400), angleAroundPlayer(0) {
        mouseButtomState = new bool[3];
        keyState = new bool[1024];
        for (int i = 0; i < 3; i++)
            mouseButtomState[i] = false;
        for (int i = 0; i < 1024; i++)
            keyState[i] = false;
        camera_look_at = glm::vec4(0, 0, 0, 0);
        roty = 0.0f;
        carAngle = 0.0f;
        carInitAngle = 0.0f;
        carYSpeed = 0.0f;
        theta0 = 45.0f;
        jumping = false;
        falling = false;
        initJump = true;
        jumpSound = false;
        fallToDeath = false;
        gameState = 0;
        timeExploding = 0.0f;
        explodingBuffer = 0.0f;
        mapLoaded = 0;
        gas = 1.0f;
        gravityVal = 0.6f;
        paused = false;
        startCount = false;
        exploding = false;
        totalExplosion = 2.0f;
    }

    ~InputManager() {
        delete[]mouseButtomState;
        delete[]keyState;
    }

    InputCodes toApplicationKey(int key);
    State toApplicationState(int state);
    MouseButtonIndex toMouseButtonIndex(int button);

    void keyPressed(InputCodes code, float deltaTime, State state);
    void mouseMoved(float mouseX, float mouseY);
    void mouseClicked(MouseButtonIndex code, float mouseX, float mouseY,
            State state);
    void mouseScroll(float yoffset);
	void startButton(int screen);
	float changeVals(float actual, float increment, float maxValue, bool pos);
    void do_movement(float deltaTime);

    glm::ivec2 getLastMousePos() {
        return lastMousePos;
    }

    bool* getKeyState() {
        return keyState;
    }

    glm::vec3 getCameraPos() {
        return cameraPos;
    }

    void setCameraPos(glm::vec3 m_pos) {
        this->cameraPos = m_pos;
    }

    float getPitch() const {
        return pitch;
    }

    float getRoll() const {
        return roll;
    }

    float getYaw() const {
        return yaw;
    }

    void setPitch(float m_pitch) {
        this->pitch = m_pitch;
    }
    
    void resetDeltaXY() {
        this->deltax = 0.0f;
        this->deltay = 0.0f;
        this->angleAroundPlayer = 0.0f;
    }

    void setRoll(float m_roll) {
        this->roll = m_roll;
    }

    void setYaw(float m_yaw) {
        this->yaw = m_yaw;
    }

    float getScrollYoffset() const {
        return scrollYoffset;
    }

    // Car control

    float getCarAngle() {
        return carAngle;
    }
    
    float getGravity() {
        return gravityVal;
    }

    void setCarAngle(float a) {
        this->carAngle = a;
    }

    void setCarYSpeed(float a) {
        this->carYSpeed = a;
    }
    
    float getCarSpeed() {
        return carSpeed;
    }
    
    float getCarSpeedB() {
        return carSpeedBuffer;
    }

    void setCarSpeed(float m_carSpeed) {
        this->carSpeed = m_carSpeed;
    }
    
    float getCarY() {
        return carY;
    }

    void setCarY(float setY) {
        this->carY = setY;
    }

    bool getJumpSound() {
        return jumpSound;
    }

    void setJumpSound(bool var) {
        this->jumpSound = var;
    }
    
    void setJumping(bool var) {
        this->jumping = var;
    }
    
    void setFalling(bool var) {
        this->falling = var;
    }

    float getGas() {
        return gas;
    }

    void setGas(float var) {
        this->gas = var;
    }

    int getGameState() {
        return gameState;
    }

    int getDistanceFromPlayer() {
        return distanceFromPlayer;
    }

    void setGameState(int var) {
        this->gameState = var;
    }

    void setFallToDeath(bool var) {
        this->fallToDeath = var;
    }
    
    bool getExploding() {
        return exploding;
    }
    
    void setExploding(bool var) {
        this->exploding = var;
    }
    
    void setInitJump(bool var) {
        this->initJump = var;
    }

    bool getFallToDeath() {
        return fallToDeath;
    }
    
    bool getPaused(){
        return paused;
    }

    bool getStartCount(){
        return startCount;
    }
    
    void setStutter(float s) {
        stutter = s;
    }
    
    void setExplodingBuffer(float s) {
        explodingBuffer = s;
    }
    
    float getTimer() {
        return timer;
    }
    
    float getTimeExploding() {
        return timeExploding;
    }
    
    void setTimeExploding(float s) {
        this->timeExploding = s;
    }
    
    void setTimer(float t) {
        this->timer = t;
    }
    
    float getAngleAroundPlayer() {
        return angleAroundPlayer;
    }

protected:
    int gameState;
    float timer;
    float timeBuffer;
    float timeExploding;
    float explodingBuffer;
    float stutter;
    bool startCount;

    glm::ivec2 lastMousePos;
    float deltax;
    float deltay;
    float scrollYoffset;
    bool * mouseButtomState;
    bool * keyState;

    glm::vec3 cameraPos;
    float yaw;
    float pitch;
    float roll;

    float distanceFromPlayer;
    float initialMovement;
    float angleAroundPlayer;

    // Car control
    float gravityVal;
    float carAngle;
    float carSpeed;
    float carSpeedBuffer;
    float carInitAngle;
    float carYSpeed;
    float carY;
    float theta0;
    float gas;
    bool jumping;
    bool jumpSound;
    bool falling;
    bool initJump;
    bool fallToDeath;
    bool paused;
    bool exploding;
};

#endif
