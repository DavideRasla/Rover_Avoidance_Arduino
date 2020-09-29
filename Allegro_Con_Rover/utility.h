#ifndef UTILITY_H
#define UTILITY_H


char get_scancode();
void time_copy(struct timespec *td, struct timespec ts);
void time_add_ms(struct timespec *t, int ms);
int time_cmp(struct timespec* t1, struct timespec t2);
int GetDistanceX(int, int);
int GetDistanceY(int, int);
void UpdateRect();
void UpdateRoverPosition(int);
void* CommandReceiver(void* arg);
void selected_cmd(char );
int GetY(int);
void DrawHistogram();

#endif