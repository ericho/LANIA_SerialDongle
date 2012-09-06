#include "SerialDongle.h"


// Commands definition
#define DEVICE_STATUS 		0x01
#define GET_NETWORK_INFO 	0x02
#define GET_CHILDREN_LIST 	0x03
#define GET_LQI_RSSI 		0x04
#define SEND_DATA_NODE 		0x05
#define CONFIGURE_PIN 		0x06
#define READ_PIN 			0x07
#define READ_ANALOG 		0x08
#define READ_PULSE 			0x09

// Message definitions
#define WRONG_HEADER		0x10
#define INCORRECT_MSG_SIZE	0x11
#define DUPLICATED_MSG_ID	0x12
#define WRONG_FOOTER		0x13
#define ACK_MSG				0x14
#define UNKNOWN_COMMAND 	0x15
#define BAD_PARAMETERS		0x16
#define DEVICE_UP			0x17