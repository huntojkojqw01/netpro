#include"list.h"

node* newNode(elementType e) {
    node *new_node=(node*)malloc(sizeof(node));
    if(new_node){
        new_node->element=e;
        new_node->next=NULL;
        return new_node;
    }
    else return NULL;
}

list* newList(){
    list *l=(list*)malloc(sizeof(list));
    if(l) {
        l->root=NULL;
        l->size=0;
        return l;
    }
    else return NULL;
} 

int addNode(list *l,elementType e) {
    node *p,*tmp;
    if(l){
        p=l->root;
        if(p){
            if(p->element.freq > e.freq) {
                p=newNode(e);
                p->next=l->root;
                l->root=p;
                l->size++;
            } 
            else{
                while(p->next && p->next->element.freq <= e.freq) {
                    p=p->next;
                }
                if(p->next){
                    tmp=newNode(e);
                    tmp->next=p->next;
                    p->next=tmp;
                }
                else{
                    p->next=newNode(e);
                }
                l->size++;
            }
        }
        else{
            l->root=newNode(e); 
            l->size++;           
        }
        return 1;
    }
    else{
        printf("List is NULL\n");   return 0;
    }
}

int addTail(list *l,elementType e){
    node *p;
    if(l){
        p=l->root;
        if(p){
            while(p->next) p=p->next;
            p->next=newNode(e); l->size++;
        }
        else{
            l->root=newNode(e); l->size++;
        }
    }
    else {
        printf("List is NULL\n");   return 0;
    }
}

node* delHead(list *l){
    node *p;
    if(l){
        p=l->root;
        if(p) l->root=p->next, l->size--;
        else return NULL;
        p->next=NULL; return p;
    }
    else return NULL;
}

int isEmpty(list *l){
    return !( l && l->root);
}

void freeList(list *l) {
    node *p,*tmp;
    if(l) {
        p=l->root;
        while(p) {
            tmp=p;
            p=p->next;
            free(tmp);
            l->size--;
        }
    }
    else{
        printf("List is Null\n");        
    }
}