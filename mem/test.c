#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int add (int a, int b) {
    return a+b;
}

int main()
{
    int a = 123;
    //char * filePath = "a.txt";
    //int fd = open( filePath, O_RDWR, S_IRUSR | S_IWUSR);
    //close(fd);
    //printf("%p\n", &a);
    //char* b = &a+100;
    //printf("%s\n", b);

    //FILE* fp = fopen("a.txt", "a+");
    //fclose(fp);
    int c = add(1,2);
    return 0;
}