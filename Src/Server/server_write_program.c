#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "server_write_program.h"
#include <unistd.h>
#include <arpa/inet.h>
#include <limits.h>
#include <math.h>
#include <time.h>



typedef int bool;
#define false 0
#define true 1




#define TRUE 1
#define SERVER_IP_ADDRESS "0.0.0.0"
#define SERVER_PORT 1235
#define BUFFER_SIZE 1024
#define ACKNOWLEDGE "Acknowledge" 
#define OUTPUTFILE "Data.txt"
/* 
#  Names: Robert Bowen, Michael Ban
#  Description: This program is a messaging program between the relay server and the write server. The server will save the messages to a file. 
#  Date: 11/5/22          	
#  Specification: To run this program first run the server_write_program.c file and then run the server_relay_program.c and then the client_program.c files. The client should connect 
to the server relay and be prompted with whether it wants to write to the server or not. Then after it decides the relay server should send it to the write server.  
*/ 





int main(int argc, char const *argv[]) 
{

  runMessageServer(); //run the Game for the user

  return 0;
}

int runMessageServer()
{


  //endpoint variables
  struct sockaddr_in server;
  struct sockaddr_in client;
  char buffer[BUFFER_SIZE];

  int serverFd = socket(AF_INET, SOCK_STREAM, 0);

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = inet_addr(SERVER_IP_ADDRESS);
  server.sin_port = htons(SERVER_PORT);

  //listen on serverFd for relay server to connect
  bind(serverFd, (struct sockaddr *)&server, sizeof(struct sockaddr_in));
  listen(serverFd, 100); 
 
 
  //need to connect to relay server

        while (TRUE) //start write loop
        {

          //connect to relay server
          printf("Waiting for relay server connection...\n");
          socklen_t sock_len = sizeof(struct sockaddr_in);
          int clientFd = accept(serverFd, (struct sockaddr *)&client, &sock_len); //accept the clients connection
          char *client_ip = inet_ntoa(client.sin_addr);
          printf("Accepted connection: %s:%d\n", client_ip, ntohs(client.sin_port)); //client ip 


          //get the message from the relay server
          memset(buffer, 0, sizeof(buffer));
          int size = read(clientFd, buffer, sizeof(buffer)); 
          printf("Message forwarded from relay server %s \n", buffer); 


          //write the message to a ASCII file
          if(writeMessageTofile(buffer, client_ip))
          {
            memset(buffer, 0, sizeof(buffer));
            strcpy(buffer, "STATUS_MESSAGE_SUCCESS"); 
          }
          else{
            memset(buffer, 0, sizeof(buffer));
            strcpy(buffer, "STATUS_MESSAGE_FAILURE"); 

          }
          //send client if it was successfully written or not. 
          write(clientFd, buffer, sizeof(buffer)); 

          close(clientFd);
        }


      

    close(serverFd);


    return 0; 
  }


  //Helper Methods 

  int checkClientResponseYN(char* response)
  {

    if(!strcmp(response, "n"))
    {
      return 0;
    }
    else if(!strcmp(response, "y"))
    {
      return 1; 

    }

  return -1; //something has gone wrong
  
}


int writeMessageTofile(char* message, char* client_ip)
{
  //variables
  FILE *fp;
  time_t now;
  char *pos;

  //get time
  time(&now);


  //create new message
  char* new_message = (char*)(malloc(sizeof(char)*( strlen(" ip: ")+ strlen(" Date: ")+ strlen("\n") + strlen(message) + strlen(client_ip) + strlen(ctime(&now))))); 


  strcpy(new_message, " ip: ") ; 
  strcat(new_message, client_ip); 
  strcat(new_message, " Date: "); 
  strcat(new_message, ctime(&now));
  strcat(new_message, message); 
  strcat(new_message, "\n"); 

   
  //write to the file
  fp = fopen(OUTPUTFILE, "a"); //open the file to append

   if(fp == NULL)
  {

    return 0; 
  }

  fputs(new_message, fp);
  fclose(fp);
  
  free(new_message); 


  return 1; 


}