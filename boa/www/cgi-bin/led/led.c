#include<stdio.h>
#include<stdlib.h>

int main(int argc, char const *argv[])
{
    printf("Content-type:text/html\n\n");
    char *data = getenv("QUERY_STRING");
 
    system("echo 58 > /sys/class/gpio/export");

    system("echo out > /sys/class/gpio/gpio58/direction");
    if(data[0] == '1')
    {
        system("echo 1 > /sys/class/gpio/gpio58/value");
        printf("off\n");
    }
    else if(data[0] == '0')
    {
        system("echo 0 > /sys/class/gpio/gpio58/value");
        printf("on\n");
    }

    return 0;
}