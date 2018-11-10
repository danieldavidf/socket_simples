#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

int obter_hora(char horario[]){
	char parser_hour[12], strhour[3]; int hour;
	
	strcpy(parser_hour, horario);
	
	strhour[0] = parser_hour[0];
	strhour[1] = parser_hour[1];
	strhour[2] = '\0';

return atoi(strhour);
}

int comp_case_sensitive(char s1[], char s2[], int n) {
    int i, cont = 0;
    for (i = 0; i < n; i++) {
        if (tolower(s1[i]) == tolower(s2[i])) {
            cont++;
        } else {
            break;
        }
    }
    return cont == n;
}

int main()
{
		// declaracao das variaveis utilizadas no processo
        int sock, connected, bytes_recebidos , true = 1;  
        char enviada [1024] , recebida[1024];       

        struct sockaddr_in server_addr,client_addr;    
        int sin_size;
        
        // cria socket
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
            perror("Socket");
            exit(1);
        }

		// definicao e configuracao de parametros de enderecamento
        if (setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&true,sizeof(int)) == -1) {
            perror("Setsockopt");
            exit(1);
        }
        server_addr.sin_family = AF_INET;         
        server_addr.sin_port = htons(5000);     
        server_addr.sin_addr.s_addr = INADDR_ANY; 
        bzero(&(server_addr.sin_zero),8); 

		// associa o socket com o endereco local
        if (bind(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr))
                                                                       == -1) {
            perror("Unable to bind");
            exit(1);
        }

		// aguarda solicitacoes de conexoes por IP
        if (listen(sock, 5) == -1) {
            perror("Listen");
            exit(1);
        }

		// abre o servidor para iniciar conexoes com algum IP
        while(1)
        {
        	//imprime mensagem enquanto nao ha cliente conectado
        	printf("\nServidor TCP aguardando por cliente...\n");
	        fflush(stdout);
        
            sin_size = sizeof(struct sockaddr_in);
			connected = accept(sock, (struct sockaddr *)&client_addr,&sin_size);
            printf("\nConectado...\nIP: %s\nPorta: %d\n\n",
                   inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));
			
			int seq_resp = 0; strcpy(recebida, "hello");
			char parser_hour[12], strhour[3];
			
			// funcao para obter hora do sistema
			int hour = obter_hora(__TIME__);

			// dialogo entre servidor e cliente
            while (strcmp(recebida , "q") != 0 &&
            	   strcmp(recebida , "Q") != 0) {

				bytes_recebidos = recv(connected,recebida,1024,0);

				recebida[bytes_recebidos] = '\0';

				if (strcmp(recebida, "q") != 0)
					printf("Client: %s \n" , recebida);
					
				int tam = strlen(recebida);

				strcpy(enviada, " ");
				
				// compara saudacao de acordo com o horario
				
				if(hour >= 0 && hour < 12 && tam==7 && comp_case_sensitive(recebida,"bom dia",tam) && seq_resp==0) {
					strcpy(enviada, "Boa Dia!");
					printf("Server : %s\n", enviada);
					seq_resp++;
				}
				if(hour >= 12 && hour < 18 && tam==9 && comp_case_sensitive(recebida,"boa tarde",tam) && seq_resp==0) {
					strcpy(enviada, "Boa Tarde!");
					printf("Server : %s\n", enviada);
					seq_resp++;
				}
				if(hour >= 18 && tam==9 && comp_case_sensitive(recebida,"boa noite",tam) && seq_resp==0) {
					strcpy(enviada, "Boa Noite!");
					printf("Server : %s\n", enviada);
					seq_resp++;
				}
				
				// verifica solicitacao do horario, respondendo de acordo com a hora do sistema
				
				if(tam==14 && comp_case_sensitive(recebida,"que horas sao?",tam) && seq_resp==1) {
					strcpy(enviada, __TIME__);
					printf("Server : %s\n", __TIME__);
					seq_resp++;
				}
				
				// verifica agradecimento do cliente que implicara no encerramento da conexao com este cliente
				
				if(tam==8 && comp_case_sensitive(recebida,"obrigado",tam) && seq_resp==2) {
					strcpy(enviada, "Disponha.");
					printf("Server : %s\n", enviada);
					seq_resp++;
				}

				send(connected, enviada,strlen(enviada), 0);
				
				if (seq_resp == 3) break;
			}
			
			// encerra conexao com este cliente
			close(connected);
			printf("A conexao foi encerrada.\n");
		}    

	close(sock);
return 0;
} 
