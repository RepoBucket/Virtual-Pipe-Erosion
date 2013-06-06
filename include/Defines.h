#ifndef DEFINES_H
#define DEFINES_H



#define hr(x) if(FAILED(x))
#define srelease(x) {if(x){(x)->Release(); (x)=nullptr;}}
#define sdelete(x) {if(x){delete x; (x)=nullptr;}}
#define sdeletearr(x) {if(x){delete[] x; (x) = nullptr;}}
#define fr(iterator,upTo) for(int iterator = 0; iterator < upTo; iterator++)
#define randf(upTo) ((float)(rand()%1000)/1000 * (float)upTo)

#endif