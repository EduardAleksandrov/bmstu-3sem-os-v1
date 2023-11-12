[![C](https://img.shields.io/badge/C-2965f1)](https://en.wikipedia.org/wiki/C_(programming_language))
[![Unix/Linux](https://img.shields.io/badge/Unix/Linux-238c05)](https://en.wikipedia.org/wiki/Unix)


# Операционные системы Unix/Linux

> BMSTU IU7 3 семестр 2023

## Лабораторные работы 
```
1. Файл (./b0901)
    1.1 команда ps, mknod(именованный программный канал), kill, конвеер( | | | )
    1.2 жесткие и гибкие ссылки, inode
    1.3 (onelab.c) вывод getpid()

2. Файл (./b1001) 
    2.0 getpid(), getppid(), getpgrp(), signal()
    2.1 (1.lab.c) смотрим как наследуется дочерний элемент child
    2.2 (2.lab.c) убиваем через kill дочернего и смотрим на вывод сигнала в термиале(выводит wait)
    2.3 (3.lab.c) запускаем два дочерних файла

3. Файл (./b1101)
    3.1 (1.threelab.c) запись и чтение в неименованный pipe, строки задаем в parent
    3.2 (2.threelab.c) обработчик сигнала SIGTERM
    3.3 (3.threelab.c) обработчик сигнала SIGINT, строки задаем в child

4. Файл (./b1102)
    4.1 (4.4.sem_lab.c) Семафор и разделяемая память. Производство/потребление. Пишем в буфер, а потом читаем из него. 
```