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

#define CONEXOES 5

struct Clientes {
    socklen_t clilen;
    struct sockaddr_in cli_addr;
    int newsockfd;
    char nome[32];
    int id;
};

struct Clientes clientes[CONEXOES];
unsigned int contclient = 0;
void *recv_message(void *clientsocket );

int main(int argc, char *argv[]){ 
    if(argc < 2){printf("ERRO!\nUse:\t%s PORTA", argv[0]);return 1;}
    pthread_t thread[CONEXOES];
    int status;
    int temp;
    int PORTA = atoi(argv[1]);
    char automsgsv[] = "Seja bem vindo ao chat KN1v1!\n";
    char msgcliente[MAX], msgservidor[MAX];
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0){printf("[ERRO] Erro na criaçao de socket!");return 1;}
    struct sockaddr_in svaddr,client;


    memset(&msgservidor,'\0',sizeof(msgservidor));
    memset(&msgcliente ,'\0',sizeof(msgcliente));
    svaddr.sin_family       = AF_INET;
    svaddr.sin_port         = htons(PORTA);
    svaddr.sin_addr.s_addr  = htons(INADDR_ANY);

    if(bind(sockfd,(struct sockaddr *)&svaddr,sizeof(svaddr)) < 0) return 1;
    if(listen(sockfd,CONEXOES) < 0) return 1;
    printf("[INFO] Servidor iniciado!\nListando a porta %i\n", PORTA);

    while(contclient != CONEXOES)
    {
        clientes[contclient].clilen = sizeof(clientes[contclient].cli_addr);
        clientes[contclient].newsockfd = accept(sockfd,
                                        (struct sockaddr *)&clientes[contclient].cli_addr,
                                        &clientes[contclient].clilen);
        if(clientes[contclient].newsockfd < 0)
            printf("[ERRO] Erro ao tentar aceitar conexao!");
        temp = recv(clientes[contclient].newsockfd,
                                 msgcliente,MAX,0);
        if(temp < 0)
            printf("[ERRO] Erro ao tentar ler o socket!");
        for(int i = 0 ; i < strlen(msgcliente); i ++)
            if('\n' == msgcliente[i])
                msgcliente[i] = '\0';
        clientes[contclient].id = contclient;
        printf("[INFO] Administrador: Usuario %s conectado\n", msgcliente);
        strncpy(msgservidor,"Bem-Vindo ",sizeof(msgservidor));
        strncat(msgservidor,msgcliente,sizeof(msgservidor));
        strncat(msgservidor, "\nVoce foi registrado com sucesso em nosso servidor!\n",MAX-1);

        strncpy(clientes[contclient].nome, msgcliente, sizeof(clientes[contclient].nome));

        temp = send(clientes[contclient].newsockfd,msgservidor,sizeof(msgservidor), 0);
        status = pthread_create(&thread[contclient], NULL, recv_message, (void *)clientes[contclient].newsockfd);
        if(status){
            printf("[ERRO] Erro ao tentar criar thread!");
            close(clientes[contclient].newsockfd);
            return EXIT_FAILURE;
        }
	sleep(5);
        contclient++;
    }
}

void *recv_message(void *clientsocket){
    int sock = (int)clientsocket;
    int size;
    char mensagem[MAX];
    char msgconc[MAX];
    int x = contclient;
    while ( (size = recv(sock,mensagem,MAX,0)) > 0 ){
        if(strstr(mensagem, "!quit")){
            close(sock);
            printf("Conexao Fechada!");
            break;
        }
	printf("%s: %s",clientes[x].nome, mensagem);
	strncpy(msgconc, clientes[x].nome, sizeof(msgconc));
	strncat(msgconc, ": ", sizeof(msgconc)-1);
	strncat(msgconc, mensagem, sizeof(msgconc)-1);
	for(int i = 0;i<contclient;i++){
	    if(i != x)
	    	send(clientes[i].newsockfd, msgconc ,sizeof(msgconc),0);
	}
	memset(&mensagem,'\0',sizeof(mensagem));
	memset(&msgconc,'\0',sizeof(msgconc));
    }
    if(size == 0){
        printf("%s: Cliente desconectou!\n", clientes[x].nome);
    }

}
