#include <iostream>
#include <set>
#include <string.h>
#include <fstream>
#include <queue>
#include <stack>
#include <unistd.h>
#include "oled/Edison_OLED.h"
#include "gpio/gpio.h"
#include <cmath>
#include <thread>
#include <stdlib.h>
#include <errno.h>
#include <curses.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <stdio.h>
#include "Kalman.h"
#include "mraa.hpp"
#include "SFE_LSM9DS0.h"

using namespace std;

void setupOLED();
void startScreen();
void cleanUp();
void updateAngle();
double angle=0;
int locating=0;
// Define an edOLED object:
edOLED oled;
//for main menu
int cntm=0;
//Compass object
LSM9DS0 *imu;
// Pin definitions:
// All buttons have pull-up resistors on-board, so just declare
// them as regular INPUT's
gpio BUTTON_UP(47, INPUT);
gpio BUTTON_DOWN(44, INPUT);
gpio BUTTON_LEFT(165, INPUT);
gpio BUTTON_RIGHT(45, INPUT);
gpio BUTTON_SELECT(48, INPUT);
gpio BUTTON_A(49, INPUT);
gpio BUTTON_B(46, INPUT);
double teta=0;
void setupOLED()
{
	oled.begin();
	oled.clear(PAGE);
	oled.display();
	oled.setFontType(0);
}

void cleanUp()
{
	oled.clear(PAGE);
	oled.display();
}

void cancel_screen(char *c)
{
	oled.clear(PAGE);
	oled.setCursor(10,24);
	oled.print(c);
	oled.display();
}

void main_menu(int cnt)
{
	oled.clear(PAGE);
	oled.setCursor(0,0);
	oled.print("Go to:");
	oled.setCursor(20,10);
	oled.print("Tent A");
	oled.setCursor(20,20);
	oled.print("Tent B");
	oled.setCursor(20,30);
	oled.print("Tent C");
	oled.setCursor(20,40);
	oled.print("Qibla");
	
	switch(cnt)
	{
		case 0:
			oled.line(19,12,14,9);
			oled.line(19,12,14,15);
			oled.display();
			break;
		
		case 1:
			oled.line(19,22,14,19);
			oled.line(19,22,14,25);
			oled.display();
			break;
		case 2: 
			oled.line(19,32,14,29);
			oled.line(19,32,14,35);
			oled.display();
			break;
		case 3:
			oled.line(19,42,14,39);
			oled.line(19,42,14,45);
			oled.display();
	}
		
}

struct hci_request ble_hci_request(uint16_t ocf, int clen, void * status, void * cparam)
{
	struct hci_request rq;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = OGF_LE_CTL;
	rq.ocf = ocf;
	rq.cparam = cparam;
	rq.clen = clen;
	rq.rparam = status;
	rq.rlen = 1;
	return rq;
}


void drawLine(double teta2)
{
//if(teta2-angle<0)
	//teta2+=6.28;
int sx=32, sy=24;
int ex= (int)(24*sin(teta2-angle)+32);
int ey= (int)(-24*cos(teta2-angle)+24);
int lex=(int)(-3*cos(teta2-angle)+17*sin(teta2-angle)+32);
int ley=(int)(-3*sin(teta2-angle)-17*cos(teta2-angle)+24);
int rex=(int)(3*cos(teta2-angle)+17*sin(teta2-angle)+32);
int rey=(int)(3*sin(teta2-angle)-17*cos(teta2-angle)+24);
//cout<<ex<<","<<ey<<endl;
oled.clear(PAGE);
oled.line(sx,sy,ex,ey);
oled.line(ex,ey,lex,ley);
oled.line(ex,ey,rex,rey);
oled.display();
}

int qiblax=0;
void qibla()
{
	while(qiblax==1)
	{		
	
		updateAngle();
		drawLine(4.6);
		sleep(1);
	
	}	
}

//void measure(int);
void getDirection(int, int);
int numbeacons[]={4,3,8};

 

int limit=50;
double *filteredrssi;
Kalman *myFilter;

char * BeaconsAll[]={"DD:D5:F8:17:96:2B", "D9:33:71:96:1F:02", "D4:6F:21:01:30:29", "C3:B1:E9:72:62:A6","C2:E9:25:EB:AC:B3","F3:12:35:E0:A1:02"}; 
//Meshal Fill please with all beacons
char * room1Beacons[]={"DD:D5:F8:17:96:2B", "D9:33:71:96:1F:02", "D4:6F:21:01:30:29", "C3:B1:E9:72:62:A6","C2:E9:25:EB:AC:B3","F3:12:35:E0:A1:02"};  //fill these with beacons of room 1
// char * room2Beacons[]={"D4:10:92:78:F6:8A","D6:F2:25:7B:2D:62","F4:F1:6F:B8:C8:0C"}; //fill these with beacons of room 2
// char * room3Beacons[]={"C2:E9:25:EB:AC:B3","F3:12:35:E0:A1:02","DD:D5:F8:17:96:00","E7:37:27:0D:0A:DA","FE:BA:0B:EC:A9:E7","DD:DD:FC:A2:08:43","FC:2F:60:A9:3A:B6","FF:2F:60:A9:3A:B6"}; 
//fill these with beacons of room 3
int cntAll=6;
int xp,yp;
void measure(int room, char** beacons)
{
	thread t;	
	int ret, status;
	
	int len;
	// Get HCI device.

	const int device = hci_open_dev(hci_get_route(NULL));
	if ( device < 0 ) { 
		perror("Failed to open HCI device.");
	}

	// Set BLE scan parameters.
	
	le_set_scan_parameters_cp scan_params_cp;
	memset(&scan_params_cp, 0, sizeof(scan_params_cp));
	scan_params_cp.type 			= 0x00; 
	scan_params_cp.interval 		= htobs(0x0010);
	scan_params_cp.window 			= htobs(0x0010);
	scan_params_cp.own_bdaddr_type 	= 0x00; // Public Device Address (default).
	scan_params_cp.filter 			= 0x00; // Accept all.

	struct hci_request scan_params_rq = ble_hci_request(OCF_LE_SET_SCAN_PARAMETERS, LE_SET_SCAN_PARAMETERS_CP_SIZE, &status, &scan_params_cp);
	
	ret = hci_send_req(device, &scan_params_rq, 1000);
	if ( ret < 0 ) {
		hci_close_dev(device);
		perror("Failed to set scan parameters data.");
	}

	// Set BLE events report mask.

	le_set_event_mask_cp event_mask_cp;
	memset(&event_mask_cp, 0, sizeof(le_set_event_mask_cp));
	int i = 0;
	for ( i = 0 ; i < 8 ; i++ ) event_mask_cp.mask[i] = 0xFF;

	struct hci_request set_mask_rq = ble_hci_request(OCF_LE_SET_EVENT_MASK, LE_SET_EVENT_MASK_CP_SIZE, &status, &event_mask_cp);
	ret = hci_send_req(device, &set_mask_rq, 1000);
	if ( ret < 0 ) {
		hci_close_dev(device);
		perror("Failed to set event mask.");
	}

	// Enable scanning.

	le_set_scan_enable_cp scan_cp;
	memset(&scan_cp, 0, sizeof(scan_cp));
	scan_cp.enable 		= 0x01;	// Enable flag.
	scan_cp.filter_dup 	= 0x00; // Filtering disabled.

	struct hci_request enable_adv_rq = ble_hci_request(OCF_LE_SET_SCAN_ENABLE, LE_SET_SCAN_ENABLE_CP_SIZE, &status, &scan_cp);

	ret = hci_send_req(device, &enable_adv_rq, 1000);
	if ( ret < 0 ) {
		hci_close_dev(device);
		perror("Failed to enable scan.");
		
	}

	// Get Results.

	struct hci_filter nf;
	hci_filter_clear(&nf);
	hci_filter_set_ptype(HCI_EVENT_PKT, &nf);
	hci_filter_set_event(EVT_LE_META_EVENT, &nf);
	if ( setsockopt(device, SOL_HCI, HCI_FILTER, &nf, sizeof(nf)) < 0 ) {
		hci_close_dev(device);
		perror("Could not set socket options\n");
		
	}
	int numBeac;
	if(room<3)
		numBeac=numbeacons[room];
	else
		numBeac=cntAll;
	

	int cnt[numBeac];
	for (int i=0;i<numBeac;i++)
		cnt[i]=0;
	uint8_t buf[HCI_MAX_EVENT_SIZE];
	evt_le_meta_event * meta_event;
	le_advertising_info * info;
	
	bool condition=true;
	
	while (condition) {
		condition=false;
		int abv=0;
		for (int i=0;i<numBeac;i++)
			if(cnt[i]>limit)
				abv++;
		if(abv<3)
			condition=true;

		// WS: Cancel event when keep pressing the button
		if ((BUTTON_A.pinRead() == LOW))
		{
			sleep(1);
			if ((BUTTON_A.pinRead() == LOW))
			{
				locating=0;
				cancel_screen("CANCELED");
				sleep(2);
				teta=-20;
				
			}
		}

		len = read(device, buf, sizeof(buf));
		if ( len >= HCI_EVENT_HDR_SIZE ) {
			meta_event = (evt_le_meta_event*)(buf+HCI_EVENT_HDR_SIZE+1);
			if ( meta_event->subevent == EVT_LE_ADVERTISING_REPORT ) {
				uint8_t reports_count = meta_event->data[0];
				void * offset = meta_event->data + 1;
				while ( reports_count-- ) {
					info = (le_advertising_info *)offset;
					char addr[18];
					//string addr2;
					ba2str(&(info->bdaddr), addr);
					//cout<<addr2<<endl;
					
					bool cond2=false;
					for(int i=0;i<numBeac;i++)
						cond2=cond2||strcmp(addr,beacons[i])==0;
					if(cond2)
					{
						int rssi=(char)info->data[info->length];
						for(int i=0;i<numBeac;i++)
							if(strcmp(addr,beacons[i])==0)
						
							{
							
								double tmp=myFilter[i].getFilteredValue(rssi);
								filteredrssi[i]=0.9*filteredrssi[i]+0.1*tmp;
								cout<<addr<<" >> ";
								printf("%f\n",filteredrssi[i]);
								printf("\n");
								cnt[i]++;
								updateAngle();
								drawLine(teta);
								// sleep(1);
							}
					}					
					offset = info->data + info->length + 2;
				}
			}
		}
	}
	limit=10;
	
}

bool goal=false;

void updateAngle()
{
	
  uint16_t imuResult = imu->begin();
  //cout<<hex<<"Chip ID: 0x"<<imuResult<<dec<<" (should be 0x49d4)"<<endl;

  bool newAccelData = false;
  bool newMagData = false;
  bool newGyroData = false;
  bool overflow = false;

  

    while ((newGyroData & newAccelData & newMagData) != true)
    {
      if (newAccelData != true)
      {
        newAccelData = imu->newXData();
      }
      if (newGyroData != true)
      {
        newGyroData = imu->newGData();
      }
      if (newMagData != true)
      {
        newMagData = imu->newMData(); // Temp data is collected at the same
                                      //  rate as magnetometer data.
      } 
    }

    newAccelData = false;
    newMagData = false;
    newGyroData = false;

    imu->readMag();
    
 

    double y= imu->calcMag(imu->my);
	double x= imu->calcMag(imu->mx);
    if(y>0)
	angle=(1.5708-atan(x/y));
	else if(y<0) angle =(4.7124-atan(x/y));
	else if(x>0) angle =0;
	else angle= 3.1415926;

	cout <<"Angel: "<<angle<<endl;
	 
 

}

int* sortRSSI(int room)
{
	int n;
	if(room<3)
		n=numbeacons[room];
	else n=cntAll;
	double angles[n];
	int *diff=new int[n];
	for (int i=0;i<n;i++)
	{
		diff[i]=i;
		angles[i]=filteredrssi[i];
	}
	for (int i=0;i<n;i++)
	{
		int min=i;
		for (int j=i+1;j<n;j++)
			if (angles[j]>angles[min])
			{
				min=j;
			}
		double tmp=angles[i];
		angles[i]=angles[min];
		angles[min]=tmp;
		
		int tmpd=diff[i];
		diff[i]=diff[min];
		diff[min]=tmpd;
	}
	return diff;
}



void getPosition(int room, int dest)
{
	int *closest=sortRSSI(room);
	//for (int i=0;i<8;i++)
		//cout<<filteredrssi[i]<<"   "<<endl;
	switch(room)
	{
		case 0:
			if(dest==0)
			{
				if(filteredrssi[0]<-70 && filteredrssi[1]<-70)
				{
					teta=1.1;				
				}
				else if (closest[0]==0 && closest[1]==1 || closest[0]==1 && closest[1]==0)
				{
					teta=0.7;
				}
				else if (closest[0]==1 && closest[1]==2 || closest[0]==2 && closest[1]==1)
				{
					teta=0.7;			
				}
				else if (closest[0]==2 && closest[1]==5 || closest[0]==5 && closest[1]==2)
				{
					teta=0.7;
				}
				else if (closest[0]==0 && closest[1]==3 || closest[0]==3 && closest[1]==0)
				{
					teta=0.7;
				}
				else if (closest[0]==3 && closest[1]==4 || closest[0]==4 && closest[1]==3)
				{
					teta=0.7;
				}
				else if (closest[0]==4 && closest[1]==5 || closest[0]==5 && closest[1]==4)
				{
					teta=0.7;
				}

				else if (closest[0]==5 && filteredrssi[5]>-70)
				{
					goal=true;
				}
				else
				{
					teta=0.7;
				}
			}
			
			
			if(dest==1)
			{
				teta=0.01;
				//cout<<filteredrssi[0]<<"   "<<filteredrssi[1]<<endl;
				if(filteredrssi[0]<-85 && filteredrssi[1]<-85)
					goal=true;				
			
				
			}
			
			break;
		case 1:
			if(dest==0)
			{
			if(closest[0]==0)
			{
				teta=6.1;
			}
			else if(closest[0]==1)
			{
				teta=0.7;
			}
			else if(filteredrssi[0]<-65 && filteredrssi[1]<-65)
				goal=true;
			else
			{
				teta=6.1;
			}
			}
			if(dest==1 || dest==2)
			{
				if(filteredrssi[0]<-65 && filteredrssi[1]<-65)
					teta=6.1;
			else if(closest[0]==0)
			{
				teta=6.1;
			}
			else if(closest[0]==1)
			{
				teta=0.7;
			}
			else if(closest[0]==2 && filteredrssi[0]>-80)
				goal=true;
			else
			{
				teta=0.7;
			}
		}
			break;
			
		case 2:
		if(dest==2)
		{
		if(closest[0]==5)
		{
			teta=0.5;
		}
		else if (closest[0]==0 || closest[0]==4 && filteredrssi[0]<-65)
		{
			teta=5.6;
		}
		else if (closest[0]==0 || closest[0]==4 && filteredrssi[0]>-65)
		{
			teta=5.4;
		}
		else if(closest[0]==1)
		{
			teta=5.4;
		}
		else if(closest[0]==3)
		{
			teta=5.4;
		}
		else if (closest[1]==7)
		{
			teta=0.01;
		}
		else if (closest[0]==2 && filteredrssi[0]>-80)
		{
			goal=true;
		}
		else
		{
			teta=5.4;
		}
		}
				
		if(dest==1)
		{
		if((closest[0]==0) && filteredrssi[0]<-58)
		{
			goal=true;
		}
		if(( closest[0]==0)&& filteredrssi[0]>-65)
		{
			teta=0.7;
		}

		else if(closest[0]==1)
		{
			teta=0.7;
		}
		else if(closest[0]==3)
		{
			teta=0.7;
		}
		else if (closest[1]==7)
		{
			teta=0.7;
		}
		else if (closest[0]==2 && filteredrssi[0]>-80)
		{
			teta=0.7;
		}
		else
		{
			teta=0.7;
		}
		}
			
	}
	
}
	int xg,yg;

void DrawSolution()
{
	//int cnt=0;

while(locating==1)
{		
	if(teta!=0)
	{
		//cout<<nextx<<","<<nexty<<","<<curx<<","<<cury<<endl;
		//double teta=atan2(yg-yp,xg-xp);
		//cout<<teta<<endl;
		//if (teta<0)
		//	teta+=6.28;
		updateAngle();
		//angle+=1.57;
		cout<<teta<<endl;
		drawLine(teta);

		
	}
		sleep(1);
	
}	
}


void getGoal(int source, int destination)
{
	
	switch(source)
	{
		case 0:
			if(destination==0)
			{
				xg=4;yg=4;
			}
			if (destination ==1)
			{
				xg=30;yg=4;
			}
			break;
		case 1:
			if(destination==0)
			{
				xg=4;yg=4;
			}
			if (destination ==1)
			{
				xg=35;yg=14;
			}
			break;		
		case 2:
			if(destination==0)
			{
				xg=4;yg=4;
			}
			if (destination ==2)
			{
				xg=16;yg=50;
			}
			break;
			
	}
	
	
}

void solve(int source, int dest)
{
	int n=numbeacons[source];
	filteredrssi=new double[n];
	for(int i=0;i< n;i++)
		filteredrssi[i]=-100;
	myFilter=new Kalman[n];
	limit=20;
	char ** beacons=new char*[n]; 
	if (source==0)
		for(int i=0;i<n;i++)
			beacons[i]=room1Beacons[i];
	else if(source==1)
		for(int i=0;i<n;i++)
			beacons[i]=room1Beacons[i];
	else
		for(int i=0;i<n;i++)
			beacons[i]=room1Beacons[i];
	
	while(!goal)
	{	
		measure(source,beacons);
		
		//cout<<xp<<","<<yp<<endl;
		if(teta==-20)
			break;
		getPosition(source,dest);
	
	}

}
int getRoom()
{
	filteredrssi=new double[cntAll];
	for(int i=0;i<cntAll;i++)
		filteredrssi[i]=-100;
	myFilter=new Kalman[cntAll];
	limit=10;
	char** beacons=new char*[cntAll];
	for(int i=0;i<cntAll;i++)
		beacons[i]=BeaconsAll[i];
	measure(4,beacons);

	int*closest=sortRSSI(6);
	int cnt1=0,cnt2=0,cnt3=0;
	for(int i=0;i<3;i++)
		if(closest[i]<4)
			cnt1++;
		else if(closest[i]<7)
			cnt2++;
		else
			cnt3++;
	if(cnt1>cnt2 && cnt1>cnt3)
		return 0;
	else if (cnt2>cnt1 && cnt2>cnt3)
		return 1;
	else 
		return 2;
	
	
}
		  
int main () {
	setupOLED();
	//startScreen();
	
	imu = new LSM9DS0(0x6B, 0x1D);
	//thread t1;
	while(1)
	{
		teta=0;
		main_menu(cntm);
		while ((BUTTON_A.pinRead() == HIGH));
		sleep(1);
		if ((BUTTON_A.pinRead() == LOW))
		{
			
			if(cntm==0)
			{
				goal=false;
				cancel_screen("LOCATING");

				cout<<"Getting the Room"<<endl;
				int room=getRoom();
				cout<<"You are in room "<<room<<endl;
				locating=1;
				thread t1(DrawSolution);
			
				limit=20;
				switch(room)
				{
					case 0:
						//getGoal(0,0);
						solve(0,0);
						if(teta!=-20)
							cancel_screen("GOAL");
						break;
					case 1:
						//getGoal(1,0);
						solve(1,0);
						goal=false;
						if(teta==-20)
							break;
						//getGoal(0,0);
						solve(0,0);
						if(teta!=-20)
							cancel_screen("GOAL");
						break;
					case 2:
						//getGoal(2,1);
						solve(2,1);
						goal=false;
						//getGoal(1,0);
						if(teta==-20)
							break;
						solve(1,0);
						if(teta==-20)
							break;
						goal=false;
						//getGoal(0,0);
						solve(0,0);
						if(teta!=-20)
							cancel_screen("GOAL");
				}
				locating=0;
				t1.join();
				
				sleep(5);
			}
			else if(cntm==1)
			{
				goal=false;
				cancel_screen("LOCATING");
				cout<<"Getting the Room"<<endl;
				int room=getRoom();
				cout<<"You are in room "<<room<<endl;
				locating=1;
				thread t1(DrawSolution);
			
				limit=20;
				switch(room)
				{
					case 0:
						//getGoal(0,1);
						//cout<<xg<<"  "<<yg<<endl;
						solve(0,1);
						cout<<"Arrived to Room 2"<<endl;
						if(teta==-20)
							break;
						goal=false;
						//getGoal(1,1);
						solve(1,1);
						if(teta!=-20)
							cancel_screen("GOAL");
						break;
					case 1:
						//getGoal(1,1);
						solve(1,1);
						if(teta!=-20)
							cancel_screen("GOAL");
						break;
					case 2:
						//getGoal(2,1);
						solve(2,1);
						if(teta==-20)
							break;
						goal=false;
						//getGoal(1,1);
						solve(1,1);
						if(teta!=-20)
							cancel_screen("GOAL");

				}
				locating=0;
				t1.join();
				
				sleep(5);
			}
			else if(cntm==2)
			{
				goal=false;
				cancel_screen("LOCATING");
				cout<<"Getting the Room"<<endl;
				int room=getRoom();
				cout<<"You are in room "<<room<<endl;
				
				locating=1;
				thread t1(DrawSolution);
			
				limit=20;
				switch(room)
				{
					case 0:
						//getGoal(0,1);
						solve(0,1);
						if(teta==-20)
							break;
						goal=false;
						//getGoal(1,2);
						solve(1,2);
						if(teta==-20)
							break;
						goal=false;
						//getGoal(2,2);
						solve(2,2);
						if(teta!=-20)
							cancel_screen("GOAL");
						break;
					case 1:
						//getGoal(1,2);
						solve(1,2);
						if(teta==-20)
							break;
						goal=false;
						//getGoal(2,2);
						solve(2,2);
						if(teta!=-20)
							cancel_screen("GOAL");
						break;
					case 2:
						//getGoal(2,2);
						solve(2,2);
						if(teta!=-20)
							cancel_screen("GOAL");
						break;
				}
				locating=0;
				t1.join();
				sleep(5);
				
			}
				else 
				{
					qiblax=1;
					thread t1(qibla);
					sleep(2);
					while ((BUTTON_A.pinRead() == HIGH));
					sleep(1);
					qiblax=0;
					t1.join();
				}
			locating=0;
			teta=0;
			cout<<"DONE"<<endl;
			//t1.join();

			cntm--;
		}
		cntm++;
		cntm%=4; 
	}
	
}
