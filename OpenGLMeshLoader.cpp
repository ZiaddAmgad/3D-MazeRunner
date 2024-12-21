#include "TextureBuilder.h"
#include "Model_3DS.h"
#include "GLTexture.h"
#include <glut.h>
#include <time.h>
#include <cstdio>
#include <cmath>
#include <chrono>
#include <cstdlib>  // For rand()
#include <ctime>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <thread>
#include <sstream>
#include <cstdio>
#include <cmath>
#include <cstdlib>  // For rand()
#include <al.h>
#include <alc.h>



// Function to initialize OpenAL
ALCdevice* device;
ALCcontext* context;
ALuint buffer, source;
ALuint bufferBackground, bufferYouDied, bufferYouWin, bufferCollision, bufferTreadmill, bufferSmith, bufferBenchPress, bufferDumbbellRack, bufferChinUp, bufferDeadlift, bufferTeleport, bufferPortal;
ALuint sourceBackground, sourceYouDied, sourceYouWin, sourceCollision, sourceTreadmill, sourceSmith, sourceBenchPress, sourceDumbbellRack, sourceChinUp, sourceDeadlift, sourceTeleport, sourcePortal;

void initOpenAL() {
	device = alcOpenDevice(NULL); // Open default device
	std::cerr << device << std::endl;
	if (!device) {
		std::cerr << "Failed to open audio device." << std::endl;
		return;
	}

	context = alcCreateContext(device, NULL);
	if (!alcMakeContextCurrent(context)) {
		std::cerr << "Failed to set audio context." << std::endl;
		return;
	}

	// Set up listener properties
	ALfloat listenerPos[] = { 0.0f, 0.0f, 0.0f }; // Listener position
	ALfloat listenerVel[] = { 0.0f, 0.0f, 0.0f }; // Listener velocity
	ALfloat listenerOri[] = { 0.0f, 0.0f, -1.0f,  // Orientation: looking down -Z axis
							  0.0f, 1.0f, 0.0f }; // Up vector: +Y axis

	alListenerfv(AL_POSITION, listenerPos);
	alListenerfv(AL_VELOCITY, listenerVel);
	alListenerfv(AL_ORIENTATION, listenerOri);

	// Generate buffer and source
	// Generate buffer and source for background music
	alGenBuffers(1, &bufferBackground);
	alGenSources(1, &sourceBackground);

	// Generate buffer and source for "You Died" sound
	alGenBuffers(1, &bufferYouDied);
	alGenSources(1, &sourceYouDied);

	// Generate buffer and source for "You Win" sound
	alGenBuffers(1, &bufferYouWin);
	alGenSources(1, &sourceYouWin);

	// Generate buffer and source for obstacle collision sound
	alGenBuffers(1, &bufferCollision);
	alGenSources(1, &sourceCollision);

	alGenBuffers(1, &bufferTreadmill);
	alGenSources(1, &sourceTreadmill);

	alGenBuffers(1, &bufferSmith);
	alGenSources(1, &sourceSmith);

	alGenBuffers(1, &bufferBenchPress);
	alGenSources(1, &sourceBenchPress);

	alGenBuffers(1, &bufferDumbbellRack);
	alGenSources(1, &sourceDumbbellRack);

	alGenBuffers(1, &bufferChinUp);
	alGenSources(1, &sourceChinUp);

	alGenBuffers(1, &bufferDeadlift);
	alGenSources(1, &sourceDeadlift);

	alGenBuffers(1, &bufferTeleport);
	alGenSources(1, &sourceTeleport);

	alGenBuffers(1, &bufferPortal);
	alGenSources(1, &sourcePortal);

	alSourcef(source, AL_GAIN, 1.0f); // Set gain to normal volume

	ALenum error = alGetError();
	if (error != AL_NO_ERROR) {
		std::cerr << "OpenAL Error: " << error << std::endl;
	}
}


// Function to load a .WAV file into OpenAL
struct WAVHeader {
	char riff[4];                // "RIFF"
	int chunkSize;                // File size minus 8 bytes
	char wave[4];                 // "WAVE"
	char fmt[4];                  // "fmt "
	int subChunk1Size;            // Size of the fmt chunk (16 for PCM)
	short audioFormat;            // Audio format (1 for PCM)
	short numChannels;            // Number of channels
	int sampleRate;               // Sample rate (e.g., 44100 Hz)
	int byteRate;                 // Byte rate (SampleRate * NumChannels * BitsPerSample / 8)
	short blockAlign;             // Block alignment
	short bitsPerSample;          // Bits per sample (e.g., 16)
	char data[4];                 // "data"
	int dataSize;                 // Size of the data chunk
};

// Function to load the WAV file and upload to OpenAL
void loadWAVFile(const char* filename, ALuint& buffer) {
	// Open the WAV file
	std::ifstream file(filename, std::ios::binary);
	if (!file) {
		std::cerr << "Failed to open WAV file: " << filename << std::endl;
		return;
	}

	// Read the WAV header
	WAVHeader header;
	file.read(reinterpret_cast<char*>(&header), sizeof(header));

	// Check if the file is in valid WAV format
	if (header.riff[0] != 'R' || header.riff[1] != 'I' || header.riff[2] != 'F' || header.riff[3] != 'F') {
		std::cerr << "Invalid WAV file format for: " << filename << std::endl;
		file.close();
		return;
	}

	// Read audio data
	char* data = new char[header.dataSize];
	file.read(data, header.dataSize);
	file.close();

	// Determine the audio format (Mono or Stereo)
	ALenum format;
	if (header.numChannels == 1) {
		format = (header.bitsPerSample == 16) ? AL_FORMAT_MONO16 : AL_FORMAT_MONO8;
	}
	else if (header.numChannels == 2) {
		format = (header.bitsPerSample == 16) ? AL_FORMAT_STEREO16 : AL_FORMAT_STEREO8;
	}
	else {
		std::cerr << "Unsupported WAV format for: " << filename << std::endl;
		delete[] data;
		return;
	}

	// Generate the buffer if not already created
	if (buffer == 0) {
		alGenBuffers(1, &buffer);
	}

	// Load the audio data into the OpenAL buffer
	alBufferData(buffer, format, data, header.dataSize, header.sampleRate);
	delete[] data; // Free audio data
}


void loadSoundInBackground() {
	loadWAVFile("Super Mario Win.wav", bufferBackground);
	loadWAVFile("Dark Souls.wav", bufferYouDied);
	loadWAVFile("Super Mario Win.wav", bufferYouWin);
	loadWAVFile("Collision.wav", bufferCollision);

	loadWAVFile("Crystal.wav", bufferTreadmill);
	loadWAVFile("Mushroom.wav", bufferSmith);
	loadWAVFile("Data.wav", bufferBenchPress);
	loadWAVFile("Portal.wav", bufferDumbbellRack);
	loadWAVFile("Teleport.wav", bufferChinUp);
	loadWAVFile("Wall-E.wav", bufferDeadlift);
	loadWAVFile("Portal.wav", bufferPortal);
	loadWAVFile("Teleport.wav", bufferTeleport);
}
// Function to play the "YOU DIED" sound
// Play the background music
void playBackgroundMusic() {
	alSourcei(sourceBackground, AL_BUFFER, bufferBackground);
	//alSourcei(sourceBackground, AL_LOOPING, AL_TRUE); // Loop background music
	alSourcePlay(sourceBackground);
}

// Stop the background music
void stopBackgroundMusic() {
	alSourceStop(sourceBackground);
}

// Play the "You Died" sound
void playYouDiedSound() {
	alSourcei(sourceYouDied, AL_BUFFER, bufferYouDied);
	alSourcePlay(sourceYouDied);
}

// Play the "You Win" sound
void playYouWinSound() {
	alSourcei(sourceYouWin, AL_BUFFER, bufferYouWin);
	alSourcePlay(sourceYouWin);
}

// Play the obstacle collision sound
void playCollisionSound() {
	alSourcei(sourceCollision, AL_BUFFER, bufferCollision);
	alSourcePlay(sourceCollision);
}
void playCrystalSound() {
	alSourcei(sourceTreadmill, AL_BUFFER, bufferTreadmill);
	alSourcePlay(sourceTreadmill);
}

void playMushroomSound() {
	alSourcei(sourceSmith, AL_BUFFER, bufferSmith);
	alSourcePlay(sourceSmith);
}

void playDataSound() {
	alSourcei(sourceBenchPress, AL_BUFFER, bufferBenchPress);
	alSourcePlay(sourceBenchPress);
}

void playOpenPortalSound() {
	alSourcei(sourcePortal, AL_BUFFER, bufferPortal);
	alSourcePlay(sourcePortal);
}

void playTeleportSound() {
	alSourcei(sourceTeleport, AL_BUFFER, bufferTeleport);
	alSourcePlay(sourceTeleport);
}

void playRobotSound() {
	alSourcei(sourceDeadlift, AL_BUFFER, bufferDeadlift);
	alSourcePlay(sourceDeadlift);
}

// Cleanup OpenAL
void cleanupOpenAL() {
	alDeleteSources(1, &sourceBackground);
	alDeleteSources(1, &sourceYouDied);
	alDeleteSources(1, &sourceYouWin);
	alDeleteSources(1, &sourceCollision);

	alDeleteSources(1, &sourceTreadmill);
	alDeleteSources(1, &sourceSmith);
	alDeleteSources(1, &sourceBenchPress);
	alDeleteSources(1, &sourceDumbbellRack);
	alDeleteSources(1, &sourceChinUp);
	alDeleteSources(1, &sourceDeadlift);

	alDeleteBuffers(1, &bufferBackground);
	alDeleteBuffers(1, &bufferYouDied);
	alDeleteBuffers(1, &bufferYouWin);
	alDeleteBuffers(1, &bufferCollision);

	alDeleteBuffers(1, &bufferTreadmill);
	alDeleteBuffers(1, &bufferSmith);
	alDeleteBuffers(1, &bufferBenchPress);
	alDeleteBuffers(1, &bufferDumbbellRack);
	alDeleteBuffers(1, &bufferChinUp);
	alDeleteBuffers(1, &bufferDeadlift);

	alcMakeContextCurrent(NULL);
	alcDestroyContext(context);
	alcCloseDevice(device);
}

int WIDTH = 1280;
int HEIGHT = 720;

GLuint tex;
char title[] = "3D Model Loader Sample";

// 3D Projection Options
GLdouble fovy = 45.0;
GLdouble aspectRatio = (GLdouble)WIDTH / (GLdouble)HEIGHT;
GLdouble zNear = 0.1;
GLdouble zFar = 300;

class Vector
{
public:
	GLdouble x, y, z;
	Vector() {}
	Vector(GLdouble _x, GLdouble _y, GLdouble _z) : x(_x), y(_y), z(_z) {}
	//================================================================================================//
	// Operator Overloading; In C++ you can override the behavior of operators for you class objects. //
	// Here we are overloading the += operator to add a given value to all vector coordinates.        //
	//================================================================================================//
	void operator +=(float value)
	{
		x += value;
		y += value;
		z += value;
	}
};

Vector Eye(24.5, 5.5, 24.5);
Vector At(0, 0, 0);
Vector Up(0, 1, 0);

int cameraZoom = 0;

// Model Variables
Model_3DS model_house;
Model_3DS model_tree;
Model_3DS model_sun;
Model_3DS model_mushroom;
Model_3DS model_rock;
Model_3DS model_log;
Model_3DS model_crystal;
Model_3DS model_statue;
Model_3DS model_corridor;
Model_3DS model_UFO;
Model_3DS model_fan;
Model_3DS model_data;
Model_3DS model_robot;
// Textures
GLTexture tex_ground;
GLTexture tex_wall;
GLuint tex_teleport;
GLuint tex_sun_morning;
GLuint tex_sun_noon;
GLuint tex_sun_evening;
GLuint tex_moon;

GLuint tex_mushroom;
GLuint tex_rock;
GLuint tex_log;
GLuint tex_crystal;
GLuint tex_statue;

struct Player {
	Model_3DS torso;
	Model_3DS leftLeg;
	Model_3DS rightLeg;
	Model_3DS leftArm;
	Model_3DS rightArm;
};
Player player;
//=======================================================================
// Lighting Configuration Function
//=======================================================================
void InitLightSource() {
	glEnable(GL_LIGHTING); // Enable lighting

	// Enable light source 0
	glEnable(GL_LIGHT0);

	// Set light position (adjust as needed)
	GLfloat lightPosition[] = { 50.0f, 100.0f, -50.0f, 1.0f }; // Sun position
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

	// WHITE LIGHT: Set ambient, diffuse, and specular to white
	GLfloat ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f }; // Soft ambient light
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

	GLfloat diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f }; // Main diffuse light (white)
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);

	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f }; // Specular highlight (white)
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);


	glEnable(GL_LIGHT0); // Turn on light 0
}

//=======================================================================
// Material Configuration Function
//======================================================================

void InitMaterial()
{
	// Enable Material Tracking
	glEnable(GL_COLOR_MATERIAL);

	// Sich will be assigneet Material Properties whd by glColor
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// Set Material's Specular Color
	// Will be applied to all objects
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);

	// Set Material's Shine value (0->128)
	GLfloat shininess[] = { 96.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
}

//=======================================================================
// OpengGL Configuration Function
//=======================================================================
void myInit(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);

	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();

	gluPerspective(fovy, aspectRatio, zNear, zFar);
	//*******************************************************************************************//
	// fovy:			Angle between the bottom and top of the projectors, in degrees.			 //
	// aspectRatio:		Ratio of width to height of the clipping plane.							 //
	// zNear and zFar:	Specify the front and back clipping planes distances from camera.		 //
	//*******************************************************************************************//

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);
	//*******************************************************************************************//
	// EYE (ex, ey, ez): defines the location of the camera.									 //
	// AT (ax, ay, az):	 denotes the direction where the camera is aiming at.					 //
	// UP (ux, uy, uz):  denotes the upward orientation of the camera.							 //
	//*******************************************************************************************//

	InitLightSource();

	InitMaterial();

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_NORMALIZE);
}

//=======================================================================
// Render Ground Function
//=======================================================================

void RenderGround() {
	glDisable(GL_LIGHTING); // Disable lighting

	glColor3f(0.6, 0.6, 0.6); // Dim the ground texture a bit

	glEnable(GL_TEXTURE_2D); // Enable 2D texturing

	glBindTexture(GL_TEXTURE_2D, tex_ground.texture[0]); // Bind the ground texture

	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0); // Set quad normal direction.
	// Adjusted for a larger ground (from -100 to 100 for both axes)
	glTexCoord2f(0, 0);
	glVertex3f(-100, 0, -100);
	glTexCoord2f(10, 0);
	glVertex3f(100, 0, -100);
	glTexCoord2f(10, 10);
	glVertex3f(100, 0, 100);
	glTexCoord2f(0, 10);
	glVertex3f(-100, 0, 100);
	glEnd();
	glPopMatrix();

	glEnable(GL_LIGHTING); // Enable lighting again for other entities

	glColor3f(1, 1, 1); // Reset material to white
}

struct Hitbox {
	Vector min;    // Minimum corner of the bounding box
	Vector max;    // Maximum corner of the bounding box
	Vector center; // Center of the bounding box
};

// Global hitbox containers
std::vector<Hitbox> treeHitboxes;
std::vector<Hitbox> mushroomHitboxes;
std::vector<Hitbox> crystalHitboxes;
std::vector<Hitbox> rockHitboxes;
std::vector<Hitbox> logHitboxes;
std::vector<Hitbox> statueHitboxes;
std::vector<Hitbox> fanHitboxes;
std::vector<Hitbox> dataHitboxes;
Hitbox playerHitbox;
float stateTimer = 0.0f;
int mushroomsCollected = 0;
int crystalsCollected = 0;
int dataCollected = 0;
int gameTime = 500;
int gameScore = 0;
enum GameState { MAIN_SCENE, TELEPORT_SCENE, WIN_SCENE, LOSE_SCENE };
GameState currentGameState = MAIN_SCENE;
float deltaTime = 0.01f;









void RenderBoundaryWalls() {
	glDisable(GL_LIGHTING);
	glColor3f(0.5, 0.5, 0.5); // Gray wall color
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex_ground.texture[0]); // Assuming you have a wall texture

	// Wall thickness
	float wallHeight = 5.0f;
	float wallThickness = 1.0f;

	// North wall
	glPushMatrix();
	glTranslatef(0, wallHeight / 2, 100);
	glScalef(200, wallHeight, wallThickness);
	glutSolidCube(1.0);
	glPopMatrix();

	// South wall
	glPushMatrix();
	glTranslatef(0, wallHeight / 2, -100);
	glScalef(200, wallHeight, wallThickness);
	glutSolidCube(1.0);
	glPopMatrix();

	// East wall
	glPushMatrix();
	glTranslatef(100, wallHeight / 2, 0);
	glRotatef(90, 0, 1, 0);
	glScalef(200, wallHeight, wallThickness);
	glutSolidCube(1.0);
	glPopMatrix();

	// West wall
	glPushMatrix();
	glTranslatef(-100, wallHeight / 2, 0);
	glRotatef(90, 0, 1, 0);
	glScalef(200, wallHeight, wallThickness);
	glutSolidCube(1.0);
	glPopMatrix();

	glEnable(GL_LIGHTING);
	glColor3f(1, 1, 1);
}



// Function to create a hitbox for a tree
Hitbox CreateTreeHitbox(const Vector& position) {
	Hitbox box;
	// Assuming tree model has approximate dimensions
	float halfWidth = 3.0f;   // Half of the tree's width
	float halfHeight = 10.0f; // Half of the tree's height

	box.center = position;
	box.min = Vector(
		position.x - halfWidth,
		position.y,
		position.z - halfWidth
	);
	box.max = Vector(
		position.x + halfWidth,
		position.y + 2 * halfHeight,
		position.z + halfWidth
	);

	return box;
}

// Function to create a hitbox for the player
Hitbox CreatePlayerHitbox(const Vector& position) {
	Hitbox box;
	float halfWidth = 0.3f;
	float halfHeight = 0.5f;

	// Use the base of the player for hitbox positioning
	Vector basePosition = position;
	basePosition.y = position.y - halfHeight;  // Adjust to player's feet level

	box.center = basePosition;
	box.min = Vector(
		basePosition.x - halfWidth,
		basePosition.y,
		basePosition.z - halfWidth
	);
	box.max = Vector(
		basePosition.x + halfWidth,
		basePosition.y + 2 * halfHeight,
		basePosition.z + halfWidth
	);
	return box;
}

// Create hitbox for a mushroom
Hitbox CreateMushroomHitbox(const Vector& position) {
	Hitbox box;
	float halfWidth = 0.7f;  // Half of the mushroom's width
	float height = 10.0f;     // Full height of the mushroom

	box.center = position;
	box.min = Vector(position.x - halfWidth, position.y, position.z - halfWidth);
	box.max = Vector(position.x + halfWidth, position.y + height, position.z + halfWidth);

	return box;
}

// Create hitbox for a rock
Hitbox CreateRockHitbox(const Vector& position) {
	Hitbox box;
	float halfWidth = 4.0f;  // Half of the rock's width
	float height = 13.0f;     // Full height of the rock

	box.center = position;
	box.min = Vector(position.x - halfWidth, position.y, position.z - halfWidth);
	box.max = Vector(position.x + halfWidth, position.y + height, position.z + halfWidth);

	return box;
}

// Create hitbox for a log
Hitbox CreateLogHitbox(const Vector& position) {
	Hitbox box;
	float halfWidth = 8.0f;  // Half of the log's width
	float length = 5.0f;     // Full length of the log

	Vector adposition = Vector(position.x + 2.0f, position.y, position.z - 1.0f);

	box.center = adposition;
	box.min = Vector(adposition.x - halfWidth, adposition.y, adposition.z - length / 2);
	box.max = Vector(adposition.x + halfWidth, adposition.y + 10.0f, adposition.z + length / 2);
	return box;
}

// Create hitbox for a crystal
Hitbox CreateCrystalHitbox(const Vector& position) {
	Hitbox box;
	float halfWidth = 1.0f; // Half of the crystal's width
	float height = 12.0f;     // Full height of the crystal

	box.center = position;
	box.min = Vector(position.x - halfWidth, position.y, position.z - halfWidth);
	box.max = Vector(position.x + halfWidth, position.y + height, position.z + halfWidth);

	return box;
}

// Create hitbox for a statue
Hitbox CreateStatueHitbox(const Vector& position) {
	Hitbox box;

	float halfWidth = 20.0f;  // Half of the statue's width
	float height = 40.0f;     // Full height of the statue
	float offsetX = 5.0f;     // Move right (increase x)
	float offsetZ = -10.0f;    // Move back (increase z)

	// Apply offsets to the position
	Vector adjustedPosition = Vector(position.x + offsetX, position.y, position.z + offsetZ);

	box.center = adjustedPosition;
	box.min = Vector(adjustedPosition.x - halfWidth, adjustedPosition.y, adjustedPosition.z - halfWidth);
	box.max = Vector(adjustedPosition.x + halfWidth, adjustedPosition.y + height, adjustedPosition.z + halfWidth);

	return box;
}

float fanPositions[5][2] = {
		{0, 0},
		{90, 250},
		{30, 500},
		{70, 750},
		{120, 1000}
};
std::vector<Hitbox> CreateFanHitboxes() {
	

	
	for (int i = 0; i < 5; i++) {
		Hitbox box;
		box.center = Vector(fanPositions[i][0], 50, fanPositions[i][1]);
		box.min = Vector(box.center.x - 50.0f, box.center.y - 100.0f, box.center.z + 10.0f);
		box.max = Vector(box.center.x + 50.0f, box.center.y + 100.0f, box.center.z + 30.0f);
		fanHitboxes.push_back(box);
	}
	return fanHitboxes;
}
std::vector <Vector> dataPositions = {
	{60, 0,100},
	{0, 0,350},
	{90, 0,600},
	{120, 0,850},
	{30, 0,1250}
};
void CreateDataHitboxes() {
	// Clear any existing hitboxes to avoid duplication
	dataHitboxes.clear();

	// Create hitboxes for each data position
	for (const auto& position : dataPositions) {
		Hitbox box;
		box.center = Vector(position.x, -10, position.z);
		box.min = Vector(box.center.x - 10.0f, box.center.y - 100.0f, box.center.z - 10.0f);
		box.max = Vector(box.center.x + 10.0f, box.center.y + 100.0f, box.center.z + 10.0f);
		dataHitboxes.push_back(box);
	}
}














// Function to check if two hitboxes intersect
bool CheckHitboxIntersection(const Hitbox& box1, const Hitbox& box2) {
	return (box1.min.x <= box2.max.x && box1.max.x >= box2.min.x) &&
		(box1.min.y <= box2.max.y && box1.max.y >= box2.min.y) &&
		(box1.min.z <= box2.max.z && box1.max.z >= box2.min.z);
}


// Render hitbox for debugging (optional)
void RenderHitbox(const Hitbox& box) {
	glDisable(GL_LIGHTING);
	glColor3f(1.0f, 0.0f, 0.0f);  // Red color for hitbox
	glLineWidth(2.0f);

	glBegin(GL_LINES);
	// Bottom face
	glVertex3f(box.min.x, box.min.y, box.min.z);
	glVertex3f(box.max.x, box.min.y, box.min.z);
	glVertex3f(box.max.x, box.min.y, box.min.z);
	glVertex3f(box.max.x, box.min.y, box.max.z);
	glVertex3f(box.max.x, box.min.y, box.max.z);
	glVertex3f(box.min.x, box.min.y, box.max.z);
	glVertex3f(box.min.x, box.min.y, box.max.z);
	glVertex3f(box.min.x, box.min.y, box.min.z);

	// Top face
	glVertex3f(box.min.x, box.max.y, box.min.z);
	glVertex3f(box.max.x, box.max.y, box.min.z);
	glVertex3f(box.max.x, box.max.y, box.min.z);
	glVertex3f(box.max.x, box.max.y, box.max.z);
	glVertex3f(box.max.x, box.max.y, box.max.z);
	glVertex3f(box.min.x, box.max.y, box.max.z);
	glVertex3f(box.min.x, box.max.y, box.max.z);
	glVertex3f(box.min.x, box.max.y, box.min.z);

	// Vertical edges
	glVertex3f(box.min.x, box.min.y, box.min.z);
	glVertex3f(box.min.x, box.max.y, box.min.z);
	glVertex3f(box.max.x, box.min.y, box.min.z);
	glVertex3f(box.max.x, box.max.y, box.min.z);
	glVertex3f(box.max.x, box.min.y, box.max.z);
	glVertex3f(box.max.x, box.max.y, box.max.z);
	glVertex3f(box.min.x, box.min.y, box.max.z);
	glVertex3f(box.min.x, box.max.y, box.max.z);
	glEnd();

	glEnable(GL_LIGHTING);
	glColor3f(1.0f, 1.0f, 1.0f); // Reset color to white
}



const float WALL_BOUNDARY_DISTANCE = 6.0f;
const float MAP_SIZE = 200.0f;

// Helper function to check if a position is valid
bool IsValidPosition(const Vector& newPos, const std::vector<Vector>& existingPositions, float minDistance = 10.0f) {
	// Check boundary walls
	if (std::abs(newPos.x) > MAP_SIZE / 2 - WALL_BOUNDARY_DISTANCE ||
		std::abs(newPos.z) > MAP_SIZE / 2 - WALL_BOUNDARY_DISTANCE) {
		return false;
	}

	// Check overlap with existing positions
	for (const auto& existingPos : existingPositions) {
		float distance = sqrt(
			pow(newPos.x - existingPos.x, 2) +
			pow(newPos.z - existingPos.z, 2)
		);
		if (distance < minDistance) {
			return false;
		}
	}
	return true;
}



std::vector<Vector> treePositions;
void RenderTrees() {
	// Generate positions only once
	if (treePositions.empty()) {
		for (int i = 0; i < 20; i++) {
			Vector newPos;
			int attempts = 0;
			do {
				newPos = Vector(
					(rand() % static_cast<int>(MAP_SIZE)) - MAP_SIZE / 2,
					0,
					(rand() % static_cast<int>(MAP_SIZE)) - MAP_SIZE / 2
				);
				attempts++;

				// Prevent infinite loop
				if (attempts > 100) {
					break;
				}
			} while (!IsValidPosition(newPos, treePositions));

			if (attempts <= 100) {
				treeHitboxes.push_back(CreateTreeHitbox(newPos));
				treePositions.push_back(newPos);
			}
		}
	}

	// Render trees using stored positions
	for (size_t i = 0; i < treePositions.size(); ++i) {
		const auto& pos = treePositions[i];
		glPushMatrix();
		glTranslatef(pos.x, pos.y, pos.z);  // Position the tree
		glScalef(10, 10, 10);           // Scale up for large size
		model_tree.Draw();                 // Render the tree
		glPopMatrix();

		// Render hitbox for this tree
	}
}

std::vector<Vector> activeMushroomPickups;
std::vector<Vector> mushroomPositions;
float mushroomPulseTime = 0.0f;
void RenderMushrooms() {
	mushroomPulseTime += deltaTime;
	if (mushroomPositions.empty()) {
		for (int i = 0; i < 5; i++) {
			Vector newPos;
			int attempts = 0;
			do {
				newPos = Vector(
					(rand() % static_cast<int>(MAP_SIZE)) - MAP_SIZE / 2,
					0.5,
					(rand() % static_cast<int>(MAP_SIZE)) - MAP_SIZE / 2
				);
				attempts++;

				// Prevent infinite loop and combine with trees and other objects
				

				if (attempts > 100) {
					break;
				}
			} while (!IsValidPosition(newPos, mushroomPositions));

			if (attempts <= 100) {
				mushroomHitboxes.push_back(CreateMushroomHitbox(newPos));
				mushroomPositions.push_back(newPos);
				
			}
		}
	}

	// Render mushrooms using stored positions
	for (size_t i = 0; i < activeMushroomPickups.size(); ++i) {
		auto& pos = activeMushroomPickups[i];

		// Calculate upward translation and rotation
		float translation = stateTimer * 2.0f; // Move up
		float rotationAngle = stateTimer * 90.0f; // Rotate

		glPushMatrix();
		glTranslatef(pos.x, pos.y + translation, pos.z); // Translate up
		glRotatef(rotationAngle, 0, 1, 0);               // Rotate
		glScalef(0.001, 0.001, 0.001);                  // Scale for size
		model_mushroom.Draw();
		glPopMatrix();

		// Remove mushroom from active list after animation
		if (stateTimer >= 3.0f) {
			activeMushroomPickups.erase(activeMushroomPickups.begin() + i);
			--i; // Adjust index after removal
		}
	}
	float pulseScale = 1.0f + 0.2f * sin(mushroomPulseTime * 4.0f);
	// Render remaining static mushrooms
	for (size_t i = 0; i < mushroomPositions.size(); ++i) {
		const auto& pos = mushroomPositions[i];
		glPushMatrix();
		
		glTranslatef(pos.x, pos.y, pos.z);
		glScalef(pulseScale, pulseScale, pulseScale);
		glScalef(0.001, 0.001, 0.001);
		model_mushroom.Draw();
		glPopMatrix();

		// Optional: Render hitbox for debugging
	}
}


std::vector<Vector> rocksPositions;
void RenderRocks() {
	if (rocksPositions.empty()) {
		for (int i = 0; i < 5; i++) {
			Vector newPos;
			int attempts = 0;
			do {
				newPos = Vector(
					(rand() % static_cast<int>(MAP_SIZE)) - MAP_SIZE / 2,
					0.5, 
					(rand() % static_cast<int>(MAP_SIZE)) - MAP_SIZE / 2
				);
				attempts++;
				if (attempts > 100) break;
			} while (!IsValidPosition(newPos, rocksPositions));

			if (attempts <= 100) {
				rockHitboxes.push_back(CreateRockHitbox(newPos)); // Generate hitbox
				rocksPositions.push_back(newPos);
			}
		}
	}

	for (size_t i = 0; i < rocksPositions.size(); ++i) {
		const auto& pos = rocksPositions[i];
		glPushMatrix();
		glTranslatef(pos.x, pos.y, pos.z);
		glScalef(2, 2, 2);
		model_rock.Draw();
		glPopMatrix();

		// Render hitbox for debugging
	}
}

std::vector<Vector> logsPositions;
void RenderLogs() {
	if (logsPositions.empty()) {
		for (int i = 0; i < 5; i++) {
			Vector newPos;
			int attempts = 0;
			do {
				newPos = Vector(
					(rand() % static_cast<int>(MAP_SIZE)) - MAP_SIZE / 2,
					0.5,
					(rand() % static_cast<int>(MAP_SIZE)) - MAP_SIZE / 2
				);
				attempts++;
				if (attempts > 100) 
					break;
			} while (!IsValidPosition(newPos, logsPositions));

			if (attempts <= 100) {
				logHitboxes.push_back(CreateLogHitbox(newPos)); // Generate hitbox
				logsPositions.push_back(newPos);
				
			}
		}
	}

	for (size_t i = 0; i < logsPositions.size(); ++i) {
		const auto& pos = logsPositions[i];
		glPushMatrix();
		glTranslatef(pos.x, pos.y, pos.z);
		glScalef(2, 2, 2);
		glRotatef(-45, 0, 1, 0);
		model_log.Draw();
		glPopMatrix();

		// Render hitbox for debugging
	}
}

std::vector<Vector> activeCrystalPickups;
std::vector<Vector> crystalsPositions;
float crystalPulseTime = 0.0f;
void RenderCrystals() {
	// Generate positions only once
	crystalPulseTime += deltaTime;
	if (crystalsPositions.empty()) {
		for (int i = 0; i < 5; i++) {
			Vector newPos;
			int attempts = 0;
			do {
				newPos = Vector(
					(rand() % static_cast<int>(MAP_SIZE)) - MAP_SIZE / 2,
					0.9,
					(rand() % static_cast<int>(MAP_SIZE)) - MAP_SIZE / 2
				);
				attempts++;
				if (attempts > 100) break;
			} while (!IsValidPosition(newPos, crystalsPositions));

			if (attempts <= 100) {
				crystalHitboxes.push_back(CreateCrystalHitbox(newPos)); // Generate hitbox
				crystalsPositions.push_back(newPos);
				
			}
		}
	}

	// Render crystals and hitboxes
	for (size_t i = 0; i < activeCrystalPickups.size(); ++i) {
		auto& pos = activeCrystalPickups[i];

		// Calculate upward translation and rotation
		float translation = stateTimer * 2.0f; // Move up
		float rotationAngle = stateTimer * 90.0f; // Rotate

		glPushMatrix();
		glTranslatef(pos.x, pos.y + translation, pos.z); // Translate up
		glRotatef(rotationAngle, 0, 1, 0);               // Rotate
		glScalef(0.25, 0.25, 0.25);                     // Scale for size
		model_crystal.Draw();
		glPopMatrix();

		// Remove crystal from active list after animation
		if (stateTimer >= 3.0f) {
			activeCrystalPickups.erase(activeCrystalPickups.begin() + i);
			--i; // Adjust index after removal
		}
	}
	float pulseScale = 1.0f + 0.2f * sin(crystalPulseTime * 4.0f);
	// Render remaining static crystals
	for (size_t i = 0; i < crystalsPositions.size(); ++i) {
		const auto& pos = crystalsPositions[i];
		glPushMatrix();
		glTranslatef(pos.x, pos.y, pos.z);
		glScalef(pulseScale, pulseScale, pulseScale);
		glScalef(0.25, 0.25, 0.25);
		model_crystal.Draw();
		glPopMatrix();

		// Optional: Render hitbox for debugging
	}
}

std::vector<Vector> statuesPositions;
void RenderStatues() {
	// Generate positions only once
	if (statuesPositions.empty()) {
		for (int i = 0; i < 1; i++) {
			float x = 50;      // Fixed X position
			float z = -90;    // Fixed Z position
			statuesPositions.push_back(Vector(x, 0, z));
			statueHitboxes.push_back(CreateStatueHitbox(Vector(x, 0, z))); // Generate hitbox
		}
	}

	// Render statues and hitboxes
	for (size_t i = 0; i < statuesPositions.size(); ++i) {
		const auto& pos = statuesPositions[i];
		glPushMatrix();
		glTranslatef(pos.x, pos.y, pos.z);  // Position the statue
		glScalef(0.5, 0.5, 0.5);           // Scale for size
		model_statue.Draw();               // Render the statue
		glPopMatrix();

		// Render hitbox for debugging
	}
}
void RenderTeleport() {
	float wallHeight = 25.0f;        // Height of the wall
	float wallWidth = 10.0f;         // Width of the wall
	float wallDepth = 2.0f;         // Depth of the wall
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex_teleport);
	// Position and render the wall in the middle of the statue
	glPushMatrix();	
	glTranslatef(55, 8 + wallHeight / 2, -110); // Position the wall
	glScalef(wallWidth, wallHeight, wallDepth); // Set size for the wall
	glColor3f(1, 1, 1); // Set color of the wall (gray)
	glutSolidCube(1.0);           // Render the wall as a cube
	glPopMatrix();
	glColor3f(1, 1, 1);
}


float dayCycleProgress = 0.0f;
float dayCycleSpeed = 0.0001f; // Adjust this to control the speed of the day/night cycle

// Function to update the day cycle (call this in your main loop)
void updateDayCycle(int value) {
	dayCycleProgress += dayCycleSpeed;
	if (dayCycleProgress >= 1.1f) {
		dayCycleProgress = 0.0f; // Reset at the end of the day
	}
	glutPostRedisplay(); // Redraw the scene
	glutTimerFunc(16, updateDayCycle, 0); // Call again after 16ms (~60 FPS)
}

void RenderSun() {
	if (dayCycleProgress > 0.55f) return;
	// Sun's vertical trajectory
	float sunRadius = 150.0f; // Radius of the sun's orbit
	float sunX = sunRadius * cos(dayCycleProgress * 2 * 3.14159f);
	float sunY = 0.0f; // Vertical motion
	float sunZ = sunRadius * sin(dayCycleProgress * 2 * 3.14159f); // Fixed Z position

	// Select texture and lighting based on day cycle
	GLuint currentTexture;
	GLfloat ambient[4], diffuse[4], specular[4];
	if (dayCycleProgress <= 0.2f) { // Morning
		currentTexture = tex_sun_morning;

		// Warmer, softer lighting for morning
		ambient[0] = 0.5f; ambient[1] = 0.3f; ambient[2] = 0.2f; ambient[3] = 1.0f; // Soft orange
		diffuse[0] = 0.9f; diffuse[1] = 0.7f; diffuse[2] = 0.5f; diffuse[3] = 1.0f; // Bright orange-yellow
		specular[0] = 0.6f; specular[1] = 0.5f; specular[2] = 0.4f; specular[3] = 1.0f; // Slightly shiny
	}
	else if (dayCycleProgress <= 0.45f) { // Noon
		currentTexture = tex_sun_noon;

		// Bright, neutral lighting for midday
		ambient[0] = 0.5f; ambient[1] = 0.5f; ambient[2] = 0.5f; ambient[3] = 1.0f; // Neutral white
		diffuse[0] = 1.0f; diffuse[1] = 1.0f; diffuse[2] = 1.0f; diffuse[3] = 1.0f; // Bright white
		specular[0] = 0.8f; specular[1] = 0.8f; specular[2] = 0.8f; specular[3] = 1.0f; // Shiny white
	}
	else { // Sunset
		currentTexture = tex_sun_evening;

		// Enhance the warm tones for a vivid sunset
		ambient[0] = 0.6f; ambient[1] = 0.2f; ambient[2] = 0.1f; ambient[3] = 1.0f; // Rich orange-red
		diffuse[0] = 1.0f; diffuse[1] = 0.5f; diffuse[2] = 0.3f; diffuse[3] = 1.0f; // Vibrant sunset tones
		specular[0] = 0.4f; specular[1] = 0.3f; specular[2] = 0.2f; specular[3] = 1.0f; // Subtle shine
	}


	// Set light source properties
	GLfloat sunPosition[] = { sunX, sunY, sunZ, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT0, GL_POSITION, sunPosition);

	// Render the sun visually with the selected texture
	glPushMatrix();
	glTranslatef(sunX, sunY, sunZ); // Position the sun
	glScalef(5.0f, 5.0f, 5.0f);    // Scale for visibility
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, currentTexture); // Bind the selected texture
	GLUquadricObj* qobj = gluNewQuadric();
	gluQuadricTexture(qobj, GL_TRUE);
	gluQuadricNormals(qobj, GL_SMOOTH);
	gluSphere(qobj, 5.0f, 32, 32); // Render a sphere for the sun
	gluDeleteQuadric(qobj);
	glDisable(GL_TEXTURE_2D); // Disable texture for other objects
	glPopMatrix();
}

void RenderMoon() {
	// Moon's vertical trajectory
	float moonRadius = 150.0f; // Radius of the moon's orbit
	float moonX = moonRadius * cos((dayCycleProgress - 0.55f) * 2 * 3.14159f);
	float moonY = moonRadius * sin((dayCycleProgress - 0.55f) * 2 * 3.14159f); // Vertical motion
	float moonZ = 0.0f; // Fixed Z position

	// Moon lighting (dim and cool)
	GLfloat moonAmbient[] = { 0.1f, 0.1f, 0.2f, 1.0f };
	GLfloat moonDiffuse[] = { 0.2f, 0.2f, 0.4f, 1.0f };
	GLfloat moonSpecular[] = { 0.1f, 0.1f, 0.3f, 1.0f };
	GLfloat moonPosition[] = { moonX, moonY, moonZ, 1.0f };

	// Set light properties for the moon
	glLightfv(GL_LIGHT0, GL_AMBIENT, moonAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, moonDiffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, moonSpecular);
	glLightfv(GL_LIGHT0, GL_POSITION, moonPosition);

	// Render the moon visually with texture
	glPushMatrix();
	glTranslatef(moonX, moonY, moonZ); // Position the moon
	glScalef(3.0f, 3.0f, 3.0f);       // Scale for visibility
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex_moon); // Bind moon texture
	GLUquadricObj* qobj = gluNewQuadric();
	gluQuadricTexture(qobj, GL_TRUE);
	gluQuadricNormals(qobj, GL_SMOOTH);
	gluSphere(qobj, 5.0f, 32, 32); // Render a sphere for the moon
	gluDeleteQuadric(qobj);
	glDisable(GL_TEXTURE_2D); // Disable texture for other objects
	glPopMatrix();
}




//=======================================================================
// Display Function
//=======================================================================



void RenderScene() {
	if (dayCycleProgress <= 0.55f) {
		RenderSun(); // Render sun until 0.55
	}
	else {
		RenderMoon(); // Render moon after 0.55
	}
}






enum playerState { STANDING, STUMBLING, PICKING};
playerState playerCurrentState = STANDING;


void UpdatePlayerState(float deltaTime) {
	switch (playerCurrentState) {
	case STANDING:
		// Normal walking/running, no timer involved
		break;

	case STUMBLING:
		stateTimer += deltaTime;
		if (stateTimer >= 5.0f) { // After 5 seconds
			playerCurrentState = STANDING;
			stateTimer = 0.0f;
		}
		break;

	case PICKING:
		stateTimer += deltaTime;
		if (stateTimer >= 3.1f) { // After 5 seconds
			playerCurrentState = STANDING;
			stateTimer = 0.0f;
		}
		break;
	}
}




bool thirdPersonView = false; // Toggle between first-person and third-person view
float mouseSensitivity = 0.5f; // Sensitivity factor for smooth movement
float pitch = 0.0f;            // Camera pitch (up/down rotation)
float yaw = 90.0f;             // Camera yaw (left/right rotation)
const float PI = 3.14159265359f;


float moveSpeed = 1.0f; // Speed of camera movement
float rotationSpeed = 5.0f; // Speed of camera rotation in degrees


static int lastX = WIDTH / 2; // Initial mouse position
static int lastY = HEIGHT / 2;

Vector playerPosition = { Eye.x, Eye.y, Eye.z }; // Initial player position

void RenderPlayerFirstPerson(float walkCycle) {
	// Update player hitbox based on current position and orientation
	playerPosition = { Eye.x, Eye.y, Eye.z };
	playerHitbox = CreatePlayerHitbox(playerPosition);

	glPushMatrix();
	glTranslatef(Eye.x - cos(yaw * (PI / 180.0f)) * 1.7f, 0, Eye.z - sin(yaw * (PI / 180.0f)) * 1.7f);
	glScalef(0.03f, 0.03f, 0.03f);
	Eye.y = 5.5f;

	// Walking animation calculations
	float armSwingAngle = sin(walkCycle) * 20.0f;  // Reduced swing for first-person
	float legSwingAngle = cos(walkCycle) * 10.0f;  // Subtle leg movement

	glRotatef(90, 0.0f, 1.0f, 0.0f);
	glRotatef(-yaw, 0.0f, 1.0f, 0.0f);

	// Render torso (relatively stable)
	player.torso.Draw();

	// Left Arm Animation (subtle swing)
	glPushMatrix();
	glTranslatef(-0.5f, 1.0f, 0.0f);
	glRotatef(-armSwingAngle * 0.5f, 1.0f, 0.0f, 0.0f);
	player.leftArm.Draw();
	glPopMatrix();

	// Right Arm Animation (subtle swing)
	glPushMatrix();
	glTranslatef(0.5f, 1.0f, 0.0f);
	glRotatef(armSwingAngle * 0.5f, 1.0f, 0.0f, 0.0f);
	player.rightArm.Draw();
	glPopMatrix();

	// Left Leg Animation (very subtle movement)
	glPushMatrix();
	glTranslatef(-0.2f, -1.0f, 0.0f);
	glRotatef(legSwingAngle * 0.3f, 1.0f, 0.0f, 0.0f);
	player.leftLeg.Draw();
	glPopMatrix();

	// Right Leg Animation (very subtle movement)
	glPushMatrix();
	glTranslatef(0.2f, -1.0f, 0.0f);
	glRotatef(-legSwingAngle * 0.3f, 1.0f, 0.0f, 0.0f);
	player.rightLeg.Draw();
	glPopMatrix();

	glPopMatrix();

	// Draw the player's hitbox
}

void RenderPlayerThirdPerson(float walkCycle) {
	glPushMatrix();
	glTranslatef(Eye.x + cos(yaw * (PI / 180.0f)) * 1.7f, 0, Eye.z + sin(yaw * (PI / 180.0f)) * 1.7f);

	// Scale down the player (adjust as needed)
	glScalef(0.03f, 0.03f, 0.03f);
	Eye.y = 7.0f;

	// Walking animation calculations
	float armSwingAngle = sin(walkCycle) * 20.0f;  // Full arm swing
	float legSwingAngle = cos(walkCycle) * 10.0f;  // Opposite phase leg swing

	// Position the player in front of the camera
	// Align the torso with the camera's height
	glRotatef(90, 0.0f, 1.0f, 0.0f);
	// Rotate the torso to follow the camera's yaw
	glRotatef(-yaw, 0.0f, 1.0f, 0.0f);

	// Render torso (stable during walk)
	player.torso.Draw();

	// Left Arm Animation
	glPushMatrix();
	glTranslatef(-0.5f, 1.0f, 0.0f);
	glRotatef(-armSwingAngle, 1.0f, 0.0f, 0.0f);
	player.leftArm.Draw();
	glPopMatrix();

	// Right Arm Animation
	glPushMatrix();
	glTranslatef(0.5f, 1.0f, 0.0f);
	glRotatef(armSwingAngle, 1.0f, 0.0f, 0.0f);
	player.rightArm.Draw();
	glPopMatrix();

	// Left Leg Animation
	glPushMatrix();
	glTranslatef(-0.2f, -1.0f, 0.0f);
	glRotatef(legSwingAngle, 1.0f, 0.0f, 0.0f);
	player.leftLeg.Draw();
	glPopMatrix();

	// Right Leg Animation
	glPushMatrix();
	glTranslatef(0.2f, -1.0f, 0.0f);
	glRotatef(-legSwingAngle, 1.0f, 0.0f, 0.0f);
	player.rightLeg.Draw();
	glPopMatrix();

	glPopMatrix();
}


void RenderStanding(float animationTime) {
	if (thirdPersonView) {
		RenderPlayerThirdPerson(animationTime);
	}
	else {
		RenderPlayerFirstPerson(animationTime);
	}
}

void RenderStumbling() {
	glPushMatrix();
	glTranslatef(playerPosition.x, 0, playerPosition.z);
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f); // Lie flat on back
	glScalef(0.03f, 0.03f, 0.03f);

	// Render the torso
	player.torso.Draw();

	// Optional: Render arms and legs at resting positions
	player.leftArm.Draw();
	player.rightArm.Draw();
	player.leftLeg.Draw();
	player.rightLeg.Draw();

	glPopMatrix();
}


void RenderPicking() {
	glPushMatrix();
	glTranslatef(playerPosition.x, 0, playerPosition.z);
	glScalef(0.03f, 0.03f, 0.03f);
	
	glPushMatrix();
	glRotatef(35.0f, 1.0f, 0.0f, 0.0f); //Bent forward
	//Render the torso
	player.torso.Draw();
	
	glPushMatrix();
	glTranslatef(0.5f, 1.0f, -0.5f); // Adjust for reaching forward
	player.leftArm.Draw();
	player.rightArm.Draw();
	glPopMatrix();
	glPopMatrix();
	// Render legs in a steady position
	player.leftLeg.Draw();
	player.rightLeg.Draw();

	glPopMatrix();
}

void RenderText(float x, float y, const std::string& text) {
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, WIDTH, 0, HEIGHT); // Set up a 2D orthographic projection
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glDisable(GL_LIGHTING); // Disable lighting for HUD
	glColor3f(1.0f, 1.0f, 1.0f); // Set text color to white

	// Render text at the specified position
	glRasterPos2f(x, y);
	for (char c : text) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
	}

	glEnable(GL_LIGHTING); // Re-enable lighting after rendering HUD
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

// Function to render the HUD
void RenderHUD() {
	std::ostringstream hudText;

	// Create HUD strings
	hudText << "Score: " << gameScore;
	RenderText(10, HEIGHT - 30, hudText.str());

	hudText.str(""); // Clear the stream
	hudText << "Mushrooms: " << mushroomsCollected << "/3";
	RenderText(10, HEIGHT - 60, hudText.str());

	hudText.str(""); // Clear the stream
	hudText << "Crystals: " << crystalsCollected << "/3";
	RenderText(10, HEIGHT - 90, hudText.str());

	hudText.str(""); // Clear the stream
	hudText << "Data " << dataCollected << "/5";
	RenderText(10, HEIGHT - 120, hudText.str());

	hudText.str(""); // Clear the stream
	hudText << "Time: " << gameTime;
	RenderText(10, HEIGHT - 150, hudText.str());
}
//=======================================================================
// Scene 2
//=======================================================================

void RenderCorridor() {
	GLfloat metalColor[] = { 0.7f, 0.7f, 0.8f, 1.0f }; // Metallic silver color
	GLfloat metalSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f }; // Bright specular highlights

	glPushMatrix();
	glTranslatef(50, -50, 600);
	glScalef(50, 50, 50);

	// Set material properties
	glMaterialfv(GL_FRONT, GL_DIFFUSE, metalColor);
	glMaterialfv(GL_FRONT, GL_SPECULAR, metalSpecular);
	glMaterialf(GL_FRONT, GL_SHININESS, 50.0f); // High shininess for metallic effect

	model_corridor.Draw();
	glPopMatrix();
}

void RenderUFO(float deltaTime) {
	static float flickerTimer = 0.0f;
	static float lightsOutTimer = 0.0f;
	static bool isFlickering = false;
	static bool lightsOut = false;
	static bool isOrangeSpark = false;
	// Update timers
	flickerTimer += deltaTime;
	lightsOutTimer += deltaTime;
	// Reset flicker timer every 6 seconds
	if (flickerTimer >= 6.0f) {
		flickerTimer = 0.0f;
	}
	isFlickering = (flickerTimer < 3.0f);
	isOrangeSpark = (flickerTimer >= 2.0f && flickerTimer < 3.0f); // Orange spark during the last second of flicker
	// Lights out logic: every 20 seconds, lights go off for 1 second
	if (lightsOutTimer >= 20.0f) {
		lightsOut = true;
	}
	if (lightsOut && lightsOutTimer >= 21.0f) {
		lightsOut = false;
		lightsOutTimer = 0.0f; // Reset lights out timer
	}
	// Handle global ambient lighting
	if (lightsOut) {
		// Completely dark during lights out
		GLfloat globalAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);
		// Disable all lights
		for (int i = 0; i < 4; i++) {
			glDisable(GL_LIGHT0 + i);
		}
		return; // Skip rendering UFOs when lights are out
	}
	else if (isFlickering) {
		GLfloat globalAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f }; // Very dark during flickering
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);
	}
	else {
		GLfloat globalAmbient[] = { 0.02f, 0.02f, 0.02f, 1.0f }; // Normal dim ambient light
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);
	}
	// Render UFOs with individual lights
	for (int i = 0; i < 4; i++) {
		float zPosition = -90 + i * 380;
		GLenum light = GL_LIGHT0 + i;
		GLfloat lightPosition[] = { 50.0f, 100.0f, zPosition, 1.0f };
		GLfloat lightAmbient[] = { 0.01f, 0.01f, 0.01f, 1.0f };       // Very low ambient
		GLfloat lightDiffuse[] = { 0.4f, 0.4f, 0.35f, 1.0f };         // Reduced diffuse intensity
		GLfloat lightSpecular[] = { 0.2f, 0.2f, 0.15f, 1.0f };        // Reduced specular
		if (isFlickering) {
			float flicker = 0.7f + static_cast<float>(rand()) / RAND_MAX * 0.2f; // Narrower flicker range
			for (int j = 0; j < 3; j++) {
				lightDiffuse[j] *= flicker;
				lightSpecular[j] *= flicker;
			}
		}
		// Add orange spark effect
		if (isOrangeSpark) {
			lightDiffuse[0] = 1.0f; // Strong red
			lightDiffuse[1] = 0.5f; // Medium green
			lightDiffuse[2] = 0.0f; // No blue
			lightSpecular[0] = 1.0f;
			lightSpecular[1] = 0.5f;
			lightSpecular[2] = 0.0f;
		}
		glEnable(light);
		glLightfv(light, GL_POSITION, lightPosition);
		glLightfv(light, GL_AMBIENT, lightAmbient);
		glLightfv(light, GL_DIFFUSE, lightDiffuse);
		glLightfv(light, GL_SPECULAR, lightSpecular);
		glPushMatrix();
		glTranslatef(50, 100, zPosition);
		glScalef(0.15, 0.15, 0.15);
		model_UFO.Draw();
		glPopMatrix();
	}
	glColor3f(1, 1, 1);
}

void RenderFan(float currentTime) {
	float fanPositions[5][2] = {
		{0, 0},
		{90, 250},
		{30, 500},
		{70, 750},
		{120, 1000}
	};
	currentTime += deltaTime;
	for (int i = 0; i < 5; i++) {
		glPushMatrix();
		glTranslatef(fanPositions[i][0], 50, fanPositions[i][1]);
		glRotatef(90, 1, 0, 0);
		glRotatef(currentTime * 1500.0f, 0, 1, 0);
		glScalef(1.3, 1.3, 1.3);
		model_fan.Draw();
		glPopMatrix();
	}
	std::vector<Hitbox> fanHitboxes = CreateFanHitboxes();
}
std::vector<Vector> activeDataPickups;

void RenderData() {
	// Render active (picked up) data
	for (size_t i = 0; i < activeDataPickups.size(); ++i) {
		auto& pos = activeDataPickups[i];
		float translation = stateTimer * 2.0f;
		float rotationAngle = stateTimer * 90.0f;

		glPushMatrix();
		glTranslatef(pos.x, pos.y + translation, pos.z);
		glRotatef(rotationAngle, 0, 1, 0);
		glScalef(0.3, 0.3, 0.3);
		model_data.Draw();
		glPopMatrix();

		// Remove from active list after animation
		if (stateTimer >= 3.0f) {
			activeDataPickups.erase(activeDataPickups.begin() + i);
			--i; // Adjust the index to account for the removed item
		}
	}

	// Render remaining static data
	for (const auto& pos : dataPositions) {
		glPushMatrix();
		glTranslatef(pos.x, -10, pos.z);
		glScalef(0.3, 0.3, 0.3);
		model_data.Draw();
		glPopMatrix();
	}
	CreateDataHitboxes();
}


void RenderDoor() {
	float wallHeight = 150.0f;        // Height of the wall
	float wallWidth = 250.0f;         // Width of the wall
	float wallDepth = 5.0f;         // Depth of the wall
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex_teleport);
	// Position and render the wall in the middle of the statue
	glPushMatrix();
	glTranslatef(65,  wallHeight / 2 - 20 , 1480); // Position the wall
	glScalef(wallWidth, wallHeight, wallDepth); // Set size for the wall
	glColor3f(1, 1, 1); // Set color of the wall (gray)
	glutSolidCube(1.0);           // Render the wall as a cube
	glPopMatrix();
	glColor3f(1, 1, 1);
}

float robotTime = 0.0f;  // Global time tracker
float totalDuration = 10.0f;

void RenderRobot() {
	glPushMatrix();
	// Interpolate X position from -20 to 120 over 10 seconds
	float currentX = -20 + (robotTime / totalDuration) * 140;

	// Robot's current position
	Vector robotPosition = { currentX, -30, 1400 };

	// Calculate 3D distance between player and robot
	float distance = sqrt(
		pow(playerPosition.x - robotPosition.x, 2) +
		pow(playerPosition.y - robotPosition.y, 2) +
		pow(playerPosition.z - robotPosition.z, 2)
	);

	// If player is within 150 units, play robot sound
	if (distance > 450)
		playRobotSound();

	// Apply translation to position the model correctly
	glTranslatef(currentX, -30, 1400);
	glRotatef(90, 0, 1, 0);

	// Apply scaling
	glScalef(0.05, 0.05, 0.05);

	// Draw the model
	model_robot.Draw();

	glPopMatrix();
}

void RenderRobotLightBeam() {
	glPushMatrix();

	// Interpolate X position for the light beam
	float currentX = -20 + (robotTime / totalDuration) * 140;

	// Position the light at the robot's head location
	glTranslatef(currentX + 10, 10, 1400);
	glRotatef(-90, 0, 1, 0);

	// Set up light parameters
	GLfloat lightPosition[4] = { 0, 0, 0, 1 };  // Position at origin relative to current matrix
	GLfloat lightDirection[3] = { 0, 0, -1 };   // Point forward along the -Z axis

	// Set light color to red
	GLfloat lightColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };  // Bright red

	// Enable OpenGL lighting
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT3);  // Use a different light source number

	// Set light position
	glLightfv(GL_LIGHT3, GL_POSITION, lightPosition);

	// Set light direction for a spot light effect
	glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, lightDirection);

	// Configure spot light parameters
	glLightf(GL_LIGHT3, GL_SPOT_CUTOFF, 30.0f);       // Cone angle (wider for a broader beam)
	glLightf(GL_LIGHT3, GL_SPOT_EXPONENT, 1000.0f);   // Intensity falloff

	// Set light color
	glLightfv(GL_LIGHT3, GL_DIFFUSE, lightColor);

	// Optional: render a visible cone (for visualization)
	glDisable(GL_LIGHTING);

	// Restore matrix state
	glPopMatrix();
}



























































//=======================================================================
// Keyboard Function
//=======================================================================


bool Teleport = false;

bool CanMoveTo(const Vector& newPosition) {
	Hitbox simulatedHitbox = CreatePlayerHitbox(newPosition);

	// Tree collision (existing implementation)
	for (const auto& treeBox : treeHitboxes) {
		if (CheckHitboxIntersection(simulatedHitbox, treeBox)) {
			return false; // Collision detected    
		}
	}

	// Collision handling for logs
	for (const auto& logBox : logHitboxes) {
		if (CheckHitboxIntersection(simulatedHitbox, logBox)) {
			if (playerCurrentState == STANDING) {
				playerCurrentState = STUMBLING;
				stateTimer = 0.0f;
				gameTime -= 30;
				playCollisionSound();
			}
			return false;
		}
	}

	// Collision handling for rocks
	for (const auto& rockBox : rockHitboxes) {
		if (CheckHitboxIntersection(simulatedHitbox, rockBox)) {
			if (playerCurrentState == STANDING) {
				playerCurrentState = STUMBLING;
				stateTimer = 0.0f;
				gameTime -= 30;
				playCollisionSound();
			}
			return false;
		}
	}

	// Collision handling for mushrooms
	for (size_t i = 0; i < mushroomPositions.size(); ++i) {
		if (CheckHitboxIntersection(playerHitbox, mushroomHitboxes[i])) {
			if (playerCurrentState == STANDING) {
				playerCurrentState = PICKING;
				stateTimer = 0.0f;
				activeMushroomPickups.push_back(mushroomPositions[i]); // Add to active pickups
				mushroomPositions.erase(mushroomPositions.begin() + i); // Remove from static list
				mushroomHitboxes.erase(mushroomHitboxes.begin() + i); // Remove its hitbox
				mushroomsCollected++;
				gameScore += 1000;
				playMushroomSound();
			}
			
		}
	}

	// Collision handling for crystals
	for (size_t i = 0; i < crystalsPositions.size(); ++i) {
		if (CheckHitboxIntersection(playerHitbox, crystalHitboxes[i])) {
			if (playerCurrentState == STANDING) {
				playerCurrentState = PICKING;
				stateTimer = 0.0f;
				activeCrystalPickups.push_back(crystalsPositions[i]); // Add to active pickups
				crystalsPositions.erase(crystalsPositions.begin() + i); // Remove from static list
				crystalHitboxes.erase(crystalHitboxes.begin() + i); // Remove its hitbox
				crystalsCollected++;
				gameScore += 2000;
				playCrystalSound();
			}
		}
	}
	for (const auto& fanBox : fanHitboxes) {
		if (CheckHitboxIntersection(simulatedHitbox, fanBox)) {
			if (playerCurrentState == STANDING) {
				playerCurrentState = STUMBLING;
				stateTimer = 0.0f;
				gameTime -= 30;
				playCollisionSound();
			}
			return false;
		}
	}

	// Collision handling for data
	for (size_t i = 0; i < dataHitboxes.size(); ) {
		if (CheckHitboxIntersection(simulatedHitbox, dataHitboxes[i])) {
			if (playerCurrentState == STANDING) {
				playerCurrentState = PICKING;
				stateTimer = 0.0f;

				// Move the collected item to active pickups
				activeDataPickups.push_back(dataPositions[i]);

				// Remove the item from positions and hitboxes
				dataPositions.erase(dataPositions.begin() + i);
				dataHitboxes.erase(dataHitboxes.begin() + i);
				CreateDataHitboxes();
				// Update collected data and score
				dataCollected++;
				gameScore += 5000;
				playDataSound();
				// No increment of `i` here since we erased an element
				continue;
			}
		}

		// Increment `i` only when no item is erased
		++i;
	}


	// Statue collision (if needed)
	for (const auto& statueBox : statueHitboxes) {
		if (CheckHitboxIntersection(simulatedHitbox, statueBox)) {
			Teleport = true;	
			return false; // Prevent movement
		}
	}

	return true; // No collision
}
void myKeyboard(unsigned char button, int x, int y) {
	if (playerCurrentState == STUMBLING || playerCurrentState == PICKING) {
		return;
	}
	Vector newPosition = playerPosition;
	switch (button) {
	case 'w': // Move forward
		newPosition.x += cos(yaw * (PI / 180.0f)) * moveSpeed;
		newPosition.z += sin(yaw * (PI / 180.0f)) * moveSpeed;
		At.x += cos(yaw * (PI / 180.0f)) * moveSpeed;
		At.z += sin(yaw * (PI / 180.0f)) * moveSpeed;
		break;
	case 's': // Move backward
		newPosition.x -= cos(yaw * (PI / 180.0f)) * moveSpeed;
		newPosition.z -= sin(yaw * (PI / 180.0f)) * moveSpeed;
		At.x -= cos(yaw * (PI / 180.0f)) * moveSpeed;
		At.z -= sin(yaw * (PI / 180.0f)) * moveSpeed;
		break;
	case 'd': // Move right
		newPosition.x -= cos((yaw - 90.0f) * (PI / 180.0f)) * moveSpeed;
		newPosition.z -= sin((yaw - 90.0f) * (PI / 180.0f)) * moveSpeed;
		At.x -= cos((yaw - 90.0f) * (PI / 180.0f)) * moveSpeed;
		At.z -= sin((yaw - 90.0f) * (PI / 180.0f)) * moveSpeed;
		break;
	case 'a': // Move left
		newPosition.x += cos((yaw - 90.0f) * (PI / 180.0f)) * moveSpeed;
		newPosition.z += sin((yaw - 90.0f) * (PI / 180.0f)) * moveSpeed;
		At.x += cos((yaw - 90.0f) * (PI / 180.0f)) * moveSpeed;
		At.z += sin((yaw - 90.0f) * (PI / 180.0f)) * moveSpeed;
		break;
	default:
		break;
	}

	if (CanMoveTo(newPosition)) {
		playerPosition = newPosition;
	}
	if (playerCurrentState == STUMBLING) {
		stateTimer += deltaTime;
		if (stateTimer >= 5.0f) {
			playerCurrentState = STANDING;
		}
	}

	else if (playerCurrentState == PICKING) {
		stateTimer += deltaTime;
		if (stateTimer >= 5.0f) {
			playerCurrentState = STANDING;
		}
	}
	// Update the camera view
	Eye.x = playerPosition.x;
	
	Eye.z = playerPosition.z;

	glLoadIdentity();
	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);

	 // Check for collisions after movement

	glutPostRedisplay(); // Redraw the scene
}

//void myKeyboard(unsigned char button, int x, int y) {
//	
//
//	switch (button) {
//	case 'w': // Move forward
//		Eye.x += cos(yaw * (PI / 180.0f)) * moveSpeed;
//		Eye.z += sin(yaw * (PI / 180.0f)) * moveSpeed;
//		At.x += cos(yaw * (PI / 180.0f)) * moveSpeed;
//		At.z += sin(yaw * (PI / 180.0f)) * moveSpeed;
//		break;
//	case 's': // Move backward
//		Eye.x -= cos(yaw * (PI / 180.0f)) * moveSpeed;
//		Eye.z -= sin(yaw * (PI / 180.0f)) * moveSpeed;
//		At.x -= cos(yaw * (PI / 180.0f)) * moveSpeed;
//		At.z -= sin(yaw * (PI / 180.0f)) * moveSpeed;
//		break;
//	case 'd': // Move right
//		Eye.x -= cos((yaw - 90.0f) * (PI / 180.0f)) * moveSpeed;
//		Eye.z -= sin((yaw - 90.0f) * (PI / 180.0f)) * moveSpeed;
//		At.x -= cos((yaw - 90.0f) * (PI / 180.0f)) * moveSpeed;
//		At.z -= sin((yaw - 90.0f) * (PI / 180.0f)) * moveSpeed;
//		break;
//	case 'a': // Move left
//		Eye.x += cos((yaw - 90.0f) * (PI / 180.0f)) * moveSpeed;
//		Eye.z += sin((yaw - 90.0f) * (PI / 180.0f)) * moveSpeed;
//		At.x += cos((yaw - 90.0f) * (PI / 180.0f)) * moveSpeed;
//		At.z += sin((yaw - 90.0f) * (PI / 180.0f)) * moveSpeed;
//		break;
//	default:
//		break;
//	}
//	
//	
//	// Update the camera view
//	glLoadIdentity();
//	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);
//
//	glutPostRedisplay(); // Redraw the scene
//}


//=======================================================================
// Motion Function
//=======================================================================


void myMotion(int x, int y) {
	

	// Calculate the difference in mouse movement
	float deltaX = (x - lastX) * mouseSensitivity;
	float deltaY = (y - lastY) * mouseSensitivity;

	// Update yaw and pitch based on mouse movement
	yaw += deltaX;
	pitch -= deltaY;

	// Constrain the pitch to avoid flipping the camera
	if (pitch > 89.0f) pitch = 89.0f;
	if (pitch < -89.0f) pitch = -89.0f;

	// Convert degrees to radians for calculations
	float radYaw = yaw * (PI / 180.0f);
	float radPitch = pitch * (PI / 180.0f);

	// Update camera direction (Eye → At)
	At.x = Eye.x + cos(radYaw) * cos(radPitch);
	At.y = Eye.y + sin(radPitch);
	At.z = Eye.z + sin(radYaw) * cos(radPitch);


	
	// Update the camera view
	glLoadIdentity();
	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);

	// Store the current mouse position for the next frame
	lastX = x;
	lastY = y;

	glutPostRedisplay(); // Redraw the scene
}





void myMouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON) {
		thirdPersonView = true;
	}
	else if (button == GLUT_RIGHT_BUTTON) {
		thirdPersonView = false;
	}
}


//=======================================================================
// Reshape Function
//=======================================================================
void myReshape(int w, int h)
{
	if (h == 0) {
		h = 1;
	}

	WIDTH = w;
	HEIGHT = h;

	// set the drawable region of the window
	glViewport(0, 0, w, h);

	// set up the projection matrix 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy, (GLdouble)WIDTH / (GLdouble)HEIGHT, zNear, zFar);

	// go back to modelview matrix so we can move the objects about
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);
}

//=======================================================================
// Assets Loading Function
//=======================================================================
void LoadAssets()
{
	/// Loading Model files
	model_tree.Load("models/tree/Tree1.3ds");
	model_mushroom.Load("models/mushroom/mushroom1.3ds");
	model_rock.Load("models/rock/Rock.3ds");
	model_log.Load("models/log/logstone1.3ds");
	model_crystal.Load("models/crystal/LighthingCrystal/Blend/crystal2.3ds");
	model_statue.Load("models/gate/darkportal02.3ds");
	player.torso.Load("models/player/torso.3ds");
	player.leftArm.Load("models/player/leftArm.3ds");
	player.rightArm.Load("models/player/rightArm.3ds");
	player.leftLeg.Load("models/player/leftLeg.3ds");
	player.rightLeg.Load("models/player/rightLeg.3ds");
	model_corridor.Load("models/SciFi/SciFi5.3ds");
	model_UFO.Load("models/UFO/UFO1.3ds");
	model_fan.Load("models/fan/fan.3ds");
	model_data.Load("models/data/data.3ds");
	model_robot.Load("models/robot/robot2.3ds");
	// Loading texture files
	tex_ground.Load("Textures/ground.bmp");
	tex_wall.Load("Textures/ground.bmp");
	
	loadBMP(&tex, "Textures/blu-sky-3.bmp", true);
	loadBMP(&tex_sun_morning, "Textures/yellow.bmp", true);
	loadBMP(&tex_sun_noon, "Textures/sun.bmp", true);
	loadBMP(&tex_sun_evening, "Textures/sunset.bmp", true);
	loadBMP(&tex_moon, "Textures/moon.bmp", true);
	loadBMP(&tex_rock, "Textures/rock.bmp", true);
	loadBMP(&tex_teleport, "Textures/ender.bmp", true);
	
}
void RenderText1(const char* text, float x, float y) {
	// Disable depth testing for 2D text rendering
	glDisable(GL_DEPTH_TEST);

	// Switch to projection mode for 2D rendering
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(-1, 1, -1, 1); // Orthographic projection

	// Switch to modelview mode
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	// Set the text color
	glColor3f(1.0f, 1.0f, 1.0f);

	// Set the raster position for the text
	glRasterPos2f(x, y);

	// Render the text using GLUT
	while (*text) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *text);
		text++;
	}

	// Restore matrices
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	// Re-enable depth testing
	glEnable(GL_DEPTH_TEST);
}

float flickerTime = 0.01f;

float animationTime = 0.0f;
float fanTime = 0.0f;
float winSceneTimer = 0.0f;
bool startWinSceneTimer = false;
bool startLoseSceneTimer = false;
bool openTeleport = false;
bool openPortal = false;
bool win = false;
bool lose = false;

void myDisplay(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (currentGameState == WIN_SCENE) {
		// WIN_SCENE rendering
		glClearColor(0.0f, 0.5f, 0.0f, 1.0f); // Green background for the win screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the buffer with the new color

		// Render "YOU WIN!" message
		glColor3f(1.0f, 1.0f, 1.0f); // White text
		RenderText1("YOU WIN!", -0, 0.2f);

		// Render score
		char scoreText[50];
		snprintf(scoreText, sizeof(scoreText), "Your Score: %d", gameScore);
		RenderText1(scoreText, -0, 0.0f);
		if (!win) {
			alSourceStop(sourceBenchPress);
			alSourceStop(sourceDeadlift);
			// Render restart instruction
			playBackgroundMusic();
			win = true;
		}
		glutSwapBuffers();
		return; // Skip further rendering
	}

	if (currentGameState == LOSE_SCENE) {
		// LOSE_SCENE rendering
		glClearColor(0.5f, 0.0f, 0.0f, 1.0f); // Red background for the lose screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the buffer with the new color

		// Render "YOU LOSE!" message
		glColor3f(0.0f, 0.0f, 0.0f); // White text
		RenderText1("YOU LOSE!", 0, 0.2);

		// Render score
		char scoreText[50];
		snprintf(scoreText, sizeof(scoreText), "Your Score: %d", gameScore);
		RenderText1(scoreText, 0, 0);
		if (!lose) {
			alSourceStop(sourceCollision);
			// Render restart instruction
			playYouDiedSound();
			lose = true;
		}
		glutSwapBuffers();
		return; // Skip further rendering
	}


	// Check if teleport condition is met
	if (crystalsCollected >= 3 && mushroomsCollected >= 3 && Teleport == true) {
		currentGameState = TELEPORT_SCENE;
		if (!openTeleport) {
			playTeleportSound();
			openTeleport = true;
		}
	}

	// Render based on the current game state
	if (currentGameState == MAIN_SCENE) {
		// MAIN_SCENE rendering
		stateTimer += deltaTime; // Increment in your update loop
		if (playerCurrentState == STANDING) {
			stateTimer = 0.0f; // Reset when idle
		}

		// Update day cycle
		RenderScene(); // Pass the dayCycleProgress

		// Draw models (this includes hitboxes)
		RenderBoundaryWalls();
		RenderGround();
		RenderTrees();
		RenderMushrooms();
		RenderRocks();
		RenderLogs();
		RenderCrystals();
		RenderStatues();
		RenderHUD();
		if(crystalsCollected >= 3 && mushroomsCollected >= 3){
			RenderTeleport();
			if (!openPortal) {
				playOpenPortalSound();
				alSourceStop(sourceSmith);
				alSourceStop(sourceTreadmill);
				openPortal = true;
			}
		}

		// Draw Player Model
		animationTime += 0.1f;
		UpdatePlayerState(deltaTime);
		
		
		switch (playerCurrentState) {
		case STANDING:
			RenderStanding(animationTime);
			break;

		case STUMBLING:
			RenderStumbling();
			break;

		case PICKING:
			RenderPicking();
			break;
		}
		
		// Draw skybox
		glPushMatrix();
		GLUquadricObj* qobj = gluNewQuadric();
		glTranslated(0, 0, 0); // Centered at the origin
		glRotated(90, 1, 0, 1); // Rotate for proper orientation
		glBindTexture(GL_TEXTURE_2D, tex); // Apply skybox texture
		gluQuadricTexture(qobj, true);
		gluQuadricNormals(qobj, GL_SMOOTH);
		gluSphere(qobj, 200.0f, 150, 150); // Large enough to encompass the scene
		gluDeleteQuadric(qobj);
		glPopMatrix();

		if (gameTime <= 0) {
			currentGameState = LOSE_SCENE;
			
		}

	}
	else if (currentGameState == TELEPORT_SCENE) {

		moveSpeed = 10.0f;
		
		
		
		// TELEPORT_SCENE rendering
		treeHitboxes.clear();
		mushroomHitboxes.clear();
		rockHitboxes.clear();
		logHitboxes.clear();
		crystalHitboxes.clear();
		statueHitboxes.clear();
		treePositions.clear();
		mushroomPositions.clear();
		rocksPositions.clear();
		logsPositions.clear();
		crystalsPositions.clear();
		statuesPositions.clear();
		
		fanTime += 0.01f;
		animationTime += 0.1f;
		UpdatePlayerState(deltaTime);

		switch (playerCurrentState) {
		case STANDING:
			RenderStanding(animationTime);
			break;

		case STUMBLING:
			RenderStumbling();
			break;

		case PICKING:
			RenderPicking();
			break;
		}
		RenderHUD();
		RenderCorridor();
		RenderUFO(deltaTime);
		RenderFan(fanTime);
		RenderData();
		RenderRobot();
		RenderRobotLightBeam();

		robotTime += deltaTime;

		// Clamp the animation time
		if (robotTime > totalDuration) {
			robotTime = 0;
		}

		if (dataCollected >= 5) {
			RenderDoor();
			currentGameState = WIN_SCENE;
		}
		if (gameTime <= 0) {
			currentGameState = LOSE_SCENE;
		}
	} 
	glutSwapBuffers();
}





//=======================================================================
// Main Function
//=======================================================================


void UpdateGameTime(int value) {
	if (gameTime > 0) {
		gameTime--; // Decrease game time by 1
	}
	glutPostRedisplay(); // Request a redraw to update the HUD
	glutTimerFunc(1000, UpdateGameTime, 0); // Call this function again after 1 second
}


void main(int argc, char** argv)
{
	glutInit(&argc, argv);
	initOpenAL();
	std::thread soundThread(loadSoundInBackground);
	soundThread.join();
	
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(WIDTH, HEIGHT);

	glutInitWindowPosition(100, 150);

	glutCreateWindow("MAZE RUNNER 3D");

	glutDisplayFunc(myDisplay);
	glutKeyboardFunc(myKeyboard);
	glutMotionFunc(myMotion);
	glutPassiveMotionFunc(myMotion);
	glutMouseFunc(myMouse);
	glutReshapeFunc(myReshape);
	
	myInit();
	LoadAssets();

	glutTimerFunc(0, updateDayCycle, 0); // Start the day cycle update timer
	glutTimerFunc(1000, UpdateGameTime, 0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);

	glShadeModel(GL_SMOOTH);

	glutMainLoop();
	cleanupOpenAL();
}