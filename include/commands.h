#include "SerialDongle.h"


// Commands definition
#define DEVICE_STATUS 		0x01
#define GET_NETWORK_STATUS 	0x02
#define GET_CHILDREN_AMOUNT 0x03
#define GET_CHILDREN_LIST 	0x04
#define GET_LQI_RSSI 		0x05
#define SEND_DATA_NODE 		0x06
#define CONFIGURE_PIN 		0x07
#define READ_PIN 			0x08
#define READ_ANALOG 		0x09
#define READ_PULSE 			0x0A

// Message definitions
#define WRONG_HEADER		0x50
#define INCORRECT_MSG_SIZE	0x51
#define DUPLICATED_MSG_ID	0x52
#define WRONG_FOOTER		0x53
#define ACK_MSG				0x54
#define UNKNOWN_COMMAND 	0x55
#define BAD_PARAMETERS		0x56
#define DEVICE_UP			0x57
#define IN_NETWORK_STATUS   0x58
#define OUT_NETWORK_STATUS  0x59