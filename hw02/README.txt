Name: Noah Le
BlazerId: nhle
Project #: HW 2

========================================
Compilation Instructions:
========================================
To compile the program, go to the directory containing 
the source files and run the command:
$ make

This will execute the Makefile and produce an executable named 'searchProgram'.
To clean the directory of executable files, run:
$ make clean

========================================
Execution Instructions:
========================================
The general syntax to run the program is:
./searchProgram [-S] [-s size] [-f pattern depth] [-t f|d] [start_directory]

Examples:
- List all files in the current directory:
  $ ./searchProgram

- List files in a specific directory:
  $ ./searchProgram ../projects

- List files showing attributes (size, perms, access time):
  $ ./searchProgram -S

- List files <= 1024 bytes:
  $ ./searchProgram -s 1024

- List files containing "docx" with depth <= 2:
  $ ./searchProgram -f docx 2

- Chain multiple options together:
  $ ./searchProgram -S -s 1024 -f jpg 1 -t f ../projects

========================================
Testing Performed:
========================================
The program was tested in a WSL Linux environment using the given projects.tar structure
in the HW2 assignment.

Tests executed:
1. Default traversal verified.
2. Used -S to ensure proper extraction of size, permissions, and access time.
3. Tested -s 100 with varying file sizes to ensure conditions were met.
4. Tested -f pattern and depth constraints, including the 0-depth edge case.
5. Evaluated multiple chained options (for example: ./searchProgram -S -s 1024 -f .c 2 projects) to ensure 
   getopt() processed arguments correctly regardless of order and applied compound AND logic.
6. Tested the graduate requirements (-t f and -t d) successfully omitted directories and files respectively.