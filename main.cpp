#include<stdio.h>
#include<unistd.h>
#include<GL/gl.h>
#include<GL/glut.h>
#include<stdlib.h>
#include<math.h>
#define PI 3.141592654

//length of the stick

int lstick=150;

int score=0; //to check if ball went inside or not

const int screenWidth=640;
const int screenHeight=420;

//radius of the ball

float rball=9.0;

//radius of the hole
float rhole=11.0;
float cal=1.732*rball;

//xy coordinates of start and end of the stick

float xcf,ycf,xcb,ycb;

int wallcoll=0;  //detect collision with wall;

int iw;

float xm,ym;  //x mouse y mouse
float xw=140.0,yw=210.0; //xwhite ywhite
float smd=4.0; //stick moved distance

int b1=0;//mouse button detection
int b2=0;//checks transition from mouse button down to mouse button up

float xmi,ymi; //capture initial mouse coordinates
float xmc,ymc; //capture current mouse coordinates
float maxsmd=60.0; //maximum moved distance

float xsi,ysi;//initial coordinates of stick before release

float xp1=390.0,yp1=210.0;//center of purple 1
float diswincx,diswincy; //main incx,incy
float swx,swy,spx,spy;


float wcounterrate;//wcounterrate defined by distance of cuestick

int flagwhite=0,flagpurple=0; //check

float changepx,changepy,changewx,changewy; //how to change white and purple after collision
struct GLPoint
{
    GLint x,y;
};

typedef struct GLPoint point;

//coordinates of the table

point cornerMain[2],cornerGreen[2];



void myInit(void)
{
    glClearColor(1.0,1.0,1.0,0.0);
    glColor3f(0.0f,0.0f,0.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0,(GLdouble)screenWidth,0.0,(GLdouble)screenHeight);
}


// general algorithm to dracircle defined bt number of segments

void drawCircle(float cx, float cy, float r, int num_segments)
{
	float theta = 2 * 3.1415926 / float(num_segments);
	float c = cosf(theta);//precalculate the sine and cosine
	float s = sinf(theta);
	float t;

	float x = r;//we start at angle = 0
	float y = 0;

	glBegin(GL_TRIANGLE_FAN);
	for(int ii = 0; ii < num_segments; ii++)
	{
		glVertex2f(x + cx, y + cy);//output vertex

		//apply the rotation matrix
		t = x;
		x = c * x - s * y;
		y = s * t + c * y;
	}
	glEnd();
}

//find distance between two points

float finddistance(float xi,float yi,float xf,float yf)
{
    return sqrt((xf-xi)*(xf-xi)+(yf-yi)*(yf-yi));
}

//draw stick

void drawStick()
{
    double theta_cs; //angle between mouse pointer and the stick
    if(xm==xw && ym>yw)
    {
        xcf=xw;
        ycf=yw-rball;
        xcb=xw;
        ycb=ycf-lstick;
    }
    else if(xm==xw && ym<yw)
    {
        xcf=xw;
        ycf=yw+rball;
        xcb=xw;
        ycb=ycf+lstick;
    }
    else
    {
        theta_cs=atan((double)(fabs(ym-yw)/fabs(xm-xw))); //angle of cue stick
        if(xm<xw && ym<yw)
        {
            xcf=xw+rball*cos(theta_cs);
            ycf=yw+rball*sin(theta_cs);
            xcb=xcf+lstick*cos(theta_cs);
            ycb=ycf+lstick*sin(theta_cs);
        }
        if(xm<xw && ym>yw)
        {
            xcf=xw+rball*cos(theta_cs);
            ycf=yw-rball*sin(theta_cs);
            xcb=xcf+lstick*cos(theta_cs);
            ycb=ycf-lstick*sin(theta_cs);
        }
        if(xm>xw && ym<yw)
        {
            xcf=xw-rball*cos(theta_cs);
            ycf=yw+rball*sin(theta_cs);
            xcb=xcf-lstick*cos(theta_cs);
            ycb=ycf+lstick*sin(theta_cs);
        }
        if(xm>xw && ym>yw)
        {
            xcf=xw-rball*cos(theta_cs);
            ycf=yw-rball*sin(theta_cs);
            xcb=xcf-lstick*cos(theta_cs);
            ycb=ycf-lstick*sin(theta_cs);
        }
    }

    glBegin(GL_LINES);
        glVertex2i(xcf,ycf);
        glVertex2i(xcb,ycb);
    glEnd();


}

//draw stick when moved away from the ball

void movedFarStick()
{
    if((finddistance(xcf,ycf,xw,yw))<=maxsmd)
    {
        double theta_cs;        //angle of cue stick
        if(xw==xmi && yw>ymi)
        {
            xcf=xw;
            ycf=ycf+smd;
            xcb=xw;
            ycb=ycb+smd;
        }
        else if(xw==xmi && yw<ymi)
        {
            xcf=xw;
            ycf=ycf-smd;
            xcb=xw;
            ycb=ycb-smd;
        }
        else
        {
            theta_cs=atan((double)(fabs(ycf-ycb)/fabs(xcf-xcb))); //angle of cue stick
            if(xmi<xw && ymi<yw)
            {
                xcf=xcf+smd*cos(theta_cs);
                ycf=ycf+smd*sin(theta_cs);
                xcb=xcb+smd*cos(theta_cs);
                ycb=ycb+smd*sin(theta_cs);
            }
            if(xmi<xw && ymi>yw)
            {
                xcf=xcf+smd*cos(theta_cs);
                ycf=ycf-smd*sin(theta_cs);
                xcb=xcb+smd*cos(theta_cs);
                ycb=ycb-smd*sin(theta_cs);
            }
            if(xmi>xw && ymi<yw)
            {
                xcf=xcf-smd*cos(theta_cs);
                ycf=ycf+smd*sin(theta_cs);
                xcb=xcb-smd*cos(theta_cs);
                ycb=ycb+smd*sin(theta_cs);
            }
            if(xmi>xw && ymi>yw)
            {
                xcf=xcf-smd*cos(theta_cs);
                ycf=ycf-smd*sin(theta_cs);
                xcb=xcb-smd*cos(theta_cs);
                ycb=ycb-smd*sin(theta_cs);
            }
        }

        glBegin(GL_LINES);
            glVertex2i(xcf,ycf);
            glVertex2i(xcb,ycb);
        glEnd();
    }
    else
    {

        glBegin(GL_LINES);
        glVertex2i(xcf,ycf);
        glVertex2i(xcb,ycb);
        glEnd();

    }

}


//draw stick when moved close to the ball


void movedCloseStick()
{
    if((finddistance(xcf,ycf,xw,yw))>=rball)
    {
        double theta_cs;
        if(xw==xsi && yw>ysi)
        {
            xcf=xw;
            ycf=ycf+smd;
            xcb=xw;
            ycb=ycb+smd;
        }
        else if(xw==xsi && yw<ysi)
        {
            xcf=xw;
            ycf=ycf-smd;
            xcb=xw;
            ycb=ycb-smd;
        }
        else
        {
            theta_cs=atan((double)(fabs(ycf-ycb)/fabs(xcf-xcb))); //angle of cue stick
            if(xsi<xw && ysi<=yw)
            {
                xcf=xcf+smd*cos(theta_cs);
                ycf=ycf+smd*sin(theta_cs);
                xcb=xcb+smd*cos(theta_cs);
                ycb=ycb+smd*sin(theta_cs);
            }
            if(xsi<xw && ysi>yw)
            {
                xcf=xcf+smd*cos(theta_cs);
                ycf=ycf-smd*sin(theta_cs);
                xcb=xcb+smd*cos(theta_cs);
                ycb=ycb-smd*sin(theta_cs);
            }
            if(xsi>xw && ysi<=yw)
            {
                xcf=xcf-smd*cos(theta_cs);
                ycf=ycf+smd*sin(theta_cs);
                xcb=xcb-smd*cos(theta_cs);
                ycb=ycb+smd*sin(theta_cs);
            }
            if(xsi>xw && ysi>yw)
            {
                xcf=xcf-smd*cos(theta_cs);
                ycf=ycf-smd*sin(theta_cs);
                xcb=xcb-smd*cos(theta_cs);
                ycb=ycb-smd*sin(theta_cs);
            }
        }

        glBegin(GL_LINES);
            glVertex2i(xcf,ycf);
            glVertex2i(xcb,ycb);
        glEnd();
    }
    else
    {

        glBegin(GL_LINES);
        glVertex2i(xcf,ycf);
        glVertex2i(xcb,ycb);
        glEnd();
        glutPostRedisplay();

    }
}

//set the speed of the white ball when struck by the cue stick

void setSpeedswxswy(){
    if(wallcoll==0)
        {
            if(xw==xsi && yw>ysi)
            {
                swx=0;
            }
            else
            if(xw==xsi && yw<ysi)
            {
                swx=0;
                swy=-swy;
            }
            else
            {
                if(xsi<xw && ysi==yw)
                {
                    swy=0;
                }else
                if(xsi<xw && ysi>yw)
                {
                    swy=-swy;
                }else
                if(xsi>xw && ysi<yw)
                {
                    swx=-swx;
                }else
                if(xsi>xw && ysi==yw)
                {
                    swy=0;
                    swx=-swx;
                }else
                if(xsi>xw && ysi>yw)
                {

                    swx=-swx;
                    swy=-swy;
                }
            }
        }
}

//draw white ball moving

void drawMovingWhite()
{
        double theta_cs;
        if(wallcoll==0)
        {
            if(xw==xsi && yw>ysi)
            {
                diswincy=(swy/iw)*100000;
                yw=yw+diswincy;
            }
            else if(xw==xsi && yw<ysi)
            {
                diswincy=(swy/iw)*100000;
                yw=yw+diswincy;
            }
            else
            {
                theta_cs=atan((double)(fabs(ysi-yw)/fabs(xsi-xw))); //angle of cue stick

                diswincx=(swx/iw)*100000;
                diswincy=(swy/iw)*100000;
                xw=xw+diswincx*cos(theta_cs);
                yw=yw+diswincy*sin(theta_cs);
            }
        }
        else{
                diswincx=(swx/iw)*100000;
                diswincy=(swy/iw)*100000;
                xw=xw+diswincx;
                yw=yw+diswincy;
            }
}


//change coordinates of the purple and white ball after collision with respect to the angle of collision

void coll1(int i)
{
    double theta_wp;
    theta_wp=atan((double)(fabs(yw-yp1)/fabs(xw-xp1)));
    changepx=(spx/i)*100000;
    changepy=(spy/i)*100000;
    changewx=(swx/i)*100000;
    changewy=(swy/i)*100000;
    xp1=xp1+changepx*1.5*sin(theta_wp);
    yp1=yp1+changepy*1.5*sin(theta_wp);
    xw=xw+changewx*0.5*sin(theta_wp);
    yw=yw+changewy*0.5*cos(theta_wp);
}

//change velocity of the purple and white ball after collision with respect to the angle of collision

void changeSpeedwhitepurple()
{
    if(xp1==xw && yp1>yw)
    {
        spx=0;
        swx=0;
        swx=-swx;
    }
    else
    if(xp1==xw && yp1<yw)
    {
        spx=0;
        swx=0;
        spx=-spx;
    }
    else
    if(xp1<xw && yp1==yw)
    {
        spy=0;
        swy=0;
        spx=-spx;
    }
    else
    if(xp1>xw && yp1==yw)
    {
        spy=0;
        swy=0;
        swx=-swx;
    }
    else
    if(xp1>xw && yp1>yw)
    {
        swx=-swx;
        swy=-swy;
    }
    else
    if(xp1>xw && yp1<yw)
    {
        swx=-swx;
        spy=-spy;
    }
    else
    if(xp1<xw && yp1<=yw)
    {
        spx=-spx;
        spy=-spy;
    }
    else
    if(xp1<xw && yp1>yw)
    {
        swy=-swy;
        swx=-swx;
    }
}

//check when purple and white ball inside the hole

int insideHole()
{
    if((finddistance(xw,yw,10,10)<=15) || (finddistance(xw,yw,320,10)<=15) || (finddistance(xw,yw,630,10)<=15) || (finddistance(xw,yw,10,410)<=15) || (finddistance(xw,yw,320,410)<=15) || (finddistance(xw,yw,630,410)<=15) || (finddistance(xp1,yp1,10,10)<=15) || (finddistance(xp1,yp1,320,10)<=15) || (finddistance(xp1,yp1,630,10)<=15) || (finddistance(xp1,yp1,10,410)<=15) || (finddistance(xp1,yp1,320,410)<=15) || (finddistance(xp1,yp1,630,410)<=15))
    {
        return 1;
    }
    else{
        return 0;
    }
}

//display function

void myDisplay(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0.4f,0.2f,0.0f);

    //draw table
    glBegin(GL_QUADS);
    glVertex2i(cornerMain[0].x,cornerMain[0].y);
    glVertex2i(cornerMain[0].x,cornerMain[1].y);
    glVertex2i(cornerMain[1].x,cornerMain[1].y);
    glVertex2i(cornerMain[1].x,cornerMain[0].y);
    glEnd();
    glColor3f(0.0f,0.6f,0.0f);
    glBegin(GL_QUADS);
    glVertex2i(cornerGreen[0].x,cornerGreen[0].y);
    glVertex2i(cornerGreen[0].x,cornerGreen[1].y);
    glVertex2i(cornerGreen[1].x,cornerGreen[1].y);
    glVertex2i(cornerGreen[1].x,cornerGreen[0].y);
    glEnd();

    //draw holes

    glColor3f(0.0f,0.0f,0.0f);
    drawCircle(10.0,410.0,rhole,50);
    drawCircle(10.0,10.0,rhole,50);
    drawCircle(630.0,410.0,rhole,50);
    drawCircle(630.0,10.0,rhole,50);
    drawCircle(320.0,410.0,rhole,50);
    drawCircle(320.0,10.0,rhole,50 );


    //draw purple ball

    glColor3f(0.6f,0.0f,0.29f);
    drawCircle(xp1,yp1,rball,50 );

    //conditions of white and purple ball collidomg with the wall

    if(xw<rball+10|| xw>640-rball-10){
        wallcoll=1;
        swx = -swx;
    }
    if(xp1<rball+10 || xp1>640-rball-10){
        wallcoll=1;
        spx = -spx;
    }
    if(yw<rball+10 || yw>420-rball-10){
        wallcoll=1;
        swy = -swy;
    }
    if(yp1<rball+10 || yp1>420-rball-10){
        wallcoll=1;
        spy = -spy;
    }


/*collision of purple and white ball*/

    if(finddistance(xw,yw,xp1,yp1)<=rball)
    {
        spx=0.0005;
        spy=0.0005;
        swx=0.0005;
        swy=0.0005;
        changeSpeedwhitepurple();
        flagpurple=1;

    }

    if(flagpurple==1)
    {
        glColor3f(1.0f,1.0f,1.0f);
        drawCircle(xw,yw,rball,50 );
        glColor3f(0.6f,0.0f,0.29f);
        drawCircle(xp1,yp1,rball,50 );
    }

    //check if any of the ball inside the hole

    if(insideHole()==1)
    {
        score=1;
        glColor3f(0.0f,0.0f,0.0f);
        drawCircle(xw,yw,rball,50 );
    }

//draw moving white ball after hit by cue stick

    if((b1==0 && b2==0) || (b1==1 && b2==0) ||(b1==0 && b2==1 && finddistance(xw,yw,xcf,ycf)>rball) && flagwhite==1 && flagpurple==0)
    {
        glColor3f(1.0f,1.0f,1.0f);
        drawCircle(xw,yw,rball,50 );
    }

//draw moving white ball before hit by cue stick

    if((b1==0 && b2==0) || (b1==1 && b2==0) ||(b1==0 && b2==1 && finddistance(xw,yw,xcf,ycf)>rball) && flagwhite==0 && flagpurple==0)
    {
        glColor3f(1.0f,1.0f,1.0f);
        drawCircle(xw,yw,rball,50 );
    }

    //wcounterrate proportional to the distance stick moved in backward direction

    //if else block when white ball struck by cue stick

    else if(b1==0 && b2==1 && (finddistance(xw,yw,xcf,ycf)<=rball) && flagpurple==0)
    {
        wcounterrate=finddistance(xw,yw,xsi,ysi);
        flagwhite=1;
        swx=0.001;
        swy=0.001;
        glColor3f(1.0,1.0f,1.0f);
         //add conditions here of swx and swy
        setSpeedswxswy();

        for(iw=1;iw<wcounterrate*200;iw++)
        {
            drawMovingWhite();
            if(score==1)
            {
                break;
            }
            if(flagpurple==1)
            {
                break;
            }
            myDisplay();
        }
        for(int i=iw;i<(wcounterrate*200-iw);i++)
        {
            if(score==1)
            {
                break;
            }
            coll1(i);
            myDisplay();
        }
        if(score==1)
        {
            xw=140.0;
            yw=210.0;
            xp1=390.0;
            yp1=210.0;
            for(int j=0;j<100000000;j++);
        }

        //all condition variables to zero to continue game again

        score=0;
        wallcoll=0;
        b1=0;
        b2=0;
        flagpurple=0;
        flagwhite=0;
        myDisplay();

    }

    //draw cuestick still,going far and coming close

    glColor3f(0.9609f,0.8352f,0.5019f);
    if(b1==0 && b2==0 && flagwhite==0 && flagpurple==0)
    {
        drawStick();
    }
    else if(b1==1 && b2==0 && flagwhite==0 && flagpurple==0){
        movedFarStick();
    }
    else if(b1==0 && b2==1 && flagwhite==0 && flagpurple==0)
    {
        movedCloseStick();
    }
    glFlush();
    glutSwapBuffers();
}

//mouse handler function

void myMouse(int button,int state,int x,int y)
{
    if(button==GLUT_LEFT_BUTTON && state==GLUT_DOWN){
        b1=1;
        xmi=(float)x;
        ymi=(float)screenHeight-(float)y;
    }
    else if(button==GLUT_LEFT_BUTTON && state==GLUT_UP)
    {
        b1=0;
        b2=1;
        xsi=xcf;
        ysi=ycf;
        glutPostRedisplay();
    }
}

//mouse handler function when mouse moved with button pressed

void myMovedMouse(int x,int y)
{
    if(b1==1 && b2==0)
    {
        glutPostRedisplay();
    }
}
//mouse handler function when mouse moved with no button pressed

void myPassiveMotion(int x,int y)
{
    if(b1==0 && b2==1 )
    {
        glutPostRedisplay();
    }
    else if(b1==0 && b2==0)
    {

        xm=(float)x;
        ym=(float)screenHeight-y;
        glutPostRedisplay();
    }
}

//main function

int main(int argc,char** argv)
{
    cornerMain[0].x=0;
    cornerMain[0].y=0;
    cornerMain[1].x=640;
    cornerMain[1].y=420;
    cornerGreen[0].x=10;
    cornerGreen[0].y=10;
    cornerGreen[1].x=630;
    cornerGreen[1].y=410;
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(screenWidth,screenHeight);
    glutInitWindowPosition(100,150);
    glutCreateWindow("IMAGE");
    myInit();
    glutMouseFunc(myMouse);
    glutDisplayFunc(myDisplay);
    glutMotionFunc(myMovedMouse);
    glutPassiveMotionFunc(myPassiveMotion);
    glutMainLoop();
    return 0;
}
