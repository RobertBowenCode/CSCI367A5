#ifndef SERVER_PROGRAM   
#define SERVER_PROGRAM

int runMessageServer(); 
int checkClientResponseYN(char* response); 
int writeMessageTofile(char* message, char* client_ip); 



#endif