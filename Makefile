simonSays: simonSays.c
	./avrgcc/bin/avr-gcc.exe -c -mmcu=attiny261a random.c
	./avrgcc/bin/avr-gcc.exe -c -mmcu=attiny261a simonSays.c
	./avrgcc/bin/avr-gcc.exe -mmcu=attiny261a simonSays.o random.O
	./avrgcc/bin/avr-objcopy.exe -O ihex -j .text -j .text a.out a.hex
	./avrdude-v7.1-windows-x64/avrdude.exe -C avrdude-v7.1-windows-x64/avrdude.conf -v -p attiny261a -c stk500v1 -P COM4 -b 19200 -U flash:w:a.hex:i

clean:
	del a.hex
	del a.out
	del simonSays.o
	del random.o
