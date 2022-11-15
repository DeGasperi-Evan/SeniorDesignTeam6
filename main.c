

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "comms.h"

#define BUFFSIZE 512

/* Hardware interfacing and communication stuff*/
#define INFILENAME "pytoc"
#define OUTFILENAME "ctopy"

struct Pipes openPipes() {
    struct Pipes ret = {0,0};
    if ((ret.in = open(INFILENAME, O_RDONLY)) < 0) {
        fprintf(stderr,"Error: open infile");
    	return ret;
    }

    if ((ret.out = open(OUTFILENAME, O_WRONLY)) < 0) {
        fprintf(stderr,"Error: open outfile");
    	return ret;
	}
    return ret;
}

int wait_loop0 = 10000/10;
int wait_loop1 = 6000;

double driveTable[4][2] = {
    {78, 0.92}, {80, 1.56}, {82, 2.19}, {84, 2.61}
};
double steerTable[16][2] = { //Modified for Nicholas CHorette's Team's car 
  {54+6, 45}, {56+6, 45}, {58+6, 55},  {60+6, 60},
  {62+6, 70}, {64+6, 75}, {66+6, 80}, {68+6, 85},
  {70+6, 90}, {72+6, 95},  {74+6, 105},  {76+6, 110}, 
  {80+6, 115}, {82+6, 120}, {84+6, 125}, {86+6, 130}
};

// { circ/line(0/1), C_x/W1_x, C_y/W1_y, R/W2_x, CW/CCW (-1,1) / W2_y, Nx(halfplane), Ny(Halfplane), Px(halfplane), Py(Halfplane) }

// /* Track Test
double controlArray[14][9] = {   
    {1,    1,    8.5,  1,    3,    0,    -1,   1,    3   }, 
    {0,    3,    3,    2,    1,    1,    0,    3,    1   },
    {1,    3,    1,    9.5,  1.5,  1,    0,    9.5,  1.5 },
    {0,    9.5,  3,    1.5,  1,    0,    1,    11,   3   },
    {1,    11,   3,    11,   4.5,  0,    1,    11,   4.5 },
    {0,    9.5,  4.5,  1.5,  1,    0,    -1,   8,    4.5 },
    {0,    7,    4.5,  1,    -1,   -1,   0,    7,    3.5 },
    {1,    7,    3.5,  5.5,  3.5,  -1,   0,    5.5,  3.5 },
    {0,    5.5,  6.0,  2.5,  -1,   1,    0,    5.5,  8.5 },
    {1,    5.5,  8.5,  9,    8.5,  1,    0,    9,    8.5 },
    {0,    9,    10,   1.5,  1,    -1,   0,    9,    11.5},
    {1,    9,    11.5, 6,    10.5, -1,   0,    6,    10.5},
    {1,    6,    10.5, 3,    10.5, -1,   0,    3,    10.5},
    {0,    3,    8.5,  2,    1,    0,    -1,   1,    8.5 }
};
// */

 /* Track Test (No circles)
double controlArray[2][9] = {   
    {1,    1,    8.5,  1,    3,    0,    -1,   1,    1.5   }, 
    {1,    1,    1.5,  9.5,  1.5,  1,    0,    9.5,  1.5 }
};
// */

/* Line Test
double controlArray[1][9] = {   
    {1,    1,    8.5,  1,    3,    0,    -1,   1,    3   }
};
// */

/* Circle Test
double controlArray[1][9] = {
    {0, 2.5, 8.5, 1.5, 1, 0, -1, 1, 100}
};
// */

 /* Mini Track Test
double controlArray[4][9] = {
    {1,    1,    6,    1,    2,    0,    -1,   1,    2   },
    {0,    2,    2,    1,    1,    0,     1,   3,    2   },
    {1,    3,    2,    3,    6,    0,     1,   3,    6   },
    {0,    2,    6,    1,    1,    0,    -1,   1,    6   }
};
// */

int controlIndex = 0;
double kp = 0;
double kd = 0;
double ki = 0;
int lap = 0;
double dt = 0.05;
double v = 1.0;
double carLocation[2] = {1, 6};
double thetaC = 3 * M_PI/2;
double thetaS = 0;
double theta = 0;
double derivLine = 0;
double derivCirc = 0;
double X[3] = {0, 0, 0};
double U[2] = {0, 0};
double pozyxInput[3] = {0, 0, 0};
double resetIntegral = 0;
double PWM[2] = {0, 0};
double X_old_predicted[2] = {0, 0};
int counter = 0;
float encoderData;

void initialize();
void AutoCodeEncoder(double U[], float encoderData);
void estimatedPos(double U[], double X[]);
void updatePosPozyx(double X[], double pozyxInput[]);
void control(double X[]);
void sendMotorPWM(double U[]);

/* Queue */
struct node {
    double index;
    double data1;
    double data2;
    struct node *link;
}*front, *rear;


void insert(); // Function used to insert the element into the queue
void delet(); // Function used to delete the elememt from the queue
void serch(int index, double out[2]); // Function used to display all the elements in the queue according to FIFO rule
int size();
void wait(int seconds);

double out[2] = {0, 0};

void insert(double pozInput[3]) {
    struct node *temp;
    temp = (struct node*)malloc(sizeof(struct node));
    /*
    printf("Enter the element to be inserted in the queue: ");
    scanf("%f", &temp->data);
    */
    temp->index = pozInput[0];
    temp->data1 = pozInput[1];
    temp->data2 = pozInput[2];
    temp->link = NULL;
    if (rear == NULL) {
        front = rear = temp;
    }
    else {
        rear->link = temp;
        rear = temp;
    }
}

void delet() {
    struct node *temp;
    temp = front;
    if (front == NULL) {
        front = rear = NULL;
    } 
    else {
        front = front->link;
        free(temp);
    }
}
        
void serch(int index, double X_old_predicted[2]) {
    struct node *temp;
    temp = front;
    while (temp) {
        if (temp->index == index) {
            X_old_predicted[0] = temp->data1;
            X_old_predicted[1] = temp->data2;
        }
        temp = temp->link;
    }
}

int size() {
    struct node *temp;
    temp = front;
    int cnt = 0;
    while (temp)
    {
        temp = temp->link;
        cnt++;
    }
    return cnt;
}
/* End Queue */

void main() {
    /*Stuff to send/receive data for HW interfacing*/
    struct Pipes pip;
	pip = openPipes();
    double heading;
	double posx;
	double posy;

	char buf[BUFFSIZE] = {0};
	char outbuf[BUFFSIZE] = {0};
    /* END of HW interfacing*/

    initialize();
    while(lap < 4 ) {
        readInPipe(pip, buf, BUFFSIZE);
        if(buf[0] != 0x0)
        {
            sscanf(strtok(buf, ","), "%lf", &posx);
            //printf("%.4lf\n", posx);
            sscanf(strtok(NULL, ","), "%lf", &posy);
            //printf("%.4lf\n", posy);
            sscanf(strtok(NULL, ","), "%lf", &heading);
            //printf("%.4lf\n", heading);
            printf("Pozyx:\n [%.4lf, %.4lf] Heading: %.4lf\n", posx, posy, heading);
        }
        double pozyxInfo[2] = {posx, posy}; 

        AutoCodeEncoder(U, encoderData);
        estimatedPos(X, U);
        updatePosPozyx(X, pozyxInput);
        control(X);
        sendMotorPWM(U);
        sprintf(outbuf, "%lf,%lf",PWM[0],PWM[1]);
        writeOutPipe(pip, outbuf, BUFFSIZE);
        if(counter > 2/dt) {
            delet();
        }
        double queueInsert[3] = {counter, X[0],X[1]};
        insert(queueInsert);
        counter +=1; 
        printf("Estimated:\n [%.4lf, %.4lf] Heading: %.4lf\n", X[0], X[1], X[2]);
        printf("PWM:\n Drive: %.lf Steer: %.lf\n", PWM[0], PWM[1]);
        //printf("Coordinates: [%lf, %lf] Drive PWM: %lf Steer PWM: %lf \n", X[0], X[1], PWM[0], PWM[1]);
        //wait(1);
    }
}

void initialize() {
    X[0] = carLocation[0];
    X[1] = carLocation[1];
    X[2] = thetaC;
    U[0] = v;
    U[1] = 0;
    pozyxInput[0] = 0;
    pozyxInput[1] = 0;
    pozyxInput[2] = 0;
}

void AutoCodeEncoder(double U[2], float encoderData)
{
    static int counter1;
    static int counter2;
    static float buffer;
    encoderData = -1; // for testing
    while(counter1 < 3 && counter2 < 5)
    {
        if (0 < encoderData && encoderData < 500)
        {
            buffer += 46.019423 / encoderData;
            counter1 += 1;
        }
        counter2 += 1;
    }

    if (counter1 == 3)
    {
        if (0 < buffer && buffer < 10)
        {
            U[0] = buffer/3;
        }
        counter1 = 0;
        counter2 = 0;
    }
    else {
        U[0] = U[0];
        counter1 = 0;
        counter2 = 0;  
    }
  
}

void estimatedPos(double X[], double U[]) {
    v = U[0];
    thetaS = U[1];
    theta = X[2];
    double X_dot[3] = {v*cos(theta), v*sin(theta), v*tan(thetaS)/0.27};
    X[0] = X_dot[0] * dt + X[0];
    X[1] = X_dot[1] * dt + X[1];
    X[2] = X_dot[2] * dt + X[2];
    while(X[2] > 2*M_PI) {
        X[2] -= 2*M_PI;
    }
    while(X[2] < 0.0) {
        X[2] += 2*M_PI;
    }
}

void updatePosPozyx(double X[], double pozyxInput[]) {
    double muPosTime = .01;
    double updatePos[2];
    if (size() > 1.9 / dt){
        int index = muPosTime/dt;
        serch(counter-index, X_old_predicted);
        updatePos[0] = pozyxInput[0] - X_old_predicted[0];
        updatePos[1] = pozyxInput[1] - X_old_predicted[1];
        X[0] += updatePos[0]/100;
        X[1] += updatePos[1]/100;
    }
}

void control(double X[]) {
    static double oldDLine;
    static double oldDCirc;
    static double integral;
    double H = 0;
    double P[2] = {controlArray[controlIndex][7], controlArray[controlIndex][8]};
    double N[2] = {controlArray[controlIndex][5], controlArray[controlIndex][6]};
    double vec1[2] = {X[0] - P[0], X[1] - P[1]};
    for (int i = 0; i < 2; i++) {
        H += vec1[i] * N[i];
    }
    if(H > 0) {
        if(controlIndex == (sizeof(controlArray) / sizeof(controlArray[0])) - 1) {
            controlIndex = 0;
            lap++;
        }
        else {
            controlIndex++;
        }
        resetIntegral = 1;
    }
    else {
        resetIntegral = 0;
    }
    if(controlArray[controlIndex][0] == 0) {
        //Control Circle
        if(resetIntegral == 1) {
            integral = 0;
        }
        double lambda = controlArray[controlIndex][4];
        double Rcirc = controlArray[controlIndex][3];
        double C[2] = {controlArray[controlIndex][1], controlArray[controlIndex][2]};
        double xy[2] = {X[0], X[1]};
        double Rcar = sqrt(((C[0]-xy[0])*(C[0]-xy[0]))+((C[1]-xy[1])*(C[1]-xy[1])));
        double error = Rcar - Rcirc;
        kp = lambda * 0.4;
        kd = lambda * 0.5;
        ki = lambda * 0.01;
        if(resetIntegral == 1) {
            integral = 0;
            derivCirc = 0;
        }
        else {
            derivCirc = (error-oldDCirc)/dt;
        }
        thetaS = error*kp + integral*ki + kd*derivCirc;
        U[0] = v;
        U[1] = thetaS;
        oldDCirc = error;
        integral = integral + error*dt;
    }
    else {
        // Control Line
        double determinant = 0;
        double inverse[3][3] = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
        double D = 0;
        double Xmatrix[3] = {X[0], X[1], 1};
        double W1[2] = {controlArray[controlIndex][1], controlArray[controlIndex][2]};
        double W2[2] = {controlArray[controlIndex][3], controlArray[controlIndex][4]};
        double xdiff = W2[0] - W1[0];
        double ydiff = W2[1] - W1[1];
        double angle = atan2(ydiff,xdiff);
        double R[3][3] = {
            {cos(angle), -sin(angle), W1[0]},
            {sin(angle), cos(angle), W1[1]},
            {0, 0, 1}
        };
        for(int i=0;i<3;i++) {
            determinant = determinant + (R[0][i]*(R[1][(i+1)%3]*R[2][(i+2)%3] - R[1][(i+2)%3]*R[2][(i+1)%3]));
        }
        for(int i=0;i<3;i++) {
            for(int j=0;j<3;j++) {
                inverse[i][j] = ((R[(j+1)%3][(i+1)%3] * R[(j+2)%3][(i+2)%3]) - (R[(j+1)%3][(i+2)%3]*R[(j+2)%3][(i+1)%3])) / determinant;
            }
        }
        D = inverse[1][0] * Xmatrix[0] + inverse[1][1] * Xmatrix[1] + inverse[1][2] * Xmatrix[2];
        kp = -0.4;
        kd = -0.5;
        derivLine = (D-oldDLine)/dt;
        thetaS = D * kp + kd * derivLine;
        U[0] = v;
        U[1] = thetaS;
        oldDLine = D;
    }

}

void sendMotorPWM(double U[]) {
    double steerAnglePWM = 0;
    double driveSpeedPWM = 0;
    double steerAngle = 57.295579513082323 * U[1];
    double driveSpeed;
    while ((steerAngle < -180.0) || (steerAngle > 180.0)) {
        if (steerAngle < -180.0) {
            steerAngle += 360.0;
        } else if (steerAngle > 180.0) {
            steerAngle -= 360.0;
        }
    }
    steerAngle += 90.0;
// Scan steer curve
  for(int i = 0; i < sizeof(steerTable) / sizeof(steerTable[0]); i++)
  {
    if(abs(steerAngle - steerTable[i][1] ) < 7 )//steerAngle - 7 < steerTable[i][1] && steerAngle + 7 > steerTable[i][1])
    {
      steerAnglePWM = steerTable[i][0];
      U[1] = (steerTable[i][1] - 90) / 57.295779513082323;
      break;
    }
  }

  if (steerAngle > 130.0) {
    steerAnglePWM = 88;
    U[1] = (steerTable[14][1] - 90) / 57.295779513082323;
  } else if (steerAngle < 45.0) {
    steerAnglePWM = 60;
    U[1] = (steerTable[0][1] - 90) / 57.295779513082323;
  }

  if (driveSpeedPWM == 0)
  {
    driveSpeedPWM = 78;
    U[0] = driveTable[0][1];
  }

  if (steerAnglePWM == 0)
  {
    steerAnglePWM = 74;
    U[1] = (steerTable[7][1] - 90) / 57.295779513082323;
  }

  PWM[0] = driveSpeedPWM;
  PWM[1] = steerAnglePWM;
}


/*Some communication/HW interfacing stuff*/
int readInPipe(struct Pipes file, char *buf, int bufsize) {
    return read(file.in, buf, bufsize);
}

void writeOutPipe(struct Pipes file, char *outbuf, int bufsize) {
	write(file.out, outbuf, bufsize);
}

void closePipes(struct Pipes file) {
    unlink(INFILENAME);
    unlink(OUTFILENAME);
    close(file.in);
    close(file.out);
}

/* Time Delay method*/
void wait( int seconds )
{   // this function needs to be finetuned for the specific microprocessor
    int i, j, k;
    for(i = 0; i < seconds; i++)
    {
        for(j = 0; j < wait_loop0; j++)
        {
            for(k = 0; k < wait_loop1; k++)
            {   // waste function, volatile makes sure it is not being optimized out by compiler
                int volatile t = 120 * j * i + k;
                t = t + 5;
            }
        }
    }
}