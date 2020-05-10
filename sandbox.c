#include <unistd.h>
#include <sys/stat.h>
#include<dlfcn.h>
#include <dirent.h>
#include <sys/types.h>
#include<stdio.h>
#include<limits.h>
#include<stdlib.h>
#include<string.h>
#include "sandbox.h"

static void *LibcHandle=NULL;
static FILE* MyOutputDevice=NULL;

int AccessRight(const char* TargetPath){
	char* RootPath;
	char RealRootPath[PATH_MAX]="",RealTargetPath[PATH_MAX]="";
	int LenRealRoot, LenRealTarget;

	RootPath = getenv("SANDBOX_ROOT");
    
	if(RootPath== NULL||TargetPath == NULL){
		printf("no root or no target\n");
		return 1;
	}

		
	realpath(RootPath,RealRootPath);
	realpath(TargetPath,RealTargetPath);

	LenRealRoot = strlen(RealRootPath);
	LenRealTarget = strlen(RealTargetPath);


	if((LenRealTarget >= LenRealRoot) && (strncmp(RealTargetPath,RealRootPath,LenRealRoot)==0)){
		return 1;	
	}else{
		return 0;
	}

}

/* arg detail
name: old funciton name
ret: type of return value 
RetFail: to decide fail return value
CheckNum: how many check we need
arg1: type of arg1
a1: function's first arguments ... */

/* arg1
int chdir(const char *path);
int remove(const char *filename)
int unlink(const char *pathname);
int rmdir(const char *path);
DIR *opendir(const char *name); */
//#define FUNC_ARG1(name,ret,RetFail,arg1,a1) 

FUNC_ARG1(chdir,int,'i',const char*,path)
FUNC_ARG1(remove,int,'i',const char*,filename)
FUNC_ARG1(unlink,int,'i',const char*,pathname)
FUNC_ARG1(rmdir,int,'i',const char*,path)
FUNC_ARG1(opendir,DIR*,'D',const char*,name)

/* arg2
FILE *fopen(const char *filename, const char *mode)
FILE *fopen64(const char *filename, const char *mode)
int open(const char *pathname, int flags);
int open64(const char *pathname, int flags);
int creat(const char *path, mode_t mode);
int creat64(const char *path, mode_t mode);
int mkdir(const char *   path, mode_t mode);
int chmod(const char *  pathname, mode_t mode);
int rename(const char *old, const char *new);
int symlink(const char *path1, const char *path2);
int link(const char *   path1, const char *path2); */
//#define FUNC_ARG2(name,ret,RetFail,CheckNum,arg1,a1,arg2,a2) 

FUNC_ARG2(fopen,FILE*,'F',1,const char*,filename,const char *,mode)
FUNC_ARG2(fopen64,FILE*,'F',1,const char*,filename,const char *,mode)
FUNC_ARG2(create,int,'i',1,const char*,path,mode_t,mode)
FUNC_ARG2(create64,int,'i',1,const char*,path,mode_t,mode)
FUNC_ARG2(mkdir,int,'i',1,const char *,path,mode_t,mode)
FUNC_ARG2(chmod,int,'i',1,const char *,pathname,mode_t,mode)
FUNC_ARG2(rename,int,'i',2,const char *,old,const char*,new)
FUNC_ARG2(symlink,int,'i',2,const char *,path1,const char*,path2)
FUNC_ARG2(link,int,'i',2,const char *,path1,const char *,path2)

/* arg3
int chown(const char *path, uid_t owner, gid_t group);
ssize_t readlink(const char *path, char *buf, size_t bufsiz);
int __xstat(int ver, const char * path, struct stat * stat_buf);
int __xstat64(int ver, const char * path, struct stat64 * stat_buf);
int open(const char *pathname, int flags, mode_t mode);
int open64(const char *pathname, int flags, mode_t mode);*/
//case2, mean we need to check a2 and we only need to check one parameter
//#define FUNC_ARG3(name, ret, CheckNum, arg1, a1, arg2, a2, arg3, a3) 

FUNC_ARG3(chown,   int,    1,const char *,path,uid_t,owner,gid_t,group)
FUNC_ARG3(readlink,ssize_t,1,const char *,path,char *,buf,size_t,bufsiz)
FUNC_ARG3(__xstat,  int,   2,int,          ver,const char *,path,struct stat *,stat_buf)
FUNC_ARG3(__xstat64,int,    2,int,         ver,const char *,path,struct stat *,stat_buf)
FUNC_ARG3(open,int,1,const char*,pathname,int,flag,mode_t,mode)
FUNC_ARG3(open64,int,1,const char*,pathname,int,flag,mode_t,mode)

/*arg4
int openat(int dirfd, const char *pathname, int flags,mode_t mode)
int openat64(int dirfd, const char *pathname, int flags,mode_t mode)*/
//case2, mean we need to check a2 and we only need to check one parameter
//#define FUNC_ARG4(name, ret, arg1, a1, arg2, a2, arg3, a3, arg4, a4) 

FUNC_ARG4(openat,int,int,dirfd,const char *,pathname,int,flags,mode_t,mode)
FUNC_ARG4(openat64,int,int,dirfd,const char *,pathname,int,flags,mode_t,mode)

/*exec
int execl(const char *path, const char *arg, ...);
int execlp(const char *file, const char *arg, ...);
int execle (const char *__path, const char *__arg, ...)
int execv(const char *path, char *const argv[]);
int execvp(const char *file, char *const argv[]);
int system(const char *command)
int execve(const char *pathname, char *const argv[],char *const envp[]);*/

FUNC_EXEC(execl,path,int,const char*path,const char*arg,...)
FUNC_EXEC(execlp,file,int,const char*file,const char*arg,...)
FUNC_EXEC(execle,__path,int,const char *__path, const char *__arg, ...)
FUNC_EXEC(execv,path,int,const char *path, char *const argv[])
FUNC_EXEC(execvp,file,int,const char *file, char *const argv[])
FUNC_EXEC(system,command,int,const char *command)
FUNC_EXEC(execve,pathname,int,const char *pathname, char *const argv[],char *const envp[])









