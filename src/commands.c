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
bool get_network_info(s_data_command *cmd);


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

		case GET_NETWORK_INFO:
			sync_response = false;
			break;

		case GET_CHILDREN_LIST:
			sync_response = false;
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
bool get_network_info(s_data_command *cmd)
{
	if (cmd != NULL)
	{
		if (cmd->data_length == 0)
		{
			
			return false;
		}
		else
		{
			cmd->command = BAD_PARAMETERS;
			return true;
		}
	}
}