# Project config

APP_NAME = SerialDongle
CONFIG_NAME = Coordinador_SerialDongle
LIST_PATH = $(CONFIG_NAME)/List
EXE_PATH = $(CONFIG_NAME)/Exe
OBJ_PATH = $(CONFIG_NAME)/Obj

# avrdude config

DEVICE_NO = m128rfa1
PROGRAMMER = dragon_jtag
PORT = usb

# Path to BitCloud SDK

BITCLOUD_PATH = /home/erich/avr/BitCloud_megaRF
include $(BITCLOUD_PATH)/lib/Makerules_Atmega128rfa1_Gcc


DEFINES = \
  -DATMEGA128RFA1 \
  -DHAL_8MHz \
  -DBOARD_RCB \
  -DRCB_ATMEGA128RFA1 \
  -DNO_SECURITY_MODE \
  -DSTACK_TYPE_COORDINATOR 


INCLUDES = \
  -I./ \
  -I./include \
  -I$(BITCLOUD_PATH)/Components/BSP/RCB/include \
  -I$(BITCLOUD_PATH)/lib \
  -I$(BITCLOUD_PATH)/Components/HAL/include \
  -I$(BITCLOUD_PATH)/Components/BSP \
  -I$(BITCLOUD_PATH)/Components/BSP/include \
  -I$(BITCLOUD_PATH)/Components/NWK/include \
  -I$(BITCLOUD_PATH)/Components/NWK/include/private \
  -I$(BITCLOUD_PATH)/Components/ZDO/include \
  -I$(BITCLOUD_PATH)/Components/ZDO/include/private \
  -I$(BITCLOUD_PATH)/Components/APS/include \
  -I$(BITCLOUD_PATH)/Components/APS/include/private \
  -I$(BITCLOUD_PATH)/Components/SystemEnvironment/include \
  -I$(BITCLOUD_PATH)/Components/ConfigServer/include \
  -I$(BITCLOUD_PATH)/Components/ConfigServer/include/private \
  -I$(BITCLOUD_PATH)/Components/PersistDataServer/include \
  -I$(BITCLOUD_PATH)/Components/PersistDataServer/include/private \
  -I$(BITCLOUD_PATH)/Components/Security/TrustCentre/include \
  -I$(BITCLOUD_PATH)/Components/Security/ServiceProvider/include \
  -I$(BITCLOUD_PATH)/Components/HAL/drivers/include \
  -I$(BITCLOUD_PATH)/Components/HAL/drivers/USBFIFO/include \
  -I$(BITCLOUD_PATH)/Components/HAL/avr/atmega128rfa1/common/include \
  -I$(BITCLOUD_PATH)/Components/MAC_PHY/include \
  -I$(BITCLOUD_PATH)/Components/MAC_PHY/MAC_ENV/include \
  -I$(BITCLOUD_PATH)/Components/MAC_PHY/MAC_HWI/include \
  -I$(BITCLOUD_PATH)/Components/MAC_PHY/MAC_HWD_PHY/include \
  -I$(BITCLOUD_PATH)/Components/MAC_PHY/MAC_HWD_PHY/ATMEGA128RFA1/PHY/include \
  -I$(BITCLOUD_PATH)/Components/MAC_PHY/MAC_HWD_PHY/ATMEGA128RFA1/MAC_HWD/include 


LIBS = \
  $(BITCLOUD_PATH)/lib/libHAL_RcbAtm128rfa1_Atmega128rfa1_8Mhz_Gcc.a \
  $(BITCLOUD_PATH)/lib/libBc_Coordinator_Atmega128rfa1_Atmega128rfa1_Gcc.a \
  $(BITCLOUD_PATH)/lib/WdtInitatmega128rfa1_Gcc.o 


SRCS = \
  $(BITCLOUD_PATH)/Components/PersistDataServer/src/pdsMem.c \
  $(BITCLOUD_PATH)/Components/PersistDataServer/src/pdsWriteData.c \
  $(BITCLOUD_PATH)/Components/PersistDataServer/src/pdsDataServer.c \
  $(BITCLOUD_PATH)/Components/PersistDataServer/src/pdsInit.c \
  $(BITCLOUD_PATH)/Components/PersistDataServer/src/pdsTimer.c \
  $(BITCLOUD_PATH)/Components/PersistDataServer/src/pdsEvents.c \
  $(BITCLOUD_PATH)/Components/PersistDataServer/src/pdsMemAccess.c \
  $(BITCLOUD_PATH)/Components/PersistDataServer/src/pdsCrcService.c \
  $(BITCLOUD_PATH)/Components/HAL/drivers/USBFIFO/src/usbFifoVirtualUsart.c \
  $(BITCLOUD_PATH)/Components/HAL/drivers/USBFIFO/src/usbFifoFT245RL.c \
  $(BITCLOUD_PATH)/Components/BSP/RCB/src/leds.c \
  $(BITCLOUD_PATH)/Components/BSP/RCB/src/bspTaskManager.c \
  $(BITCLOUD_PATH)/Components/BSP/RCB/src/buttons.c \
  $(BITCLOUD_PATH)/Components/BSP/RCB/src/fakeBSP.c \
  $(BITCLOUD_PATH)/Components/ConfigServer/src/csPersistentMem.c \
  $(BITCLOUD_PATH)/Components/ConfigServer/src/csMem.c \
  $(BITCLOUD_PATH)/Components/ConfigServer/src/configServer.c 

PREINCLUDE = MakerulesBc_Coordinator_Atmega128rfa1_Atmega128rfa1_Gcc.h

# Linea sospechosa
SRCS += $(wildcard ./src/*.c)

CSRCS = $(filter %.c, $(SRCS))
OBJS = $(addprefix $(OBJ_PATH)/, $(notdir %/$(subst .c,.o,$(CSRCS))))

ASM_FILE_EXT = s

ifneq (, $(findstring .$(ASM_FILE_EXT), $(SRCS)))
  ASRCS = $(filter %.$(ASM_FILE_EXT), $(SRCS))
  OBJS += $(addprefix $(OBJ_PATH)/, $(notdir %$(subst .$(ASM_FILE_EXT),.o,$(ASRCS))))
endif

CFLAGS += $(DEFINES)
CFLAGS += $(INCLUDES)
CFLAGS += -include $(PREINCLUDE)
CFLAGS += -g

ASFLAGS = $(INCLUDES)
ASFLAGS += $(filter -mmcu%,$(CFLAGS))

LD = $(CC)
LINKER_FLAGS = -Wl,-Map=$(LIST_PATH)/$(APP_NAME).map -Wl,--gc-sections
LINKER_FLAGS += -Wl,--script=linkerScr/atmega128rfa1.ld #Linea sospechosa
LINKER_FLAGS += -Wl,--section-start=.data=0x800200
LINKER_FLAGS += $(filter -mmcu%,$(CFLAGS))

.PHONY: all directories clean size root_files

all: directories $(EXE_PATH)/$(APP_NAME).elf $(EXE_PATH)/$(APP_NAME).hex $(EXE_PATH)/$(APP_NAME).srec $(EXE_PATH)/$(APP_NAME).bin root_files size

$(OBJ_PATH)/%.o: $(SRCS)
	$(CC) $(CFLAGS) $(filter %/$(subst .o,.c,$(notdir $@)), $(SRCS)) -o $@


$(EXE_PATH)/$(APP_NAME).elf: $(OBJS)
	$(LD) $(LINKER_FLAGS) $(OBJS) -Wl,-\( $(LIBS) -Wl,-\) -o $@

$(EXE_PATH)/$(APP_NAME).srec: $(EXE_PATH)/$(APP_NAME).elf
	$(OBJCOPY) -O srec --srec-len 128 $^ $@

$(EXE_PATH)/$(APP_NAME).hex: $(EXE_PATH)/$(APP_NAME).elf
	$(OBJCOPY) -O ihex -R .eeprom $^ $@

$(EXE_PATH)/$(APP_NAME).bin: $(EXE_PATH)/$(APP_NAME).elf
	$(OBJCOPY) -O binary --strip-debug --strip-unneeded -R .eeprom $^ $@

root_files:
	cp -f $(EXE_PATH)/*.* ./

clean:
	rm -rf $(CONFIG_NAME) $(APP_NAME).elf $(APP_NAME).hex $(APP_NAME).srec $(APP_NAME).bin

install:
	avrdude -p $(DEVICE_NO) -c $(PROGRAMMER) -P $(PORT) -U flash:w:$(APP_NAME).hex

prog_all: #Compila y programa
	make
	avrdude -p $(DEVICE_NO) -c $(PROGRAMMER) -P $(PORT) -U flash:w:$(APP_NAME).hex

directories:
	@mkdir -p $(LIST_PATH)
	@mkdir -p $(EXE_PATH)
	@mkdir -p $(OBJ_PATH)

size: $(EXE_PATH)/$(APP_NAME).elf
	@echo
	@$(SIZE) -td $(EXE_PATH)/$(APP_NAME).elf

# eof Makefile
