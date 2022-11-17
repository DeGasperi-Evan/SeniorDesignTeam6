/*
Author: Nicholas Chorette
Date: 11/15/2022
Purpose: This file was made to log data for our simulation code as well as Pozyx inorder to allow our team to analyze it better after each trial run of the program. This is the main C file that executes this process. 


*/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
// #include "comms.h"// Disabled for testing due to the fact that i do not have comms.h and comms.c NOT on the laptop when testing compiling
#include <time.h>

#include "dataCollection.h" // this is where FILE *fpt; comes from.... hopefully this works like i think it does 


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
    fprintf(fpt,"P_x, P_y, P_heading,  servoPWM, motorPWM, S_x, S_y, S_head, shaftEncoder, time\n"); 

    //please run the collectData() function to collect data   
    
} //end of fileGen()

void collectData(float P_x, float P_y, float P_heading, float servoPWM, float motorPWM,  float S_x, float S_y, float S_head, float shaftEncoder, float time ){ //implies that the file is initialized

//loop for the actual DATA! :) 
    fprintf(fpt," %f, %f, %f, %f, %f, %f, %f, %f, %f, %f\n", P_x, P_y, P_heading, servoPWM, motorPWM,   S_x, S_y, S_head, shaftEncoder, time); // the "\n" allows for the next line in the file to be filled next, an NOT the same line over and over again

	
		//when you want to properly save the data, it is recommended that you use the closeFile() function to do so
	
}//end of collectData()


void closeFile(){ //implies that the fileGen() function was run 
//close file (when the C program stops, throw this into here as well <3 )
     fclose(fpt);
}//end of closeFile()

//  /* 
void main(){ //---------PLEASE DISABLE WHEN YOU ARE DONE TESTING TY <3 ) ---------

fileGen(); //make the file
// 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 
collectData(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -999.0, 0.0 ); //collect data

collectData(1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0,  -999.0, 9.0  ); 

closeFile(); //save the file (safely)


}//end of main()
// */ 

