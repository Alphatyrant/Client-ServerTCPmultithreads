//Compile com:gcc cliente.c -o cliente -lpthread
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <errno.h>
#include <pthread.h>

#define MAX 1024
void *recv_message(void *clientsocket);

int main(int argc, char *argv[]){

    pthread_t thread;    

    if(argc < 3){
        printf("ERRO!\nUse:\t%s hostname porta nome!\n", argv[0]);
        return 1;
    }
    int statusthr;
    char *username = argv[3];
    char *hostname = argv[1];
    int PORTA = atoi(argv[2]);
    char msgclient[MAX];
    char msgserver[MAX];

    struct sockaddr_in saddr;

    saddr.sin_family        = AF_INET;
    saddr.sin_port          = htons(PORTA);
    saddr.sin_addr.s_addr   = inet_addr(hostname);

    int sockfd = socket(AF_INET,SOCK_STREAM,0);
    
    if(sockfd < 0){printf("[ERRO] Erro na criaçao de socket!");return 1;}

    if( (connect(sockfd,(struct sockaddr *)&saddr, sizeof(saddr))) < 0){
        printf("[ERRO] Houve um erro ao se conectar ao servidor!\n");
        return 1;
    }

    send(sockfd, username, 32, 0);
    recv(sockfd, msgserver, MAX, 0);
    printf("%s", msgserver);

    statusthr = pthread_create(&thread, NULL, recv_message, (void *)sockfd);
    if(statusthr){
        printf("[ERRO] Erro ao tentar criar thread!");
        close(sockfd);
        return EXIT_FAILURE;
    }
    while (1){
        memset(&msgclient,'\0', sizeof(msgclient));
        fgets(msgclient,sizeof(msgclient),stdin);
        msgclient[strlen(msgclient)] = '\0';
        send(sockfd,msgclient,sizeof(msgclient),0);
    }
    close(sockfd);
    return 0;
}

void *recv_message(void *clientsocket){
    int sock = (int)clientsocket;
    int size;
    char mensagem[MAX];
    while ( (size = recv(sock,mensagem,MAX,0)) > 0 ){
	printf("%s", mensagem);
	memset(&mensagem,'\0',sizeof(mensagem));
    }

}
