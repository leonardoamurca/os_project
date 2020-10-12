/**
 * Leonardo Aguilar Murça
 * Pedro Gabriel Cruz
 */

#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <cstdlib>

#define NUMBER_OF_ALGORITHMS 4

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

Output *outputs;

// Algoritmos de escalonamento
Output fifo();

Output prio();

Output srtf();

Output rrq(int quantum);

// Funções utilitárias
float calculateWaitingTimeAverage();

float calculateAnswerTimeAverage();

int calculateNumberOfSlots();

int calculateWaitingTime(Process process, int currentSlot);

void sortProcessesByArrivalInstant();

void sortProcessesByPriority();

void setAnswerTime(int value, int index);

int getFirstArrivalInstantGap();

bool hasProcessCompletelyExecuted(int remainingBurstTime);

void initializeTimeSlots();

void initializeProcesses();

void resetWaitingAndAnswerTimes();

// Entrada e saída de dados em arquivos
int readFileAndGetNumberOfProcesses();

void readFileAndSetProcessesData();

void writeOutputDataOnFile();

int main() {
    initializeProcesses();

    outputs = new Output[NUMBER_OF_ALGORITHMS];
    outputs[0] = fifo();
    outputs[1] = prio();
    outputs[2] = srtf();
    outputs[3] = rrq(5);

    writeOutputDataOnFile();

    return 0;
}

// Algoritmos de escalonamento
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

    resetWaitingAndAnswerTimes();

    return output;
}

Output prio() {
    sortProcessesByPriority();

    float waitingAverageTime;
    float sum = 0;

    for (int i = 1; i < numberOfProcesses; i++) {
        processes[i].waitingTime = 0;
        for (int j = 0; j < i; j++) {
            processes[i].waitingTime += processes[j].burstTime;
        }

        sum += processes[i].waitingTime;
    }

    waitingAverageTime = sum / numberOfProcesses;

    Output output;
    output.algorithm = "PRIO";
    output.averageWaitingTime = waitingAverageTime;
    output.averageBurstTime = 0;

    resetWaitingAndAnswerTimes();

    return output;
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

    resetWaitingAndAnswerTimes();

    return output;
}

Output rrq(int quantum) {
    sortProcessesByArrivalInstant();
    initializeTimeSlots();

    int slotsToBurn[numberOfProcesses];
    for (int i = 0; i < numberOfProcesses; i++) {
        slotsToBurn[i] = processes[i].burstTime;
    }

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
    output.algorithm = "RRQ" + to_string(quantum);
    output.averageWaitingTime = calculateWaitingTimeAverage();
    output.averageBurstTime = calculateAnswerTimeAverage();

    resetWaitingAndAnswerTimes();

    return output;
}

// Funções utilitárias
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

int calculateWaitingTime(Process process, int currentSlot) {
    return currentSlot - process.burstTime - process.arrivalInstant - getFirstArrivalInstantGap();
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

void sortProcessesByPriority() {
    for (int i = 0; i < numberOfProcesses; i++) {
        for (int i = 0; i < numberOfProcesses; i++) {
            bool swaps = false;
            for (int j = 0; j < numberOfProcesses - i - 1; j++) {
                if (processes[j].priority > processes[j + 1].priority) {
                    swap(processes[j], processes[j + 1]);
                    swaps = true;
                }
            }
            if (!swaps) break;
        }
    }
}

void setAnswerTime(int value, int index) {
    if (processes[index].answerTime == -1) {
        if (index == 0) processes[index].answerTime = processes[index].arrivalInstant;
        else processes[index].answerTime = value - getFirstArrivalInstantGap();
    }
}

int getFirstArrivalInstantGap() {
    return processes[0].arrivalInstant;
}

bool hasProcessCompletelyExecuted(int remainingBurstTime) {
    return remainingBurstTime == 0;
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

void resetWaitingAndAnswerTimes() {
    for(int i = 0; i < numberOfProcesses; i++) {
        processes[i].waitingTime = 0;
        processes[i].answerTime = -1;
    }
}

// Entrada e saída de dados em arquivos
int readFileAndGetNumberOfProcesses() {
    string line;
    ifstream infile("/home/leonardo/CLionProjects/os_project/input.txt");

    if (infile.good()) getline(infile, line);
    infile.close();

    return stoi(line);
}

void readFileAndSetProcessesData() {
    string fileLines[numberOfProcesses + 1];
    string line;

    Process processesAux[numberOfProcesses];

    ifstream infile("/home/leonardo/CLionProjects/os_project/input.txt");

    int i = 0;
    while (getline(infile, line)) {
        fileLines[i] = line;

        char *lineCharArray = new char[fileLines[i].length() + 1];
        strcpy(lineCharArray, fileLines[i].c_str());

        char *p = strtok(lineCharArray, " ");
        if (i > 0) {
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

void writeOutputDataOnFile() {
    ofstream outfile("/home/leonardo/CLionProjects/os_project/output.txt");
    if (outfile.is_open()) {
        for (int i = 0; i < NUMBER_OF_ALGORITHMS; i++) {
            outfile
                    << outputs[i].algorithm << " "
                    << outputs[i].averageWaitingTime << " "
                    << outputs[i].averageBurstTime
                    << "\n";
        }
    } else cout << "Não foi possível abrir o arquivo!";
    outfile.close();
}





