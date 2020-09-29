#ifndef BLUETOOTH_H
#define BLUETOOTH_H
int set_interface_attribs(int , int );
void set_mincount(int , int );
void InitBT();
void* GetFromSerial(void *arg);
#endif