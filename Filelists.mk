# Directories
EXAMPLE_STACK_DIR = $(EXAMPLES_DIR)/two_nodes/stack
EXAMPLES_DIR = $(MAX22X88_ROOT_DIR)/examples
PLATFORM_DIR = $(MAX22X88_ROOT_DIR)/src/platform

# Protocol stack
EXAMPLE_STACK_INC = $(EXAMPLE_STACK_DIR)/inc
EXAMPLE_STACK_SRCS = $(EXAMPLE_STACK_DIR)/src/homebus.c

# Core driver
MAX22X88_INC = $(MAX22X88_ROOT_DIR)/inc
MAX22X88_SRCS = $(MAX22X88_ROOT_DIR)/src/bitbang_helper.c \
	$(MAX22X88_ROOT_DIR)/src/fifo.c \
	$(MAX22X88_ROOT_DIR)/src/max22x88.c

# Bitbang IO layer driver implementation
MAX22X88_BITBANG_INC = $(MAX22X88_ROOT_DIR)/inc
MAX22X88_BITBANG_SRCS = $(MAX22X88_ROOT_DIR)/src/max22x88_bitbang.c \
	$(MAX22X88_ROOT_DIR)/src/max22x88_common.c \
	$(MAX22X88_ROOT_DIR)/src/max22x88_bitbang_rx_state_machine.c

# Protocol stack <-> driver integration
INTEGRATION_MAX22X88_SRCS = $(EXAMPLE_STACK_DIR)/integration/max22x88/homebus_max22x88.c
INTEGRATION_MAX22X88_INC = $(EXAMPLE_STACK_DIR)/integration/max22x88

# Driver HAL implementation for MAX32670
MAX22X88_HAL_MAX32670_INC = $(PLATFORM_DIR)/hal/max32670
MAX22X88_HAL_MAX32670_SRCS = $(PLATFORM_DIR)/hal/max32670/hal.c

# Example project
EXAMPLE_STACK_MAX22X88_INC =
EXAMPLE_STACK_MAX22X88_SRCS = $(EXAMPLES_DIR)/two_nodes/main.c
