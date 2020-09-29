/*file       graphics.c
details      This module contains the function to manage the graphic. In particular it draws the objects like the compass,
		     the obstacles, rover and what received from the serial on the screen.

author     Davide Rasla
todo       
*/  
#include "main.h"

//---------------------------------------------------------------------------------------------
//										Graphics task
//---------------------------------------------------------------------------------------------
//Task that manages the graphics of the system,
//draws the rover, the obstacles and change their position
//---------------------------------------------------------------------------------------------

void* UpdateGraph(void* arg){
int 	a, i;
char 	s[5];																						// To save the distance and draw it next to the obstacle

	a = get_task_index(arg);									
	bmap 	= create_bitmap(XWIN, YWIN);															// bitmap on which draws 	

	set_activation(a);	

	while(!end){

	clear_bitmap(bmap);		
	clear_to_color(bmap, ColorScreen);																// clears to 7 color the nmap 
	UpdateRect();																					// updates rect data
	UpdateCompass(); 																				// Updates compass in relation of the position of the rover
	
	for(i=PosDis0; i<=PosDis180; i++){
		circlefill(bmap, Obs[i].x, Obs[i].y, Obs[i].r, Obs[i].c);									// draws the obstacle[i]
		sprintf(s, "%d", BufFromSerialBT[i]);
		textout_ex(bmap,font,s,Obs[i].x,Obs[i].y + DXrover,12,14);  	 							// draws the distance next to him
		}

	stretch_sprite(bmap, RoverFromBehindp, RoverStatus.x, RoverStatus.y, RoverWidth, RoverHeight);	// draws the rover in the updated position

	DrawHistogram();																				// draws the histogram
	
	blit(bmap, screen, 0, 0, 0, 0, bmap->w, bmap->h);												// copying the bitmap bmap on the screen bitmap
	usleep(GraphDelay);

	if(deadline_miss(a)){																			// checks for deadline miss
				tp[a].dmiss++;
			}
	wait_for_activation(a);	
	}//while

	destroy_bitmap(bmap);																			// destroy the bitamp bmap
	pthread_exit(NULL);
}	




//--------------------------------------------------------------
//					Obstacle task
//--------------------------------------------------------------
//This task updates the position of each obstacle, reading the distance from the serial array at the arg's task position.
//--------------------------------------------------------------

void* UpdateObstacle(void* arg){
	int 	ind;													//argument used to access to the struct at the position from OBS1 to OBS5
	int 	c [SizeColArr] = {1,2,3,4,5};							//color used for obstacles

	ind = get_task_index(arg);				
	set_activation(ind);				

	while(!end){
	Obs[ind].c = c[ind];
	Obs[ind].r = RADIUSOBS;
	Obs[ind].x = GetDistanceX(ind, BufFromSerialBT[ind]);			//Calculate the distance from the rover in relation of what read from serial
	Obs[ind].y = GetDistanceY(ind, BufFromSerialBT[ind]);			//Calculate the distance from the rover in relation of what read from serial
	RoverStatus.DistanceXFromObs[ind] = Obs[ind].x;					//Update the struct RoverStatus
	RoverStatus.DistanceYFromObs[ind] = Obs[ind].y;
	if(deadline_miss(ind)){											// checks for deadline miss
			tp[ind].dmiss++;
		}
	wait_for_activation(ind);	
	}
	pthread_exit(NULL);
}


//--------------------------------------------------------------
//					Rover task
//--------------------------------------------------------------
//This task updates the position of the rover, calculating the distance from the obstacles
//--------------------------------------------------------------

void* UpdateRover(void* arg){
	int 	ind;													
	int 	posRover = 1;											// To move the position of rover 

	ind = get_task_index(arg);				
	set_activation(ind);				

	while(!end){
	UpdateRoverPosition(posRover);									// Update the position of the rover in relation to the position index
	posRover = (posRover == MaxRoverPos)? 1 : posRover + 1;
	usleep(RoverDelay);												// delay to simulate the progress of the rover 
	if(deadline_miss(ind)){											// check for deadline miss
			tp[ind].dmiss++;
		}
	wait_for_activation(ind);	
	}
	pthread_exit(NULL);
}



/*This function draws the compass and the relative arrows */
void UpdateCompass(){
	stretch_sprite(bmap, CompassBPMp, PosXCompass, PosYCompass, CompWidth, CompHeight);

		/* reading the position of the rover and drawing the arrow on the compass*/
		switch (BufFromSerialBT[PosCompass]){
			case 1://Nord
				triangle (bmap, NordArrowX1, NordArrowY1, NordArrowX2, NordArrowY2, NordArrowX3, NordArrowY3, ArrowColor);
				break;
			case 2://Nord-Est
				triangle (bmap, NordEstArrowX1, NordEstArrowY1, NordEstArrowX2, NordEstArrowY2, NordEstArrowX3, NordEstArrowY3, ArrowColor);
				break;
			case 3://Est
				triangle (bmap, EstArrowX1, EstArrowY1, EstArrowX2, EstArrowY2, EstArrowX3, EstArrowY3, ArrowColor);
				break;
			case 4://Sud-Est
				triangle (bmap, SudEstArrowX1, SudEstArrowY1, SudEstArrowX2, SudEstArrowY2, SudEstArrowX3, SudEstArrowY3, ArrowColor);

				break;
			case 5://Sud
				triangle (bmap, SudArrowX1, SudArrowY1, SudArrowX2, SudArrowY2, SudArrowX3, SudArrowY3, ArrowColor);
				break;
			case 6://Sud-Ovest
				triangle (bmap, SudOvestArrowX1, SudOvestArrowY1, SudOvestArrowX2, SudOvestArrowY2, SudOvestArrowX3, SudOvestArrowY3, ArrowColor);
				break;
			case 7://Ovest
				triangle (bmap, OvestArrowX1, OvestArrowY1, OvestArrowX2, OvestArrowY2, OvestArrowX3, OvestArrowY3, ArrowColor);	
				break;
			case 8://Nord-Ovet
				triangle (bmap, NordOvestArrowX1, NordOvestArrowY1, NordOvestArrowX2, NordOvestArrowY2, NordOvestArrowX3, NordOvestArrowY3, ArrowColor);
				break;
			default:
				break;
		}
}

/*This function creates the compass */
void CreateCompass(){
	CompassBPMp = load_bitmap("Compassp.bmp", NULL);
	if(CompassBPMp == NULL) {
		printf("file not found\n");
		exit(1);
	}
}


/*
This function initialiaze the bitmap of rover 
*/

void createRover(){
	RoverFromBehindp = load_bitmap("roverg.bmp", NULL);
	if(RoverFromBehindp == NULL) {
		printf("file not found\n");
		exit(1);
	}
	RoverStatus.x = RoverXPos;
	RoverStatus.y = RoverYPos;
/*Useful in case we must change background
	white = makecol (255, 255, 255);

	RoverFromBehindp = create_bitmap (RoverFromBehind->w, RoverFromBehind->h);
	for ( x=0; x<RoverFromBehind->w; x++ )
		for( y=0; y<RoverFromBehind->h; y++ ) {
			c = getpixel(RoverFromBehind, x, y);
			if(c == white) c = 7;
			putpixel(RoverFromBehindp, x, y, c);
		}

	get_palette (pal);
	save_bitmap ("roverg.bmp", RoverFromBehindp, pal);
*/
}

/*
Initialiaze allegro graphic.
*/

void graphics_init(){

	allegro_init();										// initialises the Allegro library
	set_color_depth(8);									// sets the color depth to 8 bit mode

	set_gfx_mode(GFX_AUTODETECT, XWIN, YWIN, 0, 0);		// switch into graphics mode 
	clear_to_color(screen, ColorScreen);				// clears to 7 color the screen 
	install_keyboard();
	createRover();										// Create the bitmap for the rover
	CreateCompass();									// Create the compass
}



/*
Sets the scheduling policy for the main task.
Creates the tasks:
 - To update the GUI
 - To get data from serial
 - To receive the command from user
 - To calculate the position of obstacles. One task for each obstacle
 - To update the position of the rover
*/
void init(){
	ptask_init(SCHED_FIFO);
	int ret;
	 ret = task_create(Upd_Graph, UpdateGraph, PER_UG, DL_UG, PRI + 5);					//task to update GUI
	 ret = task_create(BTMNG, GetFromSerial, PER_BM, BT_DIS, PRI + 3 );					//tasl to get data from serial
	 ret = task_create(CMD_RCV, CommandReceiver, PER_CR, CR_DL, PRI + 2);				//task to get commands

	
	 ret = task_create(OBS1, UpdateObstacle, PER_OBS, OBS_DL, PRI);						//init one task for each obstacle
	 ret = task_create(OBS2, UpdateObstacle, PER_OBS, OBS_DL, PRI);
	 ret = task_create(OBS3, UpdateObstacle, PER_OBS, OBS_DL, PRI);
	 ret = task_create(OBS4, UpdateObstacle, PER_OBS, OBS_DL, PRI);
	 ret = task_create(OBS5, UpdateObstacle, PER_OBS, OBS_DL, PRI);

	 ret = task_create(Rover_Tsk, UpdateRover, PER_Rover, Rover_DL, PRI);				//Init the task for update the rover position

}
