#include <QCoreApplication>
#include "GpioInterrupt.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <poll.h>
#include <linux/gpio.h>
#include <sys/ioctl.h>
#include <pthread.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    pthread_t thread_id;
    pthread_create(&thread_id, NULL, gpioirq, NULL);
    pthread_join(thread_id, NULL);

    return a.exec();
}
