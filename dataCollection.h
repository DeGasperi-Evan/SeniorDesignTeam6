/*
Author: Nicholas Chorette
Date: 11/15/2022
Purpose: This file was made to log data for our simulation code as well as Pozyx inorder to allow our team to analyze it better after each trial run of the program. This is the headder file that goes with the main C file I created. 
*/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
// #include "comms.h"// Disabled for testing due to the fact that i do not have comms.h and comms.c NOT on the laptop when testing compiling
#include <time.h>

extern FILE *fpt; //should allow other files to use these functions properly and be able to refference the file name, aka time and date stuff I have thrown together.... not sure if this is needed.... but I will do it anyhow 

void fileGen(); 

void collectData(float P_x, float P_y, float P_heading, float servoPWM, float motorPWM,  float S_x, float S_y, float S_head, float shaftEncoder ); 


void closeFile(); 

