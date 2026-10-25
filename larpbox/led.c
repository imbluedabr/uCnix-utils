#include "main.h"
#include <string.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <armv8-m/sys/gpio.h>
#include <sys/ioctl.h>
#include <sys/fcntl.h>

int builtin_led(int argc, char** argv)
{
	if (argc < 2) return 1;
	int togle = 0;
	if (strcmp(argv[1], "on") == 0) {
		togle = (1 << 12);
	} else if (strcmp(argv[1], "off") == 0) {
		togle = 0;
	} else if (strcmp(argv[1], "-?") == 0) {
		show_help(10);
		return 0;
	}

	int gpio = open("/dev/gpio0", O_RDWR);

	struct gpio_cmd pin0 = {
		.mask = (1 << 12),
		.value = GPIO_PINMODE_OUTPUT
	};

	ioctl(gpio, IOCTL_GPIO_CLAIM, &pin0);

	pin0.value = togle;
	ioctl(gpio, IOCTL_GPIO_WRITE, &pin0);	

	close(gpio);
	return 0;
}

