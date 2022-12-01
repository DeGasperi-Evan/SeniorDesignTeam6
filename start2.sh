#!/bin/sh

pkill python3 #cause for some reason, it wants to live. 0_0 B)

#might be needed, not sure: Yes, that delays, in bash, everything by X-time, the "sleep 1" or whatever funciton
#sleep 1


#NEW! Lidar Daemon <3
python3 Chorette_getLidar.py &

python3 allPozyx2.py &


#NEW! Lidar Daemon <3
#python3 Chorette_getLidar.py &

#gcc team6SimCode.c "comms.c" -lm && ./a.out

#------TECHNICALLY, THIS IS THE COMMAND IT NEEDS TO RUN TO ENABLE REVERSE, BUT ISSUE IS, REVERSE NEEDS TO BE TWEAKED INORDER TO WORK AS INTENDED 
gcc team6SimCodeNew2.c "comms.c" "dataCollection.c" "timePassed.c" "reverse.c" -lm && ./a.out

#gcc team6SimCodeNew2.c "comms.c" "timePassed.c" "reverse.c" -lm && ./a.out

#gcc team6SimCode.c "comms.c" "dataCollection.c" "timePassed.c" -lm && ./a.out

#clear && y
