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


int main(int argc, char **argv)
{
	int fd_fb;
	size_t overlay_sz;
	char *overlay_buf;
	struct mxcfb_gbl_alpha alpha;

	if (argc != 2) {
		fprintf(stderr, "Usage: ./splash </dev/fb0|1>\n");
		return EXIT_FAILURE;
	}

	if ((fd_fb = open(argv[1], O_RDWR)) < 0) {
		fprintf(stderr, "Failed to open %s!\n", argv[1]);
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

	// draw colors
	for (size_t i = 0; i < overlay_sz; i+=4) {
		overlay_buf[i]= 0xff;
		overlay_buf[i+1] = 0x00;
		overlay_buf[i+2] = 0x00;
		overlay_buf[i+3] = 0xff;
	}

	// disable FB
//	ioctl(fd_fb, FBIOBLANK, FB_BLANK_POWERDOWN);
	munmap(overlay_buf, overlay_sz);

	close(fd_fb);
	return EXIT_SUCCESS;
}
