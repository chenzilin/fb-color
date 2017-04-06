	FrameBuffer颜色测试程序


	使用方法:

		compile: arm-linux-gnueabihf-g++ -std=c++11 splash.cpp -o splash

		running example: ./splash /dev/fb1


	注: 使用fbset来判断像素点r g b a的排列顺序

<pre>
root@imx6dlwisehmi-fast:~# fbset

mode "1440x540-60"
        # D: 54.151 MHz, H: 33.201 kHz, V: 59.607 Hz
        geometry 1440 540 1440 1620 32
        timings 18467 100 86 6 9 5 2
        accel false
        rgba 8/16,8/8,8/0,8/24
endmode
</pre>

	此屏幕的像素点颜色排列顺序为b g r a .
