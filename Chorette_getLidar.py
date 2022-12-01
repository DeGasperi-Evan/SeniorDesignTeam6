#!/usr/bin/env python3
'''
Author: Nicholas Chorette
Date: 11/29/2022
Purpose: Little library that allows you to scan the area ONCE, and returns the array back. Please make sure to run the lidar init (or, literally just when the lib is imported, it nnaturally runs anything NOT in a function.... so just FORCE it to preform an init.... yeah 

'''

import os
#from math import floor #already need several other things below, as you can see I have imported EVERYTHING from math to save time, left here for note-taking reasons
from adafruit_rplidar import RPLidar

#-----Additional Libs-----
from math import *
import time
#import matplotlib.pyplot as plt
#import numpy as np




# Setup the RPLidar
PORT_NAME = '/dev/ttyUSB0'
try: 
    lidar = RPLidar(None, PORT_NAME, timeout=3)
except: #probably on the other USB1 port, for some reaosn 
    PORT_NAME = '/dev/ttyUSB1'
    lidar = RPLidar(None, PORT_NAME, timeout=3)

# used to scale data to fit on the screen
max_distance = 0

outPath = "lidarGoBrr" #The Lidar Pipe Name


#-----------------Pipe Stuff----------------

'''
def restartProcess(): 


    os.system("pkill python3") #kill the all Pozyx process AND this program 
'''



#---Make Pipe (if one is not avaliable)---
try:
    os.mkfifo(outPath, 0o600)
    print(f"Pipe named '{outPath}' is created successfully.\n")

    #out = os.open(outPath, os.O_WRONLY)
    #print("Lidar Data Pipe Now Open... ")

except:
    print(f"Pipe '{outPath}' already exists\n" )




#----I willl let the code freak out and error here: Essential for this to be open, Code cannot work otherwise! 
out = os.open(outPath, os.O_WRONLY)
print("Lidar Data Pipe Now Open... (Chorette_getLidar.py)")


#-------This is where the "real program" begins B)
scan_data = [0]*360

degToRad = pi/180

mmToMeters = 1 / 1000

def process_data(data):
    
    #---Josiah's Shenanaginz----
    left = [0]*60
    leftAvg = 0

    right = [0]*60
    rightAvg = 0

    front = [0]*60
    frontAvg = 0

    for i in range(60): #------Left------
        left[i] = scan_data[ i + 30] * sin( degToRad*(i + 30) ) * mmToMeters
        leftAvg = leftAvg + left[i]
    leftAvg = leftAvg / 60.0

    for i in range(60): #------Right#------
        holder = 59-i #cause it technically needs to decriment in the loop, but whatever 

        right[holder] = scan_data[ i + 270] * sin(  degToRad*(holder + 30) ) * mmToMeters
        rightAvg = rightAvg + right[holder]
    rightAvg = rightAvg / 60.0

    for i in range(30):  #------Front#------
        front[ i + 30] = scan_data[ i ] * cos( degToRad * i ) * mmToMeters
        frontAvg = frontAvg + front[ i + 30]
    
   #also front
    for i in range(330, 360): 
        #print("RANGE: " + str(i))
        front[ i - 330] = scan_data[ i - 330]*cos( degToRad *(360 - i ) ) * mmToMeters
        frontAvg = frontAvg + front[i-330]

    frontAvg = frontAvg / 60.0
    
    # --- bottom text ---

    #temp = "{leftAvg},{frontAvg},{rightAvg}\n"
    #print("TEMP: " + str(temp))
    #----------Pipe data over to, well, the pipe :) 
    #try: #should run if pipe is open <3 
    #sendData(data, out)

    #----THE ONE BELOW THIS SHOULD WORK1@!!----


    #print("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA")

    

   

    os.write(out, bytes(f"{leftAvg},{frontAvg}, {rightAvg}\n","utf-8"))
    #print("\n\nDATA SUCCESSFULLY TRANSFERED THRU PIPE!!! \n\n")#must come right after previous command

    viewData = 4
    print("Data Sent: \nleftAvg: " + str(leftAvg)[0:viewData] + "\nfrontAvg: " + str(frontAvg)[0:viewData] + "\nrightAvg: " + str(rightAvg)[0:viewData] + "\n\n")


   
    






#------------Start of the main lidar program------------

#makePipe(outPath) 



print("TST")
while True: #Lord, Please dont let lidar disconnect from USB port again, i gonna lose it <3 Amen - Nic Chorette  (Probably wire issue -> No, its the adapter for the lidar, acutally! Get new one from Chemical Cabinet!) 
    #try:
    try:
    #    print(lidar.get_info())
        for scan in lidar.iter_scans():
            for (_, angle, distance) in scan:
                #print(angle)
                scan_data[min([359, floor(angle)])] = float(distance)
                #print("ANGLE: " + str(angle) + "\nDistance (mm): " + str(distance) + "\n-------O----\n")
            #print("fucky wucky")
            #print(scan_data)
            process_data(scan_data)



    except KeyboardInterrupt:
        print('Stopping.')

        #if this is going down, 

        exit(0)

    #attempt to restart the lidar 
    #lidar.stop()
    #lidar.disconnect()

    #lidar = RPLidar(None, PORT_NAME, timeout=3) #SHOULD start the lidar back up 

    except: #Typically, this means the lidar failed to start right, 
        print("lidar Boo Boo: Try again (From Chorette_getLidar.py)")
        #lidar.disconnect()
        #lidar.stop()
        

        time.sleep(1)#PLEASE REMOVE B4 REAL RACE 

        #lidar = RPLidar(None, PORT_NAME, timeout=3) #sttempts to revive the lidar 

        #Just kill the program, its not doing hwat it hsould and stopping so yeah 
        #to stop all other processes, just needs to close a pipe, not sure hwy that does not ahppen here... its off (probably cause I do not have an inconsistent sensor, and the exception thing kills all othere pipes.... cause it, when its done, LITERALLY EXITS 0 AFTER IT ATTEMPTS TO CLOSE ALL PIPES 0_0 

        #sCREW IT: RE-RUN PGM, AND CLOSE ITSELF <3 (if possible )




        #os.system("//usr/bin/python3 /home/pi/Chorette_getLidar.py &")

        #Only salvation for this damned code can be found here (I hopoe 0_0) 
        os.system("//usr/bin/python3 killEverything.py &") #this is what causes it to open back up after lidar poop self, and keep going, even after its "stopped" (my b) 


     #   exit(0) #stops current pgm , cause it talked to the lidar at the wrong time, lets home that it is better :) 



