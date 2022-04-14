VPATH = src:Output:inc

OutputPath 	= ./Output
SrcPath		= ./src
IncPath		= ./inc

target = pidtest.hex
object = pidtest.ihx
rel = $(patsubst %.c, $(OutputPath)/%.rel, $(notdir $(wildcard $(SrcPath)/*.c)))
#arm-none-eabi-nm --print-size --size-sort --radix=d build/ice_fw.elf
CC = sdcc
HEX = packihx
CFLAGS = -I "$(IncPath)/" -o "$(OutputPath)/" -c

$(target): $(object)
	$(HEX) "$(OutputPath)/$(object)" > "$(OutputPath)/$(target)"

$(object): $(rel)
	$(CC) $(rel) -o "$(OutputPath)/$(object)"

$(OutputPath)/%.rel:$(SrcPath)/%.c
	$(CC) $(CFLAGS) $<

.PHONY:
clean:
	del /Q .\Output\