#include "GpioInterrupt.h"
#include <linux/gpio.h>
#include <sys/poll.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define DEV_GPIO  "/dev/gpiochip3"
#define POLL_TIMEOUT -1 /* No timeout */

int uncorrected_index;
double corrected_index;


void *gpioirq(void *arg)
{
    int fd, fd_in;
    int ret;
    int flags;

    struct gpioevent_request req;
    struct gpioevent_data evdata;
    struct pollfd fdset;

    /* open gpio */
    fd = open(DEV_GPIO, O_RDWR);
    if (fd < 0) {
        printf("ERROR: open %s ret=%d\n", DEV_GPIO, fd);
        //return -1;
    }

    /* Request gpio4_14 interrupt */
    req.lineoffset = 24;
    req.handleflags = GPIOHANDLE_REQUEST_INPUT;
    req.eventflags  = GPIOEVENT_REQUEST_RISING_EDGE;
    strncpy(req.consumer_label, "gpio_irq", sizeof(req.consumer_label) - 1);

    /* requrest line event handle */
    ret = ioctl(fd, GPIO_GET_LINEEVENT_IOCTL, &req);
    if (ret) {
        printf("ERROR: ioctl get line event ret=%d\n", ret);
        //return -1;
    }

    /* set event fd nonbloack read */
    fd_in = req.fd;
    flags = fcntl(fd_in, F_GETFL);
    flags |= O_NONBLOCK;
    ret = fcntl(fd_in, F_SETFL, flags);
    if (ret) {
        printf("ERROR: fcntl set nonblock read\n");
    }

    printf("Before while\n");

    while(1)
    {
        /* set event fd nonbloack read */
        fd_in = req.fd;
        flags = fcntl(fd_in, F_GETFL);
        flags |= O_NONBLOCK;
        ret = fcntl(fd_in, F_SETFL, flags);
        if (ret) {
            printf("ERROR: fcntl set nonblock read\n");
        }

        fdset.fd      = fd_in;
        fdset.events  = POLLIN;
        fdset.revents = 0;
        //first_irq = 1;

        printf("before poll gpio line event\n");
        /* poll gpio line event */
        ret = poll(&fdset, 1, POLL_TIMEOUT);
        if (ret <= 0)
            continue;
        printf("after poll gpio line event\n");
        if (fdset.revents & POLLIN)
        {
            printf("irq received.\n");
            ret = read(fd_in, &evdata, sizeof(evdata));
        }
    }

        /* close gpio */
        close(fd);

    return 0;
}
