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
SHARE_TARGET = libcdf.so
#compile tools
CC = gcc
LD = ld
export PROJECT_PATH = $(shell pwd)
export INCLUDES = -I $(PROJECT_PATH)/src/include

#include makefile funtions
include ./mk/Makefile.funcs

#there can't be blank space after '\' character.
all :
	@$(call build_obj);\
	$(call check_output_dir);\
	$(CC) -o $(PROJECT_PATH)/bin/$(TARGET) $$BUILD_IN_FILES -lpthread -lm -Llibs -levent_tiny;\
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
	$(CC) -shared -o $(PROJECT_PATH)/bin/$(SHARE_TARGET) $$BUILD_IN_FILES -lpthread;\
	if [ $$? -ne 0 ]; then \
		echo "Building $(SHARE_TARGET) is failed !"; \
		exit 12; \
	else\
		echo $(CC) -shared -o $(PROJECT_PATH)/bin/$(SHARE_TARGET) $$BUILD_IN_FILES -lpthread -lm;\
		echo ;\
		echo $(TARGET) has been created!;\
		echo ;\
	fi;
install:
	cp -rf ./src/include/ /usr/local/include/libcdf
	cp -rf $(PROJECT_PATH)/bin/$(SHARE_TARGET) /usr/local/lib

