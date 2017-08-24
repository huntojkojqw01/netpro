#include	"unp.h"

#define FOLDER_NAME "clientDB/"
#define MAX_BUFF 15000
#define MAX_SIZE 200000000
enum {OK,BIG,ZERO,ERR};
#define checkMem(x) if(x==NULL){\
						printf("Allocation failed , check memmory\n");\
						return 1;\
					}
#define fOpenToW(f,name)	if((f=fopen(name,"w"))==NULL) {\
								printf("Cannot create file '%s'\n",name);\
								return 1;\
							}
#define fOpenToR(f,name)	if((f=fopen(name,"r"))==NULL) {\
								printf("Cannot open file '%s'\n",name);\
								return 1;\
							}
int sendFile(char *filename,int fd);
int recvFile(char *filename,int fd);
void convertFileName(char *oname, char *iname, int encode);
int main(int argc, char **argv)
{
	int					sockfd, choose,quit=-1,port=1235;
	struct sockaddr_in	servaddr;
	char fileOut[96],fileIn[96],c;
	if (argc < 2)	
		err_quit("USAGE: client IPADDRESS");
	if(argv[2])port=atoi(argv[2]);
	do{
		printf("|======MENU======|\n");
		printf("|1>Encode        |\n");
		printf("|2>Decode        |\n");
		printf("|Other to quit   |\n");
		printf("|================|\n");
		printf("You choose : ");
		scanf("%d",&choose); while(getchar()!='\n');
		if(choose==1) printf("%s\n","ENCODE");
		if(choose==2) printf("%s\n","DECODE");		
		switch(choose){
			case 1:				
			//break;
			case 2:				
				printf("File name: ");
				scanf("%[^\n]s",fileOut);								
				convertFileName(fileOut, fileIn, choose);				
				sockfd = Socket(AF_INET, SOCK_STREAM, 0);
				bzero(&servaddr, sizeof(servaddr));
				servaddr.sin_family = AF_INET;
				servaddr.sin_port = htons(port);
				Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
				Connect(sockfd, (SA *) &servaddr, sizeof(servaddr));
				write(sockfd,&choose,sizeof(int));
				if(sendFile(fileOut,sockfd) || recvFile(fileIn,sockfd) ) {
					quit=-1;
					write(sockfd,&quit,sizeof(int));					
				}							
			break;
			default:	break;
		}		
	}while(0< choose && choose <3);	
}
//---------------------------------------------
int sendFile(char *filename,int fd){	
	unsigned int readSize,sentSize=0,hopeSize;
	char *buff, status=OK;	
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
		puts("Sending data ...");						
		while( sentSize<hopeSize && (readSize=fread(buff,sizeof(char),MAX_BUFF,f))>0 ){
			while(read(fd,&status,sizeof(char))!=1);
			//if(status==ERR) break;					
			write(fd, buff,readSize);					
			sentSize+=readSize;
		}		
		printf("Sent data( %d/%d bytes).\n",sentSize,hopeSize);
		fclose(f);
		free(buff);	
		return 0;	
	}	
}
//------------------------------------------------
int recvFile(char *filename,int fd) {
	unsigned int recvSize=0,readSize=0,hopeSize;
	char *buff, status=OK;
	FILE *f;
	buff=(char*)malloc(MAX_BUFF*sizeof(char));
	checkMem(buff);
	fOpenToW(f,filename);
	{
		while((read(fd,&hopeSize,sizeof(int)))!=4);
		if(hopeSize<=0 || hopeSize >MAX_SIZE) {
			return 1;
		}
		puts("Waiting data ...");		
		write(fd,&status,sizeof(char));					
		do{
			while((readSize=read(fd,buff,MAX_BUFF))<0);
			write(fd,&status,sizeof(char));
			fwrite(buff,sizeof(char),readSize,f);			
			recvSize+=readSize;			
		}while(readSize>0 && recvSize<hopeSize);
		printf("Recevied data( %d/%d bytes).\n",recvSize,hopeSize);		
		free(buff);
		fclose(f);
		return 0;		
	}	
}
//---------------------------------------------------------
void convertFileName(char *oname, char *iname, int encode) {    
    char *p;
    strcpy(iname,oname);	p=iname;
    while(*p && *p!='.')p++;	*p='\0';   
    if(encode==2)	strcat(iname,".txt");    
    else		  	strcat(iname,".zip");    
}
//---------------------------------------------------------