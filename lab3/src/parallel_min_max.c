#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <getopt.h>

#include "find_min_max.h"
#include "utils.h"

int main(int argc, char **argv) {
  int seed = -1;
  int array_size = -1;
  int pnum = -1;
  bool with_files = false;

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"seed", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {"pnum", required_argument, 0, 0},
                                      {"by_files", no_argument, 0, 'f'},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "f", options, &option_index);

    if (c == -1) break;

    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            seed = atoi(optarg);
            // your code here
            // error handling
            break;
          case 1:
            array_size = atoi(optarg);
            // your code here
            // error handling
            break;
          case 2:
            pnum = atoi(optarg);
            // your code here
            // error handling
            break;
          case 3:
            with_files = true;
            break;

          defalut:
            printf("Index %d is out of options\n", option_index);
        }
        break;
      case 'f':
        with_files = true;
        break;

      case '?':
        break;

      default:
        printf("getopt returned character code 0%o?\n", c);
    }
  }

  if (optind < argc) {
    printf("Has at least one no option argument\n");
    return 1;
  }

  if (seed == -1 || array_size == -1 || pnum == -1) {
    printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" \n",
           argv[0]);
    return 1;
  }

  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed);
  int active_child_processes = 0;

  struct timeval start_time;
  gettimeofday(&start_time, NULL);

  FILE* maximum_file;
  FILE* minimum_file;
  minimum_file = fopen("minimum_values.txt", "w");
  fclose(minimum_file);
  maximum_file = fopen("maximum_values.txt", "w");
  fclose(maximum_file);
  int step = array_size / pnum;

  int(*p_min)[2] = malloc(sizeof(int[2]) * pnum);
  int(*p_max)[2] = malloc(sizeof(int[2]) * pnum);
  for (int i = 0; i < pnum; i++) {
      if (pipe(p_min[i]) == -1 || pipe(p_max[i]) == -1) 
      {
          perror("pipe error");
          return 1;
      }
    }

  for (int i = 0; i < pnum; i++) {
    pid_t child_pid = fork();
    if (child_pid >= 0) {
      // successful fork
      active_child_processes += 1;
      if (child_pid == 0) {
        // child process

        // parallel somehow

       if (with_files) {
                      int begin = i * step;
                      int end = (i == pnum - 1) ? array_size : (i + 1) * step;

                      struct MinMax mm = GetMinMax(array, begin, end);

                      minimum_file = fopen("minimum_values.txt", "a+");
                      fprintf(minimum_file, "%d\n", mm.min);
                      fclose(minimum_file);
                      maximum_file = fopen("maximum_values.txt", "a+");
                      fprintf(maximum_file, "%d\n", mm.max);
                      fclose(maximum_file);

                    // use files here
                } else {
          // use pipe here
          close(p_min[i][0]); //закрываем чтение
          close(p_max[i][0]);
          int start_index = i * step;
          int end_index = (i == pnum - 1) ? array_size : (i + 1) * step;
          struct MinMax local_min_max = GetMinMax(array, start_index, end_index);
          write(p_min[i][1], &local_min_max.min, sizeof(int));
          write(p_max[i][1], &local_min_max.max, sizeof(int));

          close(p_min[i][1]); // закрываем запись для минимума
          close(p_max[i][1]); // закрываем запись для максимума

        }
        return 0;
      }

    } else {
      printf("Fork failed!\n");
      return 1;
    }
  }

  while (active_child_processes > 0) {
    // your code here
    int status;
    wait(&status);
    active_child_processes -= 1;
  }
  int min_result, max_result;
  struct MinMax min_max;
  min_max.min = INT_MAX;
  min_max.max = INT_MIN;
  minimum_file = fopen("minimum_values.txt", "r+");
  maximum_file = fopen("maximum_values.txt", "r+");

  for (int i = 0; i < pnum; i++) {
    int min = INT_MAX;
    int max = INT_MIN;

    if (with_files) {
      fscanf(minimum_file, "%d", &min_result);
      fscanf(maximum_file, "%d", &max_result);
      // read from files
    } else {
      // read from pipes
      read(p_min[i][0], &min_result, sizeof(int));
      read(p_max[i][0], &max_result, sizeof(int));
      close(p_min[i][0]);//закрываем чтение для минимума
      close(p_max[i][0]);
    }

    if (min_result < min_max.min) min_max.min = min_result;
    if (max_result > min_max.max) min_max.max = max_result;
  }
    fclose(minimum_file);
    fclose(maximum_file);

  struct timeval finish_time;
  gettimeofday(&finish_time, NULL);

  double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
  elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

  free(p_min);
  free(p_max);
  free(array);

  printf("Min: %d\n", min_max.min);
  printf("Max: %d\n", min_max.max);
  printf("Elapsed time: %fms\n", elapsed_time);
  fflush(NULL);
  return 0;
}
