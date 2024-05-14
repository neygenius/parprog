# Лабораторная работа №3

### Файлы:

1. Файл `lab3.cpp` - файл C++: генерация матриц, выполенение операций перемножения, запись матриц в файлы .txt;
2. Файл `lab1.py` - файл Python: верификация результатов перемножения, построение графика;

### В ходе выполнения данной работы были выполнены следующие шаги:

1. Взяты результаты выполнения лабораторных работ №1 и №2
2. Модифицирован процесс выполнения математической задачи с использованием библиотеки MPI
3. Произведена работа на суперкомпьютере

### Процесс выполнения работы на суперкомпьютере

1. Подключение к удаленному рабочему столу в корпоративной сети SSAU через VM Horizon
2. Подключение по `putty` к суперкомпьютеру
   
   ![1](https://github.com/neygenius/parprog/assets/117530585/2fcaca6f-3fb5-42d3-ac60-04e8d3967820)
   
4. Подключение через `WinSCP` для переброса `.cpp` файла с содержимым лабораторной работы
   
   ![2](https://github.com/neygenius/parprog/assets/117530585/2267d278-dbb2-465e-af4f-8621bea72dfc)
   
6. Загружен файл с исходным кодом, и проведена проверка наличия его в системе
   
   ![3](https://github.com/neygenius/parprog/assets/117530585/b4fd1cc2-b869-4931-9017-d8b0c363fad0)
   
8. Подключен рабочий модуль `Intel/mpi4`
9. Скомпилирован исполняемый файл лабораторной работы
    ![4](https://github.com/neygenius/parprog/assets/117530585/f1d8ad29-0dc2-455f-996f-80d031f767ae)
   
10. Написан скрипт для запуска исполняемого файла в очереди суперкомпьтера
11. Проверен результат работы программы
    
   ![5](https://github.com/neygenius/parprog/assets/117530585/3dc09f74-2b64-4a84-bd71-50c455f09b6d)


### Вывод:

В ходе лабораторной работы был получен опыт работы с библиотекой MPI для осуществления многопоточных вычислений. Также был изучен принцип работы с суперкомпьютером, и на практике осуществлен запуск программы на нем.
