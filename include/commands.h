#include "SerialDongle.h"


// Commands definition
#define DEVICE_STATUS 		0x01
#define GET_NETWORK_STATUS 	0x02
#define GET_CHILDREN_AMOUNT 0x03
#define GET_CHILDREN_LIST 	0x04
#define GET_LQI_RSSI 		0x05
#define SEND_DATA_NODE 		0x06
#define DATA_FROM_NODE      0x07
#define SET_RECV_MODE       0x08
#define CONFIGURE_PIN 		0x09
#define READ_PIN 			0x0A
#define READ_ANALOG 		0x0B
#define READ_PULSE 			0x0C

// Message definitions
#define UNKNOWN_COMMAND 	0x55
#define BAD_PARAMETERS		0x56
#define DEVICE_UP			0x57
#define IN_NETWORK_STATUS   0x58
#define OUT_NETWORK_STATUS  0x59

extern bool recv_activated;