#include "../lib/minicron.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void init_jobs(cron_jobs *jobs) {
  jobs->first = NULL;
  jobs->last = NULL;
}

void init_parsed_jobs(parsed_jobs *pJobs) {
  pJobs->first = NULL;
  pJobs->last = NULL;
}

void insert_jobs(cron_job *job, cron_jobs *jobs) {
  if (jobs->first == NULL) {
    jobs->first = job;
    jobs->last = job;
    job->next = NULL;
    return;
  }
  jobs->last->next = job;
  jobs->last = job;
  job->next = NULL;
}

void insert_parsed(parsed_job *pJob, parsed_jobs *pJobs) {
  if (pJobs->first == NULL) {
    pJobs->first = pJob;
    pJobs->last = pJob;
    pJob->next = NULL;
    return;
  }
  pJobs->last->next = pJob;
  pJobs->last = pJob;
  pJob->next = NULL;
}

void read_input(char *filename, cron_jobs *jobs) {

  /* opens file, stores it and returns file pointer */
  FILE *fp = fopen(filename, "r");
  if (fp == NULL) {
    fprintf(stderr, "Could not find input file\n");
    exit(1);
  }

  /* creates pointer to a buffer where read values will be stored */
  char *buf = malloc(sizeof(char) * MAX_BUF);
  cron_job *job;

  /* goes thru the file line by line until it reaches EOF, stores it in the
   * buffer and writes it to respective job before the job gets inserted into
   * the list */
  while (fgets(buf, MAX_BUF, fp) != NULL) {
    job = malloc(sizeof(cron_job));

    if (sscanf(buf, "%254s %254s %254s", job->minute, job->hour,
               job->fire_task) != 3) {
      fprintf(stderr, "Invalid format on line\n");
      free(job);
      continue;
    }

    insert_jobs(job, jobs);
  }

  /* close file and frees allocated memory */
  fclose(fp);
  free(buf);
}

void free_jobs(cron_jobs *jobs) {
  cron_job *job = jobs->first;
  cron_job *tmp;
  while (job != NULL) {
    tmp = job->next;
    free(job);
    job = tmp;
  }
}

void free_parsed(parsed_jobs *pJobs) {
  parsed_job *pJob = pJobs->first;
  parsed_job *tmp;
  while (pJob != NULL) {
    tmp = pJob->next;
    free(pJob);
    pJob = tmp;
  }
}

void print_jobs(cron_jobs *jobs) {
  cron_job *job = jobs->first;
  while (job != NULL) {
    printf("%s %s %s\n", job->minute, job->hour, job->fire_task);
    job = job->next;
  }
}

void parse_jobs(char *current_time, cron_jobs *src, parsed_jobs *dest) {

  /* checks if the pointer to current time is passed */
  if (current_time == NULL) {
    fprintf(stderr, "Current time is missing. Could not execute parsing\n");
    cleanup_and_exit(src, dest, 1);
  }

  /* creates copy of current time to avoid mutating initial value */
  char cpy[MAX_STR];
  strcpy(cpy, current_time);
  char delim[] = ":";

  /* checks the format of current time */
  if (strstr(cpy, delim) == NULL) {
    fprintf(stderr, "Wrong time format. Could not execute parsing\n");
    cleanup_and_exit(src, dest, 1);
  }

  /* splits current time into hours and minutes and stores them in separate
   * variables */
  char star[] = "*";
  char *endptr;
  errno = 0;
  char *token = strtok(cpy, delim);
  long current_hour = strtol(token, &endptr, 10);
  if (*endptr != '\0' || endptr == token) {
    fprintf(stderr, "Invalid hour format: %s\n", token);
    cleanup_and_exit(src, dest, 1);
  }
  token = strtok(NULL, delim);
  errno = 0;
  long current_min = strtol(token, &endptr, 10);
  if (*endptr != '\0' || endptr == token) {
    fprintf(stderr, "Invalid minute format: %s\n", token);
    cleanup_and_exit(src, dest, 1);
  }

  /* checks if time is within time limits and if not exits from program or
   * converts values respectively */
  if (current_hour > MAX_HOUR || current_min > MAX_MINUTE) {
    fprintf(stderr,
            "Numbers above 23 for hours and 59 for minutes are not allowed\n");
    exit(1);
  }
  if (current_hour == MAX_HOUR && current_min == MAX_MINUTE) {
    current_min = 0;
    current_hour = 1;
  }
  if (current_hour == MAX_HOUR - 1 && current_min == MAX_MINUTE) {
    current_min = 0;
    current_hour = 0;
  }
  if (current_min == MAX_MINUTE) {
    current_min = 0;
    current_hour++;
  }
  if (current_hour == MAX_HOUR) {
    current_hour = 0;
  }

  /* print the correctly converted time */
  printf("The correctly converted time is %02ld:%02ld\n", current_hour,
         current_min);
  printf("---------------------------------------------\n");

  /* assigns respective list pointers to job and pJob */
  cron_job *job = src->first;
  parsed_job *pJob = dest->first;

  /* actual parsing loop */
  while (job != NULL) {

    /* allocates memory for parsed_jobs */
    pJob = malloc(sizeof(parsed_job));

    /* compares hour strings and performs respective actions */
    if (strcmp(star, job->hour) == 0) {
      pJob->hour = (int)current_hour;
      snprintf(pJob->day, MAX_STR, "%s", "today");
    } else {
      errno = 0;
      long parsed_hour = strtol(job->hour, &endptr, 10);
      if (*endptr != '\0' || endptr == job->hour || parsed_hour < 0 ||
          parsed_hour > 23) {
        fprintf(stderr, "Invalid hour value in job: %s\n", job->hour);
        free(pJob);
        job = job->next;
        continue;
      }
      pJob->hour = (int)parsed_hour;
      (pJob->hour < current_hour)
          ? snprintf(pJob->day, MAX_STR, "%s", "tomorrow")
          : snprintf(pJob->day, MAX_STR, "%s", "today");
    }

    /* compares minute strings and performs respective actions */
    if (strcmp(star, job->minute) == 0) {
      pJob->minute = (int)current_min;
    } else {
      errno = 0;
      long parsed_min = strtol(job->minute, &endptr, 10);
      if (*endptr != '\0' || endptr == job->minute || parsed_min < 0 ||
          parsed_min > 59) {
        fprintf(stderr, "Invalid minute value in job: %s\n", job->minute);
        free(pJob);
        job = job->next;
        continue;
      }
      pJob->minute = (int)parsed_min;
    }
    snprintf(pJob->fire_task, MAX_STR, "%s", job->fire_task);
    insert_parsed(pJob, dest);

    /* points next pointer to current job */
    job = job->next;
  }
}

void print_parsed(parsed_jobs *pJobs) {
  parsed_job *pJob = pJobs->first;
  if (pJob == NULL) {
    fprintf(stderr, "No parsed jobs found\n");
    exit(1);
  }
  while (pJob != NULL) {
    printf("%02d:%02d %s - %s\n", pJob->hour, pJob->minute, pJob->day,
           pJob->fire_task);
    pJob = pJob->next;
  }
}

void cleanup_and_exit(cron_jobs *jobs, parsed_jobs *pJobs, int errcode) {
  if (jobs && jobs->first)
    free_jobs(jobs);
  if (pJobs && pJobs->first)
    free_parsed(pJobs);

  exit(errcode);
}
