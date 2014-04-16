#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
 
char filename[L_tmpnam];
 
void cleanup() {
  printf("Removing %s.\n", filename);
  remove(filename);

}
 
void melting(int parameter) {
printf("I'm melting! ...\n");
kill(-1,SIGUSR1);

}
 
int main(int argc, char **argv) {
  // переменная для сохранения старой функции
  void (*originalInterruptSignal)(int); 
  // установка нового обработчика сигнала с сохранением указателя на старый
  originalInterruptSignal = signal(SIGINT, melting);
// void (*originalInterruptSignal1)(int); 
  // установка нового обработчика сигнала с сохранением указателя на старый
  //originalInterruptSignal1 = signal(SIGUSR1, cleanup);
    // создаем временный файл
   mkstemp(filename);
  // выводим его имя на экран
  printf("Temporary file %s is created.\n", filename);
  // бесконечный цикл
  while(1);
  // следующая строка никогда не будет вызвана, поскольку на предыдущей
  // строке бесконечный цикл.
  // при нажатии клавиш прерывания (Ctrl+C) или посылания процессу SIGINT
  // сработает функция melting и вызовет функцию cleanup()
  // приложение корректно удалит временный файл
  cleanup();  // выполнение не должно добраться до этой строки
}
