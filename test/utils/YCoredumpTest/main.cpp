





#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <execinfo.h>
#include <unistd.h>

#include <QCoreApplication>

void ShowStack()
{
    int i;
    void *buffer[1024];
    int n = backtrace(buffer, 1024);
    char **symbols = backtrace_symbols(buffer, n);
    for (i = 0; i < n; i++) {
        printf("%s\n", symbols[i]);
    }
}

void SigSegvProc(int signo) {
    if (signo == SIGSEGV) {
        printf("Receive SIGSEGV signal\n");
        printf("-----call stack-----\n");
        ShowStack();
        exit(-1);
    } else {
        printf("this is sig %d", signo);
    }
}

void RegSig() {
    signal(SIGSEGV, SigSegvProc);
}

void crashFun()
{
    *(int*)(0) = 1;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    RegSig();
    crashFun();

    return a.exec();
}
