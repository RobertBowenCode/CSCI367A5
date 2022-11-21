#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "client_program.h"
#include <ctype.h> 

typedef int bool;
#define false 0
#define true 1

#define SERVER_IP_ADDRESS "127.0.0.1"
#define SERVER_PORT 1234
#define RECEIVE_BUFFER_SIZE 1024
#define ACKNOWLEDGE "Acknowledge" 




/* 
#  Names: Robert Bowen, Michael Ban
#  Description: This program is a writing program between the client and server. 
#  Date: 11/5/22          	
#  Specification: To run this program first run the server_program.c file and then run the client_program.c file. The client should connect 
to the server and be prompted with whether it wants to write or not. 
*/ 

int main(int argc, char const *argv[]) 
{

   writeResponse(); //have the client play the game
  return 0;
}


int writeResponse()
{

  //Variables
  bool keep_writing = true; 
  char* user_play_response = NULL; 
  char* user_write_response = NULL;   
  char receive_buffer[RECEIVE_BUFFER_SIZE];
  
  //set up Socket Structure
  struct sockaddr_in server;
  int file_desc = socket(AF_INET, SOCK_STREAM, 0); //server pipeline

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = inet_addr(SERVER_IP_ADDRESS);
  server.sin_port = htons(SERVER_PORT);

  char *server_ip = inet_ntoa(server.sin_addr);

  //connect to the server
  printf("Connecting to: %s:%d\n", server_ip, ntohs(server.sin_port));
  int result = connect(file_desc, (struct sockaddr *)&server, sizeof(struct sockaddr_in)); 
  int byte_count;



  do //write to server loop
   {

    //write prompt -	Would you like to send a message (y/n)?
    memset(receive_buffer, 0, sizeof(receive_buffer)); 
    byte_count  = read(file_desc, receive_buffer, sizeof(receive_buffer));
    printf("%s ", receive_buffer);


    //get user response
    user_play_response = getUserResponseYN(); 
    write(file_desc, user_play_response, strlen(user_play_response));

    if(!strcmp(user_play_response, "n") ) //if the user answers no
    {
            break; //break out of write loop
    }


    //get acknowledge from server
    memset(receive_buffer, 0, sizeof(receive_buffer)); 
    byte_count = read(file_desc, receive_buffer, sizeof(receive_buffer)); //get acknowledge from server


    //get message from client and send it to server. 
    user_write_response = getUserResponseWrite(); 
    write(file_desc, user_write_response, strlen(user_write_response));


    //read if saved correctly from 
    memset(receive_buffer, 0, sizeof(receive_buffer)); //clear buffer
    byte_count = read(file_desc, receive_buffer, sizeof(receive_buffer)); 
    printf("Server Status Message: %s \n \n", receive_buffer); 
    

    //Acknowledge the servers response, keep synch
    write(file_desc, ACKNOWLEDGE, strlen(ACKNOWLEDGE));


    free(user_play_response); 
    free(user_write_response); 

    } while (keep_writing);


  close(file_desc); //close the server connection

   return 0; 
}


//Helper Methods 


char* getUserResponseWrite(){


 //Variables 
 size_t characters = 0;
 char* response = (char*) malloc(sizeof(char)*RECEIVE_BUFFER_SIZE);
 size_t size = (size_t)RECEIVE_BUFFER_SIZE; 
 bool did_not_enter = false; 


 //get the message from the user. 
 while(characters <= 1) //weird behavior, since we have a '\n' in the print stream after getting the 'y' or 'n', it does characters = '\n' on the first loop.                   
 {    //odd print stream bug. It currently works, but has potential to produce bugs. 
  if(did_not_enter)
  {
    printf("Enter message: "); 

  }
  
  characters = getline(&response, &size, stdin); 
  did_not_enter = true; 
 }


 return response; 

}


char* getUserResponseYN()
{ //should be a while loop until user enters in 'y' or 'n'
    char* response = (char*) malloc(sizeof(char)*2);
    do{

        scanf("%s", response); 
        response[0] = (char)(tolower((int)response[0])); 

        if((strcmp(response, "y") !=0) && (strcmp(response, "n") !=0))
        {
          printf("please type in 'y' or 'n' "); 

        }

    }while( (strcmp(response, "y") !=0) && (strcmp(response, "n") !=0) );  
 
  return response; 
} 
