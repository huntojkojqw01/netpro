#include"huffman.h"
void reverse(char *s) {
    char tmp,*last=s+strlen(s)-1;
    while(s<last) {
        tmp=*s; *s=*last; *last=tmp;
        last--;s++;
    }
}
//-----------------------------------------

/*
Hàm update này cập nhật đường đi cho 1 nhóm các chữ cái nằm trong
xâu s bằng cách thêm kí tự c vào cuối đường đi của nó.
Giả sử xâu s="mnhk", và chữ cái h hiện tại có đường đi (code)
là '011001', và tham số c='1' thì code của chữ cái h sẽ thành
'0110011'
*/
void update(character *charac, char *s, char c) {    
    int num_of_bit;    
    while(*s) {        
        num_of_bit=strlen(charac[*s].code);
        if(num_of_bit==MAX_LEN-1){
            printf("Huffman tree too high\n");
            exit(0);
        }                  
        charac[*s].code[num_of_bit]=c;       
        s++;
    }
}
//----------------------------------
/*
Hàm này thực hiện ghép 2 node đầu tiên của danh sách và
đặt nó vào vị trí phù hợp sao cho danh sách vẫn là không giảm
Trước khi ghép, các node bị tách ra được cập nhật đường đi 
cho các chữ cái của nó.
*/
int ghepNode(list *l,character *charac) {
    elementType e;
    node *tmp;
    if(l){
        if(l->size<2) return 0;
        else {
            tmp=l->root; l->root=tmp->next;
            update(charac,tmp->element.string,'0');
            e=tmp->element;
            free(tmp);
            l->size--;
            tmp=l->root; l->root=tmp->next;
            update(charac,tmp->element.string,'1');
            strcat(e.string,tmp->element.string);
            e.freq+=tmp->element.freq;
            free(tmp);
            l->size--;
            addNode(l,e);            
            return 1;
        }
    }
    else{
        printf("List is null\n");
        exit(0);
    }
}
//----------------------------------------------
/*
Cây huffman được biểu diễn bằng mảng a gồm các chữ cái.
Giả sử chữ cái 'M' có đường đi từ gốc đến lá được biểu diễn
bởi xâu s='0110', thế thì M sẽ là phẩn tử có chỉ sổ 
index=21 trong mảng a.
*/
unsigned int indexOfCode(char *c){
    unsigned int index=0;
    while(*c) {
        index= *c=='0' ? (index<<1)+1 : (index+1)<<1  ;
        c++;
    }
    return index;
}
//-----------------------------------------------------
char *codeOfIndex(unsigned int index){
    unsigned int i=index,size=0;
    char *code,*p;
    while(i) i>>=1,size++;
    code=(char*)malloc(size*sizeof(char));
    checkMem(code);
    p=code;
    while(index){
        if(index & 1){
            *p++='0';
            index=(index-1)>>1;
        }
        else{
            *p++='1';
            index=(index>>1)-1;
        }
    }
    *p='\0';
    reverse(code);
    return code;
}
//---------------------------------------------------
/*
Hàm này nhận tham số là tên file cần mã hóa và tên file 
mã hóa muốn tạo ra.
Trình tự cơ bản thực hiện việc mã hóa như sau:
+Đếm số lượng mỗi kí tự trong file
+Đưa các chữ cái trong file vào một danh sách, sau đó tiến hành
nhóm các chữ cái theo kiểu huffman. Trong quá trình đó, mã của môi
chữ cái sẽ dần được hình thành.
+Xây dựng bảng mã huffman, sử dụng 2 mảng: một mảng chứa chỉ số
còn một mảng chứa chữ cái tương ứng.
+Ghi bảng mã ra file mã hóa.
+Mở file đầu vào, đọc từng chữ cái và ghi mã tương ứng của nó
ra file mã hóa.
*/
int enhuff(char *ifilename,char *ofilename) {
    
    unsigned int i,numOfChar=0,sizeOfFile=0;      
    unsigned int *indexA,*intp;
    char *charA,*charp,c;     
    character charac[256];    
    list *l;
    elementType e;    
    FILE *inf,*ouf;
    //KHỞI TẠO MỘT DANH SÁCH CHỨA CÁC NHÓM CHỮ CÁI:
    l=newList();    
    memset(charac,0,256*sizeof(character));

    //ĐẾM SỐ LƯỢNG MỖI KÍ TỰ TRONG FILE:
    fOpenToR(inf,ifilename);    
    
    while((c=fgetc(inf))!=EOF) {
        sizeOfFile++;
        if(0<=c&&c<=255) charac[c].freq++;
        else return 1;
    }
    fclose(inf);
    //---------------KẾT THÚC ĐẾM.

    //NẠP CÁC CHỮ CÁI XUẤT HIỆN TRONG FILE VÀO DANH SÁCH:
    for(i=0;i<256;i++) {
        if(charac[i].freq) {            
            e.string[0]=i;
            e.freq=charac[i].freq;
            addNode(l,e);
            numOfChar++;
        }
    }    
    while(ghepNode(l,charac));// thực hiện ghép node
    freeList(l);
    free(l);
    //---------------KẾT THÚC NẠP.

    /*
    TẠO RA 2 MẢNG ĐỂ BIỂU DIỄN BẢNG MÃ
    Mảng indexA sẽ chứa chỉ số của chữ cái 
    Mảng charA sẽ chứa chữ cái tương ứng
    */
    indexA=(unsigned int*)malloc(numOfChar*sizeof(unsigned int));
    charA=(char*)malloc(numOfChar*sizeof(char));
    checkMem(indexA);checkMem(charA);
    intp=indexA;charp=charA;                 
    for(i=0;i<256;i++){
        if(charac[i].code[0]){
            reverse(charac[i].code);            
            *charp=i; *intp=indexOfCode(charac[i].code);            
            charp++; intp++;            
        }
        else continue;
    }
    //----------------KẾT THÚC TẠO BẢNG MÃ.

    saveHuffmanTable(indexA,charA,numOfChar,"serverDB/EncodeTable");

    //GHI BẢNG MÃ RA FILE MÃ HÓA:  
    fOpenToW(ouf,ofilename);    
    
    fwrite(&numOfChar,sizeof(unsigned int),1,ouf);
    fwrite(&sizeOfFile,sizeof(unsigned int),1,ouf);
    for(i=0;i<numOfChar;i++){            
        fwrite(&charA[i],sizeof(char),1,ouf);
        fwrite(&indexA[i],sizeof(unsigned int),1,ouf);
    }
    free(indexA);free(charA);
    //---------------KẾT THÚC GHI BẢNG MÃ RA FILE.

    /*
    MỞ FILE ĐẦU VÀO, ĐỌC LẦN LƯỢT CÁC KÍ TỰ VÀ
    GHI MÃ TƯƠNG ỨNG RA FILE NÉN:
    */
    fOpenToR(inf,ifilename);
     
    char *p,tamBit=0;
    int size=sizeOfFile,numByteDaNen=0;
    unsigned char cursor=128;        
    while(0<size--) {
        c=fgetc(inf);        
        if(0<=c&&c<=255) p=charac[c].code;
        else  return 1;
        //puts(p);                                  
        while(*p){
            *p++=='1' ? tamBit|=cursor : 0 ;
            cursor>>=1;
            if(cursor==0 ){
                fwrite(&tamBit,sizeof(char),1,ouf);
                numByteDaNen++; 
                tamBit=0; cursor=128;
            }               
        }//while(*p){
        if(size==0){
            fwrite(&tamBit,sizeof(char),1,ouf);                    
            numByteDaNen++;
        }             
    }//while((c=fgetc(inf))!=EOF) { 
    fclose(inf);    
    fclose(ouf);
        
    return 0;
}
//------------------------------------------------------
/*
Hàm dehuff nhận tham số là tên của file mã hóa và tên của
file sau khi giải mã.
Thực hiện giải mã theo trình tự cơ bản sau:
+Đọc file mã hóa để lấy ra bảng mã, lưu chúng vào 2 mảng:
một mảng chứa index và một mảng chứa chữ cái tương ứng.
+Đọc từng bít tiếp theo của file mã hóa, đồng thời tính index
của chuỗi bít đã đọc được và kiểm tra xem nó cớ thuộc mảng indexA
hay không, nếu thuộc thì chứng tỏ chuỗi đó là 1 chữ cái, lấy nó ra rồi
lặp lại việc đọc từng bít.
*/
int dehuff(char *iname,char *oname){
    
    unsigned int *indexA,numOfChar,sizeOfFile;
    int i,j;
    char *charA,c;    
    FILE *inf,*ouf;

    //ĐỌC FILE MÃ HÓA ĐỂ LẤY RA BẢNG MÃ, LƯU VÀO 2 MẢNG:
    fOpenToR(inf,iname);
    fOpenToW(ouf,oname);
    
    fread(&numOfChar,sizeof(int),1,inf);
    fread(&sizeOfFile,sizeof(int),1,inf);
    
    indexA=(unsigned int*)malloc(numOfChar*sizeof(unsigned int));
    charA=(char*)malloc(numOfChar*sizeof(char));
    checkMem(indexA);checkMem(charA);
    for(i=0;i<numOfChar;i++) {        
        fread(&charA[i],sizeof(char),1,inf);
        fread(&indexA[i],sizeof(unsigned int),1,inf);        
    }
    //-------------------KẾT THÚC LẤY BẢNG MÃ.

    saveHuffmanTable(indexA,charA,numOfChar,"serverDB/DecodeTable");

    /*
    ĐỌC TIẾP FILE MÃ HÓA, MỖI KHI ĐỌC ĐƯỢC 1 CHỮ CÁI
    THÌ GHI NGAY VÀO FILE GIẢI MÃ:
    */    
    unsigned int index=0,numByteExtract=0;    
    while(sizeOfFile>0 && !feof(inf)) {
        fread(&c,sizeof(char),1,inf);
        for(i=7;i>=0;i--){        
            if(((1<<i)&c) == 0) index=(index<<1)+1;            
            else                index=(index+1)<<1;            
            for(j=0;j<numOfChar;j++){
                if(indexA[j]==index && sizeOfFile>0 ) {                    
                    fwrite(&charA[j],sizeof(char),1,ouf);
                    numByteExtract++;
                    sizeOfFile--;
                    index=0;
                    break;
                }
            }//for(j=0;j<numOfChar;j++){            
        }//for(i=7;i>=0;i--){       
    }//while(sizeOfFile>0) {    
    free(indexA);free(charA);    
    fclose(inf);  
    fclose(ouf);
    //------------------KẾT THÚC GHI RA FILE GIẢI MÃ.    
    return 0;
}
//-------------------------------------------
/*
Hàm ghi bảng mã theo kiểu huffman ra một file nào đó:
*/
int saveHuffmanTable(unsigned int *indexA,char *charA,int numOfChar,char *filename){
    FILE *f;
    int i;
    fOpenToW(f,filename);
    fprintf(f,"%4s|%-30s\n","Char","Code" );
    for(i=0;i<numOfChar;i++){
        switch(charA[i]){
            case '\n':
                fprintf(f, "\\n  |%-30s\n",codeOfIndex(indexA[i]));
            break;
            case '\t':
                fprintf(f, "\\t  |%-30s\n",codeOfIndex(indexA[i]));
            break;
            case ' ':
                fprintf(f, "' ' |%-30s\n",codeOfIndex(indexA[i]));
            break;
            default:
                fprintf(f, "%c   |%-30s\n",charA[i],codeOfIndex(indexA[i]));
                break;
        }
    }
    fclose(f);
    return 0;
}
//-----------------------------------------------------