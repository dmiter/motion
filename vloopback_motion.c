/*
 *    vloopback_motion.c
 *
 *    Video loopback functions for motion.
 *    Copyright 2000 by Jeroen Vreeken (pe1rxq@amsat.org)
 *    Copyright 2008 by Angel Carpintero (motiondevelop@gmail.com)
 *    This software is distributed under the GNU public license version 2
 *    See also the file 'COPYING'.
 *
 */
#include "vloopback_motion.h"
#if (defined(HAVE_LINUX_VIDEODEV_H) || defined(MOTION_V4L2)) && (!defined(WITHOUT_V4L))
#include <sys/utsname.h>
#include <dirent.h>

#define ROUND_UP_2(num)  (((num)+1)&~1)
#define ROUND_UP_4(num)  (((num)+3)&~3)
#define ROUND_UP_8(num)  (((num)+7)&~7)
#define ROUND_UP_16(num) (((num)+15)&~15)
#define ROUND_UP_32(num) (((num)+31)&~31)
#define ROUND_UP_64(num) (((num)+63)&~63)

/**
 * v4l_open_vidpipe
 *
 */
static int v4l_open_vidpipe(void)
{
    int pipe_fd = -1;
    char pipepath[255];
    char buffer[255];
    char *major;
    char *minor;
    struct utsname uts;

    if (uname(&uts) < 0) {
        MOTION_LOG(CRT, TYPE_VIDEO, SHOW_ERRNO, "%s: Unable to execute uname");
        return -1;
    }

    major = strtok(uts.release, ".");
    minor = strtok(NULL, ".");

    if ((major == NULL) || (minor == NULL) || (strcmp(major, "2"))) {
        MOTION_LOG(CRT, TYPE_VIDEO, SHOW_ERRNO, "%s: Unable to decipher OS version");
        return -1;
    }

    if (strcmp(minor, "5") < 0) {
        FILE *vloopbacks;
//        char *loop;
        char *input;
        char *istatus;
        char *output;
//        char *ostatus;

        vloopbacks = fopen("/proc/video/vloopback/vloopbacks", "r");

        if (!vloopbacks) {
            MOTION_LOG(CRT, TYPE_VIDEO, SHOW_ERRNO, "%s: Failed to open "
                       "'/proc/video/vloopback/vloopbacks'");
            return -1;
        }

        /* Read vloopback version*/
        if (!fgets(buffer, sizeof(buffer), vloopbacks)) {
            MOTION_LOG(CRT, TYPE_VIDEO, SHOW_ERRNO, "%s: Unable to read vloopback version");
            myfclose(vloopbacks);
            return -1;
        }

        fprintf(stderr, "\t%s", buffer);

        /* Read explanation line */

        if (!fgets(buffer, sizeof(buffer), vloopbacks)) {
            MOTION_LOG(CRT, TYPE_VIDEO, SHOW_ERRNO, "%s: Unable to read vloopback"
                       " explanation line");
            myfclose(vloopbacks);
            return -1;
        }

        while (fgets(buffer, sizeof(buffer), vloopbacks)) {
            if (strlen(buffer) > 1) {
                buffer[strlen(buffer)-1] = 0;
                /*loop =*/(void)strtok(buffer, "\t");
                input = strtok(NULL, "\t");
                istatus = strtok(NULL, "\t");
                output = strtok(NULL, "\t");
                /*ostatus =*/(void)strtok(NULL, "\t");

                if (istatus[0] == '-') {
                    snprintf(pipepath, sizeof(pipepath), "/dev/%s", input);
                    pipe_fd = open(pipepath, O_RDWR);

                    if (pipe_fd >= 0) {
                        MOTION_LOG(NTC, TYPE_VIDEO, NO_ERRNO, "%s: \tInput:  /dev/%s "
                                   "\tOutput: /dev/%s",  input, output);
                        break;
                    }
                }
            }
        }

        myfclose(vloopbacks);
    } else {
        DIR *dir;
        struct dirent *dirp;
        const char prefix[] = "/sys/class/video4linux/";
        char *ptr, *io;
        int fd;
        int low = 9999;
        int tfd;
        int tnum;

        if ((dir = opendir(prefix)) == NULL) {
            MOTION_LOG(CRT, TYPE_VIDEO, SHOW_ERRNO, "%s: Failed to open '%s'",
                       prefix);
            return -1;
        }

        while ((dirp = readdir(dir)) != NULL) {
            if (!strncmp(dirp->d_name, "video", 5)) {
                strncpy(buffer, prefix, sizeof(buffer));
                strncat(buffer, dirp->d_name, sizeof(buffer) - strlen(buffer));
                strncat(buffer, "/name", sizeof(buffer) - strlen(buffer));

                if ((fd = open(buffer, O_RDONLY)) >= 0) {
                    if ((read(fd, buffer, sizeof(buffer)-1)) < 0) {
                        close(fd);
                        continue;
                    }

                    ptr = strtok(buffer, " ");

                    if (strcmp(ptr, "video")) {
                        close(fd);
                        continue;
                    }

                    major = strtok(NULL, " ");
                    minor = strtok(NULL, " ");
                    io  = strtok(NULL, " \n");

                    if (strcmp(major, "Loopback") || strcmp(io, "Input")) {
                        close(fd);
                        continue;
                    }

                    if ((ptr = strtok(buffer, " ")) == NULL) {
                        close(fd);
                        continue;
                    }

                    tnum = atoi(minor);

                    if (tnum < low) {
                        mystrcpy(buffer, "/dev/");
                        strncat(buffer, dirp->d_name, sizeof(buffer) - strlen(buffer));
                        if ((tfd = open(buffer, O_RDWR)) >= 0) {
                            strncpy(pipepath, buffer, sizeof(pipepath));

                            if (pipe_fd >= 0)
                                close(pipe_fd);

                            pipe_fd = tfd;
                            low = tnum;
                        }
                    }
                    close(fd);
                }
            }
        }

        closedir(dir);

        if (pipe_fd >= 0)
            MOTION_LOG(NTC, TYPE_VIDEO, NO_ERRNO, "%s: Opened %s as input",
                       pipepath);
    }

    return pipe_fd;
}

/**
 * v4l_startpipe
 *
 */
static int v4l_startpipe(const char *dev_name, int width, int height, int type)
{
    int dev;
#ifdef MOTION_V4L2
    struct v4l2_capability vid_caps;
    struct v4l2_format vid_format;
#else
    struct video_picture vid_pic;
    struct video_window vid_win;
#endif

    if (!strcmp(dev_name, "-")) {
        dev = v4l_open_vidpipe();
    } else {
        dev = open(dev_name, O_RDWR);
        MOTION_LOG(NTC, TYPE_VIDEO, NO_ERRNO, "%s: Opened %s as input",
                   dev_name);
    }

    if (dev < 0) {
        MOTION_LOG(ERR, TYPE_VIDEO, SHOW_ERRNO, "%s: Opening %s as input failed",
                   dev_name);
        return -1;
    }

#ifdef MOTION_V4L2
    if (ioctl(dev, VIDIOC_QUERYCAP, &vid_caps) == -1) {
        MOTION_LOG(ERR, TYPE_VIDEO, SHOW_ERRNO, "%s: ioctl (VIDIOC_QUERYCAP)");
        return -1;
    }

    memset(&vid_format, 0, sizeof(vid_format));
    ioctl(dev, VIDIOC_G_FMT, &vid_format);

    vid_format.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
    vid_format.fmt.pix.width  = width;
    vid_format.fmt.pix.height = height;
    vid_format.fmt.pix.field  = V4L2_FIELD_NONE;
    vid_format.fmt.pix.colorspace  = V4L2_COLORSPACE_SRGB;
    vid_format.fmt.pix.pixelformat = V4L2_PIX_FMT_YUV420;

    switch(vid_format.fmt.pix.pixelformat) {
    case V4L2_PIX_FMT_YUV420:
    case V4L2_PIX_FMT_YVU420:
        vid_format.fmt.pix.bytesperline = width;
        vid_format.fmt.pix.sizeimage = ROUND_UP_4(width) * ROUND_UP_2(height);
        vid_format.fmt.pix.sizeimage += 2 * ((ROUND_UP_8(width) / 2) * (ROUND_UP_2(height) / 2));
        break;
    case V4L2_PIX_FMT_UYVY:
    case V4L2_PIX_FMT_Y41P:
    case V4L2_PIX_FMT_YUYV:
    case V4L2_PIX_FMT_YVYU:
        vid_format.fmt.pix.bytesperline = (ROUND_UP_2(width) * 2);
        vid_format.fmt.pix.sizeimage = vid_format.fmt.pix.bytesperline * height;
        break;
    default:
        MOTION_LOG(ERR, TYPE_VIDEO, SHOW_ERRNO, "%s: unable to guess correct settings for pixelformat");
        return -1;
    }

    if (ioctl(dev, VIDIOC_S_FMT, &vid_format) == -1) {
        MOTION_LOG(ERR, TYPE_VIDEO, SHOW_ERRNO, "%s: ioctl (VIDIOC_S_FMT)");
        return -1;
    }
#else
    if (ioctl(dev, VIDIOCGPICT, &vid_pic) == -1) {
        MOTION_LOG(ERR, TYPE_VIDEO, SHOW_ERRNO, "%s: ioctl (VIDIOCGPICT)");
        return -1;
    }

    vid_pic.palette = type;

    if (ioctl(dev, VIDIOCSPICT, &vid_pic) == -1) {
        MOTION_LOG(ERR, TYPE_VIDEO, SHOW_ERRNO, "%s: ioctl (VIDIOCSPICT)");
        return -1;
    }

    if (ioctl(dev, VIDIOCGWIN, &vid_win) == -1) {
        MOTION_LOG(ERR, TYPE_VIDEO, SHOW_ERRNO, "%s: ioctl (VIDIOCGWIN)");
        return -1;
    }

    vid_win.height = height;
    vid_win.width = width;

    if (ioctl(dev, VIDIOCSWIN, &vid_win) == -1) {
        MOTION_LOG(ERR, TYPE_VIDEO, SHOW_ERRNO, "%s: ioctl (VIDIOCSWIN)");
        return -1;
    }
#endif
    return dev;
}

/**
 * v4l_putpipe
 *
 */
static int v4l_putpipe(int dev, unsigned char *image, int size)
{
    return write(dev, image, size);
}

/**
 * vid_startpipe
 *
 */
int vid_startpipe(const char *dev_name, int width, int height, int type)
{
    return v4l_startpipe(dev_name, width, height, type);
}

/**
 * vid_putpipe
 *
 */
int vid_putpipe (int dev, unsigned char *image, int size)
{
    return v4l_putpipe(dev, image, size);
}
#endif /* !WITHOUT_V4L */
