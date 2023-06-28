#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <sys/shm.h>

void* funcion_hilo(void*);
struct values{
    int serial;
};
int **matriz;
int Robots;
int pista;
int *pistas;
int npistas;
int veces;

pthread_cond_t  cond  = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_barrier_t barrera;
int main(int args, char ** argsv){
    Robots = atoi(argsv[1]);
    printf("el numero de hilos es [%d] \n",Robots);
    
    npistas = atoi(argsv[2]);
    printf("el numero de pistas es [%d] \n",npistas);

    pista = atoi(argsv[3]);
    printf("el tamaño de la pista es [%d] \n",pista);

    veces = atoi(argsv[4]);
    printf("el numero de bailes es [%d] \n",pista);

    matriz = (int **) malloc(4*sizeof(int*));
    pistas = (int *) malloc(npistas*sizeof(int));

    for(int i =0;i<npistas;i++){
        pistas[i] = pista;
    }
    struct values *datos =  (struct values *) malloc(Robots*sizeof(struct values));
    

    pthread_barrier_init(&barrera, NULL, Robots);

    FILE *file;
    file = fopen("baile.txt","r");
    int valores[Robots][4];
    for(int j = 0; j<Robots;j++){
        fscanf(file,"%d: %d, %d, %d, %d\n",&datos[j].serial,&valores[j][0],&valores[j][1],&valores[j][2],&valores[j][3]);
        matriz[j]=valores[j];
    }
    for(int j = 0; j<Robots;j++){
        
        for(int i = 0; i<4;i++){
            printf("%d ",matriz[j][i]);
        }
        printf("\n");
    }
    pthread_t ids[Robots];
    for(int i = 0; i<Robots;i++){
        pthread_create(&ids[i],NULL,funcion_hilo,&datos[i]);
    }
    for(int i = 0;i<Robots;i++){
        pthread_join(ids[i],NULL);
    }
    
    return 0;
}
int espacio(){
    for(int i =0;i<npistas;i++){
        if(pistas[i]!=0){
            return i;
        }
    }
    return -1;
}

void* funcion_hilo(void* arg){
    /*extrayendo el dato de los argumentos*/
    struct values* data = (struct values*)arg;
    for(int i =0;i<veces;i++){
        pthread_mutex_lock(&mutex);
        int var = espacio();
        while(var==-1){
            printf("el hilo [%d] esta esperando\n",data->serial);
            pthread_cond_wait(&cond, &mutex);
            var = espacio();
        }	
        pistas[var] =pistas[var]-1;
        pthread_mutex_unlock(&mutex);
        printf("el hilo [%d] comenzara a bailar\n",data->serial);
        sleep(2);
        for(int i = 0;i<4;i++){
            printf("el robot [%d] esta ejecutando el paso[%d]\n",data->serial,matriz[data->serial-1][i]);
        }
        pthread_mutex_lock(&mutex);
        pistas[var] = pistas[var] + 1;
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
        pthread_barrier_wait(&barrera);
        printf("el hilo [%d] comenzara a trabajar \n",data->serial);
        pthread_barrier_wait(&barrera);
    }
	
    pthread_exit(0);
}