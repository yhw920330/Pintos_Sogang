#include<stdio.h>
#include<string.h>
#include <stdlib.h>
#include "list.h"
#include "hash.h"

//int_struct which uses list
struct int_data{
	struct list_elem elem;

	int data;

};
//int_data structure which uses hash_table
struct int_hash{
	struct hash_elem elem;
	int data;
};
//   ds_tab contains the name of list_name, hash_table, bitmap 
char* ds_tab[30];//reate한 list_name, hashtable_name, bit_name이 저장되어있다

//array of list, bitmap, hash_table 
struct list *list_tab[10];
struct bitmap *bitmap_tab[10];
struct hash *hash_tab[10];

//gobal variable of the number of list, hash, bitmap
int cnt_list=0;
int cnt_hash=0;
int cnt_bitmap=0;


//function related to creation
void create(char *input);
void create_list(char *name);
void create_hash(char *name);
void create_bitmap (char *name, size_t cnt);
int quit(char *input);
//function related to delete
bool delete(char *name);
//function relate to dumpdata(display the data)
void dumpdata( char *name);
void display_list(struct list*);
void display_bitmap(struct bitmap* );
void display_hash(struct hash *);

//function which get input from user and tokenizing input string then call the function appropriately by its command 
void command(char *input);




//function for debugging (not requirements)
void info_tab();
void list_info(struct list * list);


//functions of compartor which is used for creation of hash/list
static bool value_less(const struct list_elem *a, const struct list_elem *b, void *aux);

static bool value_less_hash(const struct hash_elem *a, const struct hash_elem *b, void *aux);

//function for find the list_element 
struct list_elem * order_find( struct list * list, int order);
struct list_elem * value_find( struct list * list, int data);

//hash_function 
unsigned hash_func_in (const struct hash_elem *a, void *aux);
unsigned hash_func_square (const struct hash_elem *e,void *aux);
unsigned hash_func_tri (const struct hash_elem *e, void *aux);

//hash_action function
void hash_act_sq(struct hash_elem *a ,void *aux);
void hash_act_tr(struct hash_elem *a ,void *aux);


int main()
{
	char input[100];

	int i,j;
	for(i=0;i<30; i++)
		ds_tab[i]=(char*)malloc(sizeof(char)*20);

	//input command 
	while(1){
		memset(input,0,sizeof(input));
		fflush(stdin);
	//	printf(">> ");
		fscanf(stdin,"%[^\n]s",input);
		getchar();
		if(strlen(input)==0) continue;

		//case 1: exit when user input quit"
		if(!strcmp(input,"quit"))
			break;	




		//case 2: create something
		else if(!strncmp(input,"create",6)){
			create(input);
		}
		// case 3: delete something
		else if (!strncmp(input,"delete",6)){
			strtok(input," ");
			char *ds=(char*)malloc(sizeof(char)*20);
			strcpy(ds,strtok(NULL," "));
			delete(ds);
		}
		// case 4: dump list
		else if(!strncmp(input,"dumpdata",8)){
			strtok(input," ");

			char *name= (char*)malloc(sizeof(char)*20);
			strcpy(name,strtok(NULL," "));
			dumpdata(name);
		}
		//case 5: command 
		else{
			command(input);
		
		}

	}


}

void create(char * input){
	strtok(input," "); //command
	char *ds=strtok(NULL," "); //datastructure type

	char *name=strtok(NULL," ");//datastructe name

	if(!strcmp(ds,"list")){
			
		create_list(name);
	}

	else if (!strcmp(ds,"hashtable")){
		create_hash(name);
	
	}

	else if (!strcmp(ds,"bitmap"))
	{
		char *tp=strtok(NULL," "); //size of bitmap
		size_t x;
		sscanf(tp,"%d",&x);
		create_bitmap(name,x);
	}





}


void create_list(char *name){
	strcpy(ds_tab[cnt_list],name); //add its name to ds_tab 
	struct list* newone = (struct list *) malloc(sizeof (struct list));
	list_tab[cnt_list]= newone;	
	list_init((list_tab[cnt_list]));

	cnt_list++;
}

void create_hash(char *name)
{

	strcpy(ds_tab[cnt_hash+10],name);
	struct hash * newone= (struct hash *) malloc( sizeof (struct hash));
	hash_tab[cnt_hash]= newone;
	hash_init(hash_tab[cnt_hash],&hash_func_in,	&value_less_hash,	NULL);
	cnt_hash++;
}

void create_bitmap(char *name,size_t size )
{
	strcpy(ds_tab[cnt_bitmap+20],name);
	struct bitmap *res=(struct bitmap*) bitmap_create(size);
	if(res != NULL)
		bitmap_tab[cnt_bitmap]=res;
	cnt_bitmap++;


}
int quit (char *str)
{
	if( !strcmp("quit",str)) return 1;
	return 0;
}

void list_info(struct list * list){

	printf("=================\n");
	printf("head :%p\nbegin %p\n end: %p\ntail: %p\n",list_head(list),list_begin(list),list_end(list),list_tail(list));
	printf("정보출력!\n");
	display_list(list);

	printf("================");


}


void dumpdata (char *name)
{

	int i;
	//check for list tab 
	for(i=0; i< cnt_list; i++)
	{

		if(!strcmp(name,ds_tab[i]))
		{

			display_list(list_tab[i]);	
			return;

		}
	}
	//check for list hash
	for(i=0; i<cnt_hash; i++)
	{
		if(!strcmp(name,ds_tab[i+10]))
		{
			//dump
			display_hash(hash_tab[i]);
		}
	}

	for(i=0; i<cnt_bitmap; i++)
	{

		if(!strcmp(name,ds_tab[i+20]))
		{
			//			bitmap_dump(bitmap_tab[i]);	
			display_bitmap(bitmap_tab[i]);
			return;
		}
	}


}
int find_tab_index(char *ds_name){

	int res=-1;

	int i=-1;
	for(i=0; i<cnt_list; i++)
		if(!strcmp(ds_name,ds_tab[i]))
			return i;

	for(i=0;i<cnt_hash; i++)
		if(!strcmp(ds_name,ds_tab[i+10]))
			return i;


	for(i=0; i<cnt_bitmap; i++)
		if(!strcmp(ds_name,ds_tab[i+20]))
			return i;
	return res;
}


void command(char *input){

	char *cmd=(char*)malloc(sizeof(char)*20);
	char *ds=(char*)malloc(sizeof(char)*20);
	char *argument=(char*)malloc(sizeof(char)*20);



	char *tp=strtok(input," ");


	if(tp!=NULL)strcpy(cmd,tp);
	tp=strtok(NULL," ");
	if(tp!=NULL)strcpy(ds,tp);
	tp=strtok(NULL,"");
	if(tp!=NULL)strcpy(argument,tp);

	if(!strcmp(cmd,"list_insert"))
	{

		//argument 1,2
		int arg1,arg2;

		sscanf(argument,"%d %d",&arg1,&arg2);
		//make new data to insert
		struct int_data *addptr=(struct int_data *)malloc (sizeof(struct int_data));
		addptr->data=arg2;




		//search the list in the list table
		int tab_index=find_tab_index(ds);
		if(tab_index /10 != 0 || tab_index == -1){
			printf("Not a list DS");
			return ;
		}


		//find the before location
		struct list *here = list_tab[tab_index];
		struct list_elem *tp;

		tp=list_head(here);

		int k;
		k=arg1;
		for(k=0; k<=arg1; k++){


			tp=list_next(tp);


		}

		list_insert(tp,&(addptr->elem));



	}
	else if(!strcmp(cmd,"list_splice")){

	
		int tab_index1= find_tab_index(ds);

		int des_index=0;
		char src[20]={0,};
		int first,last;
		sscanf(argument,"%d %s %d %d",&des_index,src,&first,&last);

		int tab_index2= find_tab_index(src);
		struct list_elem *before= order_find((list_tab[tab_index1]),des_index);
		struct list_elem *fi= order_find((list_tab[tab_index2]),first);
		struct list_elem *la= order_find((list_tab[tab_index2]),last);

		list_splice(before,fi,la);



	}

	else if(!strcmp(cmd,"list_push_front"))
	{
		//make_new_struct element 
		int arg1=0;
		sscanf(argument,"%d" ,&arg1);
		struct int_data *addptr=(struct int_data *)malloc (sizeof(struct int_data));
		addptr->data=arg1;

		//find the list in list tab
		int tab_index=find_tab_index(ds);

		//call list_push_back funic
		list_push_front((list_tab[tab_index]),&(addptr->elem));




	}

	else if(!strcmp(cmd,"list_push_back")){
		//make_new_struct element 
		int arg1=0;
		sscanf(argument,"%d" ,&arg1);
		struct int_data *addptr=(struct int_data *)malloc (sizeof(struct int_data));
		addptr->data=arg1;

		//find the list in list tab
		int tab_index=find_tab_index(ds);

		//call list_push_back funic
		list_push_back((list_tab[tab_index]),&(addptr->elem));


	}

	else if (!strcmp(cmd,"list_remove")){
	

		int tab_index=find_tab_index(ds);
		int index=0;
		sscanf(argument, "%d ",&index);
		struct list_elem *rm=order_find((list_tab[tab_index]),index);
		list_remove(rm);

	}

	else if (!strcmp(cmd,"list_pop_front")){

		int tab_index=find_tab_index(ds);
		struct list_elem* res=list_pop_front((list_tab[tab_index]));
		if(res ==NULL)
		{
			printf("pop_front_return ==null\n");
		}

	}


	else if (!strcmp(cmd,"list_pop_back")){
		int tab_index=find_tab_index(ds);
		struct list_elem* res=list_pop_back((list_tab[tab_index]));
		if(res ==NULL)
		{
			printf("pop_back_return ==null\n");
		}

	}

	else if (!strcmp(cmd,"list_front")){

		int tab_index=find_tab_index(ds);

		struct list_elem* res;
		struct int_data *obj;
		res= list_front((list_tab[tab_index]));


		if(res ==NULL)
		{
			printf("pop_front_return ==null\n");
		}
		else{
			obj=list_entry(res,struct int_data,elem);
			printf("%d\n",obj->data);
		}
	}

	else if (!strcmp(cmd,"list_back")){

		int tab_index=find_tab_index(ds);


		struct list_elem* res=list_back((list_tab[tab_index]));
		if(res ==NULL)
		{
			printf("pop_front_return ==null\n");
		}
		else{
			struct int_data *showthis= list_entry(res,struct int_data,elem);
			printf("%d\n",showthis->data);
		}


	}

	else if(!strcmp(cmd,"list_size")){

		int tab_index=find_tab_index(ds);

		printf("%d\n",list_size((list_tab[tab_index])));

	}

	else if(!strcmp(cmd,"list_empty")){

		int tab_index=find_tab_index(ds);
		bool res=list_empty((list_tab[tab_index]));
		if(res == 1){
			printf("true\n");

		}
		else
		{
			printf("false\n");
		}

	}

	else if( !strcmp(cmd,"list_reverse")){
		int tab_index=find_tab_index(ds);
		list_reverse((list_tab[tab_index]));
	}

	else if(!strcmp(cmd,"list_sort")){
		int tab_index=find_tab_index(ds);
		list_sort((list_tab[tab_index]),value_less,NULL);


	}

	else if(!strcmp(cmd,"list_insert_ordered")){

		int tab_index= find_tab_index(ds);
		int data=0;
		sscanf(argument,"%d",&data);
		//make the new list_elem of int_data type
		struct int_data *newbie=(struct int_data *) malloc(sizeof(struct int_data));
		newbie->data =data;

		list_insert_ordered( (list_tab[tab_index]), &(newbie->elem),value_less,NULL); 


	}

	else if (!strcmp(cmd,"list_unique")){
		int tab_index= find_tab_index(ds);

		char *dup;
		dup=(char *) malloc(sizeof(char)*20);
		if( strlen(argument) >0)
		{
			sscanf(argument,"%s",dup);
			int t_index2=find_tab_index(dup);
			list_unique((list_tab[tab_index]),(list_tab[t_index2]),value_less,NULL);
		}

		else
		{
			list_unique((list_tab[tab_index]),NULL,value_less,NULL);
		}
		;

	}

	else if(!strcmp(cmd,"list_max")){
		int tab_index=find_tab_index(ds);

		struct list_elem *max;
		max=list_max((list_tab[tab_index]),value_less,NULL);

		struct int_data *showthis;
		showthis=list_entry(max,struct int_data,elem);
		printf("%d\n",showthis->data);

	}

	else if (!strcmp(cmd,"list_min")){

		int tab_index=find_tab_index(ds);

		struct list_elem *min;
		min=list_min((list_tab[tab_index]),value_less,NULL);

		struct int_data *showthis;
		showthis=list_entry(min,struct int_data,elem);
		printf("%d\n",showthis->data);

	}


	else if(!strcmp(cmd,"list_swap")){
		//input format

		int tab_index=find_tab_index(ds);

		int idx1, idx2;
		sscanf(argument, "%d %d",&idx1,&idx2);
		if(idx2 <idx1){
			int tp=idx2;
			idx2=idx1;
			idx1=tp;
		}
		struct list_elem*a;
		struct list_elem*b;
		a=order_find((list_tab[tab_index]),idx1);
		b=order_find((list_tab[tab_index]),idx2);
		list_swap(a,b);


	}


	else if (! strcmp(cmd,"list_shuffle")){


		int tab_index=find_tab_index(ds);
		list_shuffle((list_tab[tab_index]));
	}


	else if(!strcmp(cmd,"bitmap_size")){

		int tab_index=find_tab_index(ds);

		printf("%d\n",bitmap_size(bitmap_tab[tab_index]));

	}
	else if(!strcmp(cmd,"bitmap_set")){
		int tab_index=find_tab_index(ds);
		size_t idx;
		bool value;
		char tp[5];
		sscanf(argument,"%d %s",&idx,tp);
		if( !strcmp(tp,"true") || !strcmp(tp,"TRUE"))
			value=true;
		else 
			value=false;
		bitmap_set(bitmap_tab[tab_index],idx,value);

	}


	else if (!strcmp(cmd,"bitmap_mark"))
	{
		int tab_index=find_tab_index(ds);
		size_t idx;
		sscanf(argument,"%d",&idx);
		bitmap_mark(bitmap_tab[tab_index],idx);

	}
	else if (!strcmp(cmd,"bitmap_reset"))
	{
		int tab_index=find_tab_index(ds);
		size_t idx;
		sscanf(argument,"%d",&idx);
		bitmap_reset(bitmap_tab[tab_index],idx);

	}

	else if (!strcmp(cmd,"bitmap_flip"))
	{
		int tab_index=find_tab_index(ds);
		size_t idx;
		sscanf(argument,"%d",&idx);
		bitmap_flip(bitmap_tab[tab_index],idx);

	}

	else if (!strcmp(cmd,"bitmap_test"))
	{
		int tab_index=find_tab_index(ds);
		size_t idx;
		sscanf(argument,"%d",&idx);
		printf("%s\n", bitmap_test(bitmap_tab[tab_index],idx) ? "true" : "false");

	}
	else if (!strcmp(cmd,"bitmap_set_all")){
		int tab_index=find_tab_index(ds);
		bool value= (!strcmp(argument,"true")  ? true: false);
		bitmap_set_all(bitmap_tab[tab_index],value);

	
	}

	else if (!strcmp(cmd,"bitmap_set_multiple")){
		int tab_index=find_tab_index(ds);
		size_t start,cnt;
		bool value;
		char tp[10];
		sscanf(argument, "%d %d %s",&start,&cnt,tp);
		value= (!strcmp(tp,"true")  ? true: false);
		bitmap_set_multiple(bitmap_tab[tab_index],start,cnt,value);



	}
	else if (!strcmp(cmd,"bitmap_count")){
		int tab_index=find_tab_index(ds);
		size_t start,cnt;
		bool value;
		char tp[10];
		sscanf(argument, "%d %d %s",&start,&cnt,tp);
		value= (!strcmp(tp,"true")  ? true: false);
		
		printf("%d\n",bitmap_count(bitmap_tab[tab_index],start,cnt,value));


	}

	else if (!strcmp(cmd,"bitmap_contains")){
		int tab_index=find_tab_index(ds);
		size_t start,cnt;
		bool value;
		char tp[10];
		sscanf(argument, "%d %d %s",&start,&cnt,tp);
		value= (!strcmp(tp,"true")  ? true: false);
		printf("%s\n",bitmap_contains(bitmap_tab[tab_index],start,cnt,value) ? "true": "false");

	}

	else if (!strcmp(cmd,"bitmap_any")){
		int tab_index=find_tab_index(ds);
		size_t start,cnt;
		sscanf(argument, "%d %d",&start,&cnt);
					
		printf("%s\n",bitmap_any(bitmap_tab[tab_index],start,cnt) ? "true": "false");

	}
	else if (!strcmp(cmd,"bitmap_none")){
		int tab_index=find_tab_index(ds);
		size_t start,cnt;
		sscanf(argument, "%d %d",&start,&cnt);
			
		printf("%s\n",bitmap_none(bitmap_tab[tab_index],start,cnt) ? "true": "false");



	}


	else if (!strcmp(cmd,"bitmap_all")){
		int tab_index=find_tab_index(ds);
		size_t start,cnt;
		sscanf(argument, "%d %d",&start,&cnt);
		printf("%s\n",bitmap_all(bitmap_tab[tab_index],start,cnt) ? "true": "false");


	}


	else if (!strcmp(cmd,"bitmap_scan")){
		int tab_index=find_tab_index(ds);
		size_t start,cnt;
		bool value;
		char tp[10];
		sscanf(argument, "%d %d %s",&start,&cnt,tp);
		value= (!strcmp(tp,"true")  ? true: false);
		unsigned int res=bitmap_scan(bitmap_tab[tab_index],start,cnt,value);
		printf("%u\n",res);


	}
	else if (!strcmp(cmd,"bitmap_scan_and_flip")){
		int tab_index=find_tab_index(ds);
		size_t start,cnt;
		bool value;
		char tp[10];
		sscanf(argument, "%d %d %s",&start,&cnt,tp);
		value= (!strcmp(tp,"true")  ? true: false);
		unsigned int res=bitmap_scan_and_flip(bitmap_tab[tab_index],start,cnt,value);
		printf("%u\n",res);

	}
	else if (!strcmp(cmd,"bitmap_dump")){
		int tab_index=find_tab_index(ds);
		bitmap_dump(bitmap_tab[tab_index]);


	}

	else if (!strcmp(cmd,"bitmap_expand"))
	{
		int tab_index= find_tab_index(ds);
		int size=0;
		sscanf(argument,"%d",&size);
		struct bitmap *res= (struct bitmap *)bitmap_expand(bitmap_tab[tab_index],size);
		if (!res)
			bitmap_tab[tab_index]=res;

	}
	

	else if(!strcmp(cmd,"hash_insert"))
	{
		int tab_index=find_tab_index(ds);
		int data =0;
		sscanf(argument,"%d",&data);
		struct int_hash * addptr;
		addptr= (struct int_hash *)malloc (sizeof(struct int_hash));
		addptr->data=data;
		hash_insert(hash_tab[tab_index],&(addptr->elem));
 
	}
	else if(!strcmp(cmd,"hash_replace"))
	{
		int tab_index=find_tab_index(ds);
		int data =0;
		sscanf(argument,"%d",&data);
		struct int_hash * addptr;
		addptr= (struct int_hash *)malloc (sizeof(struct int_hash));
		addptr->data=data;
	
		hash_replace(hash_tab[tab_index],&(addptr->elem));

 
	}

	else if(!strcmp(cmd,"hash_find"))
	{
		int tab_index=find_tab_index(ds);
		int data =0;
		sscanf(argument,"%d",&data);
		struct int_hash * find;
		find= (struct int_hash *)malloc (sizeof(struct int_hash));
		find->data=data;
	
		struct hash_elem *res=NULL;
		res=hash_find(hash_tab[tab_index],&(find->elem));
		
		if (res)
			{
				printf("%d\n",data);
			}
	
 
	}
	
	else if(!strcmp(cmd,"hash_delete"))
	{
		int tab_index=find_tab_index(ds);
		int data =0;
		sscanf(argument,"%d",&data);
		struct int_hash * find;
		find= (struct int_hash *)malloc (sizeof(struct int_hash));
		find->data=data;
	
		hash_delete(hash_tab[tab_index],&(find->elem));
		
	
 
	}


	else if(!strcmp(cmd,"hash_clear"))
	{
		int tab_index=find_tab_index(ds);
		int data =0;
		hash_clear(hash_tab[tab_index],NULL);
		
	 
	}
	else if(!strcmp(cmd,"hash_size"))
	{
		int tab_index=find_tab_index(ds);
		int data =0;
		printf("%d\n",hash_size(hash_tab[tab_index]));
		
	 
	}
	else if(!strcmp(cmd,"hash_empty"))
	{
		int tab_index=find_tab_index(ds);
		int data =0;
		printf("%s", hash_empty(hash_tab[tab_index]) ? "true\n": "false\n");
		
	 
	}	

	else if (!strcmp(cmd,"hash_apply"))
	{

		int tab_index=find_tab_index(ds);
		if(!strcmp(argument,"square")){
			hash_apply(hash_tab[tab_index],&hash_act_sq);
		}
			

		else if (!strcmp(argument,"triple")){

			hash_apply(hash_tab[tab_index],&hash_act_tr);

		}

		else
		{
			printf("undefined hash_action");
		}
	}


	else{
		printf("???");

	}


}
void display_list(struct list* list)
{

	if( list_empty(list)) {

		return ;
	}
	struct list_elem *tp;
	tp= list_begin(list);
	struct int_data *obj;

	for(tp=list_begin(list); tp != list_end(list); tp = list_next(tp)){

		obj= list_entry(tp,struct int_data, elem);
		printf("%d ",obj->data);
	}
	printf("\n");
}

void display_hash(struct hash *h)
{
	
	if(hash_empty(h)){
		return;
	}

	
	struct hash_iterator i;
	hash_first(&i,h);
	while(hash_next(&i))

	{
		struct hash_elem *ob_h;
		struct int_hash *obj;
		obj= hash_entry(hash_cur(&i) ,struct int_hash, elem);

		printf("%d ",obj->data);
	}
	printf("\n");

}

void display_bitmap(struct bitmap *b)
{
	size_t s= bitmap_size(b);

	size_t i;
	for(i=0; i<s; i++)
	{
		if(bitmap_test(b,i))
			printf("1");
		else 
			printf("0");
	}
	printf("\n");
}

//comparator by value in list
static bool
value_less(const struct list_elem *a, const struct list_elem *b, void *aux){
	

	struct int_data *x= list_entry(a,struct int_data,elem);
	struct int_data *y= list_entry(b,struct int_data,elem);
	return (x->data) < (y->data);

}
//compartor by value in hash
static bool value_less_hash(const struct hash_elem *a, const struct hash_elem *b, void *aux){

	struct int_hash *x= hash_entry(a,struct int_hash,elem);
	struct int_hash *y= hash_entry(b,struct int_hash,elem);
	return (x->data) < (y->data);
	
}

//hash_function which is used hash_int
unsigned hash_func_in (const struct hash_elem *a, void *aux)
{
	struct int_hash *x= hash_entry(a,struct int_hash,elem);
	unsigned int res=x->data;
	return hash_int(res);

}


unsigned hash_func_square (const struct hash_elem *a,void *aux)
{
	struct int_hash *x= hash_entry(a,struct int_hash,elem);
	unsigned int res=x->data;
	return hash_int_2(res * res);
}

unsigned hash_func_tri (const struct hash_elem *a, void *aux){
	struct int_hash *x= hash_entry(a,struct int_hash,elem);
	unsigned int res=x->data;
	return hash_int_2(res * res*res);

}
//hash act functions
void hash_act_sq(struct hash_elem *a,void *aux)
{
	struct int_hash *x= hash_entry(a,struct int_hash,elem);
	x->data = x->data * x->data;
}

void hash_act_tr(struct hash_elem *a ,void *aux)

{

	struct int_hash *x= hash_entry(a,struct int_hash,elem);
	x->data= x->data * x->data  * x->data;

}

//finding list_elemnt funct
struct list_elem *value_find(struct list *list, int data ){


	struct list * l=list;
	int size= list_size(list);
	struct list_elem *walk;
	struct int_data *obj;

	for(walk=list_begin(list) ; walk != list_end(list);walk= list_next(walk))
	{
		obj= list_entry(walk,struct int_data,elem);
		if( obj->data == data){

			return walk;
		}


	}

	printf("Error! there is no data (%d) in this list",data);

}


struct list_elem * order_find( struct list * list, int order)
{

	struct list_elem* walk;
	int size= list_size(list);

	int i;
	walk=list_begin(list);
	for(i=0; i<order; i++)
	{
		if( walk ==list_end(list)){
			printf("Tail 까지왔어열");
			break;
		}
		else{
			walk=list_next(walk);
		}	
	}


	return walk;


}


bool delete(char *name){
	//cnt, ds_tab , list_tab
	bool res=1;

	int i=-1;
	//delete list
	for(i=0; i<cnt_list; i++)
		if(!strcmp(name,ds_tab[i]))
		{


			//초기화
			memset(ds_tab[i],0,sizeof(ds_tab[i]));

			//adjust replace deleted point with last element
			if(cnt_list>1 && i<cnt_list-1)
			{
				strcpy(ds_tab[i],ds_tab[cnt_list-1]);
				list_tab[i]=list_tab[cnt_list-1];

			}	
			cnt_list--;
			return true;
		}
	//delte hash
	for(i=0;i<cnt_hash; i++)
		if(!strcmp(name,ds_tab[i+10]))
		{
			memset(ds_tab[i+10],0,sizeof(ds_tab[i+10]));
			if(cnt_hash>1 && i<cnt_hash-1)
			{
				strcpy(ds_tab[i],ds_tab[cnt_hash-1]);
				hash_tab[i]=hash_tab[cnt_hash-1];

			}
			cnt_hash--;
			return true;
		}

	//delete bitmap
	for(i=0; i<cnt_bitmap; i++)
		if(!strcmp(name,ds_tab[i+20]))
		{

			memset(ds_tab[i+20],0,sizeof(ds_tab[i+20]));
			if(cnt_bitmap>1 && i<cnt_bitmap-1)
			{
				strcpy(ds_tab[i+20], ds_tab[20+cnt_bitmap-1]);
				bitmap_tab[i]=bitmap_tab[cnt_bitmap-1];
			}
			cnt_bitmap--;

			return true;
		}
	return res;

}



//just for debugging
void info_tab()
{
	int i;
	printf("---------------");
	printf("list(%d)\n\n ",cnt_list);
	for(i=0; i<cnt_list; i++)
	{
		printf("%d ]]:%10s (%p) :: ",i,ds_tab[i],list_tab[i]);
		dumpdata(ds_tab[i]);

	}
	printf("---------------");
	printf("hash(%d)\n\n ",cnt_hash);

	for(i=0; i<cnt_hash; i++)
	{
		dumpdata(ds_tab[i+10]);	
	}

	printf("---------------");
	printf("bitmap(%d)\n\n ",cnt_bitmap);

	for(i=0; i<cnt_bitmap; i++)
	{
		printf("%d ]]:%10s (%p) :: ",i,ds_tab[i+20],bitmap_tab[i]);
		dumpdata(ds_tab[i+20]);

	}
}


