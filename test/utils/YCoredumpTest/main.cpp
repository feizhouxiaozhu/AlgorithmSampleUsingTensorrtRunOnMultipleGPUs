





#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <execinfo.h>
#include <unistd.h>

#include <QCoreApplication>

/* Coredump
 * 1: use 'ulimit -c' to check if coredump file is auto saved,
 *    if zero is returned, use 'ulimit -c size' to
 *    enable coredump file auto saving with specified size;
 * 2: use 'echo "/opt/usr/core/core-%e-%t" > /proc/sys/kernel/core_pattern'
 *    to assign coredump file location and file name. Detailed description
 *    for coredump file format is specified in output of 'man core';
 * 3: use 'gdb -c coredump_file ./program' to restore crash stack.
 *
 * signal + Objdump + c++filt   // need more study
 * 1: register user function to catch SIGSEGV signal and print call stack by using
 *    backtrace and backtrace_symbols functions;
 * 2: use 'objdump -d program > output_file' to disassemble program into output file;
 * 3: search locations printed in call stack when program crashed from output file;
 * 4: using 'c++filt functions' to recover function names or disassemble instructions
 * to locate crash code.
 *
 */

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
