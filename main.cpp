#include <iostream>

using namespace std;

// Custom structures
struct Process {
    int priority;
    int arrivalInstant;
    int burstTime;
    int waitingTime;
    int answerTime;
};

struct Output {
    string algorithm;
    float averageWaitingTime{};
    float averageServiceTime{};
};

// Global variables
int numberOfSlots;
int *timeSlots;

int numberOfProcesses;
Process *processes;

// Scheduling algorithms
Output fifo();

Output rrq(int quantum);

// Helper functions
void sortProcessesByArrivalInstant();

float calculateWaitingTimeAverage();

float calculateAnswerTimeAverage();

int calculateNumberOfSlots();

void initializeTimeSlots();

void initializeProcesses();

int getFirstArrivalInstantGap();

void burnSlotsFromTo(int start, int end, int indexOfProcess);

void setAnswerTime(int value, int index);

int main() {
    // Input
    // n = number of processes
    // p = Process priority (0 <= p <= 10) ; i = moment when the Process arrives (0 <= i <= 2^n-1) ; s = Process service time (0 <= s <= 2^n-2)

    // Output
    // Algorithm name ; await average time ; answer average time

    initializeProcesses();

    Output fifoOutput = fifo();
    cout << fifoOutput.algorithm << endl;
    cout << fifoOutput.averageWaitingTime << endl;
    cout << fifoOutput.averageServiceTime << endl;

    cout << endl;

    initializeProcesses();

    Output rrqOutput = rrq(5);
    cout << rrqOutput.algorithm << endl;
    cout << rrqOutput.averageWaitingTime << endl;
    cout << rrqOutput.averageServiceTime << endl;

    cout << endl;

    return 0;
}

Output rrq(int quantum) {
    sortProcessesByArrivalInstant();
    initializeTimeSlots();

    int slotsToBurn[numberOfProcesses];
    for (int i = 0; i < numberOfProcesses; i++) {
        slotsToBurn[i] = processes[i].burstTime;
    }

    int currentSlot = 0;
    int totalSlotsToBurn = numberOfSlots - getFirstArrivalInstantGap();
    while (totalSlotsToBurn > 0) {
        for (int i = 0; i < numberOfProcesses; i++) {
            setAnswerTime(currentSlot, i);
            if (slotsToBurn[i] > 0) {
                if (slotsToBurn[i] > quantum) {
                    // For debugging purposes
                    burnSlotsFromTo(currentSlot, currentSlot + quantum, i);

                    currentSlot += quantum;
                    totalSlotsToBurn -= quantum;
                    slotsToBurn[i] -= quantum;
                } else {
                    // For debugging purposes
                    burnSlotsFromTo(currentSlot, currentSlot + slotsToBurn[i], i);

                    processes[i].waitingTime = currentSlot - processes[i].burstTime - processes[i].arrivalInstant ;
                    currentSlot += slotsToBurn[i];
                    totalSlotsToBurn -= slotsToBurn[i];
                    slotsToBurn[i] = 0;
                }

            }
        }
    }

    for (int i = 0; i < numberOfSlots; i++) {
        cout << timeSlots[i] << ",";
    }

    Output output;
    output.algorithm = "RRQ5";
    output.averageWaitingTime = calculateWaitingTimeAverage();
    output.averageServiceTime = calculateAnswerTimeAverage();

    return output;
}

Output fifo() {
    sortProcessesByArrivalInstant();
    initializeTimeSlots();

    for (int i = 0; i < numberOfProcesses; i++) {
        int executionStartAt = processes[i].arrivalInstant;
        if (i == 0) processes[i].waitingTime = executionStartAt;
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
    output.averageServiceTime = waitingAverageTime;

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
    numberOfProcesses = 3;
    processes = new Process[numberOfProcesses];
//
//    processes[0].priority = 4;
//    processes[0].arrivalInstant = 1.0;
//    processes[0].burstTime = 3.0;
//    processes[0].waitingTime = 0;
//    processes[0].answerTime = -1;
//
//    processes[1].priority = 2;
//    processes[1].arrivalInstant = 2.0;
//    processes[1].burstTime = 2.0;
//    processes[1].waitingTime = 0;
//    processes[1].answerTime = -1;
//
//    processes[2].priority = 1;
//    processes[2].arrivalInstant = 3.0;
//    processes[2].burstTime = 1.0;
//    processes[2].waitingTime = 0;
//    processes[2].answerTime = -1;
//
//    processes[3].priority = 3;
//    processes[3].arrivalInstant = 3.0;
//    processes[3].burstTime = 4.0;
//    processes[3].waitingTime = 0;
//    processes[3].answerTime = -1;
//
//    processes[4].priority = 4;
//    processes[4].arrivalInstant = 4.0;
//    processes[4].burstTime = 3.0;
//    processes[4].waitingTime = 0;
//    processes[4].answerTime = -1;
//
//    processes[5].priority = 2;
//    processes[5].arrivalInstant = 6.0;
//    processes[5].burstTime = 2.0;
//    processes[5].waitingTime = 0;
//    processes[5].answerTime = -1;
//
//    processes[6].priority = 2;
//    processes[6].arrivalInstant = 6.0;
//    processes[6].burstTime = 1.0;
//    processes[6].waitingTime = 0;
//    processes[6].answerTime = -1;
//
//    processes[7].priority = 0;
//    processes[7].arrivalInstant = 7.0;
//    processes[7].burstTime = 1.0;
//    processes[7].waitingTime = 0;
//    processes[7].answerTime = -1;

    // ==========================

    processes[0].priority = 0;
    processes[0].arrivalInstant = 20.0;
    processes[0].burstTime = 42.0;
    processes[0].waitingTime = 0;
    processes[0].answerTime = -1;

    processes[1].priority = 2;
    processes[1].arrivalInstant = 3.0;
    processes[1].burstTime = 33.0;
    processes[1].waitingTime = 0;
    processes[1].answerTime = -1;

    processes[2].priority = 1;
    processes[2].arrivalInstant = 14.0;
    processes[2].burstTime = 54.0;
    processes[2].waitingTime = 0;
    processes[2].answerTime = -1;
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
                swap(processes[j].arrivalInstant, processes[j + 1].arrivalInstant);
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

void burnSlotsFromTo(int start, int end, int indexOfProcess) {
    for (int i = start; i < end; i++) {
        timeSlots[i] = indexOfProcess;
    }
}

void setAnswerTime(int value, int index) {
    if (processes[index].answerTime == -1) {
        if (index == 0) processes[index].answerTime = processes[index].arrivalInstant;
        else processes[index].answerTime = value - getFirstArrivalInstantGap();
    }
}





