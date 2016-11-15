

#define Mon(x) (x+1)
#define Tues(x) (x+2)
#define Wed(x) (x+3)
#define Thurs(x) (x+4)
#define Fri(x) (x+5)
#define Sat(x) (x+6)


#define Max_Number 5
#define MScript_Number 10
#define NELEMS(x)  (sizeof(x)/(sizeof(x)[0]))


time_t Current_Time;

typedef struct NumberArray
{
    int Item_One[MScript_Number];
	int Item_Two[MScript_Number];
	int Item_Three[MScript_Number];
	int Item_Four[MScript_Number];
};

int Days_Add(int*,int);
void Collect_Array(int *, int* ,int* ,int* ,int*);
void Include_All(int *, int, int*); //collect all array 
void Change_List(int *); //List from small to large
