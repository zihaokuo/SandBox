
#define PRINT_FILE_DENY(path)               \
    if(MyOutputDevice == NULL)\
    {\
        MyOutputDevice = fdopen(dup(0), "w");\
    }\
    fprintf(MyOutputDevice ,"[sandbox] %s: access to %s is not allowed\n", __func__,path);

#define PRINT_EXEC_DENY(path)               \
    if (MyOutputDevice == NULL)               \
    {                                       \
        MyOutputDevice = fdopen(dup(0), "w"); \
        if(MyOutputDevice==NULL)fprintf(stderr,"fopen error!");\
    }                                       \
    fprintf(MyOutputDevice, "[sandbox] %s(\"%s\"): not allowed\n", __FUNCTION__, path);

#define LOAD_OLD_FUNC(name)                                    \
    if (LibcHandle == NULL)                                    \
    {                                                          \
        LibcHandle = dlopen("libc.so.6", RTLD_LAZY);           \
        if (LibcHandle == NULL)                                \
            printf("libc.so.6 dlopen error:%s\n", dlerror()); \
    }                                                          \
    if (old_##name == NULL)                                    \
    {                                                          \
        old_##name = dlsym(LibcHandle, #name);                 \
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
#define FUNC_ARG1(name, ret, RetFail, arg1, a1)     \
    static ret (*old_##name)(arg1) = NULL;          \
    ret name(arg1 a1)                               \
    {                                               \
        if (AccessRight(a1))                        \
        {                                           \
            LOAD_OLD_FUNC(name)                     \
            if (old_##name != NULL)                 \
            {                                       \
                return old_##name(a1);               \
            }                                       \
        }                                           \
        else                                        \
        {                                           \
            PRINT_FILE_DENY(a1)                     \
        }                                           \
        switch (RetFail)                            \
        {                                           \
        case 'i':                                   \
            return -1;                              \
        case 'D':                                   \
            return 0;                            \
        default:                                    \
            printf("RetFail is wrong:%c", RetFail); \
            exit(EXIT_FAILURE);                     \
        }                                           \
    }

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

#define FUNC_ARG2(name, ret, RetFail, CheckNum, arg1, a1, arg2, a2)   \
    static ret (*old_##name)(arg1, arg2) = NULL;                      \
    ret name(arg1 a1, arg2 a2)                                        \
    {                                                                 \
        int SumCheck = 0;                                             \
        switch (CheckNum)                                             \
        {                                                             \
        case 1:                                                       \
            if(AccessRight(a1) > 0)\
                SumCheck++;\
            else\
            {\
                PRINT_FILE_DENY(a1); \
            }\
            break;                                                    \
        case 2:                                                       \
            if(AccessRight(a1) > 0)\
                SumCheck++;\
            else\
            {\
                PRINT_FILE_DENY(a1); \
            }\
            if(AccessRight(a2) > 0)\
                SumCheck++;\
            else\
            {\
                PRINT_FILE_DENY(a2); \
            }\
            break;                                                    \
        default:                                                      \
            printf("CheckNum is wrong:%d", CheckNum);                 \
            exit(EXIT_FAILURE);                                       \
        }                                                             \
        if (SumCheck == CheckNum)                                     \
        {                                                             \
            LOAD_OLD_FUNC(name)                                       \
            if (old_##name != NULL)                                   \
            {                                                         \
                return old_##name(a1, a2);                             \
            }                                                         \
        }                                                             \
        switch (RetFail)                                              \
        {                                                             \
        case 'i':                                                     \
            return -1;                                                \
        case 'F':                                                     \
            return 0;                                              \
        default:                                                      \
            printf("RetFail is wrong:%c", RetFail);                   \
            exit(EXIT_FAILURE);                                       \
        }                                                             \
    }

/* arg3
int chown(const char *path, uid_t owner, gid_t group);
ssize_t readlink(const char *path, char *buf, size_t bufsiz);
int __xstat(int ver, const char * path, struct stat * stat_buf);
int __xstat64(int ver, const char * path, struct stat64 * stat_buf); */
//case2, mean we need to check a2 and we only need to check one parameter

#define FUNC_ARG3(name, ret, CheckNum, arg1, a1, arg2, a2, arg3, a3) \
    static ret (*old_##name)(arg1, arg2, arg3) = NULL;                        \
    ret name(arg1 a1, arg2 a2, arg3 a3)                                       \
    {                                                                         \
        int SumCheck = 0;                                                     \
        switch (CheckNum)                                                     \
        {                                                                     \
        case 1:                                                               \
            if(AccessRight(a1) > 0)\
                SumCheck++;\
            else\
            {\
                PRINT_FILE_DENY(a1);\
            }\
            break;                                                            \
        case 2:                                                               \
            if(AccessRight(a2) > 0)\
                (SumCheck += 2);\
            else\
            {\
                PRINT_FILE_DENY(a2);\
            }\
            break;                                                            \
        default:                                                              \
            printf("CheckNum is wrong:%d", CheckNum);                         \
            exit(EXIT_FAILURE);                                               \
        }                                                                     \
        if (SumCheck == CheckNum)                                             \
        {                                                                     \
            LOAD_OLD_FUNC(name)                                               \
            if (old_##name != NULL)                                           \
            {                                                                 \
                return old_##name(a1, a2, a3);                                 \
            }                                                                 \
        }                                                                     \
        return -1;\
    }

/*arg4
int openat(int dirfd, const char *pathname, int flags,mode_t mode)
int openat64(int dirfd, const char *pathname, int flags,mode_t mode)*/
//case2, mean we need to check a2 and we only need to check one parameter

#define FUNC_ARG4(name, ret, arg1, a1, arg2, a2, arg3, a3, arg4, a4) \
    static ret (*old_##name)(arg1, arg2, arg3, arg4) = NULL;                            \
    ret name(arg1 a1, arg2 a2, arg3 a3, arg4 a4)                                        \
    {                                                                                   \
        if (AccessRight(a2))                        \
        {                                           \
            LOAD_OLD_FUNC(name)                     \
            if (old_##name != NULL)                 \
            {                                       \
                return old_##name(a1,a2,a3,a4);               \
            }                                       \
        }                                           \
        else                                        \
        {                                           \
            PRINT_FILE_DENY(a2)                     \
        }                                           \
        return -1;\
    }                                                                                   


#define FUNC_EXEC(name, path, ret, ...)\
    ret name(__VA_ARGS__)               \
    {                                   \
        PRINT_EXEC_DENY(path);          \
        return -1;                      \
    }
