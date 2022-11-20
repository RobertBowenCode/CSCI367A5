#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "server_relay_program.h"
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
#define SERVER_PORT 1234
#define SERVER_WRITE_PORT 1235
#define BUFFER_SIZE 1024
#define ACKNOWLEDGE "Acknowledge" 

/* 
#  Names: Robert Bowen, Michael Ban
#  Description: This program sends messages from the client to the data storage server. 
#  Date: 11/5/22          	
#  Specification: To run this program first run the server_write_program.c file and then run the server_relay_program.c and then the client_program.c files. The client should connect 
to the server relay and be prompted with whether it wants to write to the server or not. Then after it decides the relay server should send it to the write server.  
*/ 





int main(int argc, char const *argv[]) 
{

  runRelayServer(); //run the Game for the user

  return 0;
}

int runRelayServer()
{

  //prompt
  const char write_question[] = "Would you like to send a message (y/n)?"; 



  //set up Socket Structure for the data storage server
  struct sockaddr_in write_server;
  int byte_count;

  write_server.sin_family = AF_INET; 
  write_server.sin_addr.s_addr = inet_addr(SERVER_IP_ADDRESS);
  write_server.sin_port = htons(SERVER_WRITE_PORT);



  //endpoint variables for the relay server
  struct sockaddr_in server;
  struct sockaddr_in client;
  char buffer[BUFFER_SIZE];

  int serverFd = socket(AF_INET, SOCK_STREAM, 0);

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = inet_addr(SERVER_IP_ADDRESS);
  server.sin_port = htons(SERVER_PORT);




  //listen on serverFd for client to connect to the relay server
  bind(serverFd, (struct sockaddr *)&server, sizeof(struct sockaddr_in));
  listen(serverFd, 100); 
 




  //accept clients
  while(TRUE)
  {

    //connect to client 
    printf("Waiting for client connection...\n");
    socklen_t sock_len = sizeof(struct sockaddr_in);
    int clientFd = accept(serverFd, (struct sockaddr *)&client, &sock_len); //accept the clients connection
    int file_desc; 
    char *client_ip = inet_ntoa(client.sin_addr);
    //


    printf("Accepted connection: %s:%d\n", client_ip, ntohs(client.sin_port)); //client ip 


        while (TRUE) //start write loop
        {


          //create socket for connecting to data storage server
          file_desc = socket(AF_INET, SOCK_STREAM, 0); 




          //prompt the user if they want to write
          memset(buffer, 0, sizeof(buffer));
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
          else{
             connect(file_desc, (struct sockaddr *)&write_server, sizeof(struct sockaddr_in)); //connect to data storage server
          }

          //Acknowledge the clients response
          write(clientFd, ACKNOWLEDGE, strlen(ACKNOWLEDGE));


          //get the message from the client
          memset(buffer, 0, sizeof(buffer));
          size = read(clientFd, buffer, sizeof(buffer)); 
          printf("Client message received: %s", buffer); 

          //write the message to the data storage server
          
          write(file_desc, buffer, strlen(buffer));

          //read from the data storage server whether it was saved or not. 

          memset(buffer, 0, sizeof(buffer));
          size = read(file_desc, buffer, sizeof(buffer)); 
          printf("Relay server status message %s\n \n", buffer); 
          

          //send client if it was successfully written or not. 
          write(clientFd, buffer, sizeof(buffer)); 


          //receive acknowledge from Client about the response
          memset(buffer, 0, sizeof(buffer));
          size = read(clientFd, buffer, sizeof(buffer)); 

          close(file_desc); //end connection with Data storage server
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
