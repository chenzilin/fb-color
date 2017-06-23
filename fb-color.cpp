#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/ioctl.h>

#ifdef INITROOT_STARTUP
#include <initroot_startup.h>
#endif

#ifdef __cplusplus
}
#endif

using namespace std;

struct fb_var_screeninfo fb_var;
struct fb_fix_screeninfo fb_fix;

struct mxcfb_gbl_alpha {
	int enable;
	int alpha;
};
#define MXCFB_SET_GBL_ALPHA     _IOW('F', 0x21, struct mxcfb_gbl_alpha)

#ifdef INITROOT_STARTUP
int startup_pre_end(void) { return 0; }
#endif

int main(int argc, char **argv)
{
#ifdef INITROOT_STARTUP
	startup_begin();
	startup_cend(startup_pre_end);
#endif

        int fd_fb;
        size_t overlay_sz;
        char *overlay_buf;
        struct mxcfb_gbl_alpha alpha;

        if (argc == 1) {
            if ((fd_fb = open("/dev/fb0", O_RDWR)) < 0) {
                fprintf(stderr, "Failed to open /dev/fb0!\n");
                return EXIT_FAILURE;
            }
        }
        else if (argc == 2) {
            if ((fd_fb = open(argv[1], O_RDWR)) < 0) {
                fprintf(stderr, "Failed to open %s!\n", argv[1]);
                return EXIT_FAILURE;
            }
        }
        else {
            fprintf(stderr, "Usage: ./splash </dev/fb0|1>\n");
            return EXIT_FAILURE;
        }

        ioctl(fd_fb, FBIOGET_VSCREENINFO, &fb_var);
        fprintf(stdout, "RES: %d, %d\n", fb_var.xres, fb_var.yres);

        overlay_sz = fb_var.xres * fb_var.yres * 4;
        fprintf(stdout, "Screen Size: %zu\n", overlay_sz);

        overlay_buf = (char *)mmap(0, overlay_sz, PROT_READ | PROT_WRITE,MAP_SHARED, fd_fb, 0);
        fprintf(stdout, "SharedMem Ptr: %p\n", overlay_buf);

        // first clear FB
        memset(overlay_buf, 0x00, overlay_sz);

        // Disable global alpha since we need Pixel Alpha
        alpha.enable = 0;
        alpha.alpha = 0xff;
        ioctl(fd_fb, MXCFB_SET_GBL_ALPHA, &alpha);

        // enable FB
        ioctl(fd_fb, FBIOBLANK, FB_BLANK_UNBLANK);

        do {
            // draw red
            for (size_t i = 0; i < overlay_sz; i+=4) {
                overlay_buf[i]= 0x00;
                overlay_buf[i+1] = 0x00;
                overlay_buf[i+2] = 0xff;
                overlay_buf[i+3] = 0xff;
            }
            sleep(1);
            // draw green
            for (size_t i = 0; i < overlay_sz; i+=4) {
                overlay_buf[i]= 0x00;
                overlay_buf[i+1] = 0xff;
                overlay_buf[i+2] = 0x00;
                overlay_buf[i+3] = 0xff;
            }
            sleep(1);
            // draw blue
            for (size_t i = 0; i < overlay_sz; i+=4) {
                overlay_buf[i]= 0xff;
                overlay_buf[i+1] = 0x00;
                overlay_buf[i+2] = 0x00;
                overlay_buf[i+3] = 0xff;
            }
            sleep(1);
            // draw black
            for (size_t i = 0; i < overlay_sz; i+=4) {
                overlay_buf[i]= 0x00;
                overlay_buf[i+1] = 0x00;
                overlay_buf[i+2] = 0x00;
                overlay_buf[i+3] = 0xff;
            }
            sleep(1);
            // draw white
            for (size_t i = 0; i < overlay_sz; i+=4) {
                overlay_buf[i]= 0xff;
                overlay_buf[i+1] = 0xff;
                overlay_buf[i+2] = 0xff;
                overlay_buf[i+3] = 0xff;
            }
            sleep(1);
            // row Rainbow
            for (size_t i = 0; i < fb_var.xres; i++) {
                for (size_t j = 0; j < fb_var.yres; j++) {
                    int index = (int)(i / 288);
                    /*
                     * 列         数字下标偏移量
                     * i   +    j * fb_var.yres * 4
                     */
                    switch (index) {
                    case 0:
                        overlay_buf[(i + j * fb_var.xres) * 4] = 0xff;
                        overlay_buf[(i + j * fb_var.xres) * 4 + 1] = 0xff;
                        overlay_buf[(i + j * fb_var.xres) * 4 + 2] = 0xff;
                        overlay_buf[(i + j * fb_var.xres) * 4 + 3] = 0xff;
                        break;
                    case 1:
                        overlay_buf[(i + j * fb_var.xres) * 4] = 0x00;
                        overlay_buf[(i + j * fb_var.xres) * 4 + 1] = 0x00;
                        overlay_buf[(i + j * fb_var.xres) * 4 + 2] = 0xff;
                        overlay_buf[(i + j * fb_var.xres) * 4 + 3] = 0xff;
                        break;
                    case 2:
                        overlay_buf[(i + j * fb_var.xres) * 4] = 0x00;
                        overlay_buf[(i + j * fb_var.xres) * 4 + 1] = 0xff;
                        overlay_buf[(i + j * fb_var.xres) * 4 + 2] = 0x00;
                        overlay_buf[(i + j * fb_var.xres) * 4 + 3] = 0xff;
                        break;
                    case 3:
                        overlay_buf[(i + j * fb_var.xres) * 4] = 0xff;
                        overlay_buf[(i + j * fb_var.xres) * 4 + 1] = 0x00;
                        overlay_buf[(i + j * fb_var.xres) * 4 + 2] = 0x00;
                        overlay_buf[(i + j * fb_var.xres) * 4 + 3] = 0xff;
                        break;
                    case 4:
                        overlay_buf[(i + j * fb_var.xres) * 4] = 0x00;
                        overlay_buf[(i + j * fb_var.xres) * 4 + 1] = 0x00;
                        overlay_buf[(i + j * fb_var.xres) * 4 + 2] = 0x00;
                        overlay_buf[(i + j * fb_var.xres) * 4 + 3] = 0xff;
                        break;
                    default:
                        break;
                    }
                }
            }
            sleep(1);
            // column Rainbow
            for (size_t i = 0; i < fb_var.xres; i++) {
                for (size_t j = 0; j < fb_var.yres; j++) {
                    int index = (int)(j / 108);
                    switch (index) {
                    case 0:
                        overlay_buf[(i + j * fb_var.xres) * 4] = 0xff;
                        overlay_buf[(i + j * fb_var.xres) * 4 + 1] = 0xff;
                        overlay_buf[(i + j * fb_var.xres) * 4 + 2] = 0xff;
                        overlay_buf[(i + j * fb_var.xres) * 4 + 3] = 0xff;
                        break;
                    case 1:
                        overlay_buf[(i + j * fb_var.xres) * 4] = 0x00;
                        overlay_buf[(i + j * fb_var.xres) * 4 + 1] = 0x00;
                        overlay_buf[(i + j * fb_var.xres) * 4 + 2] = 0xff;
                        overlay_buf[(i + j * fb_var.xres) * 4 + 3] = 0xff;
                        break;
                    case 2:
                        overlay_buf[(i + j * fb_var.xres) * 4] = 0x00;
                        overlay_buf[(i + j * fb_var.xres) * 4 + 1] = 0xff;
                        overlay_buf[(i + j * fb_var.xres) * 4 + 2] = 0x00;
                        overlay_buf[(i + j * fb_var.xres) * 4 + 3] = 0xff;
                        break;
                    case 3:
                        overlay_buf[(i + j * fb_var.xres) * 4] = 0xff;
                        overlay_buf[(i + j * fb_var.xres) * 4 + 1] = 0x00;
                        overlay_buf[(i + j * fb_var.xres) * 4 + 2] = 0x00;
                        overlay_buf[(i + j * fb_var.xres) * 4 + 3] = 0xff;
                        break;
                    case 4:
                        overlay_buf[(i + j * fb_var.xres) * 4] = 0x00;
                        overlay_buf[(i + j * fb_var.xres) * 4 + 1] = 0x00;
                        overlay_buf[(i + j * fb_var.xres) * 4 + 2] = 0x00;
                        overlay_buf[(i + j * fb_var.xres) * 4 + 3] = 0xff;
                        break;
                    default:
                        break;
                    }
                }
            }
            sleep(1);
        } while (true);

	// disable FB
//	ioctl(fd_fb, FBIOBLANK, FB_BLANK_POWERDOWN);
	munmap(overlay_buf, overlay_sz);

	close(fd_fb);

#ifndef INITROOT_STARTUP
	return EXIT_SUCCESS;
#else
	startup_end(EXIT_SUCCESS);
#endif
}
