//Code for the server side

#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h> 
#include <strings.h> 
#include <string.h> 
#include <unistd.h> 

//Error function 
void error(char *msg)
{
    perror(msg);
    exit(1);
}


int main(int argc, char *argv[])
{
    //sockfd and newsockfd are both file descriptions 
    //protno is the port number 
    //clilen stores the length of the client address 
    //n is used for checking the read() and write() functions 
    //quit is used for checking if the "!QUIT" message has been sent 
    int sockfd, newsockfd, portno, clilen, quit, n;

    //buffer is where the messages are read from the socket
    char buffer[256];

    //end is the quit message so it can be checked for
    //it needs the \n becuase the messages sent and recieved contain 
    //an endline at the end and this makes checking work with strcmp()
    char end[256] = "!QUIT\n"; 

    //sockaddr_in contains the internet address 
    struct sockaddr_in serv_addr, cli_addr;

    //need to check that we have a port provided 
    if (argc < 2) 
    {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }

    //socket() creates a new socket 
    //AF_INET is the address domain of the socket 
    //SOCK_STREAM is the type of socket which reads chars as a stream 
    //the 0 denotes the protocall 
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    //socket() returns an integer that serves as a refference to the socket
    //if it returns -1 there was an error in opening it
    if (sockfd < 0) 
    {
        error("ERROR opening socket");
    }
    else 
    {
        printf("Socket Created!\n"); 
    }
    
    //bzero() sets all the values of serv_addr to zero 
    bzero((char *) &serv_addr, sizeof(serv_addr));

    //atoi converts strings of digits to ints so they can be used
    //as the port number 
    portno = atoi(argv[1]);

    //sin_family is the family address and it needs to be made the adress of the socket
    //sin_addr.s_addr needs to be made the IP adress of the machine the server runs on
    //sin_port is for the port number which has to be converted to network byte order using 
    //the htons() function 
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    //bind() binds the socket to the address and returns -1 if there is an error
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
    {
        error("ERROR on binding");
    }
    else
    {
        printf("Bind Complete!\n"); 
    }

    //while(1) makes the server look for connection even if the previous connection is ended 
    while(1)
    {

        //have to reset quit to a non zero 
        quit = 1; 

        printf("Waiting for incoming connection\n");
        //listen makes the process listen on the socket for connections 
        //the 5 is for the max number of connections that can be waiting which 
        //is standard for most systems 
        listen(sockfd,5);
    
        //setting clilen to the length of the client address
        clilen = sizeof(cli_addr);

        //accept() wakes up the system when a client connects to the server 
        //cli_addr has the address of the client on the other end and clilen the length 
        //of that address
        //newsockfd holds the new file decriptor which is used for communcation from now on
        //accept() returns -1 if theres an error
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0) 
        {
            error("ERROR on accept");
        }
        else
        {
            printf("Connection Created with Client!\n"); 
        }

        //while(quit !=0) ensure the process goes until we hit the exit parameter 
        while(quit != 0){
    
        //getting the message from the user and clearing out buffer before we store it
        printf("Your message: "); 
        bzero(buffer, 256); 
        fgets(buffer, 255, stdin);

        //checking if we got "!QUIT" as a message 
        quit = strcmp(buffer, end); 

        //sending the message to the client using write()
        //write() returns -1 if theres an error
        n = write(newsockfd, buffer, strlen(buffer));

        //process if we have our quit parameter 
        if(quit == 0)
        {
            printf("Closing Connection..\n");
            return 0; 
        }

        //checking if the message was sent properly 
        if (n < 0) 
        {
            error("ERROR writing to socket");
        }
        else
        {
            printf("Message sent to client!\n"); 
        }

        printf("Waiting for Client....\n"); 

        //resetting buffer and using read() to read what the client sends
        //read() returns -1 if there was an error 
        bzero(buffer,256);
        n = read(newsockfd,buffer,255);

        //checking if message was read properly  
        if (n < 0)
        {
            error("ERROR reading from socket");
        }

        //checking if the client sent the quit parameter
        quit = strcmp(buffer, end); 

        //proccess if the quit parameter was recieved 
        if(quit == 0)
        {
            printf("Client ended connection\n");
        }
        else
        {
        //priting clients message 
        printf("Client: %s",buffer);
    
        }
    }

    }

    return 0; 
}
