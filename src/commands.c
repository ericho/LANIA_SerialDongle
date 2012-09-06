/**
 * @file commands.c 
 * @brief Implementation of commands and functions to be executed.
 * 
 * @author Erich Cordoba
 * @date 08/28/2012
 */ 

#include "commands.h"

bool parse_command(s_data_command *cmd);
bool device_state(s_data_command *cmd);
bool get_network_status(s_data_command *cmd);
bool get_children_amount(s_data_command *cmd);
bool get_children_list(s_data_command *cmd);
bool get_children_table_list(s_data_command *cmd);
void get_network_address();

/**
 * Receive a s_data_command and execute the specified command. 
 * @param *cmd s_data_command structure.
 */ 

bool parse_command(s_data_command *cmd)
{
	bool sync_response;
	switch(cmd->command)
	{
		case DEVICE_STATUS:
			sync_response = device_state(cmd);
			break;

		case GET_NETWORK_STATUS:
			sync_response = get_network_status(cmd);
			break;

        case GET_CHILDREN_AMOUNT:
            sync_response = get_children_amount(cmd);
            break;
            
		case GET_CHILDREN_LIST:
			sync_response = get_children_list(cmd);
			break;

		case GET_LQI_RSSI:
			sync_response = true;
			break;

		case SEND_DATA_NODE:
			sync_response = false;
			break;

		case CONFIGURE_PIN:
			sync_response = true;
			break;

		case READ_PIN:
			sync_response = true;
			break;

		case READ_ANALOG:
			sync_response = true;
			break;

		case READ_PULSE:
			sync_response = true;
			break;

		default:
			// Unknown command 
			sync_response = true;
			break;

		return sync_response;
	}
}

/**
 * Device state function.
 * @param *cmd s_data_command pointer to the struct to be used. 
 */ 

bool device_state(s_data_command *cmd)
{
	if (cmd != NULL)
	{
		if (cmd->data_length == 0)
		{
			cmd->command = DEVICE_UP;
		}
		else
		{
			cmd->command = BAD_PARAMETERS;
		}
	}
	return true;
}


/**
 * Get PANID network, and status. 
 */ 

bool get_network_status(s_data_command *cmd)
{
	if (cmd != NULL)
	{
		if (cmd->data_length == 0)
		{
			if (ZDO_GetNwkStatus() == ZDO_IN_NETWORK_STATUS)
            {
                cmd->command = IN_NETWORK_STATUS;
                cmd->data = (uint8_t *)malloc(sizeof(nwk_pan_id));
                *(cmd->data) = (uint8_t) (nwk_pan_id >> 8) & 0x00ff;
                *(cmd->data + 1) = (uint8_t) (nwk_pan_id & 0x00ff);
                cmd->data_length = sizeof(nwk_pan_id);
            }
            else
                cmd->command = OUT_NETWORK_STATUS;
			
		}
		else
		{
			cmd->command = BAD_PARAMETERS;
			
		}
        return true;    
	}
    
}

/**
 * Get number of children directly connected to the device
 */ 

bool get_children_amount(s_data_command *cmd)
{
    if (cmd->data_length == 0)
    {
        ZDO_GetNeibAmount_t tmp_neib_amount;
        cmd->data = (uint8_t *) malloc (sizeof(uint8_t));
        ZDO_GetNeibAmount(&tmp_neib_amount);
        *(cmd->data) = tmp_neib_amount.childrenAmount;
        cmd->data_length = 1;
    }
    else
    {
        cmd->command = BAD_PARAMETERS;
    }
    return true;
}

bool get_children_list(s_data_command *cmd)
{
    if (cmd->data_length == 0)
    {
        NodeAddr_t children_table[CS_MAX_CHILDREN_AMOUNT];
        ZDO_GetChildrenAddr_t children = 
        {
            .childrenCount = CS_MAX_CHILDREN_AMOUNT,
            .childrenTable = children_table,
        };

        ZDO_GetChildrenAddr(&children);
        cmd->data = (uint8_t *) malloc (sizeof(uint16_t)*children.childrenCount + 1);
        *(cmd->data) = children.childrenCount;
        for (int i = 0; i < children.childrenCount; i++)
        {
            *(cmd->data + i*2 + 1) = (uint8_t) (children.childrenTable[i].shortAddr >> 8) & 0x00ff;
            *(cmd->data + i*2 + 2) = (uint8_t) (children.childrenTable[i].shortAddr & 0x00ff);
        }
        cmd->data_length = sizeof(uint16_t)*children.childrenCount + 1;
    }
    return true;
}


bool get_children_table_list(s_data_command *cmd)
{
    if (cmd->data_length == 0)
    {
        ZDO_Neib_t neighbor_table[CS_NEIB_TABLE_SIZE];

        ZDO_GetNeibTable(&neighbor_table);
        sprintf(str, "%d", neighbor_table[0].shortAddr);
        send_data_usart(str, strlen(str));
    }
}

void get_network_address()
{
    sprintf(str, "%d", nwk_pan_id);
    send_data_usart(str, strlen(str));
}