#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>


int main( int argc, char *argv[] ){

	int sckt, bytes_recebidos;  
	char enviada[1024],recebida[1024];
	struct hostent *host;
	struct sockaddr_in server_addr;
	
	char ip[21];
	
	// leitura de parametros de entrada
	if (argc > 1){
		strcpy(ip, argv[1]);
		host = gethostbyname( ip );
	}
	else{
		printf("Nao foi possivel conectar, IP nao informado.\n");
		printf("Tente: ./client < ip >\n");
		return 0;
	}

	// verifica se o socket foi criado correntamente, caso contrario, imprime erro
	if ((sckt = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Socket");
		exit(1);
	}

	// configuracao para estabelecer conexao (endereco)
	server_addr.sin_family = AF_INET;     
	server_addr.sin_port = htons(5000);   
	server_addr.sin_addr = *((struct in_addr *)host->h_addr);
	bzero(&(server_addr.sin_zero),8); 

	// verifica se a conexao foi feita com sucesso, caso contrario, imprime erro
	if (connect(sckt, (struct sockaddr *)&server_addr,
                    sizeof(struct sockaddr)) == -1) 
	{
		printf("\nFalha ao conectar... Reinicie os procedimentos.\n");
		perror("Connect");
		exit(1);
	}

    printf("=> Conectado com sucesso.\n");

    printf("\n=> Digite 'q' para encerrar conexao\n\n");
	
	// dialogo entre servidor e cliente
	while (1) {
          
        //cliente digita a mensagem e envia ao servidor
		printf("Client : ");
		gets(enviada);
        send(sckt,enviada,strlen(enviada), 0);

		// recebe resposta do servidor
		bytes_recebidos=recv(sckt,recebida,1024,0);
		recebida[bytes_recebidos] = '\0';
		
		// analisa se foi solicitado encerramento da conexao
		if ( strcmp(enviada , "q")==0){
			printf("Conexao encerrada.\n"); break;
		}
		
		// analise se foi recebido alguma resposta do servidor
		if(strcmp(recebida, " ") != 0)
			printf("Server: %s\n" , recebida);
		
		// encerra a conexao caso tenha percorrido todo o fluxograma
		if ( strcmp(recebida , "Disponha.")==0){
			printf("Conexao encerrada.\n"); break;
		}	  
	}
	// fecha o socket
	close(sckt);
	
return 0;
}
