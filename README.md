# Overview

This project provides the the software drivers for the MAX22088 and MAX22288 Home Bus System comptible transceivers.

## Tools

To take full advantage of this project, the following tools are required:

- [Analog Devices MSDK February 2024 Release](https://github.com/analogdevicesinc/msdk/releases/tag/v2024_02)
- [VSCode-Maxim](https://github.com/analogdevicesinc/VSCode-Maxim) Visual Studio Code extension
  - See the [Installation](https://github.com/analogdevicesinc/VSCode-Maxim#installation) section
- [Doxygen](https://www.doxygen.nl/)

## Project Structure

- .vscode/ - Visual Studio Code IDE files for building example projects
- doc/ - API documentation
- examples/ - Example projects
- inc/ - Driver headers
- src/ - Driver sources

## Generating Documentation

This project uses Doxygen for generating documentation.

To generate the API Reference Manual, run Doxygen with the `doc/Doxyfile` configuration file.
That can be done by `cd`ing into the `doc` folder and running the `doxygen` command.
The output will be generated at `doc/output`.

## Make integration

`Filelists.mk` provides a way to integrate the driver and protocol stack into Make projects. It provides variables with the include paths and source files that the user needs to add to their project. These variables will be relative to the variable `MAX22X88_ROOT_DIR`, which needs to be set by the user.

The following variables will be set in the parent Makefile:

- `EXAMPLE_STACK_INC`: Include paths required for the Home Bus protocol stack
- `EXAMPLE_STACK_SRCS`: Source files required for the Home Bus protocol stack
- `MAX22X88_INC`: Include paths required for the Max22x88 driver
- `MAX22X88_SRCS`: Source files required for the Max2xx88 driver
- `MAX22X88_BITBANG_INC`: Include paths required for the bitbang driver implementation
- `MAX22X88_BITBANG_SRCS`: Source files required for the bitbang driver implementation
- `INTEGRATION_MAX22X88_INC`: Include paths required for the driver/stack integration
- `INTEGRATION_MAX22X88_SRCS`: Source files required for the driver/stack integration
- `MAX22X88_HAL_MAX32670_INC`: Include paths required for the Max32670 HAL implementation for the Max22x88 driver
- `MAX22X88_HAL_MAX32670_SRCS`: Source files required for the Max32670 HAL implementation for the Max22x88 driver
- `EXAMPLE_STACK_MAX22X88_INC`: Include paths required for the example project
- `EXAMPLE_STACK_MAX22X88_SRCS`: Source files required for the example project

The user sets the variable `MAX22X88_ROOT_DIR` to the directory of this repository folder, and includes `Filelists.mk` in their project's Makefile.

``` make
MAX22X88_ROOT_DIR = path/to/root/dir
include $(MAX22X88_ROOT_DIR)/Filelists.mk
```

The variables will be prefixed with "path/to/root/dir", such as
`$(EXAMPLE_STACK_INC)` would evaluate to "path/to/root/dir/stack/inc".

See [project.mk](examples/two_nodes/project.mk) for a concrete example.

## Running the example project

The example project is based on a MSDK project and is integrated with Visual Studio Code.

1. Open the root repository folder in Visual Studio Code
2. Execute the command `Tasks: Run Build Task (CTRL + SHIFT + B)`
3. Select one of the build tasks

The tasks `flash` and `flash & run` can be selected to rebuild the example project and flash it to the connected board.
Because these tasks depend on the `build` task, the user will be prompted to select a configuration.

When the `build` runs, the user is prompted to choose between a `Debug` or `Release` configuration:

- `Debug` passes `DEBUG=1 MXC_OPTIMIZE_CFLAGS=-O0` to `Make`, overriding the default `MXC_OPTIMIZE_CFLAGS=-Og`.
- `Release` passes a target `release` to `Make`.

For more details on the example project, see [README.md](examples/two_nodes/README.md)
