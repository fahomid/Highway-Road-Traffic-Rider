#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<GL/glut.h>
#include<math.h>
#include<Windows.h>
#include<vector>
#include<math.h>
#include<string>
#include<cstdio>
using namespace std;

//texture ids
GLuint textureRoad, texturePlayerCar, texturePoliceCar, textureRegularCar1, textureRegularCar2, textureRegularCar3, textureRegularCar5, textureRegularCar6, textureRegularCar7, textureRegularCar8, textureRegularCar9, textureLogo;
boolean vehicleSetupComplete = false;

//Vehicle information 
struct vehicle {
	int x[4];
	int y[4];
	GLuint textureID;
	float speed;
};

vector<vehicle> vehicleCoordinations;
int vectorSize = 10;

const int maxX = 380;
const int minX = 0;
const int maxY = 640;
const int minY = 0;
const int minZ = 0;
const int maxZ = 100;
const int playerCarHeight = 100;
const int playerCarWidth = 100;

float scoreCard = 0;
float scoreHigh = 0;
float scoreIncreament = 0.01;
float oppositeCarDistance = 150;


const double pi = 3.14159265358979323846;
float _xx1 = 100;
float _yy1 = 40;
float _xx2 = 100 + playerCarWidth;
float _yy2 = 40;
float _xx3 = 100 + playerCarWidth;
float _yy3 = 40 + playerCarHeight;
float _xx4 = 100;
float _yy4 = 40 + playerCarHeight;

float _defxx1 = 100;
float _defyy1 = 40;
float _defxx2 = 100 + playerCarWidth;
float _defyy2 = 40;
float _defxx3 = 100 + playerCarWidth;
float _defyy3 = 40 + playerCarHeight;
float _defxx4 = 100;
float _defyy4 = 40 + playerCarHeight;

float _moveLeftRight = 0;
float _moveForwardBackword = 0;
float _moveZangel = 0;
float moveSpeed = 15;
float oppositeCarSpeed = 1.5;

int carWidth = 100;
int carHeight = 100;
int carRightPosition = carWidth;
float roadMoveSpeed = 1;
float roadY1 = maxY, roadY2 = minY, roadY3 = maxY * -1;

enum screenMode {START, GAME, SCORE, PAUSE, END};
int currentScreen = START;
bool playingGameAudio = false;
bool playingStartMode = false;

void reRenderScreen(int w, int h) {
	glutInitWindowSize(maxX, maxY);
}

GLuint LoadTexture(const char * filename) {
	// Data read from the header of the BMP file
	unsigned char header[54]; // Each BMP file begins by a 54-bytes header
	unsigned int dataPos;     // Position in the file where the actual data begins
	unsigned int width, height;
	unsigned int imageSize;
	unsigned char *ImageData;

	// Open the file
	FILE * file = fopen(filename, "rb");
	if (!file) {
		printf("%s: Image could not be opened\n", filename);
		return false;
	}
	else if (fread(header, 1, 54, file) != 54) { // If not 54 bytes read : problem
		printf("Not a correct BMP file\n");
		return false;
	}
	else if (header[0] != 'B' || header[1] != 'M') {
		printf("Not a correct BMP file\n");
		return false;
	}

	// Read ints from the byte array
	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);

	// Some BMP files are misformatted, guess missing information
	if (imageSize == 0)    imageSize = width*height * 3; // 3 : one byte for each Red, Green and Blue component
	if (dataPos == 0)      dataPos = 54; // The BMP header is done that way

	// Create a buffer
	ImageData = new unsigned char[imageSize];
	// Read the actual data from the file into the buffer
	fread(ImageData, 1, imageSize, file);
	//Everything is in memory now, the file can be closed
	fclose(file);
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_BGRA_EXT, width, height, GL_BGRA_EXT, GL_UNSIGNED_BYTE, ImageData);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, ImageData);
	//cout << gluErrorString(glGetError()) << endl;
	free(ImageData);
	return texture;
}

void createVehicle(int x[4], int y[4], GLuint textureID) {
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, textureID);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBegin(GL_QUADS);
		glVertex2i(x[0], y[0]);
		glTexCoord2i(1, 0);
		glVertex2i(x[1], y[1]);
		glTexCoord2i(1, 1);
		glVertex2i(x[2], y[2]);
		glTexCoord2i(0, 1);
		glVertex2i(x[3], y[3]);
		glTexCoord2i(0, 0);
	glEnd();
	glPopMatrix();
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
}

int roll(int min, int max) {
	double x = rand() / static_cast<double>(RAND_MAX + 1);
	int that = min + static_cast<int>(x * (max - min));
	return that;
}

//this function will simply create vechicles in opposite site randomly
bool setupVehicle(int x, int y) {
	int randomVehicle = roll(1, 10);
	vehicle temp;
	temp.x[0] = x;
	temp.x[1] = x + carWidth;
	temp.x[2] = x + carWidth;
	temp.x[3] = x;
	temp.y[0] = y;
	temp.y[1] = y;
	temp.y[2] = y + carHeight;
	temp.y[3] = y + carHeight;
	switch (randomVehicle) {
		case 1:
			createVehicle(temp.x, temp.y, textureRegularCar1);
			temp.speed = oppositeCarSpeed;
			temp.textureID = textureRegularCar1;
			break;
		case 2:
			createVehicle(temp.x, temp.y, textureRegularCar2);
			temp.speed = oppositeCarSpeed;
			temp.textureID = textureRegularCar2;
			break;
		case 3:
			createVehicle(temp.x, temp.y, textureRegularCar3);
			temp.speed = oppositeCarSpeed;
			temp.textureID = textureRegularCar3;
			break;
		case 4:
			createVehicle(temp.x, temp.y, texturePoliceCar);
			temp.speed = oppositeCarSpeed;
			temp.textureID = texturePoliceCar;
			break;
		case 5:
			createVehicle(temp.x, temp.y, textureRegularCar5);
			temp.speed = oppositeCarSpeed;
			temp.textureID = textureRegularCar5;
			break;
		case 6:
			createVehicle(temp.x, temp.y, textureRegularCar6);
			temp.speed = oppositeCarSpeed;
			temp.textureID = textureRegularCar6;
			break;
		case 7:
			createVehicle(temp.x, temp.y, textureRegularCar7);
			temp.speed = oppositeCarSpeed;
			temp.textureID = textureRegularCar7;
			break;
		case 8:
			createVehicle(temp.x, temp.y, textureRegularCar8);
			temp.speed = oppositeCarSpeed;
			temp.textureID = textureRegularCar8;
			break;
		case 9:
			createVehicle(temp.x, temp.y, textureRegularCar9);
			temp.speed = oppositeCarSpeed;
			temp.textureID = textureRegularCar3;
			break;
	}

	//vehicle creation complete. Now push vehicle information into vector
	vehicleCoordinations.push_back(temp);
	return true;
}

void roadSetup(bool roadIsMoving = true) {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureRoad);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	if (roadIsMoving) {
		if (roadY1 > (maxY * -1)) {
			roadY1 -= roadMoveSpeed;
		} else {
			roadY1 = maxY;
		}
		glPushMatrix();
		glBegin(GL_QUADS);
			glTexCoord2i(0.0, 1.0);
			glVertex2i(minX, roadY1);
			glTexCoord2i(1.0, 1.0);
			glVertex2i(maxX, roadY1);
			glTexCoord2i(1.0, 0.0);
			glVertex2i(maxX, (roadY1 + maxY + 10));
			glTexCoord2i(0.0, 0.0);
			glVertex2i(minX, (roadY1 + maxY + 10));
		glEnd();
		glPopMatrix();

		if (roadY2 > (maxY * -1)) {
			roadY2 -= roadMoveSpeed;
		} else {
			roadY2 = maxY;
		}
		glPushMatrix();
		glBegin(GL_QUADS);
			glTexCoord2i(0.0, 1.0);
			glVertex2i(minX, roadY2);
			glTexCoord2i(1.0, 1.0);
			glVertex2i(maxX, roadY2);
			glTexCoord2i(1.0, 0.0);
			glVertex2i(maxX, (roadY2 + maxY + 10));
			glTexCoord2i(0.0, 0.0);
			glVertex2i(minX, (roadY2 + maxY + 10));
		glEnd();
		glPopMatrix();

		if (roadY3 > (maxY * -1)) {
			roadY3 -= roadMoveSpeed;
		} else {
			roadY3 = maxY;
		}
		glPushMatrix();
		glBegin(GL_QUADS);
			glTexCoord2i(0.0, 1.0);
			glVertex2i(minX, roadY3);
			glTexCoord2i(1.0, 1.0);
			glVertex2i(maxX, roadY3);
			glTexCoord2i(1.0, 0.0);
			glVertex2i(maxX, (roadY3 + maxY + 10));
			glTexCoord2i(0.0, 0.0);
			glVertex2i(minX, (roadY3 + maxY + 10));
		glEnd();
		glPopMatrix();
		glDisable(GL_TEXTURE_2D);
	} else {
		glPushMatrix();
		glBegin(GL_QUADS);
			glTexCoord2i(0.0, 1.0);
			glVertex2i(minX, minY);
			glTexCoord2i(1.0, 1.0);
			glVertex2i(maxX, minY);
			glTexCoord2i(1.0, 0.0);
			glVertex2i(maxX, maxY);
			glTexCoord2i(0.0, 0.0);
			glVertex2i(minX, maxY);
		glEnd();
		glPopMatrix();
		glDisable(GL_TEXTURE_2D);
	}
}

void initiateScreenSetup() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(minX, maxX, minY, maxY);
	textureRoad = LoadTexture("road.bmp");
	texturePlayerCar = LoadTexture("player_car.bmp");
	texturePoliceCar = LoadTexture("v4.bmp");
	textureRegularCar1 = LoadTexture("v1.bmp");
	textureRegularCar2 = LoadTexture("v2.bmp");
	textureRegularCar3 = LoadTexture("v3.bmp");
	textureRegularCar5 = LoadTexture("v5.bmp");
	textureRegularCar6 = LoadTexture("v6.bmp");
	textureRegularCar7 = LoadTexture("v7.bmp");
	textureRegularCar8 = LoadTexture("v8.bmp");
	textureRegularCar9 = LoadTexture("v9.bmp");
	textureLogo = LoadTexture("logo.bmp");
	glEnable(GL_SMOOTH);
}

bool levelComplete = false;
int xCounter = 0;
void updateVehicles() {
	if (currentScreen != GAME) return;
	scoreCard += scoreIncreament;
	if (scoreCard > scoreHigh) {
		scoreHigh = scoreCard;
	}
	for (unsigned int i = 0; i < vehicleCoordinations.size(); i++) {
		vehicleCoordinations[i].y[0] = vehicleCoordinations[i].y[0] - vehicleCoordinations[i].speed;
		vehicleCoordinations[i].y[1] = vehicleCoordinations[i].y[1] - vehicleCoordinations[i].speed;
		vehicleCoordinations[i].y[2] = vehicleCoordinations[i].y[2] - vehicleCoordinations[i].speed;
		vehicleCoordinations[i].y[3] = vehicleCoordinations[i].y[3] - vehicleCoordinations[i].speed;

		if (vehicleCoordinations[i].y[0] < _yy1) {
			scoreCard += scoreIncreament;
			if (scoreCard > scoreHigh) {
				scoreHigh = scoreCard;
			}
		}

		//condition to detect car collusion between two car
		if ((((vehicleCoordinations[i].x[0] + 27) > (_xx1 + 27) && (vehicleCoordinations[i].x[0] + 27) < (_xx2 - 27)) && (vehicleCoordinations[i].y[0] > _yy1 && vehicleCoordinations[i].y[0] < _yy4)) || (((vehicleCoordinations[i].x[1] - 27) > (_xx1 + 27) && (vehicleCoordinations[i].x[1] - 27) < (_xx2 - 27)) && (vehicleCoordinations[i].y[0] > _yy1 && vehicleCoordinations[i].y[0] < _yy4))) {
			currentScreen = END;
			PlaySound(NULL, 0, 0);
			PlaySound(TEXT("car_crushed.wav"), NULL, SND_FILENAME | SND_ASYNC);
			playingGameAudio = false;
		}
		createVehicle(vehicleCoordinations[i].x, vehicleCoordinations[i].y, vehicleCoordinations[i].textureID);
	}
	if(vehicleCoordinations.size() > 0 && vehicleCoordinations[vehicleCoordinations.size() - 1].y[0] < (minY - 100)) {
		vehicleSetupComplete = false;
		vehicleCoordinations.clear();
		//cout << "All car passed!" << endl;
	}
}

void loadGameMode() {
	if (!playingGameAudio) {
		PlaySound(TEXT("street_sound.wav"), NULL, SND_FILENAME | SND_LOOP | SND_ASYNC);
		playingGameAudio = true;
	}

	glPushMatrix();
	roadSetup();
	if (vehicleSetupComplete == false) {
		int lastpos = -1;
		for (int i = maxY, counter = 0; counter < vectorSize; i += oppositeCarDistance, counter++) {
			for (int j = 0; j < 2; j++) {
				int xCor = roll(minX, maxX);
				if ((xCor > lastpos && xCor < lastpos + carWidth) || (xCor + carWidth > lastpos && xCor + carWidth < lastpos + carWidth)) continue;
				if (xCor + carWidth > maxX) continue;
				setupVehicle(xCor, i);
				lastpos = xCor;
			}
		}
		vehicleSetupComplete = true;
	}
	updateVehicles();

	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, texturePlayerCar);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBegin(GL_QUADS);
		glVertex2i(_xx1, _yy1);
		glTexCoord2i(1, 0);
		glVertex2i(_xx2, _yy2);
		glTexCoord2i(1, 1);
		glVertex2i(_xx3, _yy3);
		glTexCoord2i(0, 1);
		glVertex2i(_xx4, _yy4);
		glTexCoord2i(0, 0);
	glEnd();
	glPopMatrix();
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	glPushMatrix();
	glPushAttrib(GL_CURRENT_BIT);
	glColor3f(0.0, 1.0, 0.0);
	string scoring = "Score: ";
	scoring += to_string((int)(round(scoreCard)));
	int bitmapLength = glutBitmapLength(GLUT_BITMAP_HELVETICA_18, (const unsigned char *)scoring.c_str());
	glRasterPos2i(maxX - bitmapLength - 30, maxY - 40);
	for (string::iterator i = scoring.begin(); i != scoring.end(); ++i) {
		char c = *i;
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
	}
	glPopAttrib();
	glPopMatrix();

	glPushMatrix();
	glPushAttrib(GL_CURRENT_BIT);
	glColor3f(1.0, 0.0, 1.0);
	scoring = "High Score: ";
	scoring += to_string((int)(round(scoreHigh)));
	bitmapLength = glutBitmapLength(GLUT_BITMAP_HELVETICA_18, (const unsigned char *)scoring.c_str());
	glRasterPos2i(minX + 30, maxY - 40);
	for (string::iterator i = scoring.begin(); i != scoring.end(); ++i) {
		char c = *i;
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
	}
	glPopAttrib();
	glPopMatrix();

	glPushMatrix();
	glPushAttrib(GL_CURRENT_BIT);
	glColor3f(1.0, 0.0, 0.0);
	scoring = "\"P\" = Pause Game";
	bitmapLength = glutBitmapLength(GLUT_BITMAP_8_BY_13, (const unsigned char *)scoring.c_str());
	glRasterPos2i(maxX -bitmapLength - 20, minY + 40);
	for (string::iterator i = scoring.begin(); i != scoring.end(); ++i) {
		char c = *i;
		glutBitmapCharacter(GLUT_BITMAP_8_BY_13, c);
	}
	glPopAttrib();
	glPopMatrix();

	glPushMatrix();
	glPushAttrib(GL_CURRENT_BIT);
	glColor3f(1.0, 0.0, 0.0);
	scoring = "\"ESC\" = Main Menu";
	bitmapLength = glutBitmapLength(GLUT_BITMAP_8_BY_13, (const unsigned char *)scoring.c_str());
	glRasterPos2i(maxX - bitmapLength - 20, minY + 20);
	for (string::iterator i = scoring.begin(); i != scoring.end(); ++i) {
		char c = *i;
		glutBitmapCharacter(GLUT_BITMAP_8_BY_13, c);
	}
	glPopAttrib();
	glPopMatrix();

	glPushMatrix();
	glPushAttrib(GL_CURRENT_BIT);
	glColor3f(1.0, 0.0, 0.0);
	scoring = "Use arrow keys";
	bitmapLength = glutBitmapLength(GLUT_BITMAP_8_BY_13, (const unsigned char *)scoring.c_str());
	glRasterPos2i(minX + 20, minY + 40);
	for (string::iterator i = scoring.begin(); i != scoring.end(); ++i) {
		char c = *i;
		glutBitmapCharacter(GLUT_BITMAP_8_BY_13, c);
	}
	glPopAttrib();
	glPopMatrix();

	glPushMatrix();
	glPushAttrib(GL_CURRENT_BIT);
	glColor3f(1.0, 0.0, 0.0);
	scoring = "to move the car";
	bitmapLength = glutBitmapLength(GLUT_BITMAP_8_BY_13, (const unsigned char *)scoring.c_str());
	glRasterPos2i(minX + 20, minY + 20);
	for (string::iterator i = scoring.begin(); i != scoring.end(); ++i) {
		char c = *i;
		glutBitmapCharacter(GLUT_BITMAP_8_BY_13, c);
	}
	glPopAttrib();
	glPopMatrix();

	glutSwapBuffers();
	_moveLeftRight = minX;
}

void loadStartMode() {
	if (!playingStartMode) {
		PlaySound(TEXT("soundtrack.wav"), NULL, SND_FILENAME | SND_ASYNC);
		playingStartMode = true;
	}
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	roadSetup();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, textureLogo);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int tempX1 = 20;
	int tempX2 = maxX - 20;
	int tempWidth = tempX2 - tempX1;
	int tempY = (maxY / 2) - 100;
	glBegin(GL_QUADS);
		glVertex2i(tempX1, tempY);
		glTexCoord2i(1, 0);
		glVertex2i(tempX2, tempY);
		glTexCoord2i(1, 1);
		glVertex2i(tempX2, (tempY + tempWidth));
		glTexCoord2i(0, 1);
		glVertex2i(tempX1, (tempY + tempWidth));
		glTexCoord2i(0, 0);
	glEnd();
	glPopMatrix();
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);

	glPushMatrix();
	glPushAttrib(GL_CURRENT_BIT);
	glColor3f(1.0, 1.0, 0.0);
	string s = "Press \"Enter\" to Start Game";
	int bitmapLength = glutBitmapLength(GLUT_BITMAP_HELVETICA_18, (const unsigned char *)s.c_str());
	glRasterPos2i(((maxX / 2) - (bitmapLength / 2)), (maxY / 2) + 60);
	for (string::iterator i = s.begin(); i != s.end(); ++i) {
		char c = *i;
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
	}
	glPopAttrib();
	glPopMatrix();

	glPushMatrix();
	glPushAttrib(GL_CURRENT_BIT);
	glColor3f(0.0, 1.0, 0.0);
	s = "Current High Score: ";
	s += to_string((int)(round(scoreHigh)));
	bitmapLength = glutBitmapLength(GLUT_BITMAP_9_BY_15, (const unsigned char *)s.c_str());
	glRasterPos2i(((maxX / 2) - (bitmapLength / 2)), (maxY / 2) - 20);
	for (string::iterator i = s.begin(); i != s.end(); ++i) {
		char c = *i;
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c);
	}
	glPopAttrib();
	glPopMatrix();

	glPushMatrix();
	glPushAttrib(GL_CURRENT_BIT);
	glColor3f(0.0, 1.0, 1.0);
	s = "Press \"ESC\" to exit";
	bitmapLength = glutBitmapLength(GLUT_BITMAP_8_BY_13, (const unsigned char *)s.c_str());
	glRasterPos2i(((maxX / 2) - (bitmapLength / 2)), (maxY / 2) -100);
	for (string::iterator i = s.begin(); i != s.end(); ++i) {
		char c = *i;
		glutBitmapCharacter(GLUT_BITMAP_8_BY_13, c);
	}
	glPopAttrib();
	glPopMatrix();

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, texturePlayerCar);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int tempPoX = ((maxX / 2) - (carWidth / 2));
	int tempPoY = ((maxY / 2) - (carHeight / 2)) - 200;
	glBegin(GL_QUADS);
		glVertex2i(tempPoX, tempPoY);
		glTexCoord2i(1, 0);
		glVertex2i(tempPoX + carWidth, tempPoY);
		glTexCoord2i(1, 1);
		glVertex2i(tempPoX + carWidth, tempPoY + carHeight);
		glTexCoord2i(0, 1);
		glVertex2i(tempPoX, tempPoY + carHeight);
		glTexCoord2i(0, 0);
	glEnd();
	glPopMatrix();
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);

	glutSwapBuffers();
}

void loadPauseMode() {
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	roadSetup(false);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, textureLogo);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int tempX1 = 20;
	int tempX2 = maxX - 20;
	int tempWidth = tempX2 - tempX1;
	int tempY = (maxY / 2) - 100;
	glBegin(GL_QUADS);
	glVertex2i(tempX1, tempY);
	glTexCoord2i(1, 0);
	glVertex2i(tempX2, tempY);
	glTexCoord2i(1, 1);
	glVertex2i(tempX2, (tempY + tempWidth));
	glTexCoord2i(0, 1);
	glVertex2i(tempX1, (tempY + tempWidth));
	glTexCoord2i(0, 0);
	glEnd();
	glPopMatrix();
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);

	glPushMatrix();
	glPushAttrib(GL_CURRENT_BIT);
	glColor3f(1.0, 1.0, 0.0);
	string s = "Press \"R\" to Resume Game";
	int bitmapLength = glutBitmapLength(GLUT_BITMAP_HELVETICA_18, (const unsigned char *)s.c_str());
	glRasterPos2i(((maxX / 2) - (bitmapLength / 2)), (maxY / 2) + 60);
	for (string::iterator i = s.begin(); i != s.end(); ++i) {
		char c = *i;
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
	}
	glPopAttrib();
	glPopMatrix();

	glPushMatrix();
	glPushAttrib(GL_CURRENT_BIT);
	glColor3f(0.0, 1.0, 0.0);
	s = "Current Score: ";
	s += to_string((int)(round(scoreCard)));
	bitmapLength = glutBitmapLength(GLUT_BITMAP_9_BY_15, (const unsigned char *)s.c_str());
	glRasterPos2i(((maxX / 2) - (bitmapLength / 2)), (maxY / 2) - 20);
	for (string::iterator i = s.begin(); i != s.end(); ++i) {
		char c = *i;
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c);
	}
	glPopAttrib();
	glPopMatrix();

	glPushMatrix();
	glPushAttrib(GL_CURRENT_BIT);
	glColor3f(0.0, 1.0, 1.0);
	s = "Press \"ESC\" to exit";
	bitmapLength = glutBitmapLength(GLUT_BITMAP_8_BY_13, (const unsigned char *)s.c_str());
	glRasterPos2i(((maxX / 2) - (bitmapLength / 2)), (maxY / 2) - 100);
	for (string::iterator i = s.begin(); i != s.end(); ++i) {
		char c = *i;
		glutBitmapCharacter(GLUT_BITMAP_8_BY_13, c);
	}
	glPopAttrib();
	glPopMatrix();

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, texturePlayerCar);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int tempPoX = ((maxX / 2) - (carWidth / 2));
	int tempPoY = ((maxY / 2) - (carHeight / 2)) - 200;
	glBegin(GL_QUADS);
	glVertex2i(tempPoX, tempPoY);
	glTexCoord2i(1, 0);
	glVertex2i(tempPoX + carWidth, tempPoY);
	glTexCoord2i(1, 1);
	glVertex2i(tempPoX + carWidth, tempPoY + carHeight);
	glTexCoord2i(0, 1);
	glVertex2i(tempPoX, tempPoY + carHeight);
	glTexCoord2i(0, 0);
	glEnd();
	glPopMatrix();
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);

	glutSwapBuffers();
}

void loadEndMode() {
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	roadSetup(false);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, textureLogo);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int tempX1 = 20;
	int tempX2 = maxX - 20;
	int tempWidth = tempX2 - tempX1;
	int tempY = (maxY / 2) - 100;
	glBegin(GL_QUADS);
	glVertex2i(tempX1, tempY);
	glTexCoord2i(1, 0);
	glVertex2i(tempX2, tempY);
	glTexCoord2i(1, 1);
	glVertex2i(tempX2, (tempY + tempWidth));
	glTexCoord2i(0, 1);
	glVertex2i(tempX1, (tempY + tempWidth));
	glTexCoord2i(0, 0);
	glEnd();
	glPopMatrix();
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);

	glPushMatrix();
	glPushAttrib(GL_CURRENT_BIT);
	glColor3f(1.0, 0.0, 0.0);
	string s = "Ops! You crushed the car!";
	int bitmapLength = glutBitmapLength(GLUT_BITMAP_HELVETICA_18, (const unsigned char *)s.c_str());
	glRasterPos2i(((maxX / 2) - (bitmapLength / 2)), (maxY / 2) + 60);
	for (string::iterator i = s.begin(); i != s.end(); ++i) {
		char c = *i;
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
	}
	glPopAttrib();
	glPopMatrix();

	glPushMatrix();
	glPushAttrib(GL_CURRENT_BIT);
	glColor3f(0.0, 1.0, 0.0);
	s = "Your Score: ";
	s += to_string((int)(round(scoreCard)));
	bitmapLength = glutBitmapLength(GLUT_BITMAP_9_BY_15, (const unsigned char *)s.c_str());
	glRasterPos2i(((maxX / 2) - (bitmapLength / 2)), (maxY / 2) - 20);
	for (string::iterator i = s.begin(); i != s.end(); ++i) {
		char c = *i;
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c);
	}
	glPopAttrib();
	glPopMatrix();

	glPushMatrix();
	glPushAttrib(GL_CURRENT_BIT);
	glColor3f(1.0, 1.0, 1.0);
	s = "Press \"ENTER\" to restart or \"ESC\" to exit";
	bitmapLength = glutBitmapLength(GLUT_BITMAP_HELVETICA_18, (const unsigned char *)s.c_str());
	glRasterPos2i(((maxX / 2) - (bitmapLength / 2)), (maxY / 2) - 100);
	for (string::iterator i = s.begin(); i != s.end(); ++i) {
		char c = *i;
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
	}
	glPopAttrib();
	glPopMatrix();

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, texturePlayerCar);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int tempPoX = ((maxX / 2) - (carWidth / 2));
	int tempPoY = ((maxY / 2) - (carHeight / 2)) - 200;
	glBegin(GL_QUADS);
	glVertex2i(tempPoX, tempPoY);
	glTexCoord2i(1, 0);
	glVertex2i(tempPoX + carWidth, tempPoY);
	glTexCoord2i(1, 1);
	glVertex2i(tempPoX + carWidth, tempPoY + carHeight);
	glTexCoord2i(0, 1);
	glVertex2i(tempPoX, tempPoY + carHeight);
	glTexCoord2i(0, 0);
	glEnd();
	glPopMatrix();
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);

	glutSwapBuffers();
}

void screenSetup() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW); //Switch to the drawing perspective
	switch(currentScreen) {
		case START:
			loadStartMode();
			break;
		case GAME:
			loadGameMode();
			break;

		case PAUSE:
			loadPauseMode();
			break;

		case END:
			loadEndMode();
			break;
	}
}

void keyboardHandler(int key, int x, int y) {
	switch (key) {
		case GLUT_KEY_DOWN:
			if (currentScreen != GAME) break;
			if ((_yy1 - moveSpeed) < minY || (_yy2 - moveSpeed) < minY || (_yy3 - moveSpeed) < minY || (_yy4 - moveSpeed) < minY) break;
			_yy1 -= moveSpeed;
			_yy2 -= moveSpeed;
			_yy3 -= moveSpeed;
			_yy4 -= moveSpeed;
			break;
		case GLUT_KEY_UP:
			if (currentScreen != GAME) break;
			if ((_yy1 + moveSpeed) > maxY || (_yy2 + moveSpeed) > maxY || (_yy3 + moveSpeed) > maxY || (_yy4 + moveSpeed) > maxY) break;
			_yy1 += moveSpeed;
			_yy2 += moveSpeed;
			_yy3 += moveSpeed;
			_yy4 += moveSpeed;
			break;
		case GLUT_KEY_LEFT:
			if (currentScreen != GAME) break;
			if ((_xx1 - moveSpeed) < minX || (_xx2 - moveSpeed) < minX || (_xx3 - moveSpeed) < minX || (_xx4 - moveSpeed) < minX) break;
			_xx1 -= moveSpeed;
			_xx2 -= moveSpeed;
			_xx3 -= moveSpeed;
			_xx4 -= moveSpeed;
			break;
		case GLUT_KEY_RIGHT:
			if (currentScreen != GAME) break;
			if ((_xx1 + moveSpeed) > maxX || (_xx2 + moveSpeed) > maxX || (_xx3 + moveSpeed) > maxX || (_xx4 + moveSpeed) > maxX) break;
			_xx1 += moveSpeed;
			_xx2 += moveSpeed;
			_xx3 += moveSpeed;
			_xx4 += moveSpeed;
			break;
	}
	glutPostRedisplay();
}

void keyboardHandler(unsigned char key, int x, int y) {
	switch (key) {
		case 13:
			if (currentScreen == START || currentScreen == END) {
				currentScreen = GAME;
				scoreCard = 0;
				vehicleCoordinations.clear();
				_xx1 = _defxx1;
				_yy1 = _defyy1;
				_xx2 = _defxx2;
				_yy2 = _defyy2;
				_xx3 = _defxx3;
				_yy3 = _defyy3;
				_xx4 = _defxx4;
				_yy4 = _defyy4;
				vehicleSetupComplete = false;
			}
			break;
		case 27:
			if (currentScreen == GAME) {
				currentScreen = START;
				playingStartMode = false;
				playingGameAudio = false;
				PlaySound(NULL, 0, 0);
			} else {
				exit(0);
			}
			break;
		case 'P':
		case 'p':
			if (currentScreen == GAME) {
				currentScreen = PAUSE;
				PlaySound(NULL, 0, 0);
				playingGameAudio = false;
			}
			break;
		case 'R':
		case 'r':
			if (currentScreen == PAUSE) {
				currentScreen = GAME;
			}
			break;
	}
}

void updateScreen(int x) {
	roadSetup();
	updateVehicles();
	glutPostRedisplay();
	glutTimerFunc(10, updateScreen, 0);
}

int main(int argc, char** argv) {
	FreeConsole();

	//Initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(maxX, maxY);
	//Create the window
	glutCreateWindow("Highway Road Traffic Rider by Fahomid");
	initiateScreenSetup();

	//Set handler functions
	glutDisplayFunc(screenSetup);
	glutReshapeFunc(reRenderScreen);
	glutTimerFunc(10, updateScreen, 0); //Add a timer
	glutSpecialFunc(keyboardHandler);
	glutKeyboardFunc(keyboardHandler);
	//glutTimerFunc(10, updateScreen, 0);
	glutMainLoop();
	return 0;
}