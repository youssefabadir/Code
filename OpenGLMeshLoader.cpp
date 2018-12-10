#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "TextureBuilder.h"
#include "Model_3DS.h"

#include "GLTexture.h"
#include <glut.h>

#ifdef __linux__
#include <GL/glut.h>
#endif

#ifdef _WIN32
#include <glut.h>
#endif 

#define GLUT_KEY_ESCAPE 27
#define DEG2RAD(a) (a * 0.0174532925)

float printX, printY, xx, yy, zz = 0.5, sx, sy, sz;
float factor = 0.01;
int msk = 1 << 2;
int angle;

double bulletX = 10000;
double bulletZ = 10000;
double cannonX = 1.4;
double cannonZ = 1.4;

float cannonR = -135;


int Angle = 0;
int appear = 0;
int scoreLevel_1[3];
int scoreLevel_2[3];

int counter;

bool moveCannon = false;
bool Leval2 = false;
bool showT1 = true;
bool showSt = true;
bool moving = false;
double moveDown = 1.55;
double building2Y = 0.005;
bool sheild = false;

bool view1 = false;
bool view2 = true;
bool view3 = false;

using std::vector;
using namespace std;

Model_3DS model_house;
Model_3DS model_tree;
Model_3DS model_cannon;
Model_3DS model_ball;
Model_3DS model_circle;
Model_3DS model_building1;
Model_3DS model_building2;
Model_3DS model_sheild;
Model_3DS model_diamond;
Model_3DS model_cup;

GLuint tex;
GLuint tex2;

class Bullet {
public:
	double x;
	double z;
public:
	Bullet::Bullet(double x, double z) {
		x = x;
		z = z;
	}
	Bullet() {
		x = 0;
		z = 0;
	}
};
vector<Bullet> bulletArray;
class Vector3f {
public:
	float x, y, z;

	Vector3f(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) {
		x = _x;
		y = _y;
		z = _z;
	}

	Vector3f operator+(Vector3f v) {
		return Vector3f(x + v.x, y + v.y, z + v.z);
	}

	Vector3f operator-(Vector3f v) {
		return Vector3f(x - v.x, y - v.y, z - v.z);
	}

	Vector3f operator*(float n) {
		return Vector3f(x * n, y * n, z * n);
	}

	Vector3f operator/(float n) {
		return Vector3f(x / n, y / n, z / n);
	}

	Vector3f unit() {
		return *this / sqrt(x * x + y * y + z * z);
	}

	Vector3f cross(Vector3f v) {
		return Vector3f(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}
};
class Camera {
public:
	Vector3f eye, center, up;

	Camera(float eyeX = 2.2, float eyeY = 1.198691, float eyeZ = 2.2,
		float centerX = 1.8, float centerY = 1, float centerZ = 1.8,
		float upX = -0.297031, float upY = 0.899040, float upZ = -0.297031) {
		eye = Vector3f(eyeX, eyeY, eyeZ);
		center = Vector3f(centerX, centerY, centerZ);
		up = Vector3f(upX, upY, upZ);
	}


	void moveX(float d) {
		Vector3f right = up.cross(center - eye).unit();
		eye = eye + right * d;
		center = center + right * d;
	}

	void moveY(float d) {
		eye = eye + up.unit() * d;
		center = center + up.unit() * d;
	}

	void moveZ(float d) {
		Vector3f view = (center - eye).unit();
		eye = eye + view * d;
		center = center + view * d;
	}

	void rotateX(float a) {
		Vector3f view = (center - eye).unit();
		Vector3f right = up.cross(view).unit();
		view = view * cos(DEG2RAD(a)) + up * sin(DEG2RAD(a));
		up = view.cross(right);
		center = eye + view;
	}

	void rotateY(float a) {
		Vector3f view = (center - eye).unit();
		Vector3f right = up.cross(view).unit();
		view = view * cos(DEG2RAD(a)) + right * sin(DEG2RAD(a));
		right = view.cross(up);
		center = eye + view;
	}

	void look() {
		gluLookAt(
			eye.x, eye.y, eye.z,
			center.x, center.y, center.z,
			up.x, up.y, up.z
		);
	}
};

Camera camera;
void shoot(double x, double z) {

	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	if (Leval2)
		glTranslated(x, 0, z + 0.18);
	glTranslated(x - 0.2, 0.25, z - 0.2);
	glScaled(0.003, 0.003, 0.003);
	model_ball.Draw();
	glDisable(GL_TEXTURE_2D);

	glPopMatrix();

}
void drawShoots() {
	for (int i = 0; i < bulletArray.size(); i++) {
		double pointX = bulletArray[i].x;
		double pointZ = bulletArray[i].z;
		shoot(pointX, pointZ);

	}
}

void printText(double x, double y, double z, char *string)
{
	int len, i;
	glRasterPos3f(x, y, z);
	len = (int)strlen(string);
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
	}
}
void print()
{
	printf("%f %f %f %f %f %f %d % d %d %d\n", xx, yy, zz, sx, sy, sz, angle, (msk & (1 << 2)) ? 1 : 0, (msk & (1 << 1)) ? 1 : 0, (msk & (1 << 0)) ? 1 : 0);

}
void printCamera()
{
	printf("camera.eye = Vector3f( %f, %f, %f);\ncamera.center =  Vector3f( %f, %f, %f); \ncamera.up =  Vector3f( %f, %f, %f);\n", camera.eye.x, camera.eye.y, camera.eye.z, camera.center.x, camera.center.y, camera.center.z, camera.up.x, camera.up.y, camera.up.z);
}
void Circle1() {
	//Cylinder

	glPushMatrix();
	GLUquadricObj * qobj;
	qobj = gluNewQuadric();
	gluQuadricDrawStyle(qobj, GLU_LINE);
	glRotated(90, 1, 0, 0);
	glScaled(1, 1, 0.2);
	gluCylinder(qobj, 0.2, 0.2, 0.25, 1000, 1000);
	glPopMatrix();

	//Sphere
	glPushMatrix();
	glScaled(0.0096, 0.001, 0.0096);
	model_circle.Draw();
	glPopMatrix();

}
void Circle2() {
	//Cylinder
	glTranslated(-0.8, 0, 0);
	glPushMatrix();
	glTranslated(-0.8, 0, 0);
	GLUquadricObj * qobj;
	qobj = gluNewQuadric();
	gluQuadricDrawStyle(qobj, GLU_LINE);
	glRotated(90, 1, 0, 0);
	glScaled(1, 1, 0.2);
	gluCylinder(qobj, 0.2, 0.2, 0.25, 1000, 1000);
	glPopMatrix();

	//Sphere
	/*glPushMatrix();
	glScaled(1, 0.01, 1);
	glutSolidSphere(0.2, 100, 100);*/
	glPushMatrix();
	glTranslated(-0.8, 0, 0);
	glScaled(0.0096, 0.001, 0.0096);
	model_circle.Draw();
	glPopMatrix();


}
void drawWall1() {
	glPushMatrix();
	glTranslated(-0.4, -0.025, 0);
	glScaled(0.8, 0.05, 0.3);
	glutSolidCube(1);
	glPopMatrix();
}
void drawWall2() {
	glPushMatrix();
	glTranslated(1.35, -0.025, 0.54);
	glRotated(-45, 0, 1, 0);
	glScaled(1.4, 0.05, 0.3);
	glutSolidCube(1);
	glPopMatrix();

}
void First_Building() {
	//First building
	glPushMatrix();
	glTranslated(0, 0.7, 0);
	glScaled(0.02, 0.02, 0.02);
	glRotated(Angle, 0, 1, 0);
	model_building1.Draw();
	glPopMatrix();
}
void Second_Building() {
	glPushMatrix();
	glTranslated(-2.4, 0.02, 0.12);
	glScaled(0.008, building2Y, 0.008);
	model_building2.Draw();
	glPopMatrix();
}
void setupLights() {
	GLfloat ambient[] = { 0.7f, 0.7f, 0.7, 1.0f };
	GLfloat diffuse[] = { 0.6f, 0.6f, 0.6, 1.0f };
	GLfloat specular[] = { 1.0f, 1.0f, 1.0, 1.0f };
	GLfloat shininess[] = { 50 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

	GLfloat lightIntensity[] = { 0.7f, 0.7f, 1, 1.0f };
	GLfloat lightPosition[] = { -7.0f, 6.0f, 3.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, lightIntensity);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightIntensity);
}
void setupCamera() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, 640 / 480, 0.001, 100);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	camera.look();
}
void drawCannon() {
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);

	glTranslated(cannonX, 0.15, cannonZ);
	glRotated(cannonR, 0, 1, 0);
	glRotated(90, 1, 0, 0);
	glScaled(0.005, 0.005, 0.005);
	model_cannon.Draw();
	glDisable(GL_TEXTURE_2D);

	glPopMatrix();
}
void drawBackG() {
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	GLUquadricObj * qobj;
	qobj = gluNewQuadric();
	glTranslated(50, 50, 50);
	glRotated(90, 1, 0, 1);
	glBindTexture(GL_TEXTURE_2D, tex);
	gluQuadricTexture(qobj, true);
	gluQuadricNormals(qobj, GL_SMOOTH);
	gluSphere(qobj, 100, 100, 100);
	gluDeleteQuadric(qobj);
	glEnable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}
void drawBackG2() {
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	GLUquadricObj * qobj;
	qobj = gluNewQuadric();
	glTranslated(50, 30, 30);
	glRotated(90, 1, 0, 1);
	glBindTexture(GL_TEXTURE_2D, tex2);
	gluQuadricTexture(qobj, true);
	gluQuadricNormals(qobj, GL_SMOOTH);
	gluSphere(qobj, 100, 100, 100);
	gluDeleteQuadric(qobj);
	glEnable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}
void LoadAssets()
{
	// Loading Model files
	model_house.Load("Models/house/house.3DS");
	model_tree.Load("Models/tree/Tree1.3ds");
	model_cannon.Load("Models/Cannon/cannon.3ds");
	model_ball.Load("Models/ball/ball.3ds");
	model_circle.Load("Models/sphere/sphere.3ds");
	model_building1.Load("Models/building 1/building1.3ds");
	model_building2.Load("Models/building2/building2.3ds");
	model_diamond.Load("Models/diamond/star.3ds");
	model_cup.Load("Models/cup/star.3ds");
	model_sheild.Load("Models/sheild/sheild.3ds");

	// Loading texture files
	//tex_ground.Load("Textures/ground.bmp");
	loadBMP(&tex2, "Textures/sunset.bmp", true);
	loadBMP(&tex, "Textures/blu-sky-3.bmp", true);

}
void drawBullet() {
	drawShoots();
	shoot(bulletX, bulletZ);
}
void drawTexture() {
	if (Leval2) {
		drawBackG2();
	}
	else {
		drawBackG();
	}
}
void drawTowers() {
	if (Leval2) {
		Second_Building();
	}
	else {
		if (showT1) {
			First_Building();
		}
	}
}
void drawDiamond() {
	if (showSt) {
		glPushMatrix();
		if (showT1) {
			glTranslated(0, 1.55, 0);
		}
		else {
			glTranslated(0, moveDown, 0);

		}

		glScaled(0.0006, 0.0006, 0.0006);
		glRotated(Angle, 0, 1, 0);
		model_diamond.Draw();
		glPopMatrix();
	}
}
void drawCup() {
	if (showSt) {
		glPushMatrix();
		if (showT1) {
			glTranslated(0, 1.55, 0);
		}
		else {
			glTranslated(0, moveDown, 0);

		}

		glScaled(0.0006, 0.0006, 0.0006);
		glRotated(Angle, 0, 1, 0);
		model_diamond.Draw();
		glPopMatrix();
	}
}
void drawSheild(double ssx, double ssy) {
	if (!Leval2 && !moveCannon) {
		glPushMatrix();
		glTranslated(0.14, 0.04, 0.15);
		glScaled(ssx, ssy, ssx);
		glRotated(40, 0, 1, 0);
		model_sheild.Draw();
		glPopMatrix();
	}
}
void Score() {
	if (view2) {
		char* p1s[20];
		sprintf((char *)p1s, "%d", scoreLevel_1[1]);
		printText(0.8, 1.5, -1.5, (char *)p1s);
	}
	if (view1) {
		char* p1s[20];
		sprintf((char *)p1s, "%d", scoreLevel_1[1]);
		printText(0.8, 0.6, 0.1, (char *)p1s);
	}
	if (view3) {
		char* p1s[20];
		sprintf((char *)p1s, "%d", scoreLevel_1[1]);
		printText(0, 0.2, -2.5, (char *)p1s);
	}
	if (view2 && Leval2) {
		char* p1s[20];
		sprintf((char *)p1s, "%d", scoreLevel_1[1]);
		printText(0.8, 1.5, -1.5, (char *)p1s);
	}
	if (view1 && Leval2) {
		char* p1s[20];
		sprintf((char *)p1s, "%d", scoreLevel_1[1]);
		printText(0.8, 0.6, 0.1, (char *)p1s);
	}
	if (view3 && Leval2) {
		char* p1s[20];
		sprintf((char *)p1s, "%d", scoreLevel_1[1]);
		printText(0, 0.2, -2.5, (char *)p1s);
	}
}
void Display() {
	setupCamera();
	setupLights();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
	glScaled(1.5, 1.5, 1.5);
	Circle1();
	//drawWall1();
	Circle2();
	drawWall2();
	glPopMatrix();

	Score();

	//	The player
	drawCannon();

	//	The background

	drawTexture();

	//	The Buildings
	drawTowers();

	//	 The bullet 
	drawBullet();

	//	The diamond
	drawDiamond();

		if (sheild) {
			drawSheild(0.0012, 0.017);
		}
		else {
			drawSheild(0, 0);

		}
		glFlush();
}

void Keyboard(unsigned char key, int x, int y) {
	float d = 0.03;

	switch (key) {
	case 'w':
		camera.moveY(d);
		break;
	case 's':
		camera.moveY(-d);
		break;
	case 'a':
		camera.moveX(d);
		break;
	case 'd':
		camera.moveX(-d);
		break;
	case 'q':
		camera.moveZ(d);
		break;
	case 'e':
		camera.moveZ(-d);
		break;
	case'x': {
		if (!moving && !moveCannon) {
			Bullet bullet;
			bullet.x = cannonX;
			bullet.z = cannonZ;
			bulletArray.push_back(bullet);
			PlaySound(TEXT("sounds/laser.wav"), NULL, SND_ASYNC | SND_FILENAME);
			break;
		}
	}
	case'p':
		printCamera();
		break;
	case'm':
		moveCannon = !moveCannon;
		break;
	case'l':
		Leval2 = true;
		break;
		//Cannon view
	case'1':
		if (Leval2) {
			camera.eye = Vector3f(-0.238113, 0.252845, -0.02);
			camera.center = Vector3f(-1.232665, 0.202585, 0);
			camera.up = Vector3f(-0.077719, 0.941384, 0);
		}
		else {
			view1 = true;
			view2 = false;
			view3 = false;
			camera.eye = Vector3f(1.175055, 0.230940, 1.175055);
			camera.center = Vector3f(0.470695, 0.142894, 0.470695);
			camera.up = Vector3f(-0.062258, 0.996117, -0.062258);
		}
		break;
		//Normal view
	case'2':
		if (Leval2) {
			camera.eye = Vector3f(1, 0.7, 0);
			camera.center = Vector3f(-0.403678, 0.367368, 0.0);
			camera.up = Vector3f(-0.077719, 0.941384, 0);
		}
		else {
			view1 = false;
			view2 = true;
			view3 = false;
			camera.eye = Vector3f(2.2, 1.198691, 2.2);
			camera.center = Vector3f(1.8, 1, 1.8);
			camera.up = Vector3f(-0.297031, 0.899040, -0.297031);
		}

		break;
		//Top view
	case'3':
		if (Leval2) {
			camera.eye = Vector3f(-1.478001, 3.826365, 1.043186);
			camera.center = Vector3f(-1.364208, 2.912856, 0.652606);
			camera.up = Vector3f(-0.761961, 0.172035, -0.624355);
		}
		else {
			view1 = false;
			view2 = false;
			view3 = true;
			camera.eye = Vector3f(0.713336, 3.862442, 0.685207);
			camera.center = Vector3f(0.568973, 2.877508, 0.589999);
			camera.up = Vector3f(-0.761961, 0.172035, -0.624355);
		}

		break;

	case GLUT_KEY_ESCAPE:
		exit(EXIT_SUCCESS);
	}

	glutPostRedisplay();
}
void Special(int key, int x, int y) {
	float a = 1.0;

	switch (key) {
	case GLUT_KEY_UP:
		camera.rotateX(a);
		break;
	case GLUT_KEY_DOWN:
		camera.rotateX(-a);
		break;
	case GLUT_KEY_LEFT:
		camera.rotateY(a);
		break;
	case GLUT_KEY_RIGHT:
		camera.rotateY(-a);
		break;
	}

	glutPostRedisplay();
}
void actM(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && !moving && !moveCannon)
	{
		Bullet bullet;
		bullet.x = cannonX;
		bullet.z = cannonZ;
		bulletArray.push_back(bullet);
	}
	glutPostRedisplay();
}

//	Bullets time
void time(int val) {
		counter++;
		if (Leval2) {
			moveCannon = false;
			cannonX = 0;
			cannonZ = 0;
			cannonR = -90;
		}
	int i = 0;
	while (i < bulletArray.size()) {
		if (Leval2) {
			bulletArray[i].x = bulletArray[i].x - 0.1;
			if (bulletArray[i].x < -1) {
				bulletArray[i].x = 10000000;
			}
		}
		else {
			bulletArray[i].x = bulletArray[i].x - 0.1;
			bulletArray[i].z = bulletArray[i].z - 0.1;

			if (bulletArray[i].x < 0 && bulletArray[i].z < 0) {
				bulletArray[i].x = 10000000;
				bulletArray[i].z = 10000000;
				scoreLevel_1[1] -= 1;

				if (scoreLevel_1[1] == 0) {
					showT1 = false;
				}
				if (scoreLevel_1[1] == -1) {
					moveCannon = true;
					showSt = false;
				}
			}
		}
		i++;
	}
	if (!showT1 && (moveDown > 0.2)) {
		moving = true;
		moveDown -= 0.01;
	}
	if (moveDown <= 0.2) {
		moving = false;
	}
	if ((counter >= 100) && (counter < 200)) {
		sheild = true;
	}
	else {
		sheild = false;
	}
	if (counter >= 300) {
		counter = 0;
	}
	Angle += 3;

	glutPostRedisplay();
	glutTimerFunc(10, time, 0);
}
void moveCannonTime(int val) {
	if (moveCannon) {
		cannonX -= 0.01;
		cannonZ -= 0.01;
		camera.moveZ(0.015);
		camera.moveY(0.001);
		if (cannonX <= 0 && cannonZ <= 0) {
			camera.eye = Vector3f(1, 0.7, 0);
			camera.center = Vector3f(-0.403678, 0.367368, 0.0);
			camera.up = Vector3f(-0.077719, 0.941384, 0);
			Leval2 = true;
			return;
		}
	}

	glutPostRedisplay();
	glutTimerFunc(100, moveCannonTime, 0);
}
void moveB2(int val) {
	building2Y += 0.0005;
	if (building2Y >= 0.013) {
		building2Y = 0.005;
	}
	glutPostRedisplay();
	glutTimerFunc(100, moveB2, 0);
}

void timeAll(int val) {
	time(val);
	moveCannonTime(val);
	moveB2(val);
}
int main(int argc, char** argv) {
	glutInit(&argc, argv);
	scoreLevel_1[1] = 10;
	scoreLevel_1[2] = 50;

	glutInitWindowSize(640, 480);
	glutInitWindowPosition(50, 50);

	glutCreateWindow("Final Project");
	glutDisplayFunc(Display);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(actM);
	glutSpecialFunc(Special);
	glutTimerFunc(0, timeAll, 0);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);


	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);

	glShadeModel(GL_SMOOTH);

	LoadAssets();


	glutMainLoop();

}
