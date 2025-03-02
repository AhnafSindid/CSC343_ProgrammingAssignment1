#include <iostream>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <cmath>
#include <iomanip>
using namespace std;

const int size_mb = 160, startAddr = 2000, totalPages= 100;

void userMemoryAllocation(int fd) {
    srand(time(nullptr));
    int AllocatedPages = 0, processID = 1;
    while (AllocatedPages < 100) {
        int numPages = rand() % 30 + 1;
        int memorySize = numPages * 80;
        int pagesNeeded = (memorySize +(size_mb - 1))/ size_mb ;

        if (AllocatedPages + pagesNeeded > 100) {
            break;
        }
        int data[3] = {processID++, memorySize, pagesNeeded};
        write(fd, data, sizeof(data));
        AllocatedPages += pagesNeeded;
    }
}

int main() {
    int fd[2];

    if (pipe(fd) == -1) {
        return perror("Pipe error"), 1;
    }
    pid_t pid = fork();
    if (pid < 0) {
        return perror("fork error"), 1;
    }
    if (pid == 0) {
        close (fd[0]);
        userMemoryAllocation(fd[1]);
        close(fd[1]);
        exit(0);
    }
    close(fd[1]);
    cout << "\n               --- Memory Allocation Summary Repory ---";
    cout << "\nProcess Id   Starting Memory Address   Size of the Process MB   Unused Space MB\n";

    int startAddress = startAddr, data[3];
    while (read(fd[0], data, sizeof(data)) > 0) {
        int AllocatedMB = data[2] * size_mb;
        int unusedMB = AllocatedMB - data[1];
        cout << left << setw(20) << data[0] << setw(26) << startAddress << setw(24) << data[1] << setw(25) << unusedMB << endl;
        startAddress += AllocatedMB;
    }
    close(fd[0]);
    return 0;
}

