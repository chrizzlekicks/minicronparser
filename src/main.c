#include "../lib/minicron.h"
#include <stdio.h>

int main(int argc, char **argv) {
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <input_file> <current_time>\n", argv[0]);
    fprintf(stderr, "Example: %s input.txt 16:10\n", argv[0]);
    return 1;
  }
  cron_jobs jobs;
  init_jobs(&jobs);
  read_input(argv[1], &jobs);
  print_jobs(&jobs);
  printf("---------------------------------------------\n\r");
  parsed_jobs pJobs;
  init_parsed_jobs(&pJobs);
  parse_jobs(argv[2], &jobs, &pJobs);
  print_parsed(&pJobs);
  free_jobs(&jobs);
  free_parsed(&pJobs);
  return 0;
}
