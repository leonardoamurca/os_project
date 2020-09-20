#include <iostream>
#include <bits/stdc++.h>

using namespace std;

// Custom structures
struct Process {
    int priority;
    int arrivalInstant;
    int serviceTime;
    int waitingTime;
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

// Helper functions
void sortProcessesByArrivalInstant();

float calculateWaitingTimeAverage();

int calculateNumberOfSlots();

void initializeTimeSlots();

void initializeProcesses();

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

    return 0;
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

        int executionSlotLimit = executionStartAt + processes[i].serviceTime;
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

    processes[0].priority = 0;
    processes[0].arrivalInstant = 20.0;
    processes[0].serviceTime = 42.0;
    processes[0].waitingTime = 0;

    processes[1].priority = 2;
    processes[1].arrivalInstant = 3.0;
    processes[1].serviceTime = 33.0;
    processes[1].waitingTime = 0;

    processes[2].priority = 1;
    processes[2].arrivalInstant = 14.0;
    processes[2].serviceTime = 54.0;
    processes[2].waitingTime = 0;
}

float calculateWaitingTimeAverage() {
    float sum = 0;
    for (int i = 0; i < numberOfProcesses; i++) {
        sum += processes[i].waitingTime;
    }

    return sum / (float) numberOfProcesses;
}

int calculateNumberOfSlots() {
    int sumOfSlots = 0;
    for (int j = 0; j < numberOfProcesses; j++) {
        if (j == 0) sumOfSlots += processes[j].arrivalInstant + processes[j].serviceTime;
        else sumOfSlots += processes[j].serviceTime;
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





