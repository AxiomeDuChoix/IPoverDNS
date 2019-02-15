#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <linux /if.h>
#include <linux /if_tun.h>
#include <time.h>
//review imports:
#include "request_queue.h"
#include "reply_queue.h"
#include "DNS_Query.h"

#define MAX_SLEEP 1.5
#define BUFSIZE 32768
#define CNAME 5
#define TXT 16
#define IN 4

// Fonctions de commodité: elles permettent d'afficher les erreurs.  
void error(const char *msg){ // Affiche un message d'erreur.
    perror(msg);
    exit(0);
}
int cread(int fd, char *buf, int n){ //http://www.cis.syr.edu/~wedu/seed/Labs_12.04/Networking/Firewall_VPN/files/simpletun.c
  int nread;
  if((nread=read(fd, buf, n))<0){
    perror("Reading data");
    exit(1);
  }
  return nread;
}
int cwrite(int fd, char *buf, int n){
  int nwrite;
  if((nwrite=write(fd, buf, n))<0){
    perror("Writing data");
    exit(1);
  }
  return nwrite;
}
int read_n(int fd, char *buf, int n) {
  int nread, left = n;
  while(left > 0) {
    if ((nread = cread(fd, buf, left))==0){
      return 0 ;      
    }else {
      left -= nread;
      buf += nread;
    }
  }
  return n;  
}

// On prépare les structures de données à utiliser:
struct QQueue* request_queue = QcreateQueue(); // queue de requêtes.
struct RQueue* reply_queue = createQueue();    // queue de réponses.
// On prépare des variables globales:
int n_reply;
// On initialise un mutex et deux variables de condition: 
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t need_send = PTHREAD_COND_INITIALIZER;
pthread_cond_t need_wakeup = PTHREAD_COND_INITIALIZER;
// On crée l'interface TAP
struct ifreq ifr = {};
int tapfd, err;
if ((tapfd=open("/dev/net/tun",O_RDWR))<0)
	return fd;
ifr.ifr_flags = IFF_TAP;
strncpy(ifr.ifr_name, "tap0",IFNAMSIZ);
if ((err = ioctl(tapfd,TUNSETIFF, (void*) &ifr))<0){
	close(tapfd);
	return err;
}
// On crée une socket UDP, et on la couple au port 53 (DNS).
struct sockaddr_in port;
port.sin_family = AF_INET;
port.sin_port = htons(53);
port.sin_addr.s_addr = INADDR_ANY;
int sockfd;
if ((sockfd = socket(AF_INET, SOCK_DGRAM,0))<0)
	error("Erreur de création de la socket");
if (bind(sockfd, (struct sockaddr *)&port, sizeof(port))<0)
	error("Erreur de couplage entre port et socket");
// Petite fonction intermédiaire dont le but est de construire les réponses.

struct DNS_PACKET* build_reply(struct DNS_PACKET *requete){ //Note: on ne recopie pas: on fait
															//pointer les valeurs de la réponse
															//au même endroit que les valeurs
															//de la que requête. À voir...
	struct QUESTION *quest = requete-> question;
	struct DNS_PACKET *rep = new_from_values(1,quest->qname,quest->qtype,quest->qclass);
	rep->header->id = requete->header->id;
	rep->header->opcode = requete->header->opcode;
	rep->header->aa = 1;
	rep->header->qr = 1;
	rep->header->rd = requete->header->rd;
	rep->header->ra = 0;
	return rep;
}
// On définit les actions des divers threads créés.  
void *request_handler(void *arg){
	struct QUERY *query;
	struct DNS_PACKET *dns_packet, *rep;
	struct QUESTION *question;
	struct sockaddr_in *client_dns;
	char* buffer=malloc(BUFSIZE);
	char *qname, *domain, *text, *pack,*binary;
/*	int length; (utile pour les acks, FUTUR)*/
	int size = sizeof(struct sockaddr_in);
	while (1){
		memset(buffer,0,sizeof(buffer));
		client_dns = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
		memtset(client_dns,0,sizeof(struct sockaddr_in));
		client_dns->sin_family = AF_INET;

		nb_octets_recus = recvfrom(sockfd,buffer,BUFSIZE,0,(struct sockaddr *)client_dns,(socklen_t *)&size)

		dns_packet = DNS_from_binary(buffer); 

		question = dns_packet.question;

		qname = question->qname;

		rep = build_reply(dns_packet);

		if ((domain = strchr(qname, '$'))!=NULL){
			*domain='\0';
			if ((qname[0]=='d')&&(question.qtype == TXT)){
				text = qname+1;
				pack = DNS_Unsplit(text);
				cwrite(tapfd,pack,BUFSIZE);
				pthread_mutex_lock (&lock);
/*				length = length(reply_queue);
				pthread_mutex_unlock (&lock);
				rep->record = new_from_hash(question->qname,
								TXT,
								IN,
								0,
								"l#{length}.#{$domain}"); //à modifier.*/
				binary = Binary_from_DNS(rep);
				sendto(sockfd,binary,BUFSIZE,0,(struct sockaddr*)client_dns,size);
				//On empile rien, donc on se débarasse de ce dont on n'aura pas besoin après.
				free(rep->header);
				free(rep->record->resource);
				free(rep->record->name);
				free(rep->record->rdata);
				free(rep->record);
				free(rep->question->qname); // on se débarasse aussi de qname.
				free(rep->question);
				free(rep);
			}
			else if ((qname[0]=='r')&&(question.qtype == TXT)){
				// On a reçu une requête: on peut envoyer qqch.
				query = (struct QUERY*)malloc(sizeof(struct QUERY));
				memset(query,0,sizeof(struct QUERY));
				query->rep = rep;
				query->sender = client_dns;
				query->now = time(NULL);
				pthread_mutex_lock (&lock);
				QenQueue(query);
				pthread_cond_signal(&need_wakeup);
				pthread_cond_signal(&need_send);
				pthread_mutex_unlock(&lock);
				binary = Binary_from_DNS(rep);
				sendto(sockfd,binary,BUFSIZE,0,(struct sockaddr*)client_dns,size);
			}
		}
		free(binary);
		free(dns_packet->header);
		free(dns_packet->question);
		free(dns_packet->record->resource);
		free(dns_packet->record->name);
		free(dns_packet->record->rdata);
		free(dns_packet->record);
		free(dns_packet);
		// On ne peut pas encore libérer client_dns, ni les champs de query.
		// On ne libère pas non plus qname, car dnspacket et rep le partagent. 
	}
}
void *reply_handler(void *arg){
	char buffer[BUFSIZE];
	char split_buffer[BUFSIZE];
	struct REPLY *reply = (struct REPLY*)malloc(sizeof(struct REPLY))
	while (1){
		memset(buffer, 0, sizeof(buffer));
		memset(split_buffer,0,sizeof(*encoded_buffer));
		memset(reply,0,sizeof(struct REPLY));
		nb_octets = read_n(tapfd, (char *) &buffer, BUFSIZE);
		if (nb_octets>0){
			encoded_buffer = DNS_Split(buffer);
			pthread_mutex_lock (&lock);
			n_reply++;
			reply->id = n_reply;
			reply->msg=encoded_buffer;
			enQueue(reply_queue,*reply);
			pthread_cond_signal(&need_send);
			pthread_mutex_unlock (&lock);
		}
	}
	free(buffer);
	free(split_buffer);
	free(reply);
	return NULL;
}
void *sleep(void arg*){
	while (1){
		sleep(MAX_SLEEP);
		pthread_mutex_lock (&lock);
		if (QisEmpty(request_queue))
			pthread_cond_wait(&need_wakeup,&lock);
		pthread_cond_signal(&need_send);
		pthread_mutex_unlock (&lock);
	}
	return NULL;
}
int main(int args, char *argv[]){
	// On crée les threads pour gérer les connexions extérieures. 
	pthread_t request_thread, reply_thread, sleep_thread;
	pthread_create(&request_thread,NULL,request_handler,(void*) NULL);
	pthread_create(&reply_thread,NULL,reply_handler,(void*) NULL);
	pthread_create(&sleep_thread,NULL,sleep,(void*) NULL);

	// On définit les variables qui nous serviront durant la boucle principale
	struct sockaddr_in *client_dns; 
	struct QUERY *current_query;
	struct REPLY current_reply; //consider transforming to pointer.
	struct DNS_PACKET *rep;
	time_t age_packet;
	char* binary;
	// Thread principal: reads from reqqueue and sends replies.
	while (1){
		pthread_mutex_lock (&lock);
		if (isEmpty(reply_queue)){
			if (QisEmpty(request_queue)){
				pthread_cond_wait(&need_send,&lock);
			}
			else{
				age_packet = Qpeek(request_queue)->req->now;
				if (difftime(time(NULL),age_packet)>MAX_SLEEP){
					// On doit envoyer un packet "vide", et cela afin
					// de limiter le nombre de requêtes qu'on empile. 
					current_query = QdeQueue(request_queue);
					// ICI LE CONTENU DE LA RÉPONSE: DZERO NECESSAIRE
          			rep->record = new_from_hash(current_query->qname,TXT,IN,0,"dzero");
          			binary = Binary_from_DNS(rep);
          			sendto(sockfd,binary,BUFSIZE,0,(struct sockaddr*)client_dns,sizeof(*client_dns));
          			free(binary);
					free(current_query->sender);
					free(current_query->rep->header);
					free(current_query->rep->question->qname);
					free(current_query->rep->question);
					free(current_query->rep->record->resource);
					free(current_query->rep->record->name);
					free(current_query->rep->record->rdata);
					free(current_query->rep->record);
					free(current_query->rep);
					free(current_query);
				}
				else
					pthread_cond_wait(&need_send,&lock);
			}
		}
		else{
			// Si on est ici, c'est qu'on a une réponse.
			if (QisEmpty(request_queue)){
				printf("%s","On a quelque chose à envoyer, mais aucune requête");
				pthread_cond_wait(&need_send,&lock);
				// On doit attendre une requête.
			}
			else{
				// On peut envoyer une réponse!
				current_query = QdeQueue(request_queue);
				current_reply = deQueue(reply_queue);
				rep = current_query->rep;
				rep->record = new_from_hash(current_query->qname,TXT,IN,0,strcat("d",current_reply->msg));
          		binary = Binary_from_DNS(rep);
          		sendto(sockfd,binary,BUFSIZE,0,(struct sockaddr*)client_dns,sizeof(*client_dns));
          		free(binary);
				free(current_query->sender);
				free(current_query->rep->header);
				free(current_query->rep->question->qname);
				free(current_query->rep->question);
				free(current_query->rep->record->resource);
				free(current_query->rep->record->name);
				free(current_query->rep->record->rdata);
				free(current_query->rep->record);
				free(current_query->rep);
				free(current_query);
			}
		}
		pthread_mutex_unlock (&lock);
	}
	pthread_join(request_thread,NULL);
	pthread_join(reply_thread,NULL);
    return 0;
}


