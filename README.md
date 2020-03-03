# fs (alternative ls command)

## How to use

You can use this code only on windows OS.

Compile(Mingw gcc):
```
> gcc fs.c
```

Show all files in the current directry:
```
> fs
```

Show usage:
```
> fs -h
```

## Make configuration file ".fsrc"

You can create a configuration file under the user directry.
```
> notepad C:/Users/username/.fsrc
```

In ".fsrc", you can set the number of columns to display the file and the output color.

For example:
```
FILENAME_MAX = 24
COLUMN_SIZE  = 4
USE_COLOR    = TRUE
DIR_COLOR    = GREEN
LINK_COLOR   = BLUE
```