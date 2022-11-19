#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "server_program.h"
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
#define SERVER_PORT 8000
#define BUFFER_SIZE 1024
#define ACKNOWLEDGE "Acknowledge" 
#define OUTPUTFILE "Data.txt"
/* 
#  Names: Robert Bowen, Michael Ban
#  Description: This program is a messaging program between the client and the server. The server will save the messages to a file. 
#  Date: 11/5/22          	
#  Specification: To run this program first run the server_program.c file and then run the client_program.c file. The client should connect 
to the server and be prompted with whether it wants to write to the server or not. 
*/ 





int main(int argc, char const *argv[]) 
{

  runMessageServer(); //run the Game for the user

  return 0;
}

int runMessageServer()
{
  //prompt
  const char write_question[] = "	Would you like to send a message (y/n)?"; 

  //score


  //endpoint variables
  struct sockaddr_in server;
  struct sockaddr_in client;
  char buffer[BUFFER_SIZE];

  int serverFd = socket(AF_INET, SOCK_STREAM, 0);

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = inet_addr(SERVER_IP_ADDRESS);
  server.sin_port = htons(SERVER_PORT);

  //listen on serverFd for client to connect
  bind(serverFd, (struct sockaddr *)&server, sizeof(struct sockaddr_in));
  listen(serverFd, 100); 
 
 
  //accept clients
  while(TRUE)
  {

    //connect to client 
    printf("Waiting for client connection...\n");
    socklen_t sock_len = sizeof(struct sockaddr_in);
    int clientFd = accept(serverFd, (struct sockaddr *)&client, &sock_len); //accept the clients connection

    char *client_ip = inet_ntoa(client.sin_addr);

    printf("Accepted connection: %s:%d\n", client_ip, ntohs(client.sin_port)); //client ip 


        while (TRUE) //start write loop
        {

          memset(buffer, 0, sizeof(buffer));

          //prompt the user if they want to write
          strcpy(buffer, write_question); 
          write(clientFd, buffer, sizeof(buffer));

          
          //get user response
          memset(buffer, 0, sizeof(buffer));
          int size = read(clientFd, buffer, sizeof(buffer));
          bool response = checkClientResponseYN(buffer); 

          if(!response) //if response is 'n'
          { //break out of loop and return the results
            break; 
          }

          //Acknowledge the clients response
          write(clientFd, ACKNOWLEDGE, strlen(ACKNOWLEDGE));


          //get the message from the client
          memset(buffer, 0, sizeof(buffer));
          size = read(clientFd, buffer, sizeof(buffer)); 
          printf("Client message received: %s", buffer); 


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


          //receive acknowledge from Client about the response
          memset(buffer, 0, sizeof(buffer));
          size = read(clientFd, buffer, sizeof(buffer)); 
        }


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