
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "popen.h"


/* popen() & system() function is sys call ,don't debug,only run. */
#define BUF_SIZE 1024
#define IO_FILE_PATH "/sys/class/gpio/"

char buf[BUF_SIZE];

int EIA_DA_Contral(char *io_port,uint8_t flag )
{
    FILE *p_file = NULL;
    char iString[256];
    strcpy(iString,io_port);
    strcat(iString,"/export");
    if( (p_file = popen(iString,"r")) == NULL )
    {
        printf("%s:%d popen() error/n", __FILE__, __LINE__);
        exit(1);
    }

//    if( (p_file = popen("echo 123","w")) == NULL )
//    {
//        printf("%s:%d popen() error/n", __FILE__, __LINE__);
//        exit(1);
//    }
//    char *iPtr="123";
//    iPtr = fputs(iPtr, p_file);

    while(fgets(buf, sizeof buf, p_file))
    {
        printf("%s:%d  output context::%s", __FILE__,__LINE__,buf);
    }
    pclose(p_file);
}

int user_popen(void)
{
    FILE * p_file = NULL;


             if( (p_file = popen("sudo ifconfig lo down", "r")) == NULL )
             {
                     printf("%s:%d popen() error/n", __FILE__, __LINE__);
                     exit(1);
             }

             while(fgets(buf, sizeof buf, p_file))
             {
                     printf("%s", buf);
             }
             pclose(p_file);

    p_file = popen("sudo ifconfig eth0 down", "r");
    if (!p_file) {
        fprintf(stderr, "Erro to popen");
    }

    while (fgets(buf, BUF_SIZE, p_file) != NULL) {
        fprintf(stdout, "%s", buf);
    }
    pclose(p_file);


    p_file = popen("touch test.tmp", "w");
    if (!p_file) {
        fprintf(stderr, "Erro to popen");
    }

    pclose(p_file);

    p_file = popen("ls -l", "r");
    if (!p_file) {
        fprintf(stderr, "Erro to popen");
    }

    while (fgets(buf, BUF_SIZE, p_file) != NULL) {
        fprintf(stdout, "%s", buf);
    }
    pclose(p_file);


    return 0;
}
