#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <opencv2/opencv.hpp>

using namespace cv;

#define MAX_PATH_LEN 1024
#define MAX_CHILDREN 8
#define MAX_FILE_LEN 256

const char* input_sequentiel = "/home/yasser_jemli/syscalls/input";
const char* output_sequentiel = "/home/yasser_jemli/syscalls/output/parallele";

void processImage(const char *inputPath, const char *outputPath, double *processingTime) {
    struct timespec start, end;

    clock_gettime(CLOCK_MONOTONIC, &start); // Start timing

    Mat img = imread(inputPath, IMREAD_COLOR); // Load the image
    if (img.empty()) {
        fprintf(stderr, "Error opening image %s\n", inputPath); // Error if the image is not loaded
        exit(EXIT_FAILURE);
    }

    Mat grayImg;
    cvtColor(img, grayImg, COLOR_BGR2GRAY); // Convert the image to grayscale

    if (!imwrite(outputPath, grayImg)) {
        fprintf(stderr, "Error saving image %s\n", outputPath); // Error if the image is not saved
        exit(EXIT_FAILURE);
    }

    clock_gettime(CLOCK_MONOTONIC, &end); // End timing

    *processingTime = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
}

double getCPUUsage() {
    FILE *file = fopen("/proc/stat", "r");
    if (!file) {
        perror("Error opening /proc/stat");
        return -1;
    }

    long long int user, nice, system, idle;
    fscanf(file, "cpu %lld %lld %lld %lld", &user, &nice, &system, &idle);
    fclose(file);

    long long int total = user + nice + system + idle;
    return (double)(user + nice + system) / total * 100;
}

int countProcesses() {
    FILE *fp = popen("ps -e | wc -l", "r");
    if (!fp) {
        perror("Error running ps command");
        return -1;
    }

    int count;
    fscanf(fp, "%d", &count);
    pclose(fp);
    return count;
}

int main() {
    DIR *dir; // Pointer to the directory
    struct dirent *ent; // Pointer to a directory entry

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    double cpuUsageBefore = getCPUUsage();
    int processCountBefore = countProcesses();

    double maxProcessingTime = 0.0; // Variable to track the maximum processing time

    if ((dir = opendir(input_sequentiel)) != NULL) { // Open the input directory
        pid_t childPIDs[MAX_CHILDREN];
        int activeProcesses = 0;

        // Count the number of images
        int imageCount = 0;
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_type == DT_REG) {
                imageCount++;
            }
        }

        // Reset the directory pointer
        rewinddir(dir);

        // Create child processes for each image
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_type == DT_REG) {
                char inputPath[MAX_PATH_LEN];
                char outputPath[MAX_PATH_LEN];

                // Create the full path to the input image
                int inputPathLen = snprintf(inputPath, sizeof(inputPath), "%s/%s", input_sequentiel, ent->d_name);
                if (inputPathLen < 0 || inputPathLen >= sizeof(inputPath)) {
                    fprintf(stderr, "Error creating input path for %s\n", ent->d_name);
                    continue;
                }

                // Create the full path to the output image
                int outputPathLen = snprintf(outputPath, sizeof(outputPath), "%s/%s", output_sequentiel, ent->d_name);
                if (outputPathLen < 0 || outputPathLen >= sizeof(outputPath)) {
                    fprintf(stderr, "Error creating output path for %s\n", ent->d_name);
                    continue;
                }

                if (activeProcesses >= MAX_CHILDREN) {
                    // Wait for any child process to finish before creating a new one
                    int status;
                    pid_t finishedPID = wait(&status);
                    if (WIFEXITED(status)) {
                        char timeFile[MAX_FILE_LEN];
                        snprintf(timeFile, sizeof(timeFile), "/tmp/processing_time_%d.txt", finishedPID);
                        FILE *f = fopen(timeFile, "r");
                        if (f) {
                            double childProcessingTime;
                            fscanf(f, "%lf", &childProcessingTime);
                            fclose(f);
                            if (childProcessingTime > maxProcessingTime) {
                                maxProcessingTime = childProcessingTime;
                            }
                        }
                        remove(timeFile); // Clean up the temporary file
                    }
                    activeProcesses--;
                }

                pid_t pid = fork(); // Fork a child process

                if (pid == 0) {
                    // Child process
                    double processingTime;
                    processImage(inputPath, outputPath, &processingTime); // Process the image

                    // Write the processing time to a file
                    char timeFile[MAX_FILE_LEN];
                    snprintf(timeFile, sizeof(timeFile), "/tmp/processing_time_%d.txt", getpid());
                    FILE *f = fopen(timeFile, "w");
                    if (f) {
                        fprintf(f, "%.6f\n", processingTime);
                        fclose(f);
                    }

                    printf("Child PID: %d, Processing Time: %.6f seconds\n", getpid(), processingTime);
                    exit(EXIT_SUCCESS); // Exit the child process
                } else if (pid < 0) {
                    // Fork failed
                    perror("Fork failed");
                    exit(EXIT_FAILURE);
                } else {
                    // Parent process
                    childPIDs[activeProcesses] = pid;
                    activeProcesses++;
                }
            }
        }

        closedir(dir); // Close the directory

        // Wait for all remaining child processes to finish
        while (activeProcesses > 0) {
            int status;
            pid_t finishedPID = wait(&status);
            if (WIFEXITED(status)) {
                char timeFile[MAX_FILE_LEN];
                snprintf(timeFile, sizeof(timeFile), "/tmp/processing_time_%d.txt", finishedPID);
                FILE *f = fopen(timeFile, "r");
                if (f) {
                    double childProcessingTime;
                    fscanf(f, "%lf", &childProcessingTime);
                    fclose(f);
                    if (childProcessingTime > maxProcessingTime) {
                        maxProcessingTime = childProcessingTime;
                    }
                }
                remove(timeFile); // Clean up the temporary file
            }
            activeProcesses--;
        }

        printf("All child processes completed.\n");

    } else {
        perror("Error opening directory"); // Error if the directory cannot be opened
        return EXIT_FAILURE;
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    double timeTaken = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("Time taken: %.2f seconds\n", maxProcessingTime);
    printf("CPU usage: %.2f%%\n", getCPUUsage()); // Adjusted to only show final CPU usage
    printf("Process count: %d\n", countProcesses()); // Adjusted to only show final process count

    return EXIT_SUCCESS; // Terminate the program successfully
}
