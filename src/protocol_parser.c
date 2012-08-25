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

void init_lists(void);
void move_first_to_execute(void);
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
                        
                        flush_app_buffer();
                        cmd_state = WAIT_FOR_CMD_STATE;
                    }

                }
                else
                {
                    // Invalid header. Flush buffer, pull error message.
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
                        put_on_list(data_command);
                    }
                    else
                    {
                        // Invalid footer. Flush buffer. Pull error message.
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
            ptr_tmp->next_command = NULL;
            first_to_read = first_to_read->next_command;
            first_execute_list = ptr_tmp;
            last_execute_list = ptr_tmp;
            commands_on_recv_list--;
        }
        else
        {
            ptr_tmp = first_to_read;
            first_to_read = first_to_read->next_command;
            ptr_tmp->next_command = NULL;
            last_execute_list->next_command = ptr_tmp;
            last_execute_list = ptr_tmp;
            commands_on_recv_list--;
        }
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



