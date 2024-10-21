# This file can be used to set build configuration
# variables.  These variables are defined in a file called 
# "Makefile" that is located next to this one.

# For instructions on how to use this system, see
# https://github.com/Analog-Devices-MSDK/VSCode-Maxim/tree/develop#build-configuration

# **********************************************************
MAX22X88_ROOT_DIR = ../..
include $(MAX22X88_ROOT_DIR)/Filelists.mk

IPATH += $(MAX22X88_HAL_MAX32670_INC)
SRCS += $(MAX22X88_HAL_MAX32670_SRCS)

IPATH += $(MAX22X88_INC)
SRCS += $(MAX22X88_SRCS)

IPATH += $(EXAMPLE_STACK_INC)
SRCS += $(EXAMPLE_STACK_SRCS)

IPATH += $(INTEGRATION_MAX22X88_INC)
SRCS += $(INTEGRATION_MAX22X88_SRCS)

IPATH += $(MAX22X88_BITBANG_INC)
SRCS += $(MAX22X88_BITBANG_SRCS)

IPATH += $(EXAMPLE_STACK_MAX22X88_INC)
SRCS += $(EXAMPLE_STACK_MAX22X88_SRCS)
