//code for the client side

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <string.h> 
#include <stdlib.h>
#include <strings.h> 
#include <unistd.h> 

//Error function 
void error(char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    //Sockfd is the file descripter 
    //portno is the port nubmer 
    //n is used to check read() and write() 
    //quit is used for checking the quit parameter 
    int sockfd, portno, n, quit;

    //server_addr holds the address of the server 
    struct sockaddr_in serv_addr;

    //server is a pointer to the hostent struct which contains information about 
    //the host that we will be using later 
    struct hostent *server;

    //buffer will hold the messages as they come
    //end holds "!QUIT\n" which is our exit message and the \n is needed to make
    //strcmp() work properlly 
    char buffer[256];
    char end[256] = "!QUIT\n";

    //checking to make sure we got a device name and a port number 
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }

    //atoi() converts a string of digits to an int so we can store it as the port number 
    portno = atoi(argv[2]);

    //socket() creates a new socket 
    //AF_INET is the address domain of the socket 
    //SOCK_STREAM is the type of socket which reads chars as a stream 
    //the 0 denotes the protocall 
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    //socket() returns -1 if theres an error
    if (sockfd < 0) 
    {
        error("ERROR opening socket");
    }
    else 
    {
        printf("Socket Created!\n"); 
    }

    //gethostbyname() takes the host name given to the program and sets it as the hostent 
    //we made earlier 
    server = gethostbyname(argv[1]);

    //gethostbyname() returns NULL if theres an error
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }


    printf("Connecting to server....\n");

    //bzero() sets the server_addr (server address) to zero before we set it 
    bzero((char *) &serv_addr, sizeof(serv_addr));

    //sin_family is the family address and it needs to be made the adress of the socket
    serv_addr.sin_family = AF_INET;

    //setting serv_addr to the address of the server using bcopy() which just
    //copies one string to another  
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);

    //sin_port is for the port number which has to be converted to network byte order using 
    //the htons() function
    serv_addr.sin_port = htons(portno);

    //creating the connection to the server using connect() using the socket file decripter and the 
    //server address
    //connect() returns -1 of there is an error
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
    {
        error("ERROR connecting");
    }
    else 
    {
        printf("Connected to server!\n"); 
    }

    //continues receiving and sending messages until the quit condition is met 
    while(1){

    printf("Waiting on Server.....\n");

    //setting buffer to all zeros before we use it
    bzero(buffer,256);

    //reading the message sent by the server 
    //read() returns -1 of theres an error
    n = read(sockfd,buffer,255);

    //checking if there was an error
    if (n < 0) 
    {
        error("ERROR reading from socket");
    }

    //checking to see if the quit message was sent
    quit = strcmp(buffer, end);

    //ending the program if quit was sent
    if(quit == 0)
    {
        printf("Closing Connection..\n");
        return 0; 
    }

    //printing recieved message 
    printf("Server: %s",buffer);

    //getting message to send to server
    //setting buffer to all zeros first
    printf("Your message: "); 
    bzero(buffer, 256); 
    fgets(buffer, 255, stdin); 

    //write() sends to message to the server and returns -1 if there was an error
    n = write(sockfd, buffer, strlen(buffer)); 
    
    //checking if the quit message was sent 
    quit = strcmp(buffer, end); 

    //ending the program if the quit message was sent
    if(quit == 0)
    {
        printf("Closing Connection..\n"); 
        return 0; 
    }

    //checking if there was an error in sending 
    if (n < 0) 
     {
         error("ERROR writing to socket");
     }
     else
     {
         printf("Message sent to Server!\n"); 
     }

    }
    
    return 0;
}
