#include <GL/gl.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>
#define PI 3.14159265
#define P2 PI/2
#define P3 3*PI/2
#define DR 0.0174533 // One degree in radians

float px,py,pdx,pdy,pa; // Player position

void drawPlayer()
{
	glColor3f(1,1,0);
	glPointSize(8);
	glBegin(GL_POINTS);
	glVertex2i(px,py);
	glEnd();
	
	glLineWidth(3);
	glBegin(GL_LINES);
	glVertex2i(px,py);
	glVertex2i(px+pdx*5, py+pdy*5);
	glEnd();
}

int mapX=8,mapY=8,mapS=64;
int map[]=
{
	1,1,1,1,1,1,1,1,
	1,0,1,0,0,0,0,1,
	1,0,1,0,0,0,0,1,
	1,0,1,0,0,0,0,1,
	1,0,0,0,0,0,0,1,
	1,0,0,0,0,1,0,1,
	1,0,0,0,0,0,0,1,
	1,1,1,1,1,1,1,1
};

void drawMap2D()
{
	int x,y,xo,yo;
	for(y=0;y<mapY;y++)
	{
		for(x=0;x<mapX;x++)
		{
			if(map[y*mapX+x]==1){
				glColor3f(1,1,1);
			}
			else {
				glColor3f(0,0,0);
			}
			xo=x*mapS; yo=y*mapS;
			glBegin(GL_QUADS);
			glVertex2i(xo+1, yo+1);
			glVertex2i(xo+1, yo+mapS-1);
			glVertex2i(xo+mapS-1, yo+mapS-1);
			glVertex2i(xo+mapS-1, yo+1);
			glEnd();
		}
	}
}

float dist(float ax,float ay, float bx, float by, float ang0)
{
	return(sqrt((bx-ax)*(bx-ax)+(by-ay)*(by-ay)));
}

void drawRays2D()
{
	int r,mx,my,mp,dof; float rx,ry,ra,xo,yo,disT;
	ra=pa-DR*30;
	if(ra<0){
		ra+=2*PI;
	}
	if(ra>2*PI){
		ra-=2*PI;
	}
	for(r=0; r<60; 	r++)
	{
		// Check horizontal lines
		dof=0;
		float disH=1000000, hx=px, hy=py;
		float aTan=-1/tan(ra);
		if(ra>PI){
			ry=(((int)py>>6)<<6)-0.0001;	//
			rx=(py-ry)*aTan+px; yo=-64;		// Looking up
			xo=-yo*aTan;					//
		}
		if(ra<PI){
			ry=(((int)py>>6)<<6);	//
			rx=(py-ry)*aTan+px;		// Looking up
			yo= 64; xo=-yo*aTan;	//
		}
		if(ra==0 || ra==PI){	//
			rx=px; ry=py;		// Looking straight left or right
			dof=8;				//
		} 
		while(dof<8)
		{
			mx=(int)(rx)>>6; my=(int)(ry)>>6; mp=my*mapX+mx;
			if(mp>0 && mp<mapX*mapY && map[mp]>0){
				hx=rx; hy=ry;						//
				disH=dist(px,py,hx,hy,ra);			// Hit wall
				dof =8;								//
			} 
			else {
				rx+=xo;	//
				ry+=yo;	// Next line
				dof+=1;	//
			}
		}
		
		// Check vertical lines
		dof=0;
		float disV=1000000, vx=vx, vy=vy;
		float nTan=-tan(ra);
		if(ra>P2 && ra<P3){
			rx=(((int)px>>6)<<6)-0.0001;	//
			ry=(px-rx)*nTan+py; xo=-64;		// Looking left
			yo=-xo*nTan;					//
		}
		if(ra<P2 || ra>P3){
			rx=(((int)px>>6)<<6);		//
			ry=(px-rx)*nTan+py; xo= 64;	// Looking right
			yo=-xo*nTan;				//
		}
		if(ra==0 || ra==PI){
			rx=px;				//
			ry=py;				// Looking straight up or down
			dof=8;				//
		}
		while(dof<8)
		{
			mx=(int)(rx)>>6; my=(int)(ry)>>6; mp=my*mapX+mx;
			if(mp>0 && mp<mapX*mapY && map[mp]>0){
				vx=rx; vy=ry;						//
				disV=dist(px,py,hx,hy,ra);			// Hit wall
				dof =8;								//
			}
			else {
				rx+=xo;	//
				ry+=yo;	// Next line
				dof+=1;	//
			}
		}
		if(disV<disH){
			rx=vx; ry=vy;		//
			disT=disV;			// Vertical wall hit
			glColor3f(0.9,0,0); //
		}
		if(disH<disV){
			rx=hx; ry=hy;		//
			disT=disH;			// Horizontal wall hit
			glColor3f(0.7,0,0);	//
		}
		glColor3f(1,0,0);
		glLineWidth(3);
		glBegin(GL_LINES);
		glVertex2i(px,py);
		glVertex2i(rx,ry);
		glEnd();
		
		// Draw 3D walls
		float ca=pa-ra;
		if(ca<0){
			ca+=2*PI;
		}
		if(ca>2*PI){
			ca-=2*PI;
		}
		disT=disT*cos(ca); // Fix fisheye
		float lineH=(mapS*320)/disT;
		if(lineH>320){
			lineH=320;	// Line height
		}
		float lineO=160-lineH/2;	// Line offset
		glLineWidth(8);
		glBegin(GL_LINES);
		glVertex2i(r*8+530,lineO);
		glVertex2i(r*8+530,lineH+lineO);
		glEnd();
		
		ra+=DR;
		if(ra<0){
			ra+=2*PI;
		}
		if(ra>2*PI){
			ra-=2*PI;
		}
	}
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	drawMap2D();
	drawRays2D();
	drawPlayer();
	glutSwapBuffers();
}

void buttons(unsigned char key, int x, int y)
{
	if(key=='a'){
		pa-=0.1;
		if(pa<0){
			pa+=2*PI;
		}
		pdx=cos(pa)*5;
		pdy=sin(pa)*5;
	}
	if(key=='d'){
		pa+=0.1;
		if(pa>2*PI){
			pa-=2*PI;
		}
		pdx=cos(pa)*5;
		pdy=sin(pa)*5;
	}
	if(key=='w'){
		px+=pdx;
		py+=pdy;
	}
	if(key=='s'){
		px-=pdx;
		py-=pdy;
	}
	glutPostRedisplay();
}

void init()
{
	glClearColor(0.3,0.3,0.3,0);
	gluOrtho2D(0,1024,512,0);
	px=300; py=300; pdx=cos(pa); pdy=sin(pa)*5;
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(1024, 512);
	glutCreateWindow("FPS Mazing");
	init();
	glutDisplayFunc(display);
	glutKeyboardFunc(buttons);
	glutMainLoop();
}
