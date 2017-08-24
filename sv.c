#include	"unp.h"
#include "huffman.h"
#define FOLDER_NAME "serverDB/"
#define MAX_BUFF 15000
enum {OK,BIG,ZERO,ERR};
typedef struct log_t{
	char ip[20];
	char time[20];
    int type;
    int result;
	unsigned int iFSize;
    unsigned int recv;
    unsigned int oFSize;
	unsigned int sent;	
}log;
int sendFile(char *filename,int fd,log *lg);
int recvFile(char *filename,int fd,log *lg);
void convertFileName(char *iname, char *oname,int fd, int encode, log *lg);
void getTime(log *lg);
void saveAndQuit(log lg,char *filename,int fd);
int main(int argc, char **argv)
{	
	char fileIn[96],fileOut[96];
	char filelog[]="serverDB/filelog";	
	log lg;	

	int					listenfd, connfd, choose,port=1235;
	pid_t				childpid;
	socklen_t			clilen;
	struct sockaddr_in	cliaddr, servaddr;
	void				sig_chld(int);
	if(argv[1]) port=atoi(argv[1]);
	listenfd = Socket(AF_INET, SOCK_STREAM, 0);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(port);
	printf("Server listening on port: %d\n",port);
	Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));
	Listen(listenfd, LISTENQ);
	Signal(SIGCHLD, sig_chld);

	for ( ; ; ) {
		clilen = sizeof(cliaddr);
		if ( (connfd = Accept(listenfd, (SA *) &cliaddr, &clilen)) < 0) {
			if (errno == EINTR)
				continue;
			else
				err_sys("accept error");
		}
		if ( (childpid = Fork()) == 0) {
			Close(listenfd);
			memset(&lg,0,sizeof(log));									
			while(read(connfd,&choose,sizeof(int))!=4);
			lg.type=choose;	getTime(&lg);
			convertFileName(fileIn, fileOut,connfd, choose,&lg);
			if(recvFile(fileIn,connfd,&lg))	
				saveAndQuit(lg,filelog,connfd);						
			if(choose==1){				
				if(enhuff(fileIn,fileOut)) {					
					saveAndQuit(lg,filelog,connfd);
				}
			}
			else{								
				if(dehuff(fileIn,fileOut)) {					
					saveAndQuit(lg,filelog,connfd);
				}				
			}
			if(sendFile(fileOut,connfd,&lg))
				saveAndQuit(lg,filelog,connfd);
			remove(fileOut);remove(fileIn);			
			lg.result=1, saveAndQuit(lg,filelog,connfd);			
		}
		Close(connfd);						
	}
}
//--------------------------------------------------------------
int sendFile(char *filename,int fd,log *lg){
	unsigned int readSize=0,sentSize=0,hopeSize;
	char *buff,status=OK;
	FILE *f;
	buff=(char*)malloc(MAX_BUFF*sizeof(char));
	checkMem(buff);
	fOpenToR(f,filename);
	{
		fseek(f,0,SEEK_END);	hopeSize=ftell(f);	rewind(f);
		write(fd,&hopeSize,sizeof(int));
		if(	hopeSize>MAX_SIZE || hopeSize<=0 ){
			printf("FILE EMPTY OR TOO BIG!\n");			
			return 1;
		}				
		while( sentSize<hopeSize && (readSize=fread(buff,sizeof(char),MAX_BUFF,f))>0 ){
			while(read(fd,&status,sizeof(char))!=1);						
			write(fd, buff,readSize);						
			sentSize+=readSize;
		}		
		lg->oFSize=hopeSize;	lg->sent=sentSize;		
		fclose(f);
		free(buff);
		if(hopeSize!=sentSize) return 1;
		else return 0;
	}	
}
//--------------------------------------------------
int recvFile(char *filename,int fd,log *lg) {
	unsigned int recvSize=0, readSize=0, hopeSize;
	char *buff,status=OK;	
	FILE *f;
	buff=(char*)malloc(MAX_BUFF*sizeof(char));
	checkMem(buff);
	fOpenToW(f,filename);
	{
		while((read(fd,&hopeSize,sizeof(int)))!=4);
		if(hopeSize<=0 || hopeSize > MAX_SIZE){
			return 1;
		}					
		write(fd,&status,sizeof(char));				
		do{
			while((readSize=read(fd,buff,MAX_BUFF))<0);
			write(fd,&status,sizeof(char));
			fwrite(buff,sizeof(char),readSize,f);			
			recvSize+=readSize;			
		}while(readSize>0 && recvSize<hopeSize);		
		
		lg->iFSize=hopeSize;	lg->recv=recvSize;	
		
		free(buff);
		fclose(f);
		if (hopeSize!=recvSize) return 1;
		else return 0;		
	}	
}
//-----------------------------------------------------------------
void convertFileName(char *iname, char *oname,int fd, int encode, log *lg) {    
    struct sockaddr_in addr;
    socklen_t addr_size = sizeof(struct sockaddr_in);
    int res = getpeername(fd, (struct sockaddr *)&addr, &addr_size);
    char *clientip=(char*)malloc(20*sizeof(char));
    memset(clientip,0,20);
    strcpy(clientip, inet_ntoa(addr.sin_addr));
    printf("Connected with %s\n",clientip);    
    strcpy(lg->ip,clientip);     
    strcpy(iname,FOLDER_NAME);strcpy(oname,FOLDER_NAME);
    if(encode==2){
    	strcat(iname,clientip);    	strcat(iname,".zip");
    	strcat(oname,clientip);    	strcat(oname,".txt");
    }
    else{
    	strcat(iname,clientip);    	strcat(iname,".txt");
    	strcat(oname,clientip);    	strcat(oname,".zip");
    }     
}
//--------------------------------------------------------------
void getTime(log *lg){
	time_t timer;    
    time(&timer);
    strftime(lg->time, 20, "%Y-%m-%d %H:%M:%S", localtime(&timer));
}
//----------------------------------------------------------------
void saveAndQuit(log lg,char *filename,int fd){	
	FILE *f;	
	f=fopen(filename,"a");
	if(f){
		fprintf(f,"Time: %20s\n",lg.time);							
		fprintf(f,"IP: %20s\n",lg.ip);
		fprintf(f,"Type: %s\n",lg.type==1 ? "Enhuff" : "Dehuff");
		if(lg.iFSize)
			fprintf(f,"Recevied data( %d/%d bytes).\n",lg.recv,lg.iFSize);		
		if(lg.oFSize)
			fprintf(f,"Sent data( %d/%d bytes).\n",lg.sent,lg.oFSize);
		if(lg.type==1 && lg.result)
			fprintf(f,"Compress rate: %.2f\n",1.0*lg.iFSize/lg.oFSize);
		fprintf(f,"Result: %s\n",lg.result ? "Success": "Failed");
		fprintf(f,"--------------------------------------\n");			
		fclose(f);
	}
	int quit=-1;
	write(fd,&quit,sizeof(int));
	exit(0);	
}
//------------------------------------------------------------------