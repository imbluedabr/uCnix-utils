#include "main.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <armv8-m/sys/i2c.h>
#include <sys/ioctl.h>
#include <sys/errno.h>

int builtin_i2c_scan(int argc, char** argv)
{
	struct i2c_msg msg;

	struct i2c_rdwr_ioctl_data data = {
		.msgs = &msg,
		.nmsgs = 1
	};

	puts("i2c scanner v1.0\n");


	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-?") == 0) {
			show_help(11);
			return 0;
		}

		printf("bus: %s; addr,status\n", argv[i]);
		int bus = open(argv[i], O_RDWR);
		if (bus < 0) {
			printf("i2c_scan: failed to open i2c bus!\n");
			return 1;
		}

		for (int addr = 0x8; addr < 0x77; addr++) {
			msg.addr = addr;
			msg.flags = 0;
			msg.len = 0;
			msg.buf = NULL;
			int status = ioctl(bus, I2C_RDWR, &data);
			if (status >= 0) {
				printf("0x%x: device found!\n", addr);
			} else if (status != -ENXIO){
				printf("0x%x: abnormal error: errno=%d\n", addr, -status);
			}
		}

		close(bus);
	}

	return 0;
}


