#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>

#include <setjmp.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <sys/shm.h>
#include <sys/sem.h>

#include "ipc.h"

static void (*get_origin_alarmfunction)();
static void (*put_origin_alarmfunction)();

static unsigned int put_origin_alarmtime;
static unsigned int get_origin_alarmtime;

static jmp_buf  ENV_PUTMSG_ALARM;
static jmp_buf  ENV_GETMSG_ALARM;

#if 0
static unsigned short sems_members(int semid)
{
        SEMUN semopts;
        SEMINFO mysemds;

        semopts.buf= &mysemds;
        semctl(semid,0,IPC_STAT,semopts);
        return (semopts.buf->sem_nsems);
}
#endif

static int init_sems(int semid,int nsems,int value)
{
        SEMUN semopts;
        int cntr;

        semopts.val = value;

        for (cntr = 0;cntr< nsems;cntr++) {
                if ( semctl(semid,cntr,SETVAL,semopts) < 0 ) return (-1);
        }

        return(0);
}

static void PutMsg_Sig_Alrm_Deal()
{
        longjmp(ENV_PUTMSG_ALARM,1);
}

static void GetMsg_Sig_Alrm_Deal()
{
        longjmp(ENV_GETMSG_ALARM,1);
}

static int set_queue_maxbytes(int qid, unsigned long maxbytes)
{
        struct msqid_ds msq_st;
        int rv = 0;

        rv = msgctl(qid, IPC_STAT, &msq_st);
        if(rv == 0)
        {
                msq_st.msg_qbytes = maxbytes;
                rv = msgctl(qid, IPC_SET, &msq_st);
                if(rv){
                        return rv;
                }
        } else {
                return rv;
        }

        return 0;
}

int Ipc_CreateShm( int shmkey, int shmsize, int *shmid )
{
        int rv = 0;
        rv = shmget(shmkey, shmsize, HSM_IPC_P | IPC_CREAT | IPC_EXCL);
        if ( rv < 0 ){
                return -1;
        }
        *shmid = rv;	
        return 0;
}

int Ipc_OpenShm( int shmkey, int *shmid )
{
        int rv = 0;
        rv = shmget(shmkey, 0, HSM_IPC_P);
        if ( rv < 0 ){
                return -1;
        }
        *shmid = rv;
        return 0;
}

int Ipc_DeleteShm( int shmid )
{
        return( shmctl(shmid,IPC_RMID,NULL) );
}

int Ipc_MapShm( int shmid, void **shmaddr )
{
        *shmaddr = shmat(shmid, NULL, 0);
        if ( *shmaddr  < 0 ){
                return -1;
        }
        return 0;
}

int Ipc_DismapShm( void *shmaddr )
{
        return( shmdt(shmaddr) );
}

int Ipc_CreateQueue( int qkey, int qsize, int *qid )
{
        int rv = 0;
        unsigned long	lqsize;

        rv = msgget(qkey, HSM_IPC_P | IPC_CREAT | IPC_EXCL);
        if(rv != -1){
                *qid = rv;
        }else{ 
                return -1;
        }
        if ( HSM_Q_MIN_SIZE <= qsize && HSM_Q_MAX_SIZE >= qsize ){
                lqsize = qsize;
        }else if ( HSM_Q_MIN_SIZE > qsize ){
                lqsize = HSM_Q_MIN_SIZE;
        }else{ 
                lqsize = HSM_Q_MAX_SIZE;
        }
        return set_queue_maxbytes(*qid, lqsize);
}

int Ipc_OpenQueue( int qkey, int *qid )
{
        int rv = 0;

        rv = msgget(qkey, HSM_IPC_P);
        if(rv != -1){
                *qid = rv;	
        }else{
                return -1;
        }
        return 0;
}

int Ipc_DeleteQueue( int qid )
{
        return( msgctl(qid,IPC_RMID,NULL) );
}

int Ipc_PutMsg( int qid, long msgtype, unsigned char *buf, int buflen, int timeout )
{
        st_HsmMsgQ sysmsg;
        int rc = 0;

        if(buflen > MAX_BYTES_PERMSG)
        {
                FILE *fp;
                char msg[128];
                fp  = fopen( "/aa.txt", "wt+" );
                sprintf( msg, "buflen[%d],msglen[%d].",buflen,MAX_BYTES_PERMSG);
                fwrite( msg, 1, strlen(msg), fp );
                fclose( fp );
                return -1;
        }

        sysmsg.mtype = msgtype;
        memcpy( sysmsg.mtext,buf,buflen);
        sysmsg.mtext[buflen] = 0;	

        if ((rc = msgsnd(qid, &sysmsg, buflen, IPC_NOWAIT)) == 0) 
        {
                return 0;
        }

        if ( timeout < 0 )
        {
                return -2;
        }

        if (timeout > 0)
        {
                put_origin_alarmfunction = signal(SIGALRM,PutMsg_Sig_Alrm_Deal);

                if ( setjmp(ENV_PUTMSG_ALARM) == 0 )
                {
                        put_origin_alarmtime = alarm(timeout);

                        if ((rc = msgsnd(qid, &sysmsg, buflen, 0)) < 0) 
                        {
                                rc = -3;
                        }
                        else
                        {
                                rc = 0;
                                alarm(0);
                        }
                }
                else
                {
                        rc = -4;
                        alarm(0);
                }

                signal(SIGALRM,put_origin_alarmfunction);
                /* alarm(put_origin_alarmtime); */
        }
        else
        {
                if ((rc = msgsnd(qid, &sysmsg, buflen, 0)) < 0) 
                {
                        rc = -5;
                }
                else
                {
                        rc = 0;
                }
        }

        return(rc);	
}

int Ipc_GetMsg( int qid, long *msgtype, unsigned char *buf, int *buflen, int timeout )
{
        st_HsmMsgQ sysmsg;

        *buflen = -1;

        if ((*buflen = msgrcv(qid, &sysmsg, MAX_BYTES_PERMSG, *msgtype, IPC_NOWAIT)) > 0) 
        {
                memcpy(buf, sysmsg.mtext, *buflen);
                *(buf+*buflen) = 0;	
                *msgtype = sysmsg.mtype;
                return (*buflen);
        }

        if ( timeout < 0 ){
                return -1;
        }
        if (timeout > 0)
        {
                get_origin_alarmfunction = signal(SIGALRM,GetMsg_Sig_Alrm_Deal);

                if ( setjmp(ENV_GETMSG_ALARM) == 0 )
                {
                        get_origin_alarmtime = alarm(timeout);

                        if ((*buflen = msgrcv(qid, &sysmsg,sizeof(sysmsg.mtext), *msgtype, 0)) < 0) 
                        {
                                *buflen = -1;
                        }
                        else
                        {
                                memcpy(buf,sysmsg.mtext,*buflen);
                                *(buf+*buflen) = 0;	
                                *msgtype = sysmsg.mtype;
                                alarm(0);
                        }
                }
                else
                {
                        alarm(0);
                        *buflen = -1;
                }

                signal(SIGALRM,get_origin_alarmfunction);
                /* alarm(get_origin_alarmtime); */
        }
        else
        {
                if ((*buflen = msgrcv(qid, &sysmsg, sizeof(sysmsg.mtext), *msgtype,0)) < 0) 
                {
                        *buflen = -1;
                }
                else
                {
                        memcpy(buf,sysmsg.mtext,*buflen);
                        *(buf+*buflen) = 0;	
                        *msgtype = sysmsg.mtype;
                        return (*buflen);
                }
        }

        return 0;
}

int Ipc_CreateSem( int semkey, int semvalue, int *semid )
{
        key_t key_id = semkey;
        int nsems = 1;

        *semid  = semget(key_id, nsems, HSM_IPC_P | IPC_CREAT | IPC_EXCL) ;
        if (*semid == -1) {
                Ipc_OpenSem(key_id, semid);
                Ipc_DeleteSem(*semid);
                *semid = semget(key_id, nsems, IPC_CREAT | HSM_IPC_P) ;
        }

        if ( init_sems(*semid, nsems, semvalue) < 0 ){
                return(-2);
        }

        return 0;
}

int Ipc_OpenSem( int semkey, int *semid )
{
        key_t key_id;
        key_id = semkey;

        *semid = semget(key_id,0,HSM_IPC_P);
        if ( *semid < 0 ){
                return -1;
        }
        return 0;
}

int Ipc_DeleteSem( int semid )
{
        return(semctl(semid,0,IPC_RMID));
}

int Ipc_SemP( int semid )
{
        SEMBUF sbuf;

        sbuf.sem_num = 0 ;
        sbuf.sem_op =  -1;                		/* lock */
        sbuf.sem_flg = SEM_UNDO;       

        return( semop(semid,&sbuf,1) );	
}

int Ipc_SemV( int semid )
{
        SEMBUF sbuf;

        sbuf.sem_num = 0 ;
        sbuf.sem_op =  1;               /* unlock */
        sbuf.sem_flg = SEM_UNDO;       

        return( semop(semid,&sbuf,1) );
}



