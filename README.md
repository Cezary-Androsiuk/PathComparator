
# Path Comparator

## About

Library facilitating directory structure comparisons for backup purposes.


## Description

It is a library that enables marking and optionally saving (inside `delete_files.list.txt` and `update_files_list.txt` files) differences between specified folders or files representing the structure of these folders. These differences can be used to create an independent, periodic algorithm that operates without requiring user interaction and performs data backup, focusing only on the changes that was made since last backup.

class **PathComparator** contains two public methods:

    static void printStructure(sfp structurePath, bool new_line = false) noexcept;
**printStructure** - prints the structure specified by the **structurePath** argument. The method also provides the option to add a new line by setting the **new_line** argument to ```true```.

    static void compareStructures(sfp oldStructurePath, sfp newStructurePath, bool save_changes_to_file = false) noexcept;
**compareStructures** - compares the structure specified by the first argument **oldStructurePath**, with the structure specified by the second argument **newStructurePath** (by default newer one) and prints differences in them to ```stdout```. The method also offers the option to save the previously printed differences to a file by setting the **save_changes_to_file** argument to ```true```. This file can be interpreted by [BackupMaster](https://github.com/Cezary-Androsiuk/BackupMaster) to perform backups. 



## Required
- **C++17** <sub><sub><sup> (GCC 13.2) </sup></sub></sub>
- **CMake 3.10**


## Compile

### Windows 
:white_check_mark: Windows supported
```bash
mkdir bin && cd bin;
cmake -G "MinGW Makefiles" .. && cmake --build . && .\main
```

### Linux
:x: Linux not supported (yet)
```bash
mkdir bin && cd bin;
cmake .. && make && ./main;
```



## Screenshots
### Windows
- Directory structure by **`tree /f`** command, env1 | env2 <br/>
![0](https://github.com/Cezary-Androsiuk/PathComparator/blob/master/pictures/0.png?raw=true)
- Directory structure builded by **PathComparator**, env1 | env2 <br/>
![1](https://github.com/Cezary-Androsiuk/PathComparator/blob/master/pictures/1.png?raw=true)
- Difference in env1 and env2 structures compared by **PathComparator** <br/>
![2](https://github.com/Cezary-Androsiuk/PathComparator/blob/master/pictures/2.png?raw=true)


<strike>
### Linux
- Directory structure by **`tree`** command, env1 | env2 <br/>
</strike>
![3](https://github.com/Cezary-Androsiuk/PathComparator/blob/master/pictures/3.png?raw=true)
<strike>
- Directory structure builded by **PathComparator**, env1 | env2 <br/>
</strike>
![4](https://github.com/Cezary-Androsiuk/PathComparator/blob/master/pictures/4.png?raw=true)
<strike>
- Difference in env1 and env2 structures compared by **PathComparator** <br/>
</strike>
![5](https://github.com/Cezary-Androsiuk/PathComparator/blob/master/pictures/5.png?raw=true)