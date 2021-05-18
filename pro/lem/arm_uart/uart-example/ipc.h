#include <sys/ipc.h>

#define MAX_BYTES_PERMSG  9216  
#define HSM_IPC_P       0666

#define HSM_Q_MIN_SIZE  64*1024           /* Ĭ????Ϣ??????С?ռ? 64KB */
#define HSM_Q_MAX_SIZE  20*1024*1024    /* Ĭ????Ϣ???????ռ? 2MB */

typedef struct
{
    long mtype;
    unsigned char mtext[MAX_BYTES_PERMSG + 1];
} st_HsmMsgQ;

typedef union {
          int val;
          struct semid_ds *buf;
          unsigned short  *array;
} SEMUN; 

// semop will use it 
typedef struct sembuf SEMBUF;


// semget will use it 
typedef struct semid_ds SEMINFO;

int Ipc_CreateShm( int shmkey, int shmsize, int *shmid );
int Ipc_OpenShm( int shmkey, int *shmid );
int Ipc_DeleteShm( int shmid );
int Ipc_MapShm( int shmid, void **shmaddr );
int Ipc_DismapShm( void *shmaddr );

int Ipc_CreateQueue( int qkey, int qsize, int *qid );
int Ipc_OpenQueue( int qkey, int *qid );
int Ipc_DeleteQueue( int qid );
int Ipc_PutMsg( int qid, long msgtype, unsigned char *buf, int buflen, int timeout );
int Ipc_GetMsg( int qid, long *msgtype, unsigned char *buf, int *buflen, int timeout );

int Ipc_CreateSem( int semkey, int semvalue, int *semid );
int Ipc_OpenSem( int semkey, int *semid );
int Ipc_DeleteSem( int semid );
int Ipc_SemP( int semid );
int Ipc_SemV( int semid );

