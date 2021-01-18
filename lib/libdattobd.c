// SPDX-License-Identifier: LGPL-2.1-or-later

/*
 * Copyright (C) 2015 Datto Inc.
 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include "libdattobd.h"

int dattobd_setup_snapshot(unsigned int minor, char *bdev, char *cow, unsigned long fallocated_space, unsigned long cache_size){
	int fd, ret;
	struct setup_params sp;

	fd = open("/dev/datto-ctl", O_RDONLY);
	if(fd < 0) return -1;

	sp.minor = minor;
	sp.bdev = bdev;
	sp.cow = cow;
	sp.fallocated_space = fallocated_space;
	sp.cache_size = cache_size;

	ret = ioctl(fd, IOCTL_SETUP_SNAP, &sp);

	close(fd);
	return ret;
}
int dattobd_setup_snapshot_group(unsigned int *minors, char **bdevs, char **cows, unsigned long *fallocated_spaces, unsigned long *cache_sizes,unsigned int count){
	int fd, ret;
	struct setup_params_group* sp;

	sp = (struct setup_params_group*)malloc(sizeof(struct setup_params_group) + count * sizeof(struct setup_params));
	if(!sp)
		return -1;

	fd = open("/dev/datto-ctl", O_RDONLY);
	if(fd < 0) return -1;
	sp->count = count;
	for(int i = 0 ; i < count; ++i){
		sp->setup_params[i].minor = minors[i];
		sp->setup_params[i].bdev = bdevs[i];
		sp->setup_params[i].cow = cows[i];
		sp->setup_params[i].fallocated_space = fallocated_spaces[i];
		sp->setup_params[i].cache_size = cache_sizes[i];
	}
	ret = ioctl(fd, IOCTL_SETUP_SNAP_GROUP, sp);

	close(fd);
	free(sp);
	return ret;
}


int dattobd_reload_snapshot(unsigned int minor, char *bdev, char *cow, unsigned long cache_size){
	int fd, ret;
	struct reload_params rp;

	fd = open("/dev/datto-ctl", O_RDONLY);
	if(fd < 0) return -1;

	rp.minor = minor;
	rp.bdev = bdev;
	rp.cow = cow;
	rp.cache_size = cache_size;

	ret = ioctl(fd, IOCTL_RELOAD_SNAP, &rp);

	close(fd);
	return ret;
}

int dattobd_reload_incremental(unsigned int minor, char *bdev, char *cow, unsigned long cache_size){
	int fd, ret;
	struct reload_params rp;

	fd = open("/dev/datto-ctl", O_RDONLY);
	if(fd < 0) return -1;

	rp.minor = minor;
	rp.bdev = bdev;
	rp.cow = cow;
	rp.cache_size = cache_size;

	ret = ioctl(fd, IOCTL_RELOAD_INC, &rp);

	close(fd);
	return ret;
}

int dattobd_destroy(unsigned int minor){
	int fd, ret;

	fd = open("/dev/datto-ctl", O_RDONLY);
	if(fd < 0) return -1;

	ret = ioctl(fd, IOCTL_DESTROY, &minor);

	close(fd);
	return ret;
}

int dattobd_transition_incremental(unsigned int minor){
	int fd, ret;

	fd = open("/dev/datto-ctl", O_RDONLY);
	if(fd < 0) return -1;

	ret = ioctl(fd, IOCTL_TRANSITION_INC, &minor);

	close(fd);
	return ret;
}

int dattobd_transition_snapshot(unsigned int minor, char *cow, unsigned long fallocated_space){
	int fd, ret;
	struct transition_snap_params tp;

	tp.minor = minor;
	tp.cow = cow;
	tp.fallocated_space = fallocated_space;

	fd = open("/dev/datto-ctl", O_RDONLY);
	if(fd < 0) return -1;

	ret = ioctl(fd, IOCTL_TRANSITION_SNAP, &tp);

	close(fd);
	return ret;
}

int dattobd_transition_snapshot_group(unsigned int* minors, char **cows, unsigned long *fallocated_spaces, unsigned int count){
	int fd, ret;
	struct transition_snap_params_group *tp;

	tp = (struct transition_snap_params_group*)malloc(sizeof(struct transition_snap_params_group) + count * sizeof(struct transition_snap_params));
	tp->count = count;
	printf("%u\n",tp->count);
	printf("%lu\n",sizeof(struct transition_snap_params_group) + count * sizeof(struct transition_snap_params));
	fd = open("/dev/datto-ctl", O_RDONLY);
	if(fd < 0) return -1;
	for(int i = 0 ; i < count ;++i){
		tp->transition_snap_params[i].minor = minors[i];
		tp->transition_snap_params[i].cow = cows[i];
		tp->transition_snap_params[i].fallocated_space = fallocated_spaces[i];
	}

	ret = ioctl(fd, IOCTL_TRANSITION_SNAP_GROUP, tp);

	close(fd);
	return ret;
}

int dattobd_reconfigure(unsigned int minor, unsigned long cache_size){
	int fd, ret;
	struct reconfigure_params rp;

	fd = open("/dev/datto-ctl", O_RDONLY);
	if(fd < 0) return -1;

	rp.minor = minor;
	rp.cache_size = cache_size;

	ret = ioctl(fd, IOCTL_RECONFIGURE, &rp);

	close(fd);
	return ret;
}

int dattobd_info(unsigned int minor, struct dattobd_info *info){
	int fd, ret;

	if(!info){
		errno = EINVAL;
		return -1;
	}

	fd = open("/dev/datto-ctl", O_RDONLY);
	if(fd < 0) return -1;

	info->minor = minor;

	ret = ioctl(fd, IOCTL_DATTOBD_INFO, info);

	close(fd);
	return ret;
}

int dattobd_get_free_minor(void){
	int fd, ret, minor;

	fd = open("/dev/datto-ctl", O_RDONLY);
	if(fd < 0) return -1;

	ret = ioctl(fd, IOCTL_GET_FREE, &minor);

	close(fd);

	if(!ret) return minor;
	return ret;
}
