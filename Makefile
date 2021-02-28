all:
	make -C libavutil
	make -C libavformat
	make -C libavcodec
	rm -f pmpmod/PMPMOD.elf
	rm -f pmpmod/PARAM.SFO
	rm -f pmpmod/EBOOT.PBP
	make -C pmpmod
	make -C pmpmod kxploit

clean:
	make -C libavutil clean
	make -C libavformat clean
	make -C libavcodec clean
	make -C pmpmod clean
	rm -f -r pmpmod/PMPMOD
	rm -f -r pmpmod/PMPMOD%
