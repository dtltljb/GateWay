/* file stat example */  
 #include <unistd.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <fcntl.h> 
#include <time.h> 
#include <stdlib.h> 
#include <stdio.h> 

int main(int argc, char **argv) 
{ 
struct stat buf; 
mode_t mode; 
char type[80]; 
int fd; 

if(argc != 2) 
{ 
puts("USAGE: mstat {file}"); 
exit(EXIT_FAILURE); 
} 

if((fd = open(argv[1], O_RDONLY)) < 0) 
{ 
perror("open"); 
exit(EXIT_FAILURE); 
} 

if((fstat(fd, &buf)) < 0) 
{ 
perror("fstat"); 
exit(EXIT_FAILURE); 
} 

mode = buf.st_mode; 
printf(" FILE: %s\n", argv[1]); 
printf(" INODE: %ld\n", buf.st_ino); 
printf(" DEVICE: %d,%d\n", major(buf.st_dev), minor(buf.st_dev)); 
printf(" SMODE: %#o\n", mode); 
printf(" MODE: %#o\n", mode & ~(S_IFMT)); 
printf(" LINKS: %d\n", buf.st_nlink); 
printf(" UID: %d\n", buf.st_uid); 
printf(" GID: %d\n", buf.st_gid); 

if(S_ISLNK(mode)) 
strcpy(type, "Symbolic line"); 
else if(S_ISREG(mode)) 
strcpy(type, "Regular file"); 
else if(S_ISDIR(mode)) 
strcpy(type, "Directory"); 
else if(S_ISCHR(mode)) 
strcpy(type, "Character device"); 
else if(S_ISBLK(mode)) 
strcpy(type, "Block device"); 
else if(S_ISFIFO(mode)) 
strcpy(type, "FIFO"); 
else if(S_ISSOCK(mode)) 
strcpy(type, "Socket"); 
else 
strcpy(type, "Unknown type"); 

printf(" TYPE: %s\n", type); 
printf(" SIZE: %d\n", buf.st_size); 
printf("BLK SIZE: %ld\n", buf.st_blksize); 
printf(" BLOCKS: %d\n", (int)buf.st_blocks); 
printf("ACCESSED: %s", ctime(&buf.st_atime)); 
printf("MODIFIED: %s", ctime(&buf.st_mtime)); 
printf(" CHANGED: %s", ctime(&buf.st_ctime)); 

// close the file 
if(close(fd) < 0) 
{
perror("close"); 
exit(EXIT_FAILURE); 
} 
exit(EXIT_SUCCESS); 
} 
