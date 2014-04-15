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
  // ���������� ��� ���������� ������ �������
  void (*originalInterruptSignal)(int); 
  // ��������� ������ ����������� ������� � ����������� ��������� �� ������
  originalInterruptSignal = signal(SIGINT, melting);
// void (*originalInterruptSignal1)(int); 
  // ��������� ������ ����������� ������� � ����������� ��������� �� ������
  //originalInterruptSignal1 = signal(SIGUSR1, cleanup);
    // ������� ��������� ����
   mkstemp(filename);
  // ������� ��� ��� �� �����
  printf("Temporary file %s is created.\n", filename);
  // ����������� ����
  while(1);
  // ��������� ������ ������� �� ����� �������, ��������� �� ����������
  // ������ ����������� ����.
  // ��� ������� ������ ���������� (Ctrl+C) ��� ��������� �������� SIGINT
  // ��������� ������� melting � ������� ������� cleanup()
  // ���������� ��������� ������ ��������� ����
  cleanup();  // ���������� �� ������ ��������� �� ���� ������
}
