# Что делает данная утилита?
Анализирует бинарные файлы с результатом выполнения утилиты tcpdump и выводит количество незавершенных сессий, завершенных штатно и внештатно.
## Пример использования
tcpdump -w FILENAME.cap
^C
tcpsesscount FILENAME.cap
## Сборка из исходников
```git clone https://github.com/ZSides/funbox.ru.git
cd funbox.ru
gcc tcpsesscount.c -O2 -o tcpsesscount
```

# Справочная информация
## Технические детали
### Структура дампа
(global header | packet header | packet data | packet header | packet data | packet header | ... | packet data)
[Подробнее](https://github.com/hokiespurs/velodyne-copter/wiki/PCAP-format "PCAP format")
### Handshake
Приветственное рукопожатие - SYN на сервер, SYN/ACK на клиент, ACK на сервер
Прощальное рукопожатие - FIN/ACK на сервер, FIN/ACK на клиент, ACK на сервер

## Пояснения по коду
### main.c
TODO
### flags.h
Содержит структуру для хранения флагов.
Содержит функцию для заполнения структуры.
