//glew include
#include <GL/glew.h>

//std includes
#include <string>
#include <iostream>

//glfw include
#include <GLFW/glfw3.h>

// program include
#include "Headers/InputManager.h"
#include "Headers/TimeManager.h"
#include "Headers/Shader.h"

// Texture include
#include "Headers/Texture.h"

//Map include
#include "Headers/Map.h"

#include "Headers/CubemapTexture.h"

#include "Headers/Model.h"

// Sphere include
#include "Headers/Sphere.h"

// Collision include
#include "Headers/collision.h"

// OpenAL include
#include <AL/alut.h>

#define NUM_BUFFERS 8
#define NUM_SOURCES 8
#define NUM_ENVIRONMENTS 1

Sphere sp(1.5, 50, 50, MODEL_MODE::VERTEX_COLOR);
Sphere sp2(1.5, 50, 50, MODEL_MODE::VERTEX_LIGHT_TEXTURE);

Shader lightingShader;
Shader lampShader;
Shader mapShader;
Shader cubemapShader;
Shader envCubeShader;
Shader dashboardShader;
Shader particlesShader;

float factor = 35.f;

Model modelo1;
Model copa;

Map * maps[4];

Texture textureParticle(GL_TEXTURE_2D, "../Textures/fire.png");

Texture * textureDashboard;
Texture * texturePaused;
Texture * textureCount[4];
Texture * textureLevel[4];
Texture * textureNoGas;
Texture * textureSplash;
Texture * textureMenu;
Texture * textureNeedle;
Texture * texturePuntaje;
Texture * textureNums[10];
Texture * textureExplosion[40];
Texture * textureLife;
Texture * textureVidas;
Texture * textureGameover;
Texture * textureWinner;
Texture * textureMision;
Texture * textureCredits;

struct VertexTexture {
	glm::vec3 m_Pos;
	glm::vec2 m_TexCoord;
	glm::vec3 m_Normal;
};

CubemapTexture * cubeMaptexture[4] = {
	new CubemapTexture("../Textures/ely_darkcity", "darkcity_ft.tga", "darkcity_bk.tga", "darkcity_up.tga", "darkcity_dn.tga", "darkcity_rt.tga", "darkcity_lf.tga"),
	new CubemapTexture("../Textures/darkskies", "darkskies_ft.tga", "darkskies_bk.tga", "darkskies_up.tga", "darkskies_dn.tga", "darkskies_rt.tga", "darkskies_lf.tga"),
	new CubemapTexture("../Textures/mp_blood", "blood_ft.tga", "blood_bk.tga", "blood_up.tga", "blood_dn.tga", "blood_rt.tga", "blood_lf.tga"),
	new CubemapTexture("../Textures/wplanet", "front.png", "back.png", "up.png", "down.png", "right.png", "left.png"),
};

// Definition for the particle system
GLuint initVel, startTime;
GLuint VAOParticles;
GLuint nParticles;

GLuint VAO, VBO, EBO, DVAO, DVBO;

int screenWidth;
int screenHeight;

GLFWwindow * window;
InputManager inputManager;
double deltaTime;

// OpenAL config
ALfloat listenerPos[] = { 0.0, 0.0, 4.0 };
ALfloat listenerVel[] = { 0.0, 0.0, 0.0 };
ALfloat listenerOri[] = { 0.0, 0.0, 1.0, 0.0, 1.0, 0.0 };

ALfloat source0Pos[] = { -2.0, 0.0, 0.0 };
ALfloat source0Vel[] = { 0.0, 0.0, 0.0 };

ALfloat source1Pos[] = { 2.0, 0.0, 0.0 };
ALfloat source1Vel[] = { 0.0, 0.0, 0.0 };

ALfloat source2Pos[] = { 0.0, 0.0, -4.0 };
ALfloat source2Vel[] = { 0.0, 0.0, 0.0 };

ALuint buffer[NUM_BUFFERS];
ALuint source[NUM_SOURCES];
ALuint environment[NUM_ENVIRONMENTS];

ALsizei size, freq;
ALenum format;
ALvoid *data;
int ch;
ALboolean loop;

// Se definen todos las funciones.
void reshapeCallback(GLFWwindow* Window, int widthRes, int heightRes);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void mouseButtonCallback(GLFWwindow* window, int button, int state, int mod);
void init(int width, int height, std::string strTitle, bool bFullScreen);
void destroyWindow();
void drawNumbers(int puntos, GLint modelLoc, glm::vec3 origin, glm::vec3 scale);
void drawLives(int lives, GLint modelLoc);
void destroy();
bool processInput(bool continueApplication = true);

void initParticleBuffers() {
	nParticles = 8000;

	// Generate the buffers
	glGenBuffers(1, &initVel); // Initial velocity buffer
	glGenBuffers(1, &startTime); // Start time buffer

	// Allocate space for all buffers
	int size = nParticles * 3 * sizeof(float);
	glBindBuffer(GL_ARRAY_BUFFER, initVel);
	glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, startTime);
	glBufferData(GL_ARRAY_BUFFER, nParticles * sizeof(float), NULL, GL_STATIC_DRAW);

	// Fill the first velocity buffer with random velocities
	glm::vec3 v(0.0f);
	float velocity, theta, phi;
	GLfloat *data = new GLfloat[nParticles * 3];
	for (unsigned int i = 0; i < nParticles; i++) {

		theta = glm::mix(0.0f, glm::pi<float>() / 6.0f, ((float)rand() / RAND_MAX));
		phi = glm::mix(0.0f, glm::two_pi<float>(), ((float)rand() / RAND_MAX));

		v.x = sinf(theta) * cosf(phi);
		v.y = cosf(theta);
		v.z = sinf(theta) * sinf(phi);

		velocity = glm::mix(3.5f, 4.0f, ((float)rand() / RAND_MAX));
		v = glm::normalize(v) * velocity;

		data[3 * i] = v.x;
		data[3 * i + 1] = v.y;
		data[3 * i + 2] = v.z;
	}
	glBindBuffer(GL_ARRAY_BUFFER, initVel);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);

	// Fill the start time buffer
	delete[] data;
	data = new GLfloat[nParticles];
	float time = 0.0f;
	float rate = 0.00075f;
	for (unsigned int i = 0; i < nParticles; i++) {
		data[i] = time;
		time += rate;
	}
	glBindBuffer(GL_ARRAY_BUFFER, startTime);
	glBufferSubData(GL_ARRAY_BUFFER, 0, nParticles * sizeof(float), data);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	delete[] data;

	glGenVertexArrays(1, &VAOParticles);
	glBindVertexArray(VAOParticles);
	glBindBuffer(GL_ARRAY_BUFFER, initVel);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, startTime);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}



void applicationLoop();

// Implementacion de todas las funciones.

void init(int width, int height, std::string strTitle, bool bFullScreen) {

	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		exit(-1);
	}

	screenWidth = width;
	screenHeight = height;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	if (bFullScreen)
		window = glfwCreateWindow(width, height, strTitle.c_str(),
			glfwGetPrimaryMonitor(), nullptr);
	else
		window = glfwCreateWindow(width, height, strTitle.c_str(), nullptr,
			nullptr);

	if (window == nullptr) {
		std::cerr
			<< "Error to create GLFW window, you can try download the last version of your video card that support OpenGL 3.3+"
			<< std::endl;
		destroyWindow();
		exit(-1);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);

	glfwSetWindowSizeCallback(window, reshapeCallback);
	glfwSetWindowSizeCallback(window, reshapeCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Init glew
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		std::cerr << "Failed to initialize glew" << std::endl;
		exit(-1);
	}

	glViewport(0, 0, screenWidth, screenHeight);
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Enable test depth, must be clear depth buffer bit
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	sp.init();
	sp.load();

	sp2.init();
	sp2.load();

	modelo1.loadModel("../objects/f.obj");
	copa.loadModel("../objects/cup.obj");

	lightingShader.initialize("../Shaders/loadModelLighting.vs", "../Shaders/loadModelLighting.fs");
	mapShader.initialize("../Shaders/mvpTexture.vs", "../Shaders/mapTexture.fs");
	lampShader.initialize("../Shaders/lampShader.vs", "../Shaders/lampShader.fs");
	cubemapShader.initialize("../Shaders/cubemapTexture.vs", "../Shaders/cubemapTexture.fs");
	envCubeShader.initialize("../Shaders/envRefCubemapTexture.vs", "../Shaders/envRefCubemapTexture.fs");
	dashboardShader.initialize("../Shaders/dashboard.vs", "../Shaders/dashboard.fs");
	particlesShader.initialize("../Shaders/particles.vs", "../Shaders/particles.fs");

	//--- AUDIO LIBRARY ---//
	// OpenAL init
	alutInit(0, NULL);

	alListenerfv(AL_POSITION, listenerPos);
	alListenerfv(AL_VELOCITY, listenerVel);
	alListenerfv(AL_ORIENTATION, listenerOri);

	alGetError(); // clear any error messages

	if (alGetError() != AL_NO_ERROR) {
		printf("- Error creating buffers !!\n");
		exit(1);
	}
	else {
		printf("init() - No errors yet.");
	}

	// Generate buffers, or else no sound will happen!
	alGenBuffers(NUM_BUFFERS, buffer);

	buffer[0] = alutCreateBufferFromFile("../sounds/jump-sound2.wav");
	buffer[1] = alutCreateBufferFromFile("../sounds/TitleBgm.wav");
	buffer[2] = alutCreateBufferFromFile("../sounds/EndlessChallenge.wav");
	buffer[3] = alutCreateBufferFromFile("../sounds/gas.wav");
	buffer[4] = alutCreateBufferFromFile("../sounds/coin.wav");
	buffer[5] = alutCreateBufferFromFile("../sounds/explosion.wav");
	buffer[6] = alutCreateBufferFromFile("../sounds/powerup.wav");
	buffer[7] = alutCreateBufferFromFile("../sounds/start-beeps.wav");

	alGetError(); /* clear error */
	alGenSources(NUM_SOURCES, source);

	if (alGetError() != AL_NO_ERROR) {
		printf("- Error creating sources !!\n");
		exit(2);
	}
	else {
		printf("init - no errors after alGenSources\n");
	}

	alSourcef(source[0], AL_PITCH, 1.0f);
	alSourcef(source[0], AL_GAIN, 0.6f);
	alSourcefv(source[0], AL_POSITION, source0Pos);
	alSourcefv(source[0], AL_VELOCITY, source0Vel);
	alSourcei(source[0], AL_BUFFER, buffer[0]);
	alSourcei(source[0], AL_LOOPING, AL_FALSE);
	alSourcef(source[0], AL_MAX_DISTANCE, 1200);

	alSourcef(source[1], AL_PITCH, 1.0f);
	alSourcef(source[1], AL_GAIN, 0.3f);
	alSourcefv(source[1], AL_POSITION, source0Pos);
	alSourcefv(source[1], AL_VELOCITY, source0Vel);
	alSourcei(source[1], AL_BUFFER, buffer[1]);
	alSourcei(source[1], AL_LOOPING, AL_TRUE);
	alSourcef(source[1], AL_MAX_DISTANCE, 1200);

	alSourcef(source[2], AL_PITCH, 1.0f);
	alSourcef(source[2], AL_GAIN, 0.3f);
	alSourcefv(source[2], AL_POSITION, source0Pos);
	alSourcefv(source[2], AL_VELOCITY, source0Vel);
	alSourcei(source[2], AL_BUFFER, buffer[2]);
	alSourcei(source[2], AL_LOOPING, AL_TRUE);
	alSourcef(source[2], AL_MAX_DISTANCE, 1200);

	alSourcef(source[3], AL_PITCH, 1.0f);
	alSourcef(source[3], AL_GAIN, 0.1f);
	alSourcefv(source[3], AL_POSITION, source0Pos);
	alSourcefv(source[3], AL_VELOCITY, source0Vel);
	alSourcei(source[3], AL_BUFFER, buffer[3]);
	alSourcei(source[3], AL_LOOPING, AL_TRUE);
	alSourcef(source[3], AL_MAX_DISTANCE, 1200);

	alSourcef(source[4], AL_PITCH, 1.0f);
	alSourcef(source[4], AL_GAIN, 0.6f);
	alSourcefv(source[4], AL_POSITION, source0Pos);
	alSourcefv(source[4], AL_VELOCITY, source0Vel);
	alSourcei(source[4], AL_BUFFER, buffer[4]);
	alSourcei(source[4], AL_LOOPING, AL_FALSE);
	alSourcef(source[4], AL_MAX_DISTANCE, 1200);

	alSourcef(source[5], AL_PITCH, 1.0f);
	alSourcef(source[5], AL_GAIN, 1.0f);
	alSourcefv(source[5], AL_POSITION, source0Pos);
	alSourcefv(source[5], AL_VELOCITY, source0Vel);
	alSourcei(source[5], AL_BUFFER, buffer[5]);
	alSourcei(source[5], AL_LOOPING, AL_FALSE);
	alSourcef(source[5], AL_MAX_DISTANCE, 1200);

	alSourcef(source[6], AL_PITCH, 1.0f);
	alSourcef(source[6], AL_GAIN, 0.5f);
	alSourcefv(source[6], AL_POSITION, source0Pos);
	alSourcefv(source[6], AL_VELOCITY, source0Vel);
	alSourcei(source[6], AL_BUFFER, buffer[6]);
	alSourcei(source[6], AL_LOOPING, AL_FALSE);
	alSourcef(source[6], AL_MAX_DISTANCE, 1200);

	alSourcef(source[7], AL_PITCH, 1.0f);
	alSourcef(source[7], AL_GAIN, 2.0f);
	alSourcefv(source[7], AL_POSITION, source0Pos);
	alSourcefv(source[7], AL_VELOCITY, source0Vel);
	alSourcei(source[7], AL_BUFFER, buffer[7]);
	alSourcei(source[7], AL_LOOPING, AL_FALSE);
	alSourcef(source[7], AL_MAX_DISTANCE, 1200);

	VertexTexture vertices[36] = {
		// Back face
		{ glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)},
		{ glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec2(0.0f, 1.0f), glm::vec3(0.0f, 0.0f, -1.0f)},
		{ glm::vec3(0.5f, 0.5f, -0.5f), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, 0.0f, -1.0f)},
		{ glm::vec3(0.5f, 0.5f, -0.5f), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, 0.0f, -1.0f)},
		{ glm::vec3(0.5f, -0.5f, -0.5f), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)},
		{ glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)},
		// Front face
		{ glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)},
		{ glm::vec3(0.5f, -0.5f, 0.5f), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)},
		{ glm::vec3(0.5f, 0.5f, 0.5f), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f)},
		{ glm::vec3(0.5f, 0.5f, 0.5f), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f)},
		{ glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec2(0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f)},
		{ glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)},
		// Left face
		{ glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec2(1.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f)},
		{ glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec2(1.0f, 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f)},
		{ glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f)},
		{ glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f)},
		{ glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec2(0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f)},
		{ glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec2(1.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f)},
		// Right face
		{ glm::vec3(0.5f, 0.5f, 0.5f), glm::vec2(1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)},
		{ glm::vec3(0.5f, -0.5f, 0.5f), glm::vec2(0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)},
		{ glm::vec3(0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f)},
		{ glm::vec3(0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f)},
		{ glm::vec3(0.5f, 0.5f, -0.5f), glm::vec2(1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f)},
		{ glm::vec3(0.5f, 0.5f, 0.5f), glm::vec2(1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)},
		// Bottom face
		{ glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)},
		{ glm::vec3(0.5f, -0.5f, -0.5f), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)},
		{ glm::vec3(0.5f, -0.5f, 0.5f), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)},
		{ glm::vec3(0.5f, -0.5f, 0.5f), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)},
		{ glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)},
		{ glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)},
		// Top face
		{ glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec2(0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
		{ glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
		{ glm::vec3(0.5f, 0.5f, 0.5f), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
		{ glm::vec3(0.5f, 0.5f, 0.5f), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
		{ glm::vec3(0.5f, 0.5f, -0.5f), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
		{ glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec2(0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f)}
	};
	float dashHeight = 0.6f;
	VertexTexture dashVertices[] = {
		// Dashboard face
		// Back face
		{ glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)},
		{ glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec2(0.0f, 1.0f), glm::vec3(0.0f, 0.0f, -1.0f)},
		{ glm::vec3(0.5f, 0.5f, -0.5f), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, 0.0f, -1.0f)},
		{ glm::vec3(0.5f, 0.5f, -0.5f), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, 0.0f, -1.0f)},
		{ glm::vec3(0.5f, -0.5f, -0.5f), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)},
		{ glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)},
	};

	glGenVertexArrays(1, &VAO);
	glGenVertexArrays(1, &DVAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &DVBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]),
		(GLvoid*)0);
	glEnableVertexAttribArray(0);
	// Texture Mapping attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertices[0]),
		(GLvoid*) sizeof(vertices[0].m_Pos));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]),
		(GLvoid*)(sizeof(vertices[0].m_Pos) + sizeof(vertices[0].m_TexCoord)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(DVAO);

	glBindBuffer(GL_ARRAY_BUFFER, DVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(dashVertices), dashVertices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(dashVertices[0]),
		(GLvoid*)0);
	glEnableVertexAttribArray(0);
	// Texture Mapping attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(dashVertices[0]),
		(GLvoid*) sizeof(dashVertices[0].m_Pos));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(dashVertices[0]),
		(GLvoid*)(sizeof(dashVertices[0].m_Pos) + sizeof(dashVertices[0].m_TexCoord)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); // Unbind VAO

	maps[0] = new Map("../Maps/map0.txt", "../Maps/coins0.txt", "../Maps/end0.txt");
	maps[0]->load(factor, 0);

	maps[1] = new Map("../Maps/map1.txt", "../Maps/coins1.txt", "../Maps/end1.txt");
	maps[1]->load(factor, 1);

	maps[2] = new Map("../Maps/map2.txt", "../Maps/coins2.txt", "../Maps/end2.txt");
	maps[2]->load(factor, 2);

	maps[3] = new Map("../Maps/map3.txt", "../Maps/coins3.txt", "../Maps/end3.txt");
	maps[3]->load(factor, 3);

	textureDashboard = new Texture(GL_TEXTURE_2D, "../Textures/dashboard2.png");
	textureDashboard->load();

	texturePaused = new Texture(GL_TEXTURE_2D, "../Textures/pausa.png");
	texturePaused->load();

	textureCount[0] = new Texture(GL_TEXTURE_2D, "../Textures/transparent.png");
	textureCount[0]->load();

	textureCount[1] = new Texture(GL_TEXTURE_2D, "../Textures/c1.png");
	textureCount[1]->load();

	textureCount[2] = new Texture(GL_TEXTURE_2D, "../Textures/c2.png");
	textureCount[2]->load();

	textureCount[3] = new Texture(GL_TEXTURE_2D, "../Textures/c3.png");
	textureCount[3]->load();

	textureLevel[0] = new Texture(GL_TEXTURE_2D, "../Textures/nivel1.png");
	textureLevel[0]->load();

	textureLevel[1] = new Texture(GL_TEXTURE_2D, "../Textures/nivel2.png");
	textureLevel[1]->load();

	textureLevel[2] = new Texture(GL_TEXTURE_2D, "../Textures/nivel3.png");
	textureLevel[2]->load();

	textureLevel[3] = new Texture(GL_TEXTURE_2D, "../Textures/ultimo.png");
	textureLevel[3]->load();

	textureNoGas = new Texture(GL_TEXTURE_2D, "../Textures/dashboard.png");
	textureNoGas->load();

	textureSplash = new Texture(GL_TEXTURE_2D, "../Textures/splash.png");
	textureSplash->load();

	textureMenu = new Texture(GL_TEXTURE_2D, "../Textures/splash.png");
	textureMenu->load();

	textureNeedle = new Texture(GL_TEXTURE_2D, "../Textures/aguja.png");
	textureNeedle->load();

	texturePuntaje = new Texture(GL_TEXTURE_2D, "../Textures/puntaje.png");
	texturePuntaje->load();

	textureLife = new Texture(GL_TEXTURE_2D, "../Textures/life.png");
	textureLife->load();

	textureVidas = new Texture(GL_TEXTURE_2D, "../Textures/vidas.png");
	textureVidas->load();

	textureGameover = new Texture(GL_TEXTURE_2D, "../Textures/gameover.png");
	textureGameover->load();

	textureWinner = new Texture(GL_TEXTURE_2D, "../Textures/winbg.jpg");
	textureWinner->load();

	textureMision = new Texture(GL_TEXTURE_2D, "../Textures/mision.png");
	textureMision->load();

	textureCredits = new Texture(GL_TEXTURE_2D, "../Textures/endingcredits.png");
	textureCredits->load();

	for (int i = 0; i < 10; i++) {
		textureNums[i] = new Texture(GL_TEXTURE_2D, "../Textures/nums/" + std::to_string(i) + ".png");
		textureNums[i]->load();
	}

	for (int i = 0; i < 40; i++) {
		textureExplosion[i] = new Texture(GL_TEXTURE_2D, "../Textures/explosion/" + std::to_string(i) + ".png");
		textureExplosion[i]->load();
	}


	cubeMaptexture[0]->Load();
	cubeMaptexture[1]->Load();
	cubeMaptexture[2]->Load();
	cubeMaptexture[3]->Load();

	// The particle texture
	textureParticle.load();


}

void destroyWindow() {
	glfwDestroyWindow(window);
	glfwTerminate();
}

void destroy() {
	destroyWindow();
	lightingShader.destroy();
	mapShader.destroy();
	lampShader.destroy();
	cubemapShader.destroy();
	envCubeShader.destroy();
	dashboardShader.destroy();

	delete textureNeedle;
	delete texturePuntaje;
	delete textureSplash;
	delete textureMenu;
	delete textureNoGas;
	delete textureDashboard;
	delete texturePaused;
	delete textureLife;
	delete textureVidas;
	delete textureGameover;
	delete textureWinner;
	delete textureMision;
	delete textureCredits;

	for (int i = 0; i < 10; i++) {
		delete textureNums[i];
	}
	for (int i = 0; i < 40; i++) {
		delete textureExplosion[i];
	}

	for (int i = 0; i < 4; i++) {
		delete textureCount[i];
		//maps[i]->freeMem();
		delete maps[i];
		delete cubeMaptexture[i];
		delete textureLevel[i];
	}
}

void reshapeCallback(GLFWwindow* Window, int widthRes, int heightRes) {
	screenWidth = widthRes;
	screenHeight = heightRes;
	glViewport(0, 0, widthRes, heightRes);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	inputManager.keyPressed(inputManager.toApplicationKey(key), deltaTime * 10.0f,
		inputManager.toApplicationState(action));
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	inputManager.mouseMoved(xpos, ypos);
}

void mouseButtonCallback(GLFWwindow* window, int button, int state, int mod) {
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	inputManager.mouseClicked(inputManager.toMouseButtonIndex(button), xpos,
		ypos, inputManager.toApplicationState(state));
}

void drawNumbers(int puntos, GLint modelLoc, glm::vec3 origin, glm::vec3 scale) {
	int digit;

	glm::mat4 digitModel = glm::translate(glm::mat4(1.0f), origin);
	digitModel = glm::scale(digitModel, scale);
	for (int i = 0; i < 3; i++) {
		digit = puntos / pow(10.0, 2 - i);
		puntos -= (digit * pow(10.0, 2 - i));
		textureNums[digit]->bind(GL_TEXTURE0);
		digitModel = glm::translate(digitModel, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(digitModel));
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
}

void drawLives(int lives, GLint modelLoc) {
	glm::mat4 vidas = glm::translate(glm::mat4(1.0f), glm::vec3(screenWidth * 0.83f, screenHeight * 0.05f, 0.0f));
	vidas = glm::scale(vidas, glm::vec3(screenHeight * 0.18f, screenHeight * 0.05f, 1.0f));
	textureVidas->bind(GL_TEXTURE0);
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(vidas));
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glm::mat4 lifeModel = glm::translate(glm::mat4(1.0f), glm::vec3(screenWidth * 0.88f, screenHeight * 0.05f, 0.0f));
	lifeModel = glm::scale(lifeModel, glm::vec3(screenHeight * 0.05f, screenHeight * 0.05f, 1.0f));
	textureLife->bind(GL_TEXTURE0);
	for (int i = 0; i < lives; i++) {
		lifeModel = glm::translate(lifeModel, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(lifeModel));
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
}

bool processInput(bool continueApplication) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS
		|| glfwWindowShouldClose(window) != 0) {
		return false;
	}
	TimeManager::Instance().CalculateFrameRate(false);
	deltaTime = TimeManager::Instance().DeltaTime;
	if (deltaTime < (1000.0f / 61.0f))
		this_thread::sleep_for(chrono::milliseconds((int)((1000.0f / 61.0f) - deltaTime)));
	inputManager.do_movement(deltaTime);
	glfwPollEvents();
	return continueApplication;
}

void applicationLoop() {
	bool psi = true;

	glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
	double lastTime = TimeManager::Instance().GetTime();

	AABB aabb = getAABB(modelo1.getMeshes());
	AABB aabb1;
	AABB aabb2;
	int objType;
	std::vector<Map::ObjStruct> m_objVector;
	std::vector<Map::ObjStruct> m_wallVector;
	std::vector<Map::ObjStruct> m_obsVector;
	std::vector<Map::CoinStruct> m_coinVector;
	std::vector<SBB> m_fallsVector;
	AABB levelEnding;

	// Case init
	GLint viewPosLoc;
	GLint matDiffuseLoc;
	GLint matSpecularLoc;
	GLint matShineLoc;
	GLint lightAmbientLoc;
	GLint lightDiffuseLoc;
	GLint lightSpecularLoc;
	GLint lightPosLoc;
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;
	glm::mat4 explosionModel;
	glm::mat4 explosionView;
	glm::mat4 explosionProjection;
	glm::mat4 model1;
	glm::vec3 cameraPos;
	SBB sbb1, sbb2;
	GLint oldCullFaceMode;
	GLint oldDepthFuncMode;
	glm::mat4 cubeModel;
	float ratio;
	float source0pitch = 0.5f;
	float creditRoll = screenHeight * 1.5f;

	float offsetz = 0.0f;
	float offsetH = 0.0f;
	float offsetx = 0.0f;
	float offsety = 0.0f;
	int gameState = 0;
	int remainingLives = 2;
	int score = 0;
	float cupAngle = 0.0f;
	bool enterMain = true;
	bool paused = false;
	bool getting_gas = false;
	bool playSource1 = true;
	bool playSource2 = true;
	bool playSource3 = true;
	bool playSource4 = true;
	bool playSource5 = false;

	glm::mat4 dashboardModel;
	glm::mat4 pausedModel;
	glm::mat4 splashScreen;
	glm::mat4 aguja;
	glm::mat4 agujaGas;
	glm::mat4 puntaje;
	glm::mat4 nums[3];

	while (psi) {
		psi = processInput(true);
		// This is new, need clear depth buffer bit
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		cupAngle += 0.5f;
		GLint modelLoc;
		GLint viewLoc;
		GLint projLoc;

		GLuint cubeTextureId;


		GLint selector;
		ALfloat audioPos[3] = { 0.0f, 0.0f, 0.0f };

		switch (inputManager.getGameState()) {
		case 0:
			enterMain = true;
			inputManager.setFallToDeath(false);
			view = glm::mat4(1.0f);
			projection = glm::ortho(0.0f, (float)screenWidth, (float)screenHeight, 0.0f, -5.0f, 5.0f);

			dashboardShader.turnOn();
			// Get their uniform location
			modelLoc = dashboardShader.getUniformLocation("model");
			viewLoc = dashboardShader.getUniformLocation("view");
			projLoc = dashboardShader.getUniformLocation("projection");

			// Pass the matrices to the shader
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

			textureSplash->bind(GL_TEXTURE0);
			glUniform1i(dashboardShader.getUniformLocation("texture0"), 0);
			selector = dashboardShader.getUniformLocation("selector");
			glUniform1i(selector, 0);

			glBindVertexArray(DVAO);

			splashScreen = glm::translate(glm::mat4(1.0f), glm::vec3(screenWidth * 0.5f, screenHeight * 0.5f, 0.0f));
			splashScreen = glm::scale(splashScreen, glm::vec3(screenWidth, screenHeight, 1.0f));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(splashScreen));
			glDrawArrays(GL_TRIANGLES, 0, 6);

			glBindVertexArray(0);
			dashboardShader.turnOff();

			glfwSwapBuffers(window);

			audioPos[0] = screenWidth * 0.5f;
			audioPos[1] = screenHeight * 0.8f;

			alSourcefv(source[1], AL_POSITION, audioPos);
			alListenerfv(AL_POSITION, audioPos);

			listenerOri[0] = 0.0f;
			listenerOri[1] = 0.0f;
			listenerOri[2] = 1.0f;
			listenerOri[3] = 0.0f;
			listenerOri[4] = 1.0f;
			listenerOri[5] = 0.0f;
			alListenerfv(AL_ORIENTATION, listenerOri);
			if (playSource1) {
				alSourcePlay(source[1]);
				playSource1 = false;
			}


			break;
		case 1:

			alSourceStop(source[1]);

			if (enterMain) {
				inputManager.camera_look_at = glm::vec4(0, 0, 0, 0);
				inputManager.roty = 0.0f;
				inputManager.resetDeltaXY();
				float horDist = inputManager.getDistanceFromPlayer()
					* glm::cos(glm::radians(inputManager.getPitch()));
				float vertDist = inputManager.getDistanceFromPlayer()
					* glm::sin(glm::radians(inputManager.getPitch()));
				float theta = inputManager.roty + inputManager.getAngleAroundPlayer();
				offsetx = horDist * glm::sin(glm::radians(theta));
				offsetz = horDist * glm::cos(glm::radians(theta));

				inputManager.setCameraPos(glm::vec3(inputManager.camera_look_at.x - offsetx,
					inputManager.camera_look_at.y + vertDist,
					inputManager.camera_look_at.z - offsetz));


				offsetH = 0.0f;
				offsetx = 0.0f;
				offsetz = 0.0f;
				inputManager.setYaw(0.0f);
				inputManager.setPitch(20.0f);
				inputManager.setCarAngle(0.0f);
				inputManager.setCarSpeed(0.0f);
				inputManager.setCarY(0.0f);
				inputManager.setCarYSpeed(0.0f);

				inputManager.setInitJump(true);
				inputManager.setJumping(false);
				inputManager.setFalling(false);
				inputManager.setFallToDeath(false);
				inputManager.setGas(1.0f);
				inputManager.setExplodingBuffer(0.0f);
				inputManager.setTimeExploding(0.0f);

				//initParticleBuffers();
				enterMain = false;
			}

			// Create transformations
			view = glm::rotate(glm::mat4(1.0f),
				glm::radians(inputManager.getPitch()),
				glm::vec3(1, 0, 0));
			view = glm::rotate(view,
				glm::radians(inputManager.getYaw()),
				glm::vec3(0, 1, 0));
			cameraPos = inputManager.getCameraPos();
			if (cameraPos.y >= 0.5f)
				view = glm::translate(view,
					glm::vec3(-cameraPos.x, -cameraPos.y, -cameraPos.z));
			else
				view = glm::translate(view,
					glm::vec3(-cameraPos.x, -0.5f, -cameraPos.z));

			projection = glm::perspective(45.0f, (float)screenWidth / (float)screenHeight, 0.1f, 1000.0f);

			// --- PLAYER --- //
			lightingShader.turnOn();

			viewPosLoc = lightingShader.getUniformLocation("viewPos");
			glUniform3f(viewPosLoc, cameraPos.x, cameraPos.y,
				cameraPos.z);

			//// Set material properties
			matDiffuseLoc = lightingShader.getUniformLocation(
				"material.diffuse");
			matSpecularLoc = lightingShader.getUniformLocation(
				"material.specular");
			matShineLoc = lightingShader.getUniformLocation(
				"material.shininess");
			glUniform1i(matDiffuseLoc, 0);
			glUniform1i(matSpecularLoc, 1);
			glUniform1f(matShineLoc, 32.0f);

			//// Set lights properties
			lightAmbientLoc = lightingShader.getUniformLocation(
				"light.ambient");
			lightDiffuseLoc = lightingShader.getUniformLocation(
				"light.diffuse");
			lightSpecularLoc = lightingShader.getUniformLocation(
				"light.specular");
			lightPosLoc = lightingShader.getUniformLocation("light.position");
			glUniform3f(lightAmbientLoc, 1.0f, 1.0f, 1.0f);
			glUniform3f(lightDiffuseLoc, 1.0f, 0.2f, 0.5f); // Let's darken the light a bit to fit the scene
			glUniform3f(lightSpecularLoc, 1.0f, 1.0f, 1.0f);
			glUniform3f(lightPosLoc, offsetx, inputManager.getCarY() + 1.0f, offsetz);

			//// Get the uniform locations
			modelLoc = lightingShader.getUniformLocation("model");
			viewLoc = lightingShader.getUniformLocation("view");
			projLoc = lightingShader.getUniformLocation("projection");

			//// Pass the matrices to the shader
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

			offsetH = inputManager.getCarSpeed() * 2.0f;
			offsetx += offsetH * sin(glm::radians(inputManager.roty));
			offsetz += offsetH * cos(glm::radians(inputManager.roty));
			if (inputManager.getCarY() < 0.0f && !inputManager.getFallToDeath())
				inputManager.setCarY(0.0f);
			model = glm::translate(glm::mat4(1.0f), glm::vec3(offsetx, inputManager.getCarY(), offsetz));
			model = glm::rotate(model, glm::radians(inputManager.roty), glm::vec3(0.0f, 1.0f, 0.0f));
			// model = glm::translate(model, glm::vec3(0.0f, 0.0f, offsetH));
			model = glm::rotate(model, glm::radians(inputManager.getCarAngle()), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.3, 0.3, 0.3));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

			if (!inputManager.getExploding())
				modelo1.render(&lightingShader);

			aabb1.min = glm::vec3(-0.2f + offsetx, 0.0f + inputManager.getCarY(), -0.2f + offsetz);
			aabb1.max = glm::vec3(0.2f + offsetx, 0.2f + inputManager.getCarY(), 0.2f + offsetz);

			inputManager.camera_look_at = glm::vec4(glm::rotate(model, glm::radians(inputManager.roty), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

			glUniform1i(matDiffuseLoc, 0);
			glUniform1i(matSpecularLoc, 1);
			glUniform1f(matShineLoc, 32.0f);

			explosionModel = model;
			explosionView = view;
			explosionProjection = projection;


			lightingShader.turnOff();

			// --- CUBEMAP --- //
			cubemapShader.turnOn();

			glGetIntegerv(GL_CULL_FACE_MODE, &oldCullFaceMode);
			glGetIntegerv(GL_DEPTH_FUNC, &oldDepthFuncMode);

			viewLoc = cubemapShader.getUniformLocation("view");
			projLoc = cubemapShader.getUniformLocation("projection");
			modelLoc = cubemapShader.getUniformLocation("model");

			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

			cubeModel = glm::scale(glm::mat4(1.0f), glm::vec3(20.0f, 20.0f, 20.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(cubeModel));

			cubeMaptexture[inputManager.mapLoaded]->Bind(GL_TEXTURE0);
			cubeTextureId = cubemapShader.getUniformLocation("skybox");
			glUniform1f(cubeTextureId, 0);

			glCullFace(GL_FRONT);
			glDepthFunc(GL_LEQUAL);
			sp2.render();
			glCullFace(oldCullFaceMode);
			glDepthFunc(oldDepthFuncMode);

			cubemapShader.turnOff();

			// --- MAP --- //
			mapShader.turnOn();
			// Get their uniform location
			modelLoc = mapShader.getUniformLocation("model");
			viewLoc = mapShader.getUniformLocation("view");
			projLoc = mapShader.getUniformLocation("projection");

			// Pass the matrices to the shader
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
			//modelo1.render(&mapShader);
			glBindVertexArray(VAO);

			maps[inputManager.mapLoaded]->drawInteractive(modelLoc, factor, &mapShader);
			maps[inputManager.mapLoaded]->draw(modelLoc, factor, &mapShader);


			m_objVector = maps[inputManager.mapLoaded]->getBBS();
			m_wallVector = maps[inputManager.mapLoaded]->getWallbbs();
			m_obsVector = maps[inputManager.mapLoaded]->getObstaclebbs();
			m_fallsVector = maps[inputManager.mapLoaded]->getFallsbbs();

			for (decltype(m_objVector.size()) i = 0; i < maps[inputManager.mapLoaded]->coinVector.size(); i++) {
				if (maps[inputManager.mapLoaded]->coinVector[i].notTaken && !inputManager.getExploding()) {
					if (testBoxSphereIntersection(maps[inputManager.mapLoaded]->coinVector[i].coinBB, aabb1) &&
						maps[inputManager.mapLoaded]->coinVector[i].notTaken) {
						maps[inputManager.mapLoaded]->coinVector[i].notTaken = false;
						score++;
						source0Pos[0] = maps[inputManager.mapLoaded]->coinVector[i].coinBB.center.x;
						source0Pos[1] = maps[inputManager.mapLoaded]->coinVector[i].coinBB.center.y;
						source0Pos[2] = maps[inputManager.mapLoaded]->coinVector[i].coinBB.center.z;
						if (score % 50 == 0) {
							remainingLives++;
							alSourcefv(source[6], AL_POSITION, source0Pos);
							alSourcePlay(source[6]);
						}
						alSourcefv(source[4], AL_POSITION, source0Pos);
						alSourcePlay(source[4]);
					}
				}
			}

			glBindVertexArray(0);

			mapShader.turnOff();


			// --- DASHBOARD --- //
			dashboardShader.turnOn();

			view = glm::mat4(1.0f);
			projection = glm::ortho(0.0f, (float)screenWidth, (float)screenHeight, 0.0f, -5.0f, 5.0f);

			// Get their uniform location
			modelLoc = dashboardShader.getUniformLocation("model");
			viewLoc = dashboardShader.getUniformLocation("view");
			projLoc = dashboardShader.getUniformLocation("projection");

			// Pass the matrices to the shader
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
			if (inputManager.getGas() > 0.0f)
				textureDashboard->bind(GL_TEXTURE0);
			else
				textureNoGas->bind(GL_TEXTURE0);
			glUniform1i(dashboardShader.getUniformLocation("texture0"), 0);
			selector = dashboardShader.getUniformLocation("selector");
			glUniform1i(selector, 0);

			glBindVertexArray(DVAO);

			dashboardModel = glm::translate(glm::mat4(1.0f), glm::vec3(screenWidth * 0.5f, screenHeight * 0.8f, 0.0f));
			dashboardModel = glm::scale(dashboardModel, glm::vec3(screenWidth, screenHeight * 0.4f, 1.0f));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(dashboardModel));
			glDrawArrays(GL_TRIANGLES, 0, 6);

			pausedModel = glm::translate(glm::mat4(1.0f), glm::vec3(screenWidth * 0.5f, screenHeight * 0.5f, 0.0f));
			pausedModel = glm::scale(pausedModel, glm::vec3(screenWidth, screenHeight, 1.0f));
			if (inputManager.getPaused() && !inputManager.getStartCount()) {
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				texturePaused->bind(GL_TEXTURE0);

				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(pausedModel));
				glDrawArrays(GL_TRIANGLES, 0, 6);
				glDisable(GL_BLEND);
			}

			if (inputManager.getStartCount()) {
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

				if (inputManager.getTimer() < 2.0f) {
					textureLevel[inputManager.mapLoaded]->bind(GL_TEXTURE0);
				}
				else if (inputManager.getTimer() < 2.05f) {
					textureCount[3]->bind(GL_TEXTURE0);
					alSourcePlay(source[7]);
				}
				else if (inputManager.getTimer() < 3.0f) {
					textureCount[3]->bind(GL_TEXTURE0);
				}
				else if (inputManager.getTimer() < 4.0f) {
					textureCount[2]->bind(GL_TEXTURE0);
				}
				else if (inputManager.getTimer() < 5.0f) {
					textureCount[1]->bind(GL_TEXTURE0);
				}

				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(pausedModel));
				glDrawArrays(GL_TRIANGLES, 0, 6);

				glDisable(GL_BLEND);
			}
			aguja = glm::translate(glm::mat4(1.0f), glm::vec3(screenWidth * 0.35f, screenHeight * 0.8f, 1.0f));
			if (inputManager.getPaused())
				aguja = glm::rotate(aguja, glm::radians(-40.0f) + glm::radians(abs(260.0f * inputManager.getCarSpeedB())), glm::vec3(0.0f, 0.0f, 1.0f));
			else
				aguja = glm::rotate(aguja, glm::radians(-40.0f) + glm::radians(abs(260.0f * inputManager.getCarSpeed())), glm::vec3(0.0f, 0.0f, 1.0f));
			aguja = glm::scale(aguja, glm::vec3(screenWidth / 5.0f, screenHeight / 100.0f, 1.0f));

			textureNeedle->bind(GL_TEXTURE0);
			glUniform1i(selector, 0);
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(aguja));
			glDrawArrays(GL_TRIANGLES, 0, 6);

			agujaGas = glm::translate(glm::mat4(1.0f), glm::vec3(screenWidth * 0.66f, screenHeight * 0.9f, 1.0f));
			agujaGas = glm::rotate(agujaGas, glm::radians(35.0f) + glm::radians(abs(100.0f * inputManager.getGas())), glm::vec3(0.0f, 0.0f, 1.0f));
			agujaGas = glm::scale(agujaGas, glm::vec3(screenWidth / 5.0f, screenHeight / 100.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(agujaGas));
			glDrawArrays(GL_TRIANGLES, 0, 6);

			puntaje = glm::translate(glm::mat4(1.0f), glm::vec3(screenHeight * 0.12f, screenHeight * 0.05f, 0.0f));
			puntaje = glm::scale(puntaje, glm::vec3(screenHeight * 0.22f, screenHeight * 0.05f, 1.0f));
			texturePuntaje->bind(GL_TEXTURE0);
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(puntaje));
			glDrawArrays(GL_TRIANGLES, 0, 6);

			drawNumbers(score, modelLoc,
				glm::vec3(screenHeight * 0.2f, screenHeight * 0.05f, 0.0f),
				glm::vec3(screenHeight * 0.05f, screenHeight * 0.05f, 1.0f));

			drawLives(remainingLives, modelLoc);

			if (inputManager.getExploding()) {
				explosionModel = glm::translate(glm::mat4(1.0f), glm::vec3(screenWidth * 0.5f, screenHeight * 0.5f, 0.0f));
				explosionModel = glm::scale(explosionModel, glm::vec3(screenHeight * 0.5f, screenHeight * 0.5f, 1.0f));
				if (inputManager.getTimeExploding() < inputManager.totalExplosion) {
					glEnable(GL_BLEND);
					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					textureExplosion[(int)(inputManager.getTimeExploding() / (inputManager.totalExplosion / 40.0f))]->bind(GL_TEXTURE0);

					glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(explosionModel));
					glDrawArrays(GL_TRIANGLES, 0, 6);

					glDisable(GL_BLEND);
				}
				else {
					remainingLives--;
					inputManager.setGameState(4);
					if (remainingLives < 0) {
						for (int i = 0; i < 4; i++) {
							for (unsigned int j = 0; j < maps[i]->coinVector.size(); j++)
								if (!maps[i]->coinVector[j].notTaken)
									maps[i]->coinVector[j].notTaken = true;
						}
						score = 0;
						remainingLives = 2;
					}
					else
						inputManager.setGameState(0);

					enterMain = true;
					inputManager.setExploding(false);
					inputManager.setFallToDeath(false);

				}
			}
			else {
				for (unsigned int i = 0; i < m_obsVector.size(); i++) {
					if (testBoxBoxIntersection(aabb1, m_obsVector[i].m_aabb)) {
						inputManager.setExploding(true);
						inputManager.setExplodingBuffer(inputManager.getTimer());
					}
				}
				for (unsigned int i = 0; i < m_wallVector.size(); i++) {
					if (testBoxBoxIntersection(aabb1, m_wallVector[i].m_aabb)) {
						inputManager.setExploding(true);
						inputManager.setExplodingBuffer(inputManager.getTimer());
					}
				}

				if (inputManager.getGas() == 0 && inputManager.getCarSpeed() == 0) {
					inputManager.setExploding(true);
					inputManager.setExplodingBuffer(inputManager.getTimer());
				}
				for (unsigned int i = 0; i < m_fallsVector.size(); i++) {
					if (testBoxSphereIntersection(m_fallsVector[i], aabb1) && inputManager.getCarY() == 0.0f) {
						inputManager.setFallToDeath(true);
					}
				}
				if (inputManager.getCarY() < -10.0f) {
					inputManager.setExploding(true);
					inputManager.setExplodingBuffer(inputManager.getTimer());
				}
				if (inputManager.getExploding()) {
					playSource5 = true;
				}
			}

			glBindVertexArray(0);
			dashboardShader.turnOff();

			for (unsigned int i = 0; i < m_objVector.size(); i++) {
				if (testBoxBoxIntersection(aabb1, m_objVector[i].m_aabb) && !inputManager.getExploding()) {
					getting_gas = true;
					if (playSource4) {
						alSourcePlay(source[3]);
						playSource4 = false;
					}
				}
			}

			if (getting_gas) {
				inputManager.setGas(inputManager.changeVals(inputManager.getGas(), 0.015f, 1.0f, true));
			}
			else {
				alSourceStop(source[3]);
				playSource4 = true;
			}

			getting_gas = false;
			alSourcef(source[3], AL_PITCH, inputManager.getGas()*2.0f);

			if (testBoxBoxIntersection(maps[inputManager.mapLoaded]->getLevelEnding(), aabb1) && !inputManager.getExploding()) {
				if (inputManager.mapLoaded == 3) {
					inputManager.setGameState(5);
				}
				else {
					inputManager.setGameState(3);
					inputManager.mapLoaded++;
					inputManager.setTimer(0.0f);
					inputManager.setStutter(0.0f);
					enterMain = true;
					if (inputManager.mapLoaded > 3)
						inputManager.mapLoaded = 0;
				}
			}

			glfwSwapBuffers(window);

			source0Pos[0] = inputManager.getCameraPos().x;
			source0Pos[1] = inputManager.getCameraPos().y;
			source0Pos[2] = inputManager.getCameraPos().z;
			alSourcefv(source[0], AL_POSITION, source0Pos);
			alSourcefv(source[2], AL_POSITION, source0Pos);


			listenerPos[0] = inputManager.getCameraPos().x;
			listenerPos[1] = inputManager.getCameraPos().y;
			listenerPos[2] = inputManager.getCameraPos().z;
			alListenerfv(AL_POSITION, listenerPos);

			listenerOri[0] = inputManager.camera_look_at.x;
			listenerOri[1] = inputManager.camera_look_at.y;
			listenerOri[2] = inputManager.camera_look_at.z;
			listenerOri[3] = 0.0f;
			listenerOri[4] = 1.0f;
			listenerOri[5] = 0.0f;
			alListenerfv(AL_ORIENTATION, listenerOri);

			if (inputManager.getJumpSound()) {
				alSourcePlay(source[0]);
				inputManager.setJumpSound(false);
			}
			if (playSource2) {
				alSourcePlay(source[2]);
				playSource2 = false;
			}
			if (playSource5) {
				alSourcePlay(source[5]);
				playSource5 = false;
			}

			break;
		case 2:
			enterMain = true;
			inputManager.setFallToDeath(false);
			view = glm::mat4(1.0f);
			projection = glm::ortho(0.0f, (float)screenWidth, (float)screenHeight, 0.0f, -5.0f, 5.0f);

			dashboardShader.turnOn();
			// Get their uniform location
			modelLoc = dashboardShader.getUniformLocation("model");
			viewLoc = dashboardShader.getUniformLocation("view");
			projLoc = dashboardShader.getUniformLocation("projection");

			// Pass the matrices to the shader
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

			textureMenu->bind(GL_TEXTURE0);
			glUniform1i(dashboardShader.getUniformLocation("texture0"), 0);
			selector = dashboardShader.getUniformLocation("selector");
			glUniform1i(selector, 0);

			glBindVertexArray(DVAO);

			splashScreen = glm::translate(glm::mat4(1.0f), glm::vec3(screenWidth * 0.5f, screenHeight * 0.5f, 0.0f));
			splashScreen = glm::scale(splashScreen, glm::vec3(screenWidth, screenHeight, 1.0f));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(splashScreen));
			glDrawArrays(GL_TRIANGLES, 0, 6);

			glBindVertexArray(0);
			dashboardShader.turnOff();

			glfwSwapBuffers(window);

			break;
		case 3:

			view = glm::mat4(1.0f);
			projection = glm::ortho(0.0f, (float)screenWidth, (float)screenHeight, 0.0f, -5.0f, 5.0f);

			dashboardShader.turnOn();
			// Get their uniform location
			modelLoc = dashboardShader.getUniformLocation("model");
			viewLoc = dashboardShader.getUniformLocation("view");
			projLoc = dashboardShader.getUniformLocation("projection");

			// Pass the matrices to the shader
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

			textureSplash->bind(GL_TEXTURE0);
			glUniform1i(dashboardShader.getUniformLocation("texture0"), 0);
			selector = dashboardShader.getUniformLocation("selector");
			glUniform1i(selector, 0);

			glBindVertexArray(DVAO);

			splashScreen = glm::translate(glm::mat4(1.0f), glm::vec3(screenWidth * 0.5f, screenHeight * 0.5f, 0.0f));
			splashScreen = glm::scale(splashScreen, glm::vec3(screenWidth, screenHeight, 1.0f));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(splashScreen));
			glDrawArrays(GL_TRIANGLES, 0, 6);

			glBindVertexArray(0);
			dashboardShader.turnOff();

			glfwSwapBuffers(window);

			break;
		case 4:
			//remainingLives = 2;
			view = glm::mat4(1.0f);
			projection = glm::ortho(0.0f, (float)screenWidth, (float)screenHeight, 0.0f, -5.0f, 5.0f);

			inputManager.mapLoaded = 0;
			dashboardShader.turnOn();
			// Get their uniform location
			modelLoc = dashboardShader.getUniformLocation("model");
			viewLoc = dashboardShader.getUniformLocation("view");
			projLoc = dashboardShader.getUniformLocation("projection");

			// Pass the matrices to the shader
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

			glBindVertexArray(DVAO);
			textureSplash->bind(GL_TEXTURE0);
			glUniform1i(dashboardShader.getUniformLocation("texture0"), 0);

			splashScreen = glm::translate(glm::mat4(1.0f), glm::vec3(screenWidth * 0.5f, screenHeight * 0.5f, 0.0f));
			splashScreen = glm::scale(splashScreen, glm::vec3(screenWidth, screenHeight, 1.0f));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(splashScreen));
			glDrawArrays(GL_TRIANGLES, 0, 6);

			textureGameover->bind(GL_TEXTURE0);
			glUniform1i(dashboardShader.getUniformLocation("texture0"), 0);

			splashScreen = glm::translate(glm::mat4(1.0f), glm::vec3(screenWidth * 0.5f, screenHeight * 0.5f, 1.0f));
			splashScreen = glm::scale(splashScreen, glm::vec3(screenWidth * 0.8f, screenHeight * 0.2f, 1.0f));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(splashScreen));
			glDrawArrays(GL_TRIANGLES, 0, 6);

			glBindVertexArray(0);
			dashboardShader.turnOff();

			glfwSwapBuffers(window);

			break;
		case 5:

			alSourceStop(source[1]);

			view = glm::mat4(1.0f);
			projection = glm::ortho(0.0f, (float)screenWidth, (float)screenHeight, 0.0f, -100.0f, 100.0f);

			dashboardShader.turnOn();
			// Get their uniform location
			modelLoc = dashboardShader.getUniformLocation("model");
			viewLoc = dashboardShader.getUniformLocation("view");
			projLoc = dashboardShader.getUniformLocation("projection");

			// Pass the matrices to the shader
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));


			glBindVertexArray(DVAO);

			glUniform1i(dashboardShader.getUniformLocation("texture0"), 0);
			selector = dashboardShader.getUniformLocation("selector");
			glUniform1i(selector, 0);

			textureWinner->bind(GL_TEXTURE0);
			puntaje = glm::translate(glm::mat4(1.0f), glm::vec3(screenWidth * 0.5f, screenHeight * 0.5f, -99.0f));
			puntaje = glm::scale(puntaje, glm::vec3(screenWidth, screenHeight, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(puntaje));
			glDrawArrays(GL_TRIANGLES, 0, 6);

			textureMision->bind(GL_TEXTURE0);
			puntaje = glm::translate(glm::mat4(1.0f), glm::vec3(screenWidth * 0.5f, screenHeight * 0.2f, -98.0f));
			puntaje = glm::scale(puntaje, glm::vec3(screenWidth * 0.8f, screenHeight * 0.2f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(puntaje));
			glDrawArrays(GL_TRIANGLES, 0, 6);

			texturePuntaje->bind(GL_TEXTURE0);
			puntaje = glm::translate(glm::mat4(1.0f), glm::vec3(screenWidth * 0.4f, screenHeight * 0.8f, -98.0f));
			puntaje = glm::scale(puntaje, glm::vec3(screenHeight * 0.4f, screenHeight * 0.1f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(puntaje));
			glDrawArrays(GL_TRIANGLES, 0, 6);

			drawNumbers(score, modelLoc,
				glm::vec3(screenWidth * 0.5f, screenHeight * 0.8f, 0.0f),
				glm::vec3(screenHeight * 0.1f, screenHeight * 0.1f, 1.0f));

			glBindVertexArray(0);
			dashboardShader.turnOff();

			// Create transformations
			view = glm::mat4(1.0f);
			cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
			view = glm::translate(view, glm::vec3(-cameraPos.x, -cameraPos.y, -cameraPos.z));

			projection = glm::perspective(45.0f, (float)screenWidth / (float)screenHeight, 0.1f, 1000.0f);

			// --- PLAYER --- //
			lightingShader.turnOn();

			viewPosLoc = lightingShader.getUniformLocation("viewPos");
			glUniform3f(viewPosLoc, cameraPos.x, cameraPos.y,
				cameraPos.z);

			//// Set material properties
			matDiffuseLoc = lightingShader.getUniformLocation(
				"material.diffuse");
			matSpecularLoc = lightingShader.getUniformLocation(
				"material.specular");
			matShineLoc = lightingShader.getUniformLocation(
				"material.shininess");
			glUniform1i(matDiffuseLoc, 0);
			glUniform1i(matSpecularLoc, 1);
			glUniform1f(matShineLoc, 32.0f);

			//// Set lights properties
			lightAmbientLoc = lightingShader.getUniformLocation(
				"light.ambient");
			lightDiffuseLoc = lightingShader.getUniformLocation(
				"light.diffuse");
			lightSpecularLoc = lightingShader.getUniformLocation(
				"light.specular");
			lightPosLoc = lightingShader.getUniformLocation("light.position");
			glUniform3f(lightAmbientLoc, 0.6f, 0.6f, 0.6f);
			glUniform3f(lightDiffuseLoc, 0.8f, 0.7f, 0.6f); // Let's darken the light a bit to fit the scene
			glUniform3f(lightSpecularLoc, 1.0f, 1.0f, 1.0f);
			glUniform3f(lightPosLoc, 0.5f, 1.0f, 2.0f);

			//// Get the uniform locations
			modelLoc = lightingShader.getUniformLocation("model");
			viewLoc = lightingShader.getUniformLocation("view");
			projLoc = lightingShader.getUniformLocation("projection");

			//// Pass the matrices to the shader
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

			model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.5f, 0.0f));
			model = glm::rotate(model, glm::radians(20.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::rotate(model, glm::radians(cupAngle), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.1, 0.1, 0.1));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			glUniform1i(matDiffuseLoc, 0);
			glUniform1i(matSpecularLoc, 1);
			glUniform1f(matShineLoc, 32.0f);

			inputManager.camera_look_at = glm::vec4(glm::rotate(model, glm::radians(inputManager.roty), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
			copa.render(&lightingShader);
			lightingShader.turnOff();

			glfwSwapBuffers(window);

			break;
		case 6:

			alSourceStop(source[1]);

			view = glm::mat4(1.0f);
			projection = glm::ortho(0.0f, (float)screenWidth, (float)screenHeight, 0.0f, -100.0f, 100.0f);

			dashboardShader.turnOn();
			// Get their uniform location
			modelLoc = dashboardShader.getUniformLocation("model");
			viewLoc = dashboardShader.getUniformLocation("view");
			projLoc = dashboardShader.getUniformLocation("projection");

			// Pass the matrices to the shader
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));


			glBindVertexArray(DVAO);

			glUniform1i(dashboardShader.getUniformLocation("texture0"), 0);
			selector = dashboardShader.getUniformLocation("selector");
			glUniform1i(selector, 0);


			textureWinner->bind(GL_TEXTURE0);
			puntaje = glm::translate(glm::mat4(1.0f), glm::vec3(screenWidth * 0.5f, screenHeight * 0.5f, -99.0f));
			puntaje = glm::scale(puntaje, glm::vec3(screenWidth, screenHeight, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(puntaje));
			glDrawArrays(GL_TRIANGLES, 0, 6);

			creditRoll = inputManager.changeVals(creditRoll, 5.0f, screenHeight * 0.5f, false);
			textureCredits->bind(GL_TEXTURE0);
			puntaje = glm::translate(glm::mat4(1.0f), glm::vec3(screenWidth * 0.5f, creditRoll, -98.0f));
			puntaje = glm::scale(puntaje, glm::vec3(screenWidth, screenHeight, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(puntaje));
			glDrawArrays(GL_TRIANGLES, 0, 6);

			glBindVertexArray(0);
			dashboardShader.turnOff();

			glfwSwapBuffers(window);

			break;
		}
	}
}

int main(int argc, char ** argv) {
	init(1200, 800, "Skyways", false);
	applicationLoop();
	destroy();
	return 1;
}

