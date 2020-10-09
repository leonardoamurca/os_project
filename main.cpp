#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <cstdlib>

using namespace std;

// Estruturas customizadas
struct Process {
    int priority; // prioridade
    int arrivalInstant; // instante de chegada
    int burstTime; // tempo de serviço
    int waitingTime; // tempo de espera
    int answerTime; // tempo de resposta
};

struct Output {
    string algorithm; // nome do algoritmo
    float averageWaitingTime{}; // tempo médio de espera
    float averageBurstTime{}; // tempo médio de serviço
};

// Variáveis globais
int numberOfSlots;
int *timeSlots;

int numberOfProcesses;
Process *processes;

// Algoritmos de escalonamento
Output fifo();

Output rrq(int quantum);

Output srtf();

// Funções utilitárias
float calculateWaitingTimeAverage();

float calculateAnswerTimeAverage();

int calculateNumberOfSlots();

int calculateWaitingTime(Process process, int currentSlot);

void initializeTimeSlots();

void initializeProcesses();

void sortProcessesByArrivalInstant();

void setAnswerTime(int value, int index);

int getFirstArrivalInstantGap();

bool hasProcessCompletelyExecuted(int remainingBurstTime);

// Entrada e saída de dados em arquivos
int readFileAndGetNumberOfProcesses();

void readFileAndSetProcessesData();

int main() {

    initializeProcesses();

    Output fifoOutput = fifo();
    cout << fifoOutput.algorithm << endl;
    cout << fifoOutput.averageWaitingTime << endl;
    cout << fifoOutput.averageBurstTime << endl;

    cout << endl;

    Output rrqOutput = rrq(5);
    cout << rrqOutput.algorithm << endl;
    cout << rrqOutput.averageWaitingTime << endl;
    cout << rrqOutput.averageBurstTime << endl;

    cout << endl;

    Output srtfOutput = srtf();
    cout << srtfOutput.algorithm << endl;
    cout << srtfOutput.averageWaitingTime << endl;
    cout << srtfOutput.averageBurstTime << endl;

    cout << endl;

    return 0;
}

int readFileAndGetNumberOfProcesses() {
    string line;
    ifstream infile("/home/leonardo/CLionProjects/os_project/filename.txt");

    if (infile.good()) getline(infile, line);
    infile.close();

    return stoi(line);
}

void readFileAndSetProcessesData() {
    string fileLines[numberOfProcesses + 1];
    string line;

    ifstream infile("/home/leonardo/CLionProjects/os_project/filename.txt");

    Process processesAux[numberOfProcesses];

    int i = 0;
    while (getline(infile, line)) {
        fileLines[i] = line;

        char *lineCharArray = new char[fileLines[i].length() + 1];
        strcpy(lineCharArray, fileLines[i].c_str());

        char *p = strtok(lineCharArray, " ");
        if (i > 0) { // etapa de separar os tokens de cada processo
            int j = 0;
            while (p != nullptr) {
                if (j == 0) {
                    processesAux[i - 1].priority = (int) strtol(p, nullptr, 10);
                } else if (j == 1) {
                    processesAux[i - 1].arrivalInstant = (int) strtol(p, nullptr, 10);
                } else {
                    processesAux[i - 1].burstTime = (int) strtol(p, nullptr, 10);
                }
                p = strtok(nullptr, " ");
                j++;
            }
            delete[] lineCharArray;
        }
        i++;
    }

    infile.close();

    processes = new Process[numberOfProcesses];
    for (int j = 0; j < numberOfProcesses; j++) {
        processes[j].priority = processesAux[j].priority;
        processes[j].arrivalInstant = processesAux[j].arrivalInstant;
        processes[j].burstTime = processesAux[j].burstTime;
        processes[j].waitingTime = 0;
        processes[j].answerTime = -1;
    }
}

Output srtf() {
    initializeTimeSlots();

    int remainingBurstTimes[numberOfProcesses];
    for (int i = 0; i < numberOfProcesses; i++)
        remainingBurstTimes[i] = processes[i].burstTime;

    int complete = 0, currentTimeSlot = 0, minimumBurstTime = INT32_MAX;
    int shortest = 0, finish_time;
    bool check = false;

    while (complete != numberOfProcesses) {
        for (int j = 0; j < numberOfProcesses; j++) {
            int currentRemainingBurstTime = remainingBurstTimes[j];
            int currentProcessArrivalInstant = processes[j].arrivalInstant;

            if ((currentProcessArrivalInstant <= currentTimeSlot) && (currentRemainingBurstTime < minimumBurstTime) &&
                currentRemainingBurstTime > 0) {
                minimumBurstTime = currentRemainingBurstTime;
                shortest = j;
                check = true;
            }
        }
        if (!check) {
            currentTimeSlot++;
            continue;
        }

        remainingBurstTimes[shortest]--;
        minimumBurstTime = remainingBurstTimes[shortest];
        if (minimumBurstTime == 0) minimumBurstTime = INT32_MAX;

        if (hasProcessCompletelyExecuted(remainingBurstTimes[shortest])) {
            complete++;
            check = false;
            finish_time = currentTimeSlot + 1;

            processes[shortest].waitingTime =
                    finish_time - processes[shortest].burstTime - processes[shortest].arrivalInstant;
            if (processes[shortest].waitingTime < 0) processes[shortest].waitingTime = 0;
        }

        currentTimeSlot++;
    }

    float waitingAverageTime = calculateWaitingTimeAverage();

    Output output;
    output.algorithm = "SRTF";
    output.averageWaitingTime = waitingAverageTime;
    output.averageBurstTime = waitingAverageTime;

    return output;
}


Output rrq(int quantum) {
    sortProcessesByArrivalInstant();
    initializeTimeSlots();

    int slotsToBurn[numberOfProcesses];
    for (int i = 0; i < numberOfProcesses; i++)
        slotsToBurn[i] = processes[i].burstTime;

    int currentTimeSlot = 0;
    int totalSlotsToBurn = numberOfSlots - getFirstArrivalInstantGap();
    while (totalSlotsToBurn > 0) {
        for (int i = 0; i < numberOfProcesses; i++) {
            setAnswerTime(currentTimeSlot, i);
            if (slotsToBurn[i] > 0) {
                if (slotsToBurn[i] > quantum) {
                    currentTimeSlot += quantum;
                    totalSlotsToBurn -= quantum;
                    slotsToBurn[i] -= quantum;
                } else {
                    processes[i].waitingTime = calculateWaitingTime(processes[i], currentTimeSlot);
                    currentTimeSlot += slotsToBurn[i];
                    totalSlotsToBurn -= slotsToBurn[i];
                    slotsToBurn[i] = 0;
                }
            }
        }
    }

    Output output;
    output.algorithm = "RRQ5";
    output.averageWaitingTime = calculateWaitingTimeAverage();
    output.averageBurstTime = calculateAnswerTimeAverage();

    return output;
}


Output fifo() {
    sortProcessesByArrivalInstant();
    initializeTimeSlots();

    for (int i = 0; i < numberOfProcesses; i++) {
        int executionStartAt = processes[i].arrivalInstant;
        if (i == 0) processes[i].waitingTime = executionStartAt - getFirstArrivalInstantGap();
        while (timeSlots[executionStartAt] != -1) {
            processes[i].waitingTime++;
            executionStartAt++;
        }

        int executionSlotLimit = executionStartAt + processes[i].burstTime;
        for (int k = executionStartAt; k < executionSlotLimit; k++) {
            timeSlots[k] = i;
        }
    }

    float waitingAverageTime = calculateWaitingTimeAverage();

    Output output;
    output.algorithm = "FIFO";
    output.averageWaitingTime = waitingAverageTime;
    output.averageBurstTime = waitingAverageTime;

    return output;
}

void initializeTimeSlots() {
    numberOfSlots = calculateNumberOfSlots();
    timeSlots = new int[numberOfSlots];

    for (int k = 0; k < numberOfSlots; k++) {
        timeSlots[k] = -1;
    }
}

void initializeProcesses() {
    numberOfProcesses = readFileAndGetNumberOfProcesses();
    readFileAndSetProcessesData();
}

float calculateWaitingTimeAverage() {
    float sum = 0;
    for (int i = 0; i < numberOfProcesses; i++) {
        sum += processes[i].waitingTime;
    }

    return sum / (float) numberOfProcesses;
}

float calculateAnswerTimeAverage() {
    float sum = 0;
    for (int i = 0; i < numberOfProcesses; i++) {
        sum += processes[i].answerTime;
    }

    return sum / (float) numberOfProcesses;
}

int calculateNumberOfSlots() {
    int sumOfSlots = 0;
    for (int j = 0; j < numberOfProcesses; j++) {
        if (j == 0) sumOfSlots += processes[j].arrivalInstant + processes[j].burstTime;
        else sumOfSlots += processes[j].burstTime;
    }

    return sumOfSlots;
}

void sortProcessesByArrivalInstant() {
    for (int i = 0; i < numberOfProcesses; i++) {
        bool swaps = false;
        for (int j = 0; j < numberOfProcesses - i - 1; j++) {
            if (processes[j].arrivalInstant > processes[j + 1].arrivalInstant) {
                swap(processes[j], processes[j + 1]);
                swaps = true;
            }
        }
        if (!swaps) break;
    }
}

// Use for sorted arrays only
int getFirstArrivalInstantGap() {
    return processes[0].arrivalInstant;
}

void setAnswerTime(int value, int index) {
    if (processes[index].answerTime == -1) {
        if (index == 0) processes[index].answerTime = processes[index].arrivalInstant;
        else processes[index].answerTime = value - getFirstArrivalInstantGap();
    }
}

int calculateWaitingTime(Process process, int currentSlot) {
    return currentSlot - process.burstTime - process.arrivalInstant - getFirstArrivalInstantGap();
}

bool hasProcessCompletelyExecuted(int remainingBurstTime) {
    return remainingBurstTime == 0;
}





