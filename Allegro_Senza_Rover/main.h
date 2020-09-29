#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>
#include <allegro.h>
#include <time.h>
#include <math.h>
#include <errno.h>
#include <fcntl.h> 
#include <termios.h>


#include "ptask.h"
#include "graphics.h"
#include "utility.h"
#include "BtComunication.h"


#ifndef MAIN_H
#define MAIN_H
//-----------------------------------------------------
// 				TASK CONSTANTS
//-----------------------------------------------------

#define PER_UG		20			// GRAPHICS Manager task period in ms

#define DL_UG 		20			// relative deadline in ms for GRAPHICS Managertask

#define PER_OBS		40			// Obstacle task period in ms

#define OBS_DL		40			// relative deadline in ms for Obstacle task

#define PER_Rover	40			// Rover task period in ms

#define Rover_DL	40			// relative deadline in ms for Rover task

#define PER_CR		60			// Command_receiver's period in ms

#define CR_DL		60			// relative deadline in ms for Command_receivers task

#define PER_BM		80			// BT Manager's period in ms

#define BT_DIS		80			// relative deadline in ms for BT Manager task

#define PRI 		80			// task base priority

#define OBS1 		1			// Obstacle 1 argument

#define OBS2 		2			// Obstacle 2 argument

#define OBS3 		3			// Obstacle 3 argument

#define OBS4 		4			// Obstacle 4 argument

#define OBS5 		5			// Obstacle 5 argument

#define BTMNG 		6			// BT Manager's argument

#define Upd_Graph	7			// Graphics Manager's argument

#define CMD_RCV		8			// Command_receiver's argument

#define Rover_Tsk	9			// Rover's argument

#define NumTsk		9			// Number of task


//-----------------------------------------------------
// 				GRAPHICS CONSTANTS
//-----------------------------------------------------

#define XWIN 		1920		// screen x resolution

#define YWIN 		1080		// screen y resolution

#define ColorScreen 7			// Background Color

#define SizeColArr 	5			// Size of array Color

#define Num_Obst	5			// Numer of obstacles

#define RADIUSOBS   30			// The radius of the obstacle

#define RoverXPos   900			// X position of the rover

#define RoverYPos	800			// Y positon of the rover

#define RoverWidth	150			// Rover's bitmap witdh

#define RoverHeight 150			// Rover's bitmap height

#define MaxRoverPos 5			// Rover's max update of distance 

#define DXrover 	50			// Rover's constant to increment his position

#define CompWidth	150			// Compass's bitmap witdh

#define CompHeight 	150			// Compass's bitmap height

#define ArrowColor	5			// Arrow's color

#define RoverDelay  500000		// delay to update position of rover

#define GraphDelay  800			// delay to update the graphics



//------------------------- Graphic constant of the distances from rover----------------

#define Meter		100			// 100 cm

#define RangeFromXP	280			// To simulate the range on the x axis of 30cm from rover and avoid collisions for positive value

#define RangeFromXN	170			// To simulate the range on the x axis of 30cm from rover and avoid collisions for negative value

#define RangeFromY	150			// To simulate the range on the y axis of 30cm from rover and avoid collisions 

#define RangeFromY	150			// To simulate the range on the y axis of 30cm from rover and avoid collisions 

#define MaxRangeX	650			// Max distance from rover in the x axis 

#define MaxRangeY	600			// Max distance from rover in the y axis 

#define ONEHUNDRED	100			// 100 value for proportions

//------------------------- Graphic constant for histogram----------------

#define HeightH 	700			// Height of histogram's axis
	
#define ColorHist	12			// Color of histogram

#define PositionH 	250			// POsition of histogram 

#define PosXLine 	80			// Position of X axis

#define PosYLine 	280			// Position of Y axis

#define Position100 450			// Position Y of label 100

#define Position50 	575			// Position Y of label 50

#define LabelPos100	80			// Position X of label 100

#define DistXLabel  20			// Position X of label 50

#define SizeS		80			// Size of Buffer 

#define SizePoints	8			// Number of points


//-------------------Rect position----------------------------------

#define H_BOX_X1	1400 		// length of the header box

#define H_BOX_X2	1870 		// length of the header box

#define H_BOX_Y1	10			// height of the header box

#define H_BOX_Y2	160			// height of the header box


//-------------------Compass's position ----------------

#define PosXCompass 100 		// Compass X position

#define PosYCompass 250			// Compass Y position

//------------------- The position of the arArrows of the compass ----------------


#define NordArrowX1	185			// Compass Nord  Arrow position

#define NordArrowY1	210

#define NordArrowX2	155

#define NordArrowY2	240

#define NordArrowX3	215

#define NordArrowY3	240



#define NordEstArrowX1	245		// Compass Nord-Est  Arrow position

#define NordEstArrowY1	260

#define NordEstArrowX2	210

#define NordEstArrowY2	275

#define NordEstArrowX3	235

#define NordEstArrowY3	295




#define EstArrowX1	280			// Compass Est Arrow position

#define EstArrowY1	325

#define EstArrowX2	265

#define EstArrowY2	305

#define EstArrowX3	265

#define EstArrowY3	345



#define SudEstArrowX1 235			// Compass Sud-Est Arrow position

#define SudEstArrowY1 385

#define SudEstArrowX2 205

#define SudEstArrowY2 380

#define SudEstArrowX3 235

#define SudEstArrowY3 350



#define SudArrowX1	180			// Compass Sud Arrow position

#define SudArrowY1	430

#define SudArrowX2	160

#define SudArrowY2	410

#define SudArrowX3	200

#define SudArrowY3	410



#define SudOvestArrowX1	150		// Compass Sud-Ovest Arrow position

#define SudOvestArrowY1	365

#define SudOvestArrowX2	110

#define SudOvestArrowY2	375

#define SudOvestArrowX3	130

#define SudOvestArrowY3	345



#define OvestArrowX1	70			// Compass Ovest Arrow position

#define OvestArrowY1	325

#define OvestArrowX2	90

#define OvestArrowY2	305

#define OvestArrowX3	90

#define OvestArrowY3	345



#define NordOvestArrowX1	130		// Compass Nord-Ovest Arrow position

#define NordOvestArrowY1	305

#define NordOvestArrowX2	110

#define NordOvestArrowY2	270

#define NordOvestArrowX3	150

#define NordOvestArrowY3	270

//------------------Histogram data-------------------

//Histogram Vertix

//		P2-P3
//		|  |
//		|  |
//		|  |
//		|  |
//		|  |
//		|  |
//		P1-P4

#define XP1 170
#define YP1 700

#define XP2 170

#define XP3 180

#define XP4 180
#define YP4 700

//------------------Bluetooth data------------------

#define StartByte 	112			// First valid value in the buffer of bluetooth serial

#define DimBTBuffer 10			// Size of buffer bluetooth

//------------------------- Constant for Serial Array----------------

#define PosStartByte 	0		//Position of StartByte equals to 112

#define PosDis0 		1		//Position of distance from 0 in the serial array

#define PosDis45 		2		//Position of distance from 45 in the serial array

#define PosDis90 		3		//Position of distance from 90 in the serial array

#define PosDis135 		4		//Position of distance from 135 in the serial array

#define PosDis180		5		//Position of distance from 180 in the serial array

#define PosPing 		7		//Position of Ping in the serial array

#define PosCompass 		8		//Position of the compass's result in the serial array


//-----------------------------------------------------
// 				GLOBAL DATA STRUCTURE
//-----------------------------------------------------

/*
Structure used to create a periodic task 
*/

struct task_par {
	
	int 	arg;					// task argument
	long 	wcet;					// worst case esecution time in microseconds
	int 	period;					// task period in milliseconds
	int 	deadline;				// relative deadline (ms)
	int 	priority;				// task priority in [0,99]
	int 	dmiss;					// number of deadline missed
	struct 	timespec at;			// next activation time
	struct 	timespec dl;			// absolute deadline
};




struct Rover_Status
{
	int x;										// X position
	int y; 										// Y position
	int DistanceXFromObs[Num_Obst + 1 ];		// distance X from obstacles
	int DistanceYFromObs[Num_Obst + 1 ];		// distance  Y from obstacles
	int DistFromScan; 							// distance obtained from task that scan and ping 	
};


struct Obstacle
{
	int 	c;						// color of obstacle
	int 	r;						// radius
	float 	x;						// coordinate x	
	float	y;						// coordinate y;
};




int 			end;													// used to terminate the tasks
pthread_t 		tid[NumTsk + 1];										// array containing the threadID of all the activated trhead
struct task_par tp[NumTsk + 1];	   										// array containing the struct task_par

struct Rover_Status 		RoverStatus;  								// initialize of struct status of rover
struct Obstacle 			Obs[Num_Obst + 1];		   					// initialize of struct obstacle


//Bitmap for graphics

BITMAP* RoverFromBehind,*RoverFromBehindp, *CompassBPM, *CompassBPMp;	// pointers to bitmap of rover and compass

BITMAP* bmap;															// pointer to bmap used to draw

/* BT data */
 char 			portname[60];											// Port rfcomm used to communication
 int 			fd;														// fd of rfcomm
 int 			wlen;													// len of read
 unsigned char  BufFromSerialBT[DimBTBuffer];							// buffer that contains the data recived from rover over BT

#endif	