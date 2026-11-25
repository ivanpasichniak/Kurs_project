#include <fstream>
#include <iostream>
#include <string>
#include "Functions.h"

#ifndef UNIT_TEST

static bool inputVector(const std::string& prompt, Vector<double>& out, std::string& error) {
    std::cout << prompt << " (format: <N> <a1> <a2> ... <aN>): ";
    std::string line;
    if (!std::getline(std::cin, line)) {
        error = "invalid size";
        return false;
    }
    stripCR(line);
    return parseVectorLine<double>(line, out, error);
}

static void executeAndSave(int& number, const Task<double>& task,
                           const std::string& parseError, std::ofstream& outFile) {
    ++number;
    std::string message = parseError;
    Vector<double> result;
    bool ok = false;

    if (message.empty()) {
        try {
            auto transform = createTransform(task);
            result = transform->execute(task.a);
            ok = true;
        } catch (const std::exception& e) {
            message = e.what();
        }
    }

    if (ok) {
        writeResult(outFile, number, task, result);
        writeResult(std::cout, number, task, result);
    } else {
        writeError(outFile, number, message);
        writeError(std::cout, number, message);
    }
    outFile.flush();
}

int main() {
    std::ofstream outFile("output.txt");
    if (!outFile.is_open()) {
        std::cout << "ERROR: cannot open file output.txt\n";
        return 1;
    }

    int operationNumber = 0;
    while (true) {
        std::cout << "\n=== Vector Transformations ===\n"
                  << "1. Process data from file\n"
                  << "2. Reflect vector (REFLECT)\n"
                  << "3. Multiply vector by constant (SCALE)\n"
                  << "4. Add two vectors (ADD)\n"
                  << "0. Exit\n"
                  << "Select option: ";

        std::string choice;
        if (!std::getline(std::cin, choice)) {
            break;
        }
        stripCR(choice);
        choice = trim(choice);

        if (choice == "0") {
            std::cout << "Results saved to output.txt\n";
            break;
        } else if (choice == "1") {
            std::cout << "Enter input file name (e.g. input.txt): ";
            std::string fileName;
            if (!std::getline(std::cin, fileName)) {
                break;
            }
            stripCR(fileName);
            fileName = trim(fileName);

            std::ifstream inFile(fileName);
            if (!inFile.is_open()) {
                std::cout << "ERROR: cannot open file " << fileName << "\n";
                continue;
            }
            const int processed = processAll<double>(inFile, outFile, std::cout, operationNumber);
            operationNumber += processed;
            outFile.flush();
        } else if (choice == "2") {
            Task<double> task;
            task.op = "REFLECT";
            std::string error;
            inputVector("Enter vector", task.a, error);
            executeAndSave(operationNumber, task, error, outFile);
        } else if (choice == "3") {
            Task<double> task;
            task.op = "SCALE";
            std::string error;

            std::cout << "Enter constant k: ";
            std::string kStr;
            if (!std::getline(std::cin, kStr)) {
                break;
            }
            stripCR(kStr);
            if (!parseNumber<double>(trim(kStr), task.k)) {
                error = "invalid constant";
            } else {
                inputVector("Enter vector", task.a, error);
            }
            executeAndSave(operationNumber, task, error, outFile);
        } else if (choice == "4") {
            Task<double> task;
            task.op = "ADD";
            std::string error;
            if (inputVector("Enter first vector", task.a, error)) {
                inputVector("Enter second vector", task.b, error);
            }
            executeAndSave(operationNumber, task, error, outFile);
        } else {
            std::cout << "ERROR: unknown menu option\n";
        }
    }

    return 0;
}

#endif
