#
#=====================================================================================
#  Filename:    Makefile
#  Description: This is top Makefile, it uses with src dir' Makefiles and mk's Makefile
#         	functions.
#  Usage:       Add the code dir that you want to compile to TARGET_DIRS value below,
#               then open an shell and change dir to top Makefile's dir and key make.
#  Version:     2.0
#  Created:     11/05/2015 11:27:03 PM
#  Author:      alan lin , a1an1in@sina.com
#  Company:     vigor
#=====================================================================================
#
#TARGET file name
TARGET = test
SHARE_TARGET = libcutils.so
STATIC_TARGET = libcutils.a

export MACROS+=$(MACRO)
export PROJECT_PATH = $(shell pwd)
export INCLUDES = -I $(PROJECT_PATH)/src/include

#compile tools
#export CC_PATH = /home/alan/workspace/HOS-R30/HOS-R30/staging_dir/toolchain-mips_34kc_gcc-5.2.0_uClibc-0.9.33.2/bin
#export STAGING_DIR=$(CC_PATH):$STAGING_DIR
#export CC = $(CC_PATH)/mips-openwrt-linux-uclibc-gcc
#export LD = $(CC_PATH)/mips-openwrt-linux-uclibc-ld

export CC_PATH = /home/alan/workspace/HOS-R30/HOS-R30/staging_dir/toolchain-arm_cortex-a7_gcc-5.2.0_uClibc-1.0.14_eabi/bin
export STAGING_DIR=$(CC_PATH):$STAGING_DIR
export CC = $(CC_PATH)/arm-openwrt-linux-gcc
export LD = $(CC_PATH)/arm-openwrt-linux-ld

#export CC = gcc
#export LD = ld

#include makefile funtions
include ./mk/Makefile.funcs

#there can't be blank space after '\' character.
all :
	@$(call build_obj);\
	$(call check_output_dir);\
	$(CC) -o $(PROJECT_PATH)/bin/$(TARGET) $$BUILD_IN_FILES -lpthread -lm -lrt;\
	if [ $$? -ne 0 ]; then \
		echo "Building $(TARGET) is failed !"; \
		exit 12; \
	else\
		echo $(CC) -o $(PROJECT_PATH)/bin/$(TARGET) $$BUILD_IN_FILES -lpthread ;\
		echo ;\
		echo $(TARGET) has been created!;\
		echo ;\
	fi;
clean:
	$(call make_clean)
	rm -rf bin/$(TARGET)
cp:
	make
	cp -rf bin/$(TARGET) /var/ftp
so:
	@$(call build_obj);\
	$(call check_output_dir);\
	$(CC) -shared -o $(PROJECT_PATH)/bin/$(SHARE_TARGET) $$BUILD_IN_FILES -lpthread -lrt ;\
	if [ $$? -ne 0 ]; then \
		echo "Building $(SHARE_TARGET) is failed !"; \
		exit 12; \
	else\
		echo $(CC) -shared -o $(PROJECT_PATH)/bin/$(SHARE_TARGET) $$BUILD_IN_FILES -lpthread -lm;\
		echo ;\
		echo $(TARGET) has been created!;\
		echo ;\
	fi;
ar:
	@$(call build_obj);\
	$(call check_output_dir);\
	$(AR) cqs  $(PROJECT_PATH)/bin/$(STATIC_TARGET) $$BUILD_IN_FILES;\
	if [ $$? -ne 0 ]; then \
		echo "Building $(STATIC_TARGET) is failed !"; \
		exit 12; \
	else\
		echo $(AR) cqs  $(PROJECT_PATH)/bin/$(STATIC_TARGET) $$BUILD_IN_FILES;\
		echo ;\
		echo $(TARGET) has been created!;\
		echo ;\
	fi;
install:
	mkdir -p /usr/local/include/libcutils
	cp -rf ./src/include/* /usr/local/include/libcutils/
	cp -rf $(PROJECT_PATH)/bin/$(SHARE_TARGET) /usr/local/lib

