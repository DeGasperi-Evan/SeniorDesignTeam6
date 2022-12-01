#!/usr/bin/env python3

import os
from time import *

'''
Author: Nicholas Chorette
Date: 11/30/2022
Purpose: To act as a middle-man between the Chorette_getLidar.py program. 
Due to the lidar program needing to be fully restarted in python when it fails, this program's goal is to simply sleep for X-time, THEN re-run the process, finishing its job <3

'''
sleep(1) #sleep for X-time (seconds) 

os.system("pkill bash")

os.system("/home/pi/start2.sh ")
#end of program :) 
