#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
// #include "comms.h"// Disabled for testing due to the fact that i do not have comms.h and comms.c NOT on the laptop when testing compiling
#include <time.h>


//needs to take in the current Pozyx XY data, make the vector in here, and compare it with a previously stored one. If the difference is minimal, the car is probably not omving. 


float diff; //difference between present and previous Pozyx dist. vectors (for whatever the reason, this needs to ALSO filter out zeros... and points tht will go WAY past Pozyx's bounds as well... )
float prev; //previous Pozyx dist. vector calculated
float pres; // present Dist. Vector. 
int counter; //depending on how big this number is, tells us when to start 

float diffAccept = 0.01;
//should be a differnece of a meter difference or something
 //if the difference is lower than this threashhold, then car is still 


int maxCounter = 1; //max number coutner will go before car reverses, should be intheroy 10



//Px and Py are floats, than GOD

int carNoMove(float PosX, float PosY){ 

    pres = prev; //update the previous datapoint with the old data: IF IT ERRORS, INITIALIZE THEM BOTH AS ZERO 


    pres = sqrt( (PosX)*(PosX) + (PosY)*(PosY)); //Get current position vector 
    diff = abs(pres - prev); //aka, we need to know how different this all is 
    printf("%f\n", diff); 

    if( diff <= diffAccept ){ // distance is not within proper range... yeah 
        counter++; 

    }//end of if statment 
    else{// thing is within range, SHOULD work now that misc. , aka, should reset counter 
        counter = 0; 
    }//end of IF-ELSE 

    if( counter >= maxCounter   ){ // counter has been hit -> Reverse for X-time, then proceed as normal... (normally, you would set ur sim location as the present one after movement)
    return 1; //do this when the car IS NOT moving
    }  //end of if stamtent  


    return 0; // car IS moving, and checked nothing
}//end of carNoMove()

int reverseCheck(float PosX, float PosY){ 

    //checks to see if X or Y is zero, in that case, we do not care 
    if( (PosX <= 0) || (PosX >= 12) || (PosY <= 0) || (PosY >=15)){ //the positioning data is out of range of the Pozyx track, ignore data 

        return 0; //keep going straight, do NOT reverse

    }//end of if statement 


    /*
    pres = prev; //update the previous datapoint with the old data: IF IT ERRORS, INITIALIZE THEM BOTH AS ZERO 


    pres = sqrt( (PosX)*(PosX) + (PosY) * (PosY)); //Get current position vector 
    diff = abs(pres - prev); //aka, we need to know how different this all is 

    if( diff <= diffAccept ){ // distance is not within proper range... yeah 
        counter++; 

    }//end of if statment 
    else{// thing is within range, SHOULD work now that misc. , aka, should reset counter 
        counter = 0; 
    }//end of IF-ELSE 

    if( counter >= 10   ){ // counter has been hit -> Reverse for X-time, then proceed as normal... (normally, you would set ur sim location as the present one after movement)

    // 10 data pts are consistent,  @20 Hz refresh, that should be  -> 20 times / seconds -> 1/2 a second... if (counter >=10) that is 
    return 1; 


    }//end of if stamtent 
    */
   int holder = carNoMove(PosX, PosY); 
   if(holder == 1){ //reverse
    return 1; 
   }//end of if statement

    //this only happens if the range IS outside of the bounds 
    return 0; //do NOT reverse. If its 1, REVERSE!!!
}//end of revCheck

//




void main(){ 

    float tstX[] = {1.0, 2, 3, 4, 5, 5.1, 4.9,}; 
    float tstY[] = {0, 0, 1, 2, 3, 3.1, 3.2}; 


    int holder = reverseCheck(0.0, 0.0); //this will be PosX and PoxY when actually impelemted 
    printf("%f\n", holder); 

    holder = reverseCheck(3.0, 4.0); //should be a 5 hypotenuse -> dist boi 
    printf("%f\n", holder); 

    holder = reverseCheck(3.01, 4.0); 
    printf("%f\n", holder); 

    holder = reverseCheck(3.0, 4.01); 
    printf("%f\n", holder); 


    if(holder == 1){ // reverse!Q
        //set reverse PWM for set time. set it to zero for a certain amount of time, check and see if the coorcinates are stable again, if it thinks its still, then set currest Pozyx position as SImulation position and go from there 

        printf("REVERSE!"); 


    }//end of if statment 
    else{ 
        printf("forward (FAILURE) "); 
    }//end of if else 

    // Proceed with normal while loop for simualktikon 

}//end of main()