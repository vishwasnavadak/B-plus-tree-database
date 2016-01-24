## Description

A simple In-memory B+ Tree database with two fields, one being the roll number which is indexed and other being the student name which is not indexed. Once completed it will support operations like Insertion, Deletion, Searching.

## Compiling

you can compile the bplustree.cpp using any of the IDE or command line compilers. To compile in MinGW(in Windows) use following command.

```
g++ bplustee.cpp -o bplustree.exe
```
## Usage

Once the executable is generated, insertion can be done by passing a roll number and student name as parameters or by specifying a file name which contains multiple records. List of options can be generate using following command,
```
> bplustree -h
Command Line Options
-i <roll_number> <name> : for inserting one record through command line
-if <filename>.txt      : for inserting records from the data present in file
-h                      : for displaying this help text
```

You can even perform insertion and other operations after running the executable file and selecting the options from menu.
```
> bplustree.exe

Run Time Options:
1)Insert
2)Search
4)Print
5)Quit
Enter your choice :
```
