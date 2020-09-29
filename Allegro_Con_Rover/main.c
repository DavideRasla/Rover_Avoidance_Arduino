/*file       main.c
details      This module contains the function to start the whole system.

author     Davide Rasla
todo       
*/  
#include "main.h"

//-----------------------------------------------------
//					MAIN TASK
//-----------------------------------------------------
//Activates the graphic allegro mode,
//Open Serial Bluetooth
//Initialize graphics and tasks
//-----------------------------------------------------
 
int main(int argc, char **argv)
{
/*Open Serial Bluetooth*/
int i;
	if(argc < 2) 													//if we cannot define which rfcomm open
		exit(1); 													//exit

	strcpy(portname,"/dev/rfcomm");									
	strncat(portname, argv[1], 2);
	fd = open(portname, O_RDWR | O_NOCTTY | O_SYNC);
		if (fd < 0) {
		printf("Error opening %s: %s\n", portname, strerror(errno));
		return -1;
		}

	InitBT();														//Sets the baudrate and parity bit 
	graphics_init();												//init the GUI	
	init();															//Init the tasks

		for(i = 1; i <= NumTsk; i++){						   		// i is the argument. OBS1, OBS2, OBS3, OBS4, OBS5, Upd_Graph, BTMNG, CMD_RCV, Rover_Tsk
				wait_for_task_end(i);
		}
/*the same above but extended
		wait_for_task_end(OBS1);
		wait_for_task_end(OBS2);
		wait_for_task_end(OBS3);
		wait_for_task_end(OBS4);
		wait_for_task_end(OBS5);
		wait_for_task_end(Upd_Graph);
		wait_for_task_end(BTMNG);
		wait_for_task_end(CMD_RCV);
		*/
    allegro_exit();

	return 0;
}