#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>


#define ElementsCount 1000
#define ThreadsCount  4


double *a;
long int j;
double m[ThreadsCount + 1];
double o[ThreadsCount + 1];
  
struct Task
{
  int b, c;
};

struct Thread
{
  pthread_t id;
  int result;
};


void* my_thread(void* task) 
{
    long int i;
    for(i = ((struct Task*)task) -> c; i < ((struct Task*)task) -> b; i++)
    {
        m[j] = m[j] + a[i]; 
    } 
    return NULL;
}

void* my_thread1(void* task) 
{
    long int i;
    pthread_t my_thread_id;
    for (i = ((struct Task*)task) -> c; i < ((struct Task*)task) -> b; i++)
    {
        o[j] = o[j] + (a[i] - m[ThreadsCount + 1]) * (a[i] - m[ThreadsCount + 1]);
    } 
    return NULL;
}


int main()
{
    int i;
    struct Task tasks[ElementsCount];
    struct Thread threads[ElementsCount];
    srand(time(NULL));
    
    
    for(i = 0; i < ThreadsCount; i++)
    {
        tasks[i].c = i * ElementsCount / ThreadsCount ;
        tasks[i].b = (i + 1) * ElementsCount / ThreadsCount;
    }
    
    if (ElementsCount % ThreadsCount != 0)
    {
        tasks[ThreadsCount - 1].b = ElementsCount;
    }
   
    a = (double *)malloc(ElementsCount*sizeof(double));
    
    for (i = 0; i < ElementsCount; i++)
    a[i] = 1 + rand() % 2;  
    
    printf("Данные: ");
    
    for (i = 0; i < ElementsCount; i++)
    {
        printf("%f " , a[i]);
    }    
    printf("\n");
    
    
    for(j = 0; j <= ThreadsCount; j++)
    {
        threads[j].result = pthread_create(&(threads[j].id) ,
                                     (pthread_attr_t *)NULL ,
                                      my_thread ,
                                      &tasks[j]);
    }  
     
    for(i = 0; i < ThreadsCount; i++)
    {
        pthread_join(threads[i].id , (void **) NULL);
    }
      
    
    for (i = 0; i <= ThreadsCount; i++)
    {
        m[ThreadsCount + 1] = m[ThreadsCount + 1] + m[i];
    } 
  
    
    m[ThreadsCount + 1] = m[ThreadsCount + 1] / ElementsCount;
    
    
    for(j = 0; j <= ThreadsCount; j++)
    {
        threads[j].result = pthread_create(&(threads[j].id) ,
                                     (pthread_attr_t *)NULL ,
                                      my_thread1 ,
                                      &tasks[j]);
    }  
     
    for(i = 0; i < ThreadsCount; i++)
    {
        pthread_join(threads[i].id , (void **) NULL);
    }
   
    for (i = 0; i <= ThreadsCount; i++)
    {
        o[ThreadsCount + 1] = o[ThreadsCount + 1] + o[i];
    }
    
    
    o[ThreadsCount + 1] = o[ThreadsCount + 1] / ElementsCount;
    
    
    printf("Среднее значение = %f\n" , m[ThreadsCount + 1]);
    printf("Дисперсия = %f\n" , o[ThreadsCount + 1]);
    return 0;
}
