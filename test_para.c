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

const char* input_sequentiel = "/home/yasser_jemli/syscalls/input";
const char* output_sequentiel = "/home/yasser_jemli/syscalls/output/parallele";

void processImage(const char *inputPath, const char *outputPath) {
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

    if ((dir = opendir(input_sequentiel)) != NULL) { // Open the input directory
        int imageCount = 0;
        
        // First, count the number of images
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_type == DT_REG) {
                imageCount++;
            }
        }

        // Reset the directory pointer
        rewinddir(dir);

        // Create a child process for each image
        for (int i = 0; i < imageCount; i++) {
            if ((ent = readdir(dir)) != NULL && ent->d_type == DT_REG) {
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

                pid_t pid = fork(); // Fork a child process

                if (pid == 0) {
                    // Child process
                    processImage(inputPath, outputPath); // Process the image
                    exit(EXIT_SUCCESS); // Exit the child process
                } else if (pid < 0) {
                    // Fork failed
                    perror("Fork failed");
                    exit(EXIT_FAILURE);
                }
                // Parent process continues to next iteration
            }
        }

        closedir(dir); // Close the directory

        // Wait for all child processes to finish
        while (wait(NULL) > 0);
    } else {
        perror("Error opening directory"); // Error if the directory cannot be opened
        return EXIT_FAILURE;
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    double cpuUsageAfter = getCPUUsage();
    int processCountAfter = countProcesses();

    double timeTaken = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("Time taken: %.2f seconds\n", timeTaken);
    printf("CPU usage before: %.2f%%\n", cpuUsageBefore);
    printf("CPU usage after: %.2f%%\n", cpuUsageAfter);
    printf("Process count before: %d\n", processCountBefore);
    printf("Process count after: %d\n", processCountAfter);

    return EXIT_SUCCESS; // Terminate the program successfully
}
