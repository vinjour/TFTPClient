#include "gettftp.h"


int main(int argc, char **argv){
	
	struct addrinfo hints;
	struct addrinfo *result;
	int sfd, s, lenRRQ, lenACK, nbwr, file;
	int blocksize = BUF_SIZE;
	char RRQ[50]; 
	char ACK[50];
	char *port = "1069";
	char *ip_adress = argv[1];
	char *mode = "octet";
	char *filename = argv[2];
	ssize_t nsend, nrecv;
	char buf[BUF_SIZE];
	
	if (argc < 3){
		fprintf(stderr, "Usage: %s host port msg...\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	
	/* Obtenir l'adresse correspondant à l'hôte */
	memset(&hints, 0, sizeof(struct addrinfo)); // Initialise la structure hints avec des 0
	hints.ai_family = AF_INET; 	// Autorise IPv4 ou IPv6
	hints.ai_socktype = SOCK_DGRAM; // Datagram socket
	hints.ai_flags = 0;
	hints.ai_protocol = IPPROTO_UDP; // Protocole UDP
	
	s = getaddrinfo(ip_adress, port, &hints, &result);
	if (s != 0){ // Gestion de l'erreur getaddrinfo
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
		exit(EXIT_FAILURE);
	}
	
	/* 	getaddrinfo() retourne une liste de structures d'adresse. */
	
	sfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (sfd == -1) { // Gestion de l'erreur socket
        printf("Socket error\n");
        exit(EXIT_FAILURE);
    }
	
	//RRQ = 01-zeros256-0-octet-0
	RRQ[0] = 0;
	RRQ[1] = 1;
	strcpy(RRQ+2, filename);
	RRQ[2+strlen(filename)] = 0;
	strcpy(RRQ+3+strlen(filename), mode);
	RRQ[3+strlen(filename)+strlen(mode)] = 0;
	lenRRQ = 2+strlen(filename)+strlen(mode)+1+1;
	
	nsend = sendto(sfd, RRQ, lenRRQ, 0, result->ai_addr, result->ai_addrlen);
	 
	ACK[0] = 0;
	ACK[1] = 4;
	lenACK = 4;
	
	printf("Server IP : %s\n", ip_adress);
	printf("Server Port : %s\n", port);
	printf("Nom du fichier : %s\n", argv[2]);
	printf("Nombre de bits envoyés : %ld\n", nsend);
	
	file = creat(filename, S_IRUSR | S_IWUSR | S_IRGRP);
	if(file == 0){ // Gestion de l'erreur dans la création du fichier
		perror("create");
		exit(EXIT_FAILURE);
	}
	
	while(1){
		nrecv = recvfrom(sfd, buf, blocksize+4, 0, result->ai_addr, &(result->ai_addrlen));
		ACK[2] = buf[2];
		ACK[3] = buf[3];
		nsend = sendto(sfd, ACK, lenACK, 0, result->ai_addr, result->ai_addrlen);
		nbwr = write(file, buf+4, nrecv-4); // Recopie dans notre fichier vide en local, les données du fichier du serveur
		printf("Nombre de bits reçus : %ld\n %d %d %d %d\n", nrecv, buf[0], buf[1], buf[2], buf[3]);
		
		if (nbwr == -1){ // Gestion de l'erreur write
			perror("write");
			close(file);
			exit(EXIT_FAILURE);
		}
	
		if(nrecv < 516){ // Arrête le transfert quand les données reçues sont < 516
			printf("Fin du téléchargement\n");
			break;
		}
	}
	
	
	close(file);
	exit(EXIT_SUCCESS);
}
