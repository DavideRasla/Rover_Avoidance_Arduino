/*file       utility.c
details      This module contains the functions to calculate some data like distances, positions, and read commands

author     Davide Rasla
todo       
*/  
#include "main.h"


//---------------------------------------------------------------------------------------------
//										Utility 
//---------------------------------------------------------------------------------------------


/*	these functions take in input the index of the obstacle 
	and the distance taken from the serial array. 
	Using a proportion they calculate where to place the obstacle.
	RangeFromXP has been calculated experimentally to simulate the 30 cm beyond which the rover must be stopped
	MaxRangeX has been calculated in relation to the size of the screen
*/
int GetDistanceX( int i, int x){

int X = (x >=Meter)?Meter:x;									//if x > 100, assume 100
	switch(i){
		case 1://0 degree
			return RoverXPos + RangeFromXP + ((X * MaxRangeX) / ONEHUNDRED);  
		case 2://45 degree
			return RoverXPos + RangeFromXP + ((X * MaxRangeX) / ONEHUNDRED);
		case 3://90 degree
			return RoverXPos;
		case 4://135 degree
			return RoverXPos - RangeFromXN - ((X * MaxRangeX) / ONEHUNDRED);
		case 5://180 degree
			return RoverXPos - RangeFromXN - ((X * MaxRangeX) / ONEHUNDRED);
		default:
				break;
			}
	return RoverXPos;
}
int GetDistanceY(int i, int y){

	int Y = (y >=Meter)?Meter:y;
	switch(i){
		case 1://0 degree
			return RoverYPos;
		case 2://45 degree
			return RoverYPos - RangeFromY -  ((Y * MaxRangeY) / ONEHUNDRED);
		case 3://90 degree
			return RoverYPos - RangeFromY -  ((Y * MaxRangeY) / ONEHUNDRED);
		case 4://135 degree
			return RoverYPos - RangeFromY -  ((Y * MaxRangeY) / ONEHUNDRED);
		case 5://189 degree
			return RoverYPos;
		default:
				break;
			}
	return RoverYPos;
}



/* This function calculate the max distance from rover */
int getmaxPosition(){
int max = 0; 
int i;
	for(i = PosDis0; i<PosDis180; i++){
		if(BufFromSerialBT[i] >= max)
			max = BufFromSerialBT[i];
	}

	for(i = PosDis0; i<PosDis180; i++){
		if(BufFromSerialBT[i] == max)
			return i;
	}
}
/* 	This function updates the position of the rover in relation of the farthest direction from him.
	To do this, we can multiply a constant dx for an index from 1 to 5 passed by value   */
void UpdateRoverPosition(int IndexPos){

const int dx = DXrover;
	switch(getmaxPosition()){
	case 1: //go to 0 degree	
			RoverStatus.x = RoverXPos + IndexPos*dx;
			RoverStatus.y = RoverYPos;
			break;
	case 2: //go to 45 degree
			RoverStatus.x = RoverXPos + IndexPos*dx;
			RoverStatus.y = RoverYPos - IndexPos*dx;
			break;	
	case 3: //go to 90 degree
			RoverStatus.x = RoverXPos;
			RoverStatus.y = RoverYPos - IndexPos*dx;
			break;
	case 4: //go to 135 degree
			RoverStatus.x = RoverXPos - IndexPos*dx;
			RoverStatus.y = RoverYPos - IndexPos*dx;
			break;
	case 5: //go to 180 degree
			RoverStatus.x = RoverXPos - IndexPos*dx;
			RoverStatus.y = RoverYPos;
			break;
	default:
			break;
		}
}

/*This funcion uses the position received from rover to return the correct direction */
char*  GetPositionFromCompass(char pos){
	switch (pos){
		case 1:
			return "Nord";
		case 2:
			return "Nord-Est";
		case 3:
			return "Est";
		case 4:
			return "Sud-Est";
		case 5:
			return "Sud";
		case 6:
			return "Sud-Ovest";
		case 7:
			return "Ovest";
		case 8:
			return "Nord-Ovest";			
		 default:
		 	return "Error";
		 		break;
		}			
}
/*
This function writes data collect from serial buffer in the rect:

The serial is in this form:

[112, Distance from 0, Distance from 45, Distance from 90, Distance from 135, Distance from 180, 114, Last distance ping, Position and direction, 115]
*/

void UpdateRect(){
char s[80];

sprintf(s, "L'array seriale è: [%d, %d, %d, %d, %d, %d, %d, %d, %d, %d]", BufFromSerialBT[PosStartByte], BufFromSerialBT[PosDis0], BufFromSerialBT[PosDis45], BufFromSerialBT[PosDis90], BufFromSerialBT[PosDis135], BufFromSerialBT[PosDis180], BufFromSerialBT[6],  BufFromSerialBT[PosPing], BufFromSerialBT[PosCompass], BufFromSerialBT[9]);
textout_ex(bmap, font, s, H_BOX_X1 + 10, 15, 4, -1);

sprintf(s, "La distanza dall'ostacolo a 0 gradi è: %d cm", BufFromSerialBT[PosDis0]);
textout_ex(bmap, font, s, H_BOX_X1 + 10, 25, 4, -1);

sprintf(s, "La distanza dall'ostacolo a 45 gradi è: %d cm", BufFromSerialBT[PosDis45]);
textout_ex(bmap, font, s, H_BOX_X1 + 10, 35, 4, -1);

sprintf(s, "La distanza dall'ostacolo a 90 gradi è: %d cm", BufFromSerialBT[PosDis90]);
textout_ex(bmap, font, s, H_BOX_X1 + 10, 45, 4, -1);

sprintf(s, "La distanza dall'ostacolo a 135 gradi è: %d cm", BufFromSerialBT[PosDis135]);
textout_ex(bmap, font, s, H_BOX_X1 + 10, 55, 4, -1);

sprintf(s, "La distanza dall'ostacolo a 180 gradi è: %d cm", BufFromSerialBT[PosDis180]);
textout_ex(bmap, font, s, H_BOX_X1 + 10, 65, 4, -1);

sprintf(s, "La distanza calcolata al ping è: %d cm", BufFromSerialBT[PosPing]);
textout_ex(bmap, font, s, H_BOX_X1 + 10, 75, 4, -1);

sprintf(s, "Il rover si sta dirigendo verso: %s", GetPositionFromCompass(BufFromSerialBT[PosCompass]));
textout_ex(bmap, font, s, H_BOX_X1 + 10, 85, 4, -1);
}


/*This function calculates the new height of the histogram in relation of the last ping, read from the serial*/

int GetY(int Ping){
/*750 - 450 = 250 => 250:100cm = x: Ping
 	=> X = (Ping * 250)/100;*/
 	return HeightH - ((Ping * PositionH) / ONEHUNDRED);

}

/*This function draws and update the histogram*/

void DrawHistogram(){
int YP2 = GetY(BufFromSerialBT[PosPing]);										//incrment the histogram
int YP3 = GetY(BufFromSerialBT[PosPing]);
int pointsHistogram[SizePoints]={XP1, YP1, XP2, YP2, XP3, YP3, XP4, YP4};
char s[SizeS];


polygon(bmap, 4, pointsHistogram, ColorHist);
line(bmap, PosXLine, HeightH, PosXLine, Position100, ColorHist);				// Y axis
line(bmap, PosXLine, HeightH, PosYLine, HeightH, ColorHist);					// X axis
sprintf(s, "100 cm");
textout_ex(bmap, font, s, DistXLabel, Position100, 4, -1);
sprintf(s, "50 cm");
textout_ex(bmap, font, s, DistXLabel, Position50, 4, -1);
sprintf(s, " %d cm", BufFromSerialBT[7]);
textout_ex(bmap, font, s, XP2 + DistXLabel, YP2, 4, -1);						//print next to the histogram the distance
}

/*
This function check for the esc key pressed 
*/

void selected_cmd(char s){
	switch (s){
		case KEY_ESC:
			printf("esc\n");
			end = 1;
			pthread_exit(NULL);
		break;
		default:
		break;
	}
	return;
}


//*************Utility for commands**********//


//--------------------------------------------------------------
//					Command interpreter task
//--------------------------------------------------------------
//This task reads the commands from the keyboard
//--------------------------------------------------------------

void* CommandReceiver(void* arg){
	int 	ind;
	char 	Command;

	ind = get_task_index(arg);				// contains task index inside the array of the task_par structure
	set_activation(ind);					// computes next activation of the task of index ti
	while (!end){
		Command = get_scancode();			// returns the scancode of a pressed key

		selected_cmd(Command);				// Execute the function specified
		if(deadline_miss(ind)){				// checks for deadline miss
			tp[ind].dmiss++;
		}
		wait_for_activation(ind);			// blocks until next activation
	}
	pthread_exit(NULL);					
}

/*
This function returns the scancode of a pressed key
*/
char get_scancode(){
	
	if (keypressed())
		return readkey() >> 8;
	else
		return 0;
}




/*
function to copy timespec data structure into another
*/
void time_copy(struct timespec *td, struct timespec ts){
	td->tv_sec 	= ts.tv_sec;
	td->tv_nsec = ts.tv_nsec;
}



/*
function to add millisecond to a timespec data structure
*/
void time_add_ms(struct timespec *t, int ms){
	
	t->tv_sec 	+= ms/1000;
	t->tv_nsec 	+= (ms%1000)*1000000;
	
	if (t->tv_nsec > 1000000000) {

		t->tv_nsec 	-= 1000000000;
		t->tv_sec 	+= 1;

	}
}



/*
fuction to compare two timespec data structure.
*/
int time_cmp(struct timespec* t1, struct timespec t2){

	if (t1->tv_sec > t2.tv_sec) 
		return 1;

	if (t1->tv_sec < t2.tv_sec) 
		return -1;
	
	if (t1->tv_nsec > t2.tv_nsec) 
		return 1;
	
	if (t1->tv_nsec < t2.tv_nsec) 
		return -1;
	
	return 0;
}



