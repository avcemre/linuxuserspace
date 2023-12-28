#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <linux/gpio.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <stdint.h>

#define UART_DEVICE "/dev/ttymxc3"  // UART4
#define GPIO_DEVICE "/dev/gpiochip0"

int main() {
    int uart_fd;
    int bytes_read;
    int gpio_fd, ret;

    struct termios options;

    uart_fd = open(UART_DEVICE, O_RDWR | O_NDELAY | O_NOCTTY);
    gpio_fd = open(GPIO_DEVICE, O_RDONLY);
    if(gpio_fd < 0)
    {
        perror("Open gpio device error");
    }

    struct gpiohandle_request rq;
    rq.lineoffsets[0] = 18;
    rq.lines = 1;
    rq.flags = GPIOHANDLE_REQUEST_OUTPUT;

    ret = ioctl(gpio_fd, GPIO_GET_LINEHANDLE_IOCTL, &rq);

    struct gpiohandle_data data;


    if (uart_fd == -1)
    {
        perror("Failed to open UART");
        return -1;
    }


    tcgetattr(uart_fd, &options);
    cfsetospeed(&options, B115200);
    cfsetispeed(&options, B115200);

    // Set RS485 mode
    options.c_cflag = B115200 | CS8 | CLOCAL | CREAD;
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;

    tcflush(uart_fd, TCIFLUSH);
    tcsetattr(uart_fd, TCSANOW, &options);

    data.values[0] = 1;
    ret = ioctl(rq.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data);

    write(uart_fd, "Hello!", 6);

    usleep(10000);
    data.values[0] = 0;
    ret = ioctl(rq.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data);
    usleep(10000);
    if (ret == -1)
    {
        perror("Unable to set line value using ioctl");
    }

    char buffer[255];

    while (1)
    {

        data.values[0] = 0;
        ret = ioctl(rq.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data);

        memset(buffer, 0, sizeof(buffer));

        bytes_read = read(uart_fd, buffer, sizeof(buffer));
        if (bytes_read > 0)
        {
            printf("Received: %s\n", buffer);
        }


        usleep(100000);  // Bekleme süresi

        //data.values[0] = 1;
        //ret = ioctl(rq.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data);


    }

    close(uart_fd);
    return 0;
}

