CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

SET(PROJECT_NAME brickletboot)
SET(CHIP_FAMILY SAM0)
SET(CHIP SAMD09D14A)
SET(MCU cortex-m0plus\ -mthumb)
PROJECT(${PROJECT_NAME})

# Optimization level, can be [0, 1, 2, 3, s]. 
# 0 = turn off optimization. s = optimize for size.
#SET(OPTIMIZATION_LEVEL 0)
SET(DEBUG "-g -ggdb")

SET(OPTIMIZATION_LEVEL s)
#SET(DEBUG "")

SET(CMAKE_BUILD_TYPE None)

INCLUDE_DIRECTORIES(
	"${PROJECT_SOURCE_DIR}/src/"
	"${PROJECT_SOURCE_DIR}/src/bricklib2/hal/asf_includes"
	"${PROJECT_SOURCE_DIR}/src/bricklib2/asf/thirdparty/CMSIS/Include/"
)

# find source files
SET(SOURCES
	"${PROJECT_SOURCE_DIR}/src/main.c"
	"${PROJECT_SOURCE_DIR}/src/bootloader_spitfp.c"
	"${PROJECT_SOURCE_DIR}/src/tfp_common.c"
	"${PROJECT_SOURCE_DIR}/src/boot.c"
	"${PROJECT_SOURCE_DIR}/src/firmware_entry.c"
#	"${PROJECT_SOURCE_DIR}/src/temperature.c"

	"${PROJECT_SOURCE_DIR}/src/bricklib2/hal/startup/startup_samd09.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib2/bootloader/tinydma.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib2/protocols/tfp/tfp.c"
#	"${PROJECT_SOURCE_DIR}/src/bricklib2/logging/logging.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib2/utility/ringbuffer.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib2/utility/pearson_hash.c"

	"${PROJECT_SOURCE_DIR}/src/bricklib2/asf/sam0/drivers/wdt/wdt.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib2/asf/sam0/drivers/nvm/nvm.c"
#	"${PROJECT_SOURCE_DIR}/src/bricklib2/asf/sam0/drivers/adc/adc_sam_d_r/adc.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib2/asf/sam0/drivers/dsu/crc32/crc32.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib2/asf/sam0/drivers/system/clock/clock_samd09_d10_d11/clock.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib2/asf/sam0/drivers/system/clock/clock_samd09_d10_d11/gclk.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib2/asf/sam0/drivers/system/interrupt/system_interrupt.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib2/asf/sam0/drivers/system/pinmux/pinmux.c"
#	"${PROJECT_SOURCE_DIR}/src/bricklib2/asf/sam0/drivers/sercom/usart/usart.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib2/asf/sam0/drivers/sercom/sercom.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib2/asf/sam0/drivers/sercom/spi/spi.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib2/asf/sam0/utils/syscalls/gcc/syscalls.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib2/asf_overwrite/common/utils/interrupt/interrupt_sam_nvic.c"
#	"${PROJECT_SOURCE_DIR}/src/bricklib2/asf/common/utils/stdio/write.c"
#	"${PROJECT_SOURCE_DIR}/src/bricklib2/asf/common/utils/stdio/read.c"
)

MESSAGE(STATUS "\nFound following source files:\n ${SOURCES}\n")

# define executable
ADD_EXECUTABLE(${PROJECT_NAME}.elf ${SOURCES})
SET_TARGET_PROPERTIES(${PROJECT_NAME}.elf PROPERTIES LINKER_LANGUAGE C)

TARGET_LINK_LIBRARIES(${PROJECT_NAME}.elf -lc_nano)

# define compile flags
SET_TARGET_PROPERTIES(${PROJECT_NAME}.elf PROPERTIES COMPILE_FLAGS
	"${DEBUG} -mcpu=${MCU} -std=gnu99  -Wall -mlong-calls -ffunction-sections -O${OPTIMIZATION_LEVEL}"
)

#define linker flags
SET_TARGET_PROPERTIES(${PROJECT_NAME}.elf PROPERTIES LINK_FLAGS
	"-mcpu=${MCU} -Wl,--gc-sections -T\"${PROJECT_SOURCE_DIR}/src/bricklib2/linker_script/samd09_brickletboot.ld\" "
)

ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME}.elf POST_BUILD COMMAND 
                   ${CMAKE_OBJCOPY} -S -O binary 
                   ${PROJECT_NAME}.elf ${PROJECT_NAME}.bin)

ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME}.elf POST_BUILD COMMAND 
                   ${CMAKE_SIZE} 
                   ${OBJECT} ${PROJECT_NAME}.elf)
                   
ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME}.elf POST_BUILD COMMAND 
                   ${PROJECT_SOURCE_DIR}/src/bricklib2/bootloader/crc32.py
                   ${PROJECT_NAME}.bin > ${PROJECT_NAME}-checksum.bin)
                   
ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME}.elf POST_BUILD COMMAND 
                   cat
                   ${PROJECT_NAME}.bin ${PROJECT_NAME}-checksum.bin > ${PROJECT_NAME}-with-checksum.bin)

# add preprocessor defines
ADD_DEFINITIONS(-D__${CHIP}__ -D__${CHIP_FAMILY}__ -Dflash -Dprintf=iprintf)
 
if(EXISTS "${PROJECT_SOURCE_DIR}/src/configs/config_custom_bootloader.h")
ADD_DEFINITIONS(-DUSE_CUSTOM_BOOTLOADER)
endif()