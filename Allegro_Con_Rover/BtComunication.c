/*file       BtComunication.c
details      This module contains the functions in order to manage the BT serial, which is used to communicate with the rover.
		     It also contains the methods to the correct syncronization. 
author     Davide Rasla
todo       
*/  


#include "main.h"

int set_interface_attribs(int fd, int speed)
{
    struct termios tty;

    if (tcgetattr(fd, &tty) < 0) {
        printf("Error from tcgetattr: %s\n", strerror(errno));
        return -1;
    }

    cfsetospeed(&tty, (speed_t)speed);
    cfsetispeed(&tty, (speed_t)speed);

    tty.c_cflag |= (CLOCAL | CREAD);                    // ignore modem controls 
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;                                 // 8-bit characters 
    tty.c_cflag &= ~PARENB;                             // no parity bit 
    tty.c_cflag &= ~CSTOPB;                             // only need 1 stop bit 
    tty.c_cflag &= ~CRTSCTS;                            // no hardware flowcontrol 

    // setup for non-canonical mode 
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    tty.c_oflag &= ~OPOST;

    // fetch bytes as they become available 
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 1;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        printf("Error from tcsetattr: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}

void set_mincount(int fd, int mcount)
{
    struct termios tty;

    if (tcgetattr(fd, &tty) < 0) {
        printf("Error tcgetattr: %s\n", strerror(errno));
        return;
    }

    tty.c_cc[VMIN] = mcount ? 1 : 0;
    tty.c_cc[VTIME] = 5;        

    if (tcsetattr(fd, TCSANOW, &tty) < 0)
        printf("Error tcsetattr: %s\n", strerror(errno));
}


void InitBT()
{
   
    set_interface_attribs(fd, B115200);                              //baudrate 115200, 8 bits, no parity, 1 stop bit 
    set_mincount(fd, 0);                                             // set to pure timed read 

    tcdrain(fd);                                                     // delay for output 
        
}

//--------------------------------------------------------------
//                  BT Serial task
//--------------------------------------------------------------
//This function reads from serial a buffer in this form:
//[112, Distance from 0, Distance from 45, Distance from 90, Distance from 135, Distance from 180, 114, Last distance ping, Position and direction, 115]

void* GetFromSerial(void *arg){
    int a;
   // int count = 0;
    unsigned int found = 0;                                                  // used to be sure we have found 112 in the serial
    int j = 0;                                                               // used to correct the serial and syncronize it
    int i;                                                                   // used to scan the array
    int rdlen;                                                               // used to get exactly 10 bytes
    a  = get_task_index(arg);                                 
    set_activation(a);  
/*
//useful in case we want simulate without starting the rover 
 BufFromSerialBT[0] = 112; 
 BufFromSerialBT[1] =  20;
 BufFromSerialBT[2] =  37;
 BufFromSerialBT[3] =  14;
 BufFromSerialBT[4] =  35;
 BufFromSerialBT[5] =  40;
 BufFromSerialBT[6] = 114;
 BufFromSerialBT[7] =  81;
 BufFromSerialBT[8] =   3;
 BufFromSerialBT[9] = 115;

    do {


if(count == 50){
 BufFromSerialBT[0] = 112; 
 BufFromSerialBT[1] =  20;
 BufFromSerialBT[2] =  37;
 BufFromSerialBT[3] =  14;
 BufFromSerialBT[4] =  35;
 BufFromSerialBT[5] =  40;
 BufFromSerialBT[6] = 114;
 BufFromSerialBT[7] =  81;
 BufFromSerialBT[8] =  rand() % 100 + 1;
 BufFromSerialBT[9] = 115;

}else if (count == 60){

 BufFromSerialBT[0] = 112; 
 BufFromSerialBT[1] = 45;
 BufFromSerialBT[2] =  77;
 BufFromSerialBT[3] =  34;
 BufFromSerialBT[4] =  45;
 BufFromSerialBT[5] =  60;
 BufFromSerialBT[6] =  114;
 BufFromSerialBT[7] =  rand() % 100 + 1;
 BufFromSerialBT[8] =  rand() % 8 + 1;
 BufFromSerialBT[9] = 115;
}
else if (count == 70){//Change values of serial

 BufFromSerialBT[0] = 112; 
 BufFromSerialBT[1] = 34;
 BufFromSerialBT[2] =  66;
 BufFromSerialBT[3] =  87;
 BufFromSerialBT[4] =  77;
 BufFromSerialBT[5] =  43;
 BufFromSerialBT[6] =  114;
 BufFromSerialBT[7] =  rand() % 100 + 1;
 BufFromSerialBT[8] =  rand() % 8 + 1;
 BufFromSerialBT[9] = 115;
count = 0;
}
count ++;
*/
    do{

        rdlen = read(fd, BufFromSerialBT, DimBTBuffer);
        rdlen = rdlen;
        if (rdlen > 0) {
            if(rdlen < DimBTBuffer){
                while(rdlen != DimBTBuffer)                                                   // I want exactly 10 bytes
                    rdlen += read(fd, &BufFromSerialBT[rdlen], 1);
            }

            if(BufFromSerialBT[0] != StartByte)                                               //The first position is not correct ( !='p') see below for an example
            {
                for (i = 0; i< DimBTBuffer; i++){
                   printf("i've read [%d]: %d\n", i, (int)BufFromSerialBT[i]);
                    }
               printf("Correction\n");
                for(i = 0; i < DimBTBuffer; i++){                                             //look for the position with 112
                    if((int)BufFromSerialBT[i] == StartByte){
                        rdlen = i;
                        found = 1;
                        }
                    }

                if(found){      
                    for(i = rdlen; i < DimBTBuffer; i++){
                         BufFromSerialBT[j] = BufFromSerialBT[i];                              //shift in the correct position
                         j++;
                        }
                         while(j != DimBTBuffer)
                                j += read(fd, &BufFromSerialBT[j], 1);                         //read the last 10 - j byte
                        found = 0;
                        }
            }
                for (i = 0; i< DimBTBuffer; i++){
                   printf("i've read [%d]: %d\n", i, (int)BufFromSerialBT[i]);
                    }
       } else if (rdlen < 0) {
            printf("Error from read: %d: %s\n", rdlen, strerror(errno));
            end = 1;
            pthread_exit(NULL);                                                               //MUST RECONNECT, the module bt is down
         }else {
                                                                                               //nothing to do 
             }
   if(deadline_miss(a)){                                                                       // checks for deadline miss
        tp[a].dmiss++;
    }
    wait_for_activation(a);   
} while (!end);

pthread_exit(NULL);
}
/*
i've read [0]: 36
i've read [1]: 36
i've read [2]: 36
i've read [3]: 112  <====== rdlen = 3;
i've read [4]: 89
i've read [5]: 39
i've read [6]: 40
i've read [7]: 42
i've read [8]: 79
i've read [9]: 114

correction

i've read [0]: 112  <====== buffer[j] = buffer[rdlen] = 112
i've read [1]: 89           // Distance from 0 degree
i've read [2]: 39           // Distance from 45 degree
i've read [3]: 40           // Distance from 90 degree
i've read [4]: 42           // Distance from 135 degree
i've read [5]: 79           // Distance from 180 degree
i've read [6]: 114          // 'r' 
i've read [7]: 100          // Ping distance 
i've read [8]: 2            // Compass's result, from 1 to 8
i've read [9]: 115          // 's'
*/
