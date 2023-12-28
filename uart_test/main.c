#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

int main(){
    int fd, len;
    char text[255];
    struct termios options;  /*Serial ports setting */

    fd = open("/dev/ttymxc2", O_RDWR | O_NDELAY | O_NOCTTY);
    if(fd<0)
    {
        perror("Error opening serial port");
        return -1;
    }

    /* Set up serial port */

    options.c_cflag = B115200 | CS8 | CLOCAL | CREAD;
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;


    /*Apply the settings*/
    tcflush(fd, TCIFLUSH);
    tcsetattr(fd, TCSANOW, &options);

    strcpy(text, "ledyak\n\r");
    len = strlen(text);
    len = write(fd, text, len);
    printf("Wrote %d bytes over UART\n", len);

    printf("You have 5s to send me some input data...\n");
    sleep(5);

    /*Read from serial port */
    memset(text, 0, 255);
    len = read(fd, text, 255);
    printf("Received %d bytes\n", len);
    printf("Received string: %s\n", text);


    close(fd);
    return 0;

}
