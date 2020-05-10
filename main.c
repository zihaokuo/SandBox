#include <unistd.h>  /* getopt */
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h> /* getopt */

int main(int argc, char **argv)
{
    char opt;
    char *PathLib = "./sandbox.so";
    char *BaseLocation = "./";
    while((opt = getopt(argc,argv,"p:d:"))!= -1)
    {
        switch (opt)
        {
        case 'p':
            PathLib = optarg;
            break;
        case 'd':
            BaseLocation = optarg;
            break;
        default:
            fprintf(stderr, "usage: ./sandbox [-p sopath] [-d basedir] [--] cmd [cmd args ...]\n");
            fprintf(stderr, "       -p: set the path to sandbox.so, default = ./sandbox.so\n");
            fprintf(stderr, "       -d: the base directory that is allowed to access, default = .\n");
            fprintf(stderr, "       --: separate the arguments for sandbox and for the executed command\n");
            exit(EXIT_FAILURE);
        }    
    }

	char cmd[10000];//4096*2
	char tmp[10000];

	if(argc==optind){
		fprintf(stderr,"no command given.\n");
		exit(1);
	}

	snprintf(cmd,10000,"LD_PRELOAD=%s SANDBOX_ROOT=%s",PathLib, BaseLocation);
	for(;optind<argc;optind++){
		snprintf(tmp,10000,"%s %s",cmd,argv[optind]);
		snprintf(cmd,10000,"%s",tmp);
	}

	system(cmd);

    return 0;
}