#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <time.h>

#define ElementsCount 100000
#define ThreadsCount  4

long double *array;

long double sum[ThreadsCount];
long double disp[ThreadsCount];
long double average, dispersion;
  
struct Task
{
  int b, a, index;
};

struct Thread
{
  pthread_t id;
};

void* my_thread(void* task) 
{
    long int i;
    /*
     * Не используйте транслит, просто sum.
     */
    long double sum1 = 0;
    pthread_t my_thread_id;
    for(i = ((struct Task*)task) -> a; i < ((struct Task*)task) -> b; i++)
    {
        sum1 = sum1 + array[i]; 
    } 
    sum[((struct Task*)task)->index] = sum1;
}

void* my_thread1(void* task) 
{
    long int i;
    long double sum2 = 0;
    pthread_t my_thread_id;
    for (i = ((struct Task*)task) -> a; i < ((struct Task*)task) -> b; i++)
    {
        sum2 = sum2 + (array[i] - average) * (array[i] - average);
    } 
    disp[((struct Task*)task)->index] = sum2;
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
        tasks[i].index = i;
        tasks[i].a =  i * ElementsCount / ThreadsCount;
        tasks[i].b = (i + 1) * ElementsCount / ThreadsCount;
    }

    if (ElementsCount % ThreadsCount != 0)
    {
        tasks[ThreadsCount - 1].b = ElementsCount;
    }

    array = (long double *)malloc(ElementsCount * sizeof(long double));
    
    for (i = 0; i < ElementsCount; i++)
    array[i] = 1 + rand() % 2;  
    
   
    
    /*printf("Данные: ");
    
    for (i = 0; i < ElementsCount; i++)
    {
        printf("%Lf " , array[i]);
    }
    printf("\n");*/
    
    
    clock_t startTime = clock();
    
    for(i = 0; i < ThreadsCount; i++)
    {
        sum[i] = pthread_create(&(threads[i].id) ,
                                     (pthread_attr_t *)NULL ,
                                      my_thread ,
                                      &tasks[i]);
    }  
     
    for(i = 0; i < ThreadsCount; i++)
    {
        pthread_join(threads[i].id , (void **) NULL);
    }
      
    /*
     * если вы объявили массив
     * int a[5], то у вас 5 элементов, которые номеруются с 0, т.е.
     * к элементу с индексом a[5] обращаться нельзя.
     */
    for (i = 0; i < ThreadsCount; i++)
    {
       average = average + sum[i];
    } 
    
    average = average / ElementsCount;
    
    for(i = 0; i < ThreadsCount; i++)
    {
        disp[i] = pthread_create(&(threads[i].id) ,
                                     (pthread_attr_t *)NULL ,
                                      my_thread1 ,
                                      &tasks[i]);
    }  
     
    for(i = 0; i < ThreadsCount; i++)
    {
        pthread_join(threads[i].id , (void **) NULL);
    }
   
    /*
     * Аналогично про индексацию массивов
     */
    for (i = 0; i < ThreadsCount - 1; i++)
    {
        dispersion = dispersion + disp[i];
    }
    
    
    dispersion = dispersion / ElementsCount;
    
    /*
     * Также время работы можно измерять с помощью системной утилиты time:
     * time ./a.out
     */
    
    clock_t endTime = clock();
    printf("Time elapsed: %Lf\n", (long double)(endTime - startTime) / CLOCKS_PER_SEC);
    
    printf("Среднее значение = %Lf\n" , average);
    printf("Дисперсия = %Lf\n" , dispersion);
    return 0;
}
