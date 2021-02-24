struct buf;
struct context;
struct file;
struct inode;
struct pipe;
struct proc;
struct rtcdate;
struct spinlock;
struct stat;
struct superblock;


// bio.c
void            binit(void);
struct  buf*    bread(uint, uint);
void            brelse(struct buf*);
void            bwrite(struct buf*);

// console.c
void            consoleinit(void);
void            cprintf(char*,...);
void            consoleintr(int(*)(void));
void            panic(char*) __attribute__((noreturn));

// exec.c
int             exec(char*,char**);

// file.c
struct file *   filealloc(void);
void            fileclose(struct file*);
struct file*    filedup(struct file*);
void            fileinit(void);
int             fileread(struct file*,char*, int n);
int             filestat(struct file*,struct stat*);
int             filewrite(struct file*,char*,int n);

// fs.c
void            readsb(int dev, struct superblock *sb);
int             dirlink(struct inode*, char*, uint);
struct inode*   dirlookup(struct inode*, char*, uint*);
struct inode*   ialloc(uint, short);
struct inode*   idup(struct inode*);
void            iinit(int dev);
void            ilock(struct inode*);
void            iput(struct inode*);
void            iunlock(struct inode*);
void            iunlockput(struct inode*);
void            iupdate(struct inode*);
int             namecmp(const char *,const char*);
struct inode*   namei(char*);
struct inode*   nameiparent(char*,char *);
int             readi(struct inode*,char*, uint, uint);
void            stati(struct inode*, struct stat*);
int             writei(struct inode*,char*,uint, uint);

// ide.c
void            ideinit(void);
void            ideintr(void);
void            iderw(struct buf*);

// ioapic
void            ioapicenable(int irq,int cpu);
extern uchar    ioapicid;
void            ioapicinit(void);

// kalloc.c
char*           kalloc(void);
void            kfree(char*);
void            kinit1(void*,void*);
void            kinit2(void*, void*);

// kbd.c
void            kbdintr(void);

// lapic.c
void            cmostime(struct rtcdata *r);
int             cpunum(void);
extern volatile uint* lapic;
void            lapiceoi(void);
void            lapicinit(void);
void            lapicstartap(uchar, uint);
void            microdelay(int);

// log.c
void            initlog(int dev);
void            log_write(struct buf*);
void            begin_op();
void            end_op();

// mp.c
extern int      ismp;
void            mpinit(void);

// picireq.c
void            picenable(int);
void            picinit(void);

// pipe.c
int             pipealloc(struct file**,struct file**)

