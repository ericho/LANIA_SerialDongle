/**
 * @file UsartController.c 
 * @brief Implementation of USART relationed functions. Message interpreter and Protocol
 * 
 * @author Erich Cordoba
 * @date 08/01/2012
 */ 

#include "protocol_parser.h"


void analize_data_received(void);
void put_on_list(s_data_command *cmd);
void send_first_output(void);

void init_lists(void);
void move_first_to_execute(void);
void move_executed_to_output(void);
void create_response_message(uint8_t msg);
void put_on_output_list(s_data_command *cmd);
bool check_id_duplicated(uint8_t msg_id);

uint16_t recv_data_len;
uint8_t cmd_state;


void init_lists(void)
{
    first_to_read = NULL;
    last_to_read = NULL;
    data_command = NULL;
    first_execute_list = NULL;
    last_execute_list = NULL;
    first_output_list = NULL;
    last_output_list = NULL;
    commands_on_recv_list = 0;
    cmd_state  == WAIT_FOR_CMD_STATE;
}

void analize_data_received(void)
{
        if (offset_rx_buffer >= 3)
        {
            if (cmd_state == WAIT_FOR_CMD_STATE)
            {
                if (app_rx_buffer[0] == HEADER_FROM_COMPUTER)
                {
                    // Obtain the lenght of data received. 
                    recv_data_len = (uint16_t) (app_rx_buffer[1] << 8);
                    recv_data_len |= (uint8_t) app_rx_buffer[2];
                    if (recv_data_len >= 6 && recv_data_len <= 261)
                    {
                        cmd_state = CMD_DETECTED_STATE;
                        
                    }
                    else
                    {
                        // Wrong length
                        create_response_message(INCORRECT_MSG_SIZE);
                        flush_app_buffer();
                        cmd_state = WAIT_FOR_CMD_STATE;
                    }

                }
                else
                {
                    // Invalid header. Flush buffer, pull error message.
					create_response_message(WRONG_HEADER);
                    flush_app_buffer();
                    cmd_state = WAIT_FOR_CMD_STATE;
                }
            }
            else if (cmd_state == CMD_DETECTED_STATE)
            {
                
                if (offset_rx_buffer == recv_data_len)
                {
                    // Checking for end of data.
                    if (app_rx_buffer[offset_rx_buffer-1] == FOOTER_FROM_COMPUTER)
                    {
                        
                        // Extract received data. 
                        data_command = (s_data_command *) malloc(sizeof(s_data_command));
                        data_command->id_message = app_rx_buffer[3];
                        data_command->command = app_rx_buffer[4];
                        data_command->data_length = offset_rx_buffer - 6;
                        data_command->next_command = NULL;
                        
                        if (data_command->data_length > 0)
                        {
                            data_command->data = (uint8_t *) malloc(data_command->data_length);
                            for (int i = 0; i < data_command->data_length; i++)
                            {
                                *(data_command->data + i) = app_rx_buffer[i + 5];
                            }
                            
                        }
                        else
                        {
                            data_command->data = NULL;
                        }
						recv_data_len = 0;
                        flush_app_buffer();
                        cmd_state = WAIT_FOR_CMD_STATE;
                        
						if (!check_id_duplicated(data_command->id_message))
						{
							recv_data_len = 0;
                        	flush_app_buffer();
                        	cmd_state = WAIT_FOR_CMD_STATE;
                        	put_on_list(data_command);
						}
						else
							create_response_message(DUPLICATED_MSG_ID);
                    }
                    else
                    {
                        // Invalid footer. Flush buffer. Pull error message.
						create_response_message(WRONG_FOOTER);
                        flush_app_buffer();
                        cmd_state = WAIT_FOR_CMD_STATE;
                    }
                
                }
            }
        }
}

/**
 * Move de first element on recv list to execute list. 
 * 
 */

void move_first_to_execute(void)
{
    s_data_command *ptr_tmp;
    if (first_to_read != NULL)
    {
        if(first_execute_list == NULL && last_execute_list == NULL)
        {
            ptr_tmp = first_to_read;
            first_to_read = first_to_read->next_command;
			if (first_to_read == NULL)
				last_to_read = NULL;
			ptr_tmp->next_command = NULL;
			
            first_execute_list = ptr_tmp;
            last_execute_list = ptr_tmp;
            commands_on_recv_list--;
        }
        else
        {
            ptr_tmp = first_to_read;
            first_to_read = first_to_read->next_command;
			if (first_to_read == NULL)
				last_to_read = NULL;
            ptr_tmp->next_command = NULL;
            last_execute_list->next_command = ptr_tmp;
            last_execute_list = ptr_tmp;
            commands_on_recv_list--;
        }
    }
}

/**
 * Moves the first element on executed list to output list
 * 
 */ 

void move_executed_to_output(void)
{
	s_data_command *ptr_tmp;
	if(first_execute_list != NULL)
	{
		if(first_output_list == NULL && last_output_list == NULL)
		{
			ptr_tmp = first_execute_list;
			first_execute_list = first_execute_list->next_command;
			if (first_execute_list == NULL)
				last_execute_list = NULL;
			ptr_tmp->next_command = NULL;
			first_output_list = ptr_tmp;
			last_output_list = ptr_tmp;
		}
		else
		{
			ptr_tmp = first_execute_list;
			first_execute_list = first_execute_list->next_command;
			if (first_execute_list == NULL)
				last_execute_list = NULL;
			ptr_tmp->next_command = NULL;
			last_output_list->next_command = ptr_tmp;
			last_output_list = ptr_tmp;
		}
	}
}

/**
 * Send first element on output list
 */

void send_first_output(void)
{
	s_data_command *ptr_tmp;
	if (first_output_list != NULL)
	{
		// Create memory space 4 bytes + data_length
		// Copy byte by byte 
		// send througth usart
		uint16_t total_message_length;
		total_message_length = 6 + first_output_list->data_length;
		ptr_output_data = (uint8_t *)malloc(sizeof(uint8_t)*total_message_length);
		*ptr_output_data = HEADER_TO_COMPUTER;
		*(ptr_output_data + total_message_length - 1) = FOOTER_TO_COMPUTER;
		*(ptr_output_data + 1) = (uint8_t) (total_message_length >> 8) & 0x00ff;
		*(ptr_output_data + 2) = (uint8_t) (total_message_length & 0x00ff);
		*(ptr_output_data + 3) = first_output_list->id_message;
		*(ptr_output_data + 4) = first_output_list->command;
		if (first_output_list->data_length > 0)
		{
			for (int i=5; i<total_message_length - 1; i++)
			{
				*(ptr_output_data + i) = *(first_output_list->data + i - 5);
			}
		}
		ptr_tmp = first_output_list;
		first_output_list = first_output_list->next_command;
		if (first_output_list == NULL)
			last_output_list = NULL;
        send_data_usart((void*)ptr_output_data, total_message_length);
		free(ptr_tmp);
	}
}

/**
 * Puts a new message command on recv list
 * 
 * @param *cmd A s_data_command pointer of the new command message. 
 * 
 */ 

void put_on_list(s_data_command *cmd)
{
    
    if (commands_on_recv_list <= MAX_COMMAND_LIST_SIZE)
    {
        if (first_to_read == NULL && commands_on_recv_list == 0)
        {
            // The list it is empty.
            first_to_read = cmd;
            last_to_read = cmd;
            commands_on_recv_list++;         
            
        }
        else
        {
            last_to_read->next_command = cmd;
            last_to_read = cmd;
            commands_on_recv_list++;
        }
    }
    else
    {
        // The list is full, push error
        return;
    }
}

/**
 * Creates an output message for different events like errors and acks
 * @param msg The message identifier
 */ 
void create_response_message(uint8_t msg)
{
	s_data_command *tmp_msg;
	tmp_msg = (s_data_command *) malloc (sizeof(s_data_command));
	tmp_msg->id_message = 0;
	tmp_msg->data_length = 0;
	tmp_msg->data = NULL;
	tmp_msg->next_command = NULL;

	switch (msg)
	{
		case WRONG_HEADER:
			tmp_msg->command = WRONG_HEADER;
			break;
			
		case INCORRECT_MSG_SIZE:
			tmp_msg->command = INCORRECT_MSG_SIZE;
			break;

		case DUPLICATED_MSG_ID:
			tmp_msg->command = DUPLICATED_MSG_ID;
			break;

		case WRONG_FOOTER:
			tmp_msg->command = WRONG_FOOTER;
			break;

		case ACK_MSG:
			tmp_msg->command = ACK_MSG;
			break;

		default:
			free(tmp_msg);
			tmp_msg = NULL;
			break;
	}


	if (tmp_msg != NULL)
	{
		put_on_output_list(tmp_msg);
	}
}

/**
 * Puts a output message on output list
 * @param *cmd The pointer to the message structure.
 */ 

void put_on_output_list(s_data_command *cmd)
{
	if (first_output_list == NULL && last_output_list == NULL)
	{
		first_output_list = last_output_list = cmd;
	}
	else
	{
		last_output_list->next_command = cmd;
		last_output_list = cmd;
	}
}


/**
 * Checks if exists a message id
 */ 

bool check_id_duplicated(uint8_t msg_id)
{
	s_data_command *ptr;
	if (first_to_read != NULL)
	{
		ptr = first_to_read;
		do
		{
			if (ptr->id_message == msg_id)
				return true;
			ptr = ptr->next_command;
		}while(ptr != NULL);
	}
	else
		return false;
}

