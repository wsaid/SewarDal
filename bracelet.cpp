#include <iostream>
#include "oled/Edison_OLED.h"
#include "gpio/gpio.h"
#include "math.h"
#include <unistd.h> 
#include <stdlib.h>
#include <stdio.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <thread>

using namespace std;


void initOLED();
void startScreen();
void cleanUp();
void measure(int room, char** beacons);

int main(int argc, char * argv[])
{
	initOLED();
	startScreen();
}

// Initialize OLED
void initOLED() {
	oled.begin();
	oled.clear(PAGE);
	oled.display();
	oled.setFontType(0);
}

void startScreen()
{
	
	// Draw the splash screen:
	oled.clear(PAGE);
	oled.setCursor(0, 0);
	oled.print("SEWAR DAL");
	oled.print("Press A");
	oled.setCursor(0, 8);
	oled.print("# ");
	oled.setCursor(11,8);
	oled.print("0");
	oled.display();
	oled.setCursor(11,8);
	oled.print("1");
	// Call display to actually draw it on the OLED:
	oled.display();

	
}

void cleanUp()
{
	oled.clear(PAGE);
	oled.display();
}

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