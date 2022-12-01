#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "comms.h"
#include <stdio.h>
#include "stdbool.h"
//#include "dataCollection.h"
#include "timePassed.h"

#define BUFFSIZE 512 //TO SMALL, CRINGE :(
//#define BUFFSIZE 2048 //New buffer size, cause it was syruggling to get data FROM the pipe (when lidar data was being send FROM allPozyx.py, you can shrink this back when dedicated pipe is used )


/* Hardware interfacing and communication stuff*/
#define INFILENAME "pytoc"
#define OUTFILENAME "ctopy"

//-----EXPERIMENTAL: LIDAR ------
#define LIDAR "lidarGoBrr"


//------------EXPERIMENTAL: NOT WRITING TO CSV FILE! ATTEMPTING TO MOVE FUNCTIONS TO OVER HERE INSTEAD!-----

/*
FILE *fpt; //need to make this avaliable globally for this thing to work... may need to be defined in the headder.... unfortunately, inorder for things to work right

void fileGen(){ // make file based, Similar to the "init" function for some programs
// File name is based on time and date. Order is: Stuff gotten from python code, THEN things from the C-simulation program. 

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    char cTime [1000]; 

    sprintf(cTime, "%d-%02d-%02d_%02d-%02d-%02d.csv", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec) ; // copy name to the array 

    //prove current time collection working
    printf(cTime); 

    // Opens and creates the new csv file
    fpt = fopen(cTime, "w+");

    //the headder of the data :) 
    fprintf(fpt,"P_x, P_y, P_heading,  servoPWM, motorPWM, S_x, S_y, S_head, shaftEncoder\n"); 

    //please run the collectData() function to collect data   
    
} //end of fileGen()


void collectData(float P_x, float P_y, float P_heading, float servoPWM, float motorPWM,  float S_x, float S_y, float S_head, float shaftEncoder, double time ){ //implies that the file is initialized

//loop for the actual DATA! :) 
    fprintf(fpt," %f, %f, %f, %f, %f, %f, %f, %f, %f, %lf\n", P_x, P_y, P_heading, servoPWM, motorPWM,   S_x, S_y, S_head, shaftEncoder, time ); // the "\n" allows for the next line in the file to be filled next, an NOT the same line over and over again

    printf(" %f, %f, %f, %f, %f, %f, %f, %f, %f, %lf\n", P_x, P_y, P_heading, servoPWM, motorPWM,   S_x, S_y, S_head, shaftEncoder, time ); //I want to see and make sure that the data that is SUPPOSE to be being written to this file is beinf properly recieved and working right.... reception is fine... writing is not for some reason. (YOu do not need to close the file, I just checked that... so why is this happening...? -NIC_ 11/27/2022 @ 4:51PM) 
	
		//when you want to properly save the data, it is recommended that you use the closeFile() function to do so
	
}//end of collectData()
// */


/*
void closeFile(){ //implies that the fileGen() function was run 
//close file (when the C program stops, throw this into here as well <3 )
     fclose(fpt);
}//end of closeFile()
*/



//---------END OF EXPERIEMNTAL-------------





/*
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

// */

int wait_loop0 = 10000/10;
int wait_loop1 = 6000;

 /* Variable Speed
double driveTable[4][2] = {
    {78, 0.92}, {80, 1.56}, {82, 2.19}, {84, 2.61}
};
// */

// /* Constant Speed
double driveTable[1][2] = {
    {78, 0.9}
};
// */

double steerTable[16][2] = { //Modified for Nicholas CHorette's Team's car 
  {54+6, 45}, {56+6, 45}, {58+6, 55},  {60+6, 60},
  {62+6, 70}, {64+6, 75}, {66+6, 80}, {68+6, 85},
  {70+6, 90}, {72+6, 95},  {74+6, 105},  {76+6, 110}, 
  {80+6, 115}, {82+6, 120}, {84+6, 125}, {86+6, 130}
};

double controlArray[17][9] = {   
    {1,    0.9,    7.7,  0.9,    3.4,    0,    -1,   0.9,    3.4   }, 
    {0,    2.7,    3.4,    1.8,    1,    1,    0,    2.7,    1.4   },
    {1,    2.7,    1.4,    5.4,  1.8,  1,    0,    5.4,  1.8 },
    {1,    5.4,    1.8,    8.1,  1,  1,    0,    8.1,  1 },
    /*{1,    8.5,    1.1,  10.8,  3.2,  1,   0,    10.8,  3.2 }, */{0,    8.3,  3.2,    2,  1,    0,    1,    10.5,   3.2   },
    {1,    10.7,   3.2,    10.7,   4.5,  0,    1,    10.7,   4.5 },
    {0,    9.2,  4.5,  1.2,  1,    0,    -1,   8,    4.5 }, /*{0,    9.1,  4.5,  1.5,  1,    0,    -1,   8.2,    5 },*/  /*{0,    9.2,  4.5,  1.2,  1,    0,    -1,   8,    4.5 },*/
    /*{1,    9.3,  5.8,  7.5,  4,    -0.707,    -0.707,   7.5,    4 },*/ {1,    8,    4.5,  7.5,  4,  -0.707,   -0.707,    7.5,  4 }, /*{0,    6.8,    4.5,  1,    -1,   -1,   0,    6.8,   3.5  },*/
    {1,    7.5,    4,  4.4,  4,  -1,   0,    4.4,  4 },
    {0,    4.5,  5,  1,  -1,   0,    1,    3.4,  5 },
    {1,    3.4,  5,  3.4,    7,  0,    1,    3.4,    7.2 },
    {0,    4.4,    7.1,   1,  -1,    1,   0,    4.4,    8},
    {1,    4.4,    8.4, 9,    8.4, 1,   0,    9,    8.4},
    {0,    9,    9.8,   1.2,  1,    -1,   0,    9,    11.2},
    {1,    9,    11.2,   5.7,  10.1,    -1,   0,    5.7,    10.1},
    {1,    5.7,    10.1,   3.2,  10.2,    -1,   0,    3.4,    10.2},
    {0,    3.4,    7.7,   2.5,  1,   0,   -1,    0.7,    7.7}
};
// */

/* Track Test
double controlArray[15][9] = {   
    {1,    1.5,    7,    1.5,    3,    0,    -1,   1.5,    3   }, 
    {0,    3.5,    3.5,    2,    1,    1,    0,    3.5,    1.5   },
    {1,    3.5,    1.5,    9,  1.5,  1,    0,    9,  1.5 },
    {0,    9,  3,    1.5,  1,    0,    1,    10.5,   3   },
    {1,    10.5,   3,    10.5,   4.5,  0,    1,    10.5,   4.5 },
    {0,    9.5,  4.5,  1,  1,    0,    -1,   8.5,    4.5 },
    {0,    7.5,    4.5,  1,    -1,   -1,   0,    7.5,    3.5 },
    {1,    7.5,    3.5,  5.5,  3.5,  -1,   0,    5.5,  3.5 },
    {0,    5.5,  6.0,  2.5,  -1,   1,    0,    5.5,  8.5 },
    {1,    5.5,  8.5,  9,    8.5,  1,    0,    9,    8.5 },
    {0,    9,    10,   1.5,  1,    -1,   0,    9,    11.5},
    {1,    9,    11.5, 8,    10.5, -1,   0,    8,    10.5},
    {1,    8,    10.5, 3,    10.5, -1,   0,    3,    10.5},
    {0,    3,    8.5,  2,    1,    0,    -1,   1,    8.5 },
    {1,    1,    8.5,  1,    7,    0,    -1,   1,    7   }
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
    {1,    10.927,    9.554,    -100,    9.464,    -1,    0,   -100,    9.464}
};
// */

 /* Circle Test
double controlArray[1][9] = {
  //{0, 9.415, 9.592, 1.5, -1, 0, 1, 0, 100}
    {0, 2, 6, 1, 1, 0, 1, 1, 100} 
  // {0, 2.5, 8.5, 1.5, 1, 0, -1, 1, 100}
};
// */

 /* Mini Track Test
double controlArray[4][9] = {
    {1,    1,    6,    1,    4,    0,    -1,   1,    4   },
    {0,    1.5,  4,    .5,   1,    0,     1,   2,    4   },
    {1,    2,    4,    2,    6,    0,     1,   2,    6   },
    {0,    1.5,  6,    .5,   1,    0,    -1,   1,    6   }
};
// */

// double pozyxHistory[10][2] = {
//     {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}
// };

int controlIndex = 0;
double kp = 0;
double kd = 0;
double ki = 0;
int lap = 0;
double dt = 0.05;
double v = 0.9;
double carLocation[2] = {0.7, 7.7};
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
float pozr = 3; // pozyx ratio
double lidarData[360] = {0};
bool car = 0;
double accelData = 0;

int c = 0;
// int reverse = 0;

void initialize();
void AutoCodeEncoder(double U[], float encoderData);
void estimatedPos(double U[], double X[]);
void updatePosPozyx(double X[], double pozyxInput[]);
void control(double X[]);
void sendMotorPWM(double U[]);
// void checkReverse(double pozyxInput[]);

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
double lidarSteer(double leftAvg, double rightAvg, double frontAvg);
int reverse(double left, double front, double right, double accelData);

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
    //fileGen(); ///////******************8this is the log file function! please use! 


    //fprintf(fpt,"No bitches?" );
    


    /*Stuff to send/receive data for HW interfacing*/
    struct Pipes pip;
	pip = openPipes();

    struct Pipes lidar = {0,0}; //God, please work 

    double prevLeft; 
    double prevFront; 
    double prevRight; 

    double revMargin = 0.1; // 0.1 = 10%

    double leftMath; 
    double frontMath; 
    double rightMath;
    double currentSpeed; 


    double heading;
	double posx;
	double posy;

	char buf[BUFFSIZE] = {0};
	char outbuf[BUFFSIZE] = {0};


    //lidar buffer probably issue, needed dedicated buffer
    char lidarBuff[BUFFSIZE] = {0};
    
    /* END of HW interfacing*/

    initialize();
    timeInit(); //start logging time of sim here 

    //---EXPERIMENTAL: LIDAR PIPE NAME---


    mkfifo(LIDAR, 0600);
    double leftAvg = 0; 
    double frontAvg = 0; 
    double rightAvg = 0; 



    //----Attempting to try and open the *beautiful* lidar pipe <3
    lidar.in = open(LIDAR, O_RDONLY);


    while(lap < 11 ) { 
        readInPipe(pip, buf, BUFFSIZE); //from allPozyx file 
        if(buf[0] != 0x0)
        {
            //printf("POZYX BUFFER: "); 
            //printf(buf);

            sscanf(strtok(buf, ","), "%lf", &posx);
            //printf("%.4lf\n", posx);
            sscanf(strtok(NULL, ","), "%lf", &posy);
            //printf("%.4lf\n", posy);
            sscanf(strtok(NULL, ","), "%lf", &heading);
            //printf("%.4lf\n", heading);

            // ***THIS IS WHERE YOU WILL NEED TO PULL THE DATA OUT FROM THE LIDAR 0_0 (please verify that the RPM of the lidar speed to make sure that data collection does not impeed main program 0_0)

            printf("Pozyx:     [%.4lf, %.4lf] Heading: %.4lf\n", posx, posy, heading);
        }
        double pozyxInfo[2] = {posx, posy}; 


        //-----------EXPERIMENTAL: Lidar Data Piping (collect Lidar data via pipe )---------
        //readInPipe(pip, buf, BUFFSIZE);


        read(lidar.in, lidarBuff, BUFFSIZE); //from Lidar file , Erik said to do this over the toher thing 0_0 

        printf("~~~~~~~~~~~~~~~~~~~~\nChecking lidar buffer...\n");
        if(lidarBuff[0] != 0x0) //If there is SOMETHING in the lidar buffer, go for it! 
        {   //printf("BUFFER_C: "); //-----WARNING: @ present, for some reason data being thrown into the buffer is "0.00.0000", and its looking for comma's, and if ther are none, then it just errors the comamnd and that SPECIFIC command according to Erik is replying "Segfault" whenit errors, aka, that's the cuperate, but WHY is the pipe sending, or this, ercieving it as such
            //printf(lidarBuff);

            sscanf(strtok(lidarBuff, ","), "%lf", &leftAvg); //if this fails, it CLAIMS its a seg-fault.... but its not so -_-
            //printf("%.4lf\n", leftAvg);
            //printf("%.2f\n", leftAvg);
            sscanf(strtok(NULL, ","), "%lf", &frontAvg);
            //printf("%.2f\n", frontAvg);
            sscanf(strtok(NULL, ","), "%lf", &rightAvg);
           // printf("%.2f\n", rightAvg);

            // ***THIS IS WHERE YOU WILL NEED TO PULL THE DATA OUT FROM THE LIDAR 0_0 (please verify that the RPM of the lidar speed to make sure that data collection does not impeed main program 0_0)

            printf("LIDAR Avg's:     [%.4lf, %.4lf, %.4lf] \n", leftAvg, frontAvg, rightAvg);
        }//end of IF statement _ Get Lidar data
        else{ 
            printf("LIDAR_BUFFER -> Empty!\n~~~~~~~~~~~~~~~~~~~~\n"); 
        }

        //--------End of Lidar Data Collection Experimental Area--------





        AutoCodeEncoder(U, encoderData);
        estimatedPos(X, U);
        //updatePosPozyx(X, pozyxInfo);//-------RE ENABLE POOZYX -> UNCOMMENT THIS LINE 



//------EXPERIMENTAL----REVERSE FUNCTION ATTEMPT

/*        
        int holder = reverseCheck(posx, posy); 
        if( holder == 1){ //should reverse car 
            //!!! -----reverse car, cannot find reverse function !!! 
        printf("\n REVERSE IN PROGRESS \n");
        PWM[0] = 54.0; //reverse PWM (according to controller code)
        sendMotorPWM(U); // believe this updates it for me <3 

        //sendMotorPWM(54); 

        sleep(1); //reverse for 1 second 

        //sendMotorPWM(72); //set it to neutural 
        PWM[0] = 72.0; //should be neutral 
        sendMotorPWM(U); 
        
        sleep(0.5); //should be enough time to let it stop moving.... just hard coded it cause quick and dirty check 

  

            //estimatedPos(X, pozyxInfo); // needs to be sat to the pozyx data acutally after this is ran (reverse fucntion)
            //updatePosPozyx(X, pozyxInfo);
        }//end of if statment 
        // */ 
        
        //NEEDS TO UPDATE POZYX DATA to current car position after reversing 

        //-----END OF EXPERIMENTAL----



        control(X);
                
        //Josiah Magic
        
        
        // checkReverse(pozyxInfo);
        sendMotorPWM(U);
        //U[1] = lidarSteer(leftAvg, rightAvg, frontAvg);
        
        if (frontAvg < 1.3) car = 1;
        if(car == 1 || pozyxInfo[0] <= 0 || pozyxInfo[1] <= 0)   
        {
            PWM[1] = lidarSteer(leftAvg, rightAvg, frontAvg);
            printf("Angle: %.2lf\n~~~~~~~~~~~~~~~~~~~~\n",PWM[1]);
            car = 0;
        }
        /*if (frontAvg<.7 && frontAvg > 0){
            PWM[0] = 40;
            PWM[1] = 64 + (88-PWM[1]);
        }*/

        if(leftAvg > prevLeft){ 
            leftMath = prevLeft / leftAvg; 
        } else{ 
            leftMath = leftAvg / prevLeft; 
        }


        if(frontAvg > prevFront){ 
            frontMath = prevFront / frontAvg; 
        } else{ 
            frontMath = frontAvg / prevFront; 
        }


        if(rightAvg > prevRight){ 
            rightMath = prevFront / frontAvg;  
        } else{ 
            rightMath = frontAvg / prevFront; 
        }
        if(c<5&&prevFront==frontAvg) c++;
        else if (c=5) c++;
        else c=0;

        if(  ((leftMath < revMargin) || (frontMath < revMargin) || (rightMath < revMargin)) || (prevFront == frontAvg && c > 4)){ 

            currentSpeed = PWM[0]; 
            PWM[0] = 40; //reverse motor
            PWM[1] = 64 + (88-PWM[1]);
            //sleep(1); //reverse for 1 sec
            //PWM[0] = currentSpeed; //sets speed back to what it was before
            //sleep for X-seconds -> delay for X seconds -> reverse for nX seconds
            //resume PWM Forwards it had before 
        }//end of if stamtent

        //Set current nums -> prev, for next round in loop
        prevLeft = leftAvg; 
        prevFront = frontAvg; 
        prevRight = rightAvg; 

        sprintf(outbuf, "%lf,%lf",PWM[0],PWM[1]); //Literally pipes the PWM for Servo and Drive Motor to the allPython.py file LOL
        writeOutPipe(pip, outbuf, BUFFSIZE);
        if(counter > 2/dt) {
            delet();
        }
        double queueInsert[3] = {counter, X[0],X[1]};
        insert(queueInsert);
        counter +=1; 
        printf("Estimated: [%.4lf, %.4lf] Heading: %.4lf\n", X[0], X[1], X[2]);
        printf("PWM:       Drive: %.lf Steer: %.lf\n\n", PWM[0], PWM[1]);

        //collectData(posx, posy, heading, PWM[1], PWM[0], X[0], X[1], X[2], encoderData, timePassed() ); // LOGS THE DATA FROM THE DRIVE 
    }
    exit(0); 
} //End of main( )


//-----Experimental: REVERSE FUNCTION-----------

int checkReverse = 0; 
//int reverse(double left, double front, double right){



 //   return 0; 
//}//end of reverse function



//-----------Josiah's Handy Work---------
double lidarSteer(double leftAvg, double rightAvg, double frontAvg){
    /*********************\
    double left[60] = {0};
    double leftAvg = 0;
    double right[60] = {0};
    double rightAvg = 0;
    double front[60] = {0};
    double frontAvg = 0;
    \**********************/
    double angle = 76; //not needed in python implementatimon

    /*****************************************\
    for(int i = 0; i < 60; i++){
        left[i] = lidar[i+30]*sin(i+30);
        leftAvg += left[i];
    }
    leftAvg /= 60;

    for(int i = 59; i >= 0; i--){ 
        right[i] = lidar[i+270]*sin(i+30);
        rightAvg += right[i];
    }
    rightAvg /= 60;

    for(int i = 0; i < 30; i++){
        front[i+30] = lidar[i]*cos(i);
        frontAvg+=front[i+30];
    }


    for(int i = 330; i < 360; i++){
        front[i-330] = lidar[i-330]*cos(360-i);
        frontAvg+=front[i];
    }
    frontAvg /= 60;
    \********************************************/

    if (frontAvg == 0 || leftAvg == 0 || rightAvg == 0) return angle;

    if(car && rightAvg > leftAvg) {
        angle = 64;
        PWM[0] = 78;
    }
    else if ((car && leftAvg > rightAvg)) {
        angle = 88;
        PWM[0] = 78;
    }
    else if (leftAvg > rightAvg) {
        if(rightAvg+leftAvg > 1.5){
            if(leftAvg-rightAvg>1) angle = 84;
            else angle = (leftAvg-rightAvg)*8+76;
        }
        else{
            if(leftAvg-rightAvg>1) angle = 88;
            else angle = (leftAvg-rightAvg)*12+76;
        }
    }
    else {
        if(rightAvg+leftAvg < 1.5){
            if(rightAvg-leftAvg>1) angle = 64;
            else angle = 152-((rightAvg-leftAvg)*12+76);
        }
        else {
            if(rightAvg-leftAvg>1) angle = 68;
            else angle = 152-((rightAvg-leftAvg)*8+76);
        }
    }
    
    return angle;
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

void AutoCodeEncoder(double U[], float encoderData)
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
    double muPosTime = .05;
    double updatePos[2];
    if (size() > 1.9 / dt){
        int index = muPosTime/dt;
        serch(counter-index, X_old_predicted);
        if(pozyxInput[0] > 0.0 && pozyxInput[1] > 0.0) {
            updatePos[0] = pozyxInput[0] - X_old_predicted[0];
            updatePos[1] = pozyxInput[1] - X_old_predicted[1];
            X[0] += updatePos[0]/pozr;
            X[1] += updatePos[1]/pozr;
        }
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
        ki = lambda * 0.1;
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
    driveSpeedPWM = 80;
    U[0] = driveTable[0][1];
  }

  if (steerAnglePWM == 0)
  {
    steerAnglePWM = 74;
    U[1] = (steerTable[7][1] - 90) / 57.295779513082323;
  }

  PWM[0] = driveSpeedPWM;
  PWM[1] = steerAnglePWM;
}//end of sendMotorPWM()


/*Some communication/HW interfacing stuff*/
/*
int readInPipe(struct Pipes file, char *buf, int bufsize) {
    return read(file.in, buf, bufsize);
}
// */

/*
void writeOutPipe(struct Pipes file, char *outbuf, int bufsize) {
	write(file.out, outbuf, bufsize);
}
// */

/*
void closePipes(struct Pipes file) {
    unlink(INFILENAME);
    unlink(OUTFILENAME);
    close(file.in);
    close(file.out);
}
// */

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

// void checkReverse(double pozyxInput[]) {
//     double minXPos = pozyxInput[0];
//     double minYPos = pozyxInput[1];
//     double maxXPos = 0;
//     double maxYPos = 0;
//     //double posSum[] = {0, 0};
//     double XDiff = 0;
//     double YDiff = 0;
//     reverse = 0;
//     //double temp[] = pozyxInput[];
//     for(int i = 9; i > 0; i--) {
//         if(pozyxHistory[i][0] > maxXPos) {
//             maxXPos = pozyxHistory[i][0];
//         }
//         if(pozyxHistory[i][0] > maxXPos) {
//             maxXPos = pozyxHistory[i][0];   
//         } 
//         if(pozyxHistory[i][1] < minYPos) {
//             minYPos = pozyxHistory[i][1];
//         }
//         if(pozyxHistory[i][1] > minYPos) {
//             minYPos = pozyxHistory[i][1];   
//         } 
//         //posDiff[0] += (pozyxHistory[i][0] - pozyxHistory[i-1][0]) ;
//         //posDiff[1] += (pozyxHistory[i][1] - pozyxHistory[i-1][0]);
//         pozyxHistory[i][0] = pozyxHistory[i-1][0];
//         pozyxHistory[i][1] = pozyxHistory[i-1][1];

//     }
//     pozyxHistory[0][0] = pozyxInput[0];
//     pozyxHistory[0][1] = pozyxInput[1];
//     if(minXPos == 0 && minYPos == 0) {
//         reverse = 0;
//     }
//     if((maxXPos - minXPos) < 0.1 && (maxYPos - minYPos) < 0.1) {
//         reverse = 1;
//         PWM[0] = 54;
//         PWM[1] = 76;
//         wait(2);
//     }
//     else {
//         reverse = 0;
//     }
// }
