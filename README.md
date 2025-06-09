# Pseudo Operating System

This project implements a pseudo-operating system in pure C, designed for educational purposes. The system is structured to allow collaborative development and includes modules for process management, memory management, I/O resource management, and file system operations.

## Project Structure

The project is organized into several directories:

- **src/**: Contains the source code for the operating system.
  - **core/**: Core functionalities including the main entry point and system-wide definitions.
  - **process/**: Manages processes, including scheduling and dispatching.
  - **memory/**: Handles memory allocation and I/O resource management.
  - **filesystem/**: Manages file system operations and file handling.

- **include/**: Contains shared header files with constants and error definitions.

- **tests/**: Contains unit tests for each module to ensure functionality and reliability.

- **examples/**: Provides example input files for testing the system.

- **Makefile**: Contains build instructions for compiling the project.

- **.gitignore**: Specifies files and directories to be ignored by Git.

## Building the Project

To build the project, navigate to the root directory of the project and run:

```
make
```

This will compile all modules and create the executable for the pseudo-operating system.

## Running the System

After building the project, you can run the system using the following command:

```
./pseudo-os
```

Make sure to provide the necessary input files (`processes.txt` and `files.txt`) in the appropriate format as specified in the project documentation.

## Contributing

This project is open for contributions. Please follow the guidelines below:

1. Fork the repository.
2. Create a new branch for your feature or bug fix.
3. Make your changes and commit them.
4. Push your branch and create a pull request.

## License

This project is licensed under the MIT License. See the LICENSE file for more details.

## Acknowledgments

Thanks to all contributors and educators who have provided guidance and support in the development of this project.