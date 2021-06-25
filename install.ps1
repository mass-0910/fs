gcc -finput-charset=UTF-8 -fexec-charset=CP932 -Wall -o fs .\fs.c
if ($?){
    Move-Item .\fs.exe ~\MyTools\ -Force
}