#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>

#define bufsize 1024

void eatZombies(int n){
  // This function removes the zombie process state left
  // after the child exits. Learn about this in NWEN 301!

  wait3(NULL,WNOHANG,NULL); // Nom Nom
}

char upcase(char ch){
  if(islower(ch))
  ch -= 'a' - 'A';
  return ch;
}

int main(int argc, char *argv[]){
  int sock, length, msgsock, status;
  struct sockaddr_in server, client;
  char buf[bufsize];

  // for forking, and cleaning up zombie child state afterwards
  // You must not change this code.

  pid_t id;
  signal(SIGCHLD, &eatZombies);

  // OK, NWEN 243 code starts here.

  // Create a socket (see Lab 2) - it is exactly the same for a server!
  // YOUR CODE HERE

  sock = socket(AF_INET, SOCK_STREAM, 0);

  // Next, create the socket addressing structure
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(atoi(argv[1])); // this time 1st arg is port#

  // Next you need to BIND your socket.
  // YOUR CODE HERE

  if(bind(sock, (struct sockaddr *)&server, sizeof(server)) < 0){
    printf("Error Binding\n");
  }

  // Right, now we are in the main server loop.
  // YOUR CODE HERE

  // Start by listening

  if(listen(sock, 5) < 0){
    printf("ERROR LISTENING\n");
  }

  socklen_t socklength = sizeof(client);

  while(1){

    // you need to accept the connection request

    msgsock = accept(sock, (struct sockaddr *)&client, &socklength);

    // check the accept was correct
    if(msgsock < 0){
      printf("ERROR ACCEPTING\n");
    }
    else{
      printf("Accepted: %d\n", htons(msgsock));
    }

    // the next call makes a new child process that will actually handle the client.
    id = fork();

    // there was a problem with the fork
    if(id < 0){
      printf("ERROR forking\n");
      exit(1);
    }
    // this process doesn't belong to the correct process so loop back around
    else if(id > 0){
      continue;
    }

    // this implies that id == 0

    // Your code here for the child process, that will read from the connection socket, process the data
    // write back to the socket, and then close and finally call exit(0) when done.
    if(read(msgsock, buf, bufsize) < 0){
      printf("%s\n", "Error reading");
    }
    else{
      printf("Recieved: %s \n", buf);
    }

    // complete the function upcase on each of the characters in the buffer to convert them all to uppercase
    // to impletement the SHOUTING function
    for (size_t i = 0; i < bufsize; i++) {
      buf[i] = upcase(buf[i]);
    }

    // write the SHOUTED message back to the socket
    if(write(msgsock, buf, strlen(buf))<0){
      printf("%s\n", "Error Writing");
    }
    else{
      printf("Wrote: %s\n", buf);
    }

    // close the message socket after it has been used
    close(msgsock);

    // exit the forked process
    exit(0);

  }

  // finally close the socket
  close(sock);

}
