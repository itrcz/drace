TARGET              = firmware
MCU                 = cortex-m0plus
F_CPU               = 48000000L
USB_VID             = 0x2a03
USB_PID             = 0x804f
USB_PRODUCT         = "Drace Performance Box"
USB_MANUFACTURER    = "AweSoftware"

SERIAL_PORT		= /dev/tty.usbmodem00*

# Compiler & Linker
CC			= ARM/gcc-arm-none-eabi-4.8.3-2014q1/bin/arm-none-eabi-gcc
CXX			= ARM/gcc-arm-none-eabi-4.8.3-2014q1/bin/arm-none-eabi-g++
AR			= ARM/gcc-arm-none-eabi-4.8.3-2014q1/bin/arm-none-eabi-ar
COPY		= ARM/gcc-arm-none-eabi-4.8.3-2014q1/bin/arm-none-eabi-objcopy
SIZE		= ARM/gcc-arm-none-eabi-4.8.3-2014q1/bin/arm-none-eabi-size
DUDE		= ARM/avrdude
DUDE_CFG	= ARM/avrdude.conf

LD_SCRIPT	= ARM/arduino/variants/arduino_zero/linker_scripts/gcc/flash_with_bootloader.ld

ARDUINO_FLAGS = -DARDUINO=107010 -DARDUINO_SAM_ZERO -DARDUINO_ARCH_SAMD -D__SAMD21G18A__


# Необходимые библиотеки для ядра
CORE_LIBS = -I ARM/CMSIS/CMSIS/Include/ -I ARM/ATMEL/ -I ARM/arduino -I ARM/arduino/variants/arduino_zero
USER_LIBS =

# Флаги для C
CFLAGS = -c -g -Os -w -ffunction-sections -fdata-sections -nostdlib --param max-inline-insns-single=500 -Dprintf=iprintf -mcpu=$(MCU) -DF_CPU=$(F_CPU) $(ARDUINO_FLAGS) -mthumb -DUSB_VID=$(USB_VID) -DUSB_PID=$(USB_PID) -DUSBCON '-DUSB_MANUFACTURER=$(USB_MANUFACTURER)' '-DUSB_PRODUCT=$(USB_PRODUCT)'
# Флаги для C++
CXXFLAGS = -c -g -Os -w -std=c++11 -ffunction-sections -fdata-sections -nostdlib --param max-inline-insns-single=500 -fno-rtti -fno-exceptions -Dprintf=iprintf -mcpu=$(MCU) -DF_CPU=$(F_CPU) $(ARDUINO_FLAGS) -mthumb -DUSB_VID=$(USB_VID) -DUSB_PID=$(USB_PID) '-DUSB_MANUFACTURER=$(USB_MANUFACTURER)' '-DUSB_PRODUCT=$(USB_PRODUCT)'


all: build upload

mkdir:
	@mkdir -p build/core
	@mkdir -p build/libraries

#
#	Компиляция ядра Arduino
#
build_core: $(patsubst ARM/arduino/%.c, build/core/%.c.o, $(wildcard ARM/arduino/*.c)) $(patsubst ARM/arduino/%.cpp, build/core/%.cpp.o, $(wildcard ARM/arduino/*.cpp))

	$(CC) $(CFLAGS) $(CORE_LIBS) ARM/arduino/avr/dtostrf.c -o build/core/dtostrf.c.o
	$(CC) $(CFLAGS) $(CORE_LIBS) ARM/arduino/USB/samd21_host.c -o build/core/samd21_host.c.o
	$(CXX) $(CXXFLAGS) $(CORE_LIBS) ARM/arduino/USB/CDC.cpp -o build/core/CDC.cpp.o
	$(CXX) $(CXXFLAGS) $(CORE_LIBS) ARM/arduino/USB/HID.cpp -o build/core/HID.cpp.o
	$(CXX) $(CXXFLAGS) $(CORE_LIBS) ARM/arduino/USB/USBCore.cpp -o build/core/USBCore.cpp.o

	$(AR) rcs build/core.a build/core/dtostrf.c.o
	$(AR) rcs build/core.a build/core/samd21_host.c.o
	$(AR) rcs build/core.a build/core/CDC.cpp.o
	$(AR) rcs build/core.a build/core/HID.cpp.o
	$(AR) rcs build/core.a build/core/USBCore.cpp.o

build/core/%.c.o: ARM/arduino/%.c
	$(CC) $(CFLAGS) $(CORE_LIBS) $^ -o $@
	$(AR) rcs build/core.a $@
build/core/%.cpp.o: ARM/arduino/%.cpp
	$(CXX) $(CXXFLAGS) $(CORE_LIBS) $^ -o $@
	$(AR) rcs build/core.a $@



build: mkdir build_core
	# Компилируем файлы прошивки
	$(CXX) $(CXXFLAGS) $(CORE_LIBS) ARM/arduino/variants/arduino_zero/variant.cpp -o build/variant.cpp.o

	# main.cpp
	$(CXX) $(CXXFLAGS) $(CORE_LIBS) src/main.cpp -o build/main.cpp.o
	$(AR) rcs build/program.a build/main.cpp.o

	$(CXX) $(CXXFLAGS) $(CORE_LIBS) src/State.cpp -o build/State.cpp.o
	$(AR) rcs build/program.a build/State.cpp.o

	$(CXX) $(CXXFLAGS) $(CORE_LIBS) src/Gyro.cpp -o build/Gyro.cpp.o
	$(AR) rcs build/program.a build/Gyro.cpp.o

	$(CXX) $(CXXFLAGS) $(CORE_LIBS) src/Accel.cpp -o build/Accel.cpp.o
	$(AR) rcs build/program.a build/Accel.cpp.o


	$(CXX) $(CXXFLAGS) $(CORE_LIBS) src/GPS.cpp -o build/GPS.cpp.o
	$(AR) rcs build/program.a build/GPS.cpp.o

	$(CXX) $(CXXFLAGS) $(CORE_LIBS) src/UI.cpp -o build/UI.cpp.o
	$(AR) rcs build/program.a build/UI.cpp.o

	#Wire library
	$(CXX) $(CXXFLAGS) $(CORE_LIBS) src/Wire/Wire.cpp -o build/Wire.cpp.o
	$(AR) rcs build/program.a build/Wire.cpp.o

	$(CXX) $(CXXFLAGS) $(CORE_LIBS) src/display/Adafruit_GFX.cpp -o build/Adafruit_GFX.cpp.o
	$(CXX) $(CXXFLAGS) $(CORE_LIBS) src/display/ST7565.cpp -o build/ST7565.cpp.o
	$(CC) $(CFLAGS) $(CORE_LIBS) src/display/glcdfont.c -o build/glcdfont.c.o
	$(AR) rcs build/program.a build/Adafruit_GFX.cpp.o
	$(AR) rcs build/program.a build/ST7565.cpp.o
	$(AR) rcs build/program.a build/glcdfont.c.o



	# Линкуем файлы в .elf
	$(CXX) -Os -Wl,--gc-sections -save-temps -mcpu=$(MCU) -T $(LD_SCRIPT) -Wl,-Map,build/$(TARGET).map -o $(TARGET).elf --specs=nano.specs -L build/ -Wl,--start-group -lm -lgcc -Wl,--end-group -mthumb -Wl,--cref -Wl,--check-sections -Wl,--gc-sections -Wl,--entry=Reset_Handler -Wl,--unresolved-symbols=report-all -Wl,--warn-common -Wl,--warn-section-align -Wl,--warn-unresolved-symbols -Wl,--start-group build/core/syscalls.c.o build/program.a build/variant.cpp.o build/core.a -Wl,--end-group -Wl,--section-start=.text=0x4000

	# Создаем .epp
	$(COPY) -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 $(TARGET).elf $(TARGET).eep

	# Создаем .hex
	$(COPY) -O ihex -R .eeprom $(TARGET).elf $(TARGET).hex

	# Создаем .bin
	$(COPY) -O binary $(TARGET).elf $(TARGET).bin

size:
	# Размер прошивки
	$(eval PROGRAM_SIZE := $(shell $(SIZE) $(TARGET).elf))


#@osascript -e 'tell application "System Events" to display dialog "$(PROGRAM_SIZE)" buttons {"UPLOAD"} default button {"UPLOAD"} with title "FIRMWARE"'

upload: size
	# Зашиваем через Serial
	$(DUDE) -C $(DUDE_CFG) -v -patmega2560 -c stk500v2 -P $(SERIAL_PORT) -b 57600 -U flash:w:$(TARGET).hex:i

clean:
	@rm -Rf build
	@rm -Rf firmware.elf firmware.hex firmware.eep firmware.bin

