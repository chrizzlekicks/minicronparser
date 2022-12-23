#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../lib/minicron.h"

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
		perror("Could not find input file\n");
		exit(1);
	}

	/* creates pointer to a buffer where read values will be stored */
	char *buf = malloc(sizeof(char) * MAX_BUF);
	cron_job *job;

	/* goes thru the file line by line until it reaches EOF, stores it in the buffer and writes it to respective job
	 * before the job gets inserted into the list */
	while (fgets(buf, MAX_BUF, fp) != NULL) {
		job = malloc(sizeof(cron_job));
		sscanf(buf, "%s %s %s", job->minute, job->hour, job->fire_task);
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
		perror("Current time is missing. Could not execute parsing\n");
		exit(1);
	}

	/* creates copy of current time to avoid mutating initial value */
	char cpy[MAX_STR];
	strcpy(cpy, current_time);
	char delim[] = ":";

	/* checks the format of current timte */
	if (strstr(cpy, delim) == NULL) {
		perror("Wrong time format. Could not execute parsing\n");
		exit(1);
	}

	/* splits current time into hours and minutes and store them in separate variables */
	char star[] = "*";
	char *token = strtok(cpy, delim);
	int current_hour = atoi(token);
	token = strtok(NULL, delim);
	int current_min = atoi(token);

	/* checks if time is within time limits and if not exits from program or converts values respectively */
	if (current_hour > MAX_HOUR || current_min > MAX_MINUTE) {
		perror("Numbers above 23 for hours and 59 for minutes are not allowed\n");
		exit(1);
	}
	else if (current_hour == MAX_HOUR && current_min == MAX_MINUTE) {
		current_min = 0;
		current_hour = 1;
		printf("The correctly converted time is %02d:%02d\n", current_hour, current_min);
		printf("---------------------------------------------\n");
	}
	else if (current_hour == MAX_HOUR - 1 && current_min == MAX_MINUTE) {
		current_min = 0;
		current_hour = 0;
		printf("The correctly converted time is %02d:%02d\n", current_hour, current_min);
		printf("---------------------------------------------\n");
	}
	else if (current_min == MAX_MINUTE) {
		current_min = 0;
		current_hour++;
		printf("The correctly converted time is %02d:%02d\n", current_hour, current_min);
		printf("---------------------------------------------\n");
	}
	else {
		current_hour = 0;
		printf("The correctly converted time is %02d:%02d\n", current_hour, current_min);
		printf("---------------------------------------------\n");
	}

	/* assigns respective list pointers to job and pJob */
	cron_job *job = src->first;
	parsed_job *pJob = dest->first;

	/* actual parsing loop */
	while (job != NULL) {

		/* allocates memory for parsed_jobs */
		pJob = malloc(sizeof(parsed_job));

		/* compares hour strings and performs respective actions */
		if (strcmp(star, job->hour) == 0) {
			pJob->hour = current_hour;
			snprintf(pJob->day, MAX_STR, "%s", "today");
		} 
		else {
			pJob->hour = atoi(job->hour);
			(pJob->hour < current_hour) 
				? snprintf(pJob->day, MAX_STR, "%s", "tomorrow") 
				: snprintf(pJob->day, MAX_STR, "%s", "today");
		}

		/* compares minute strings and performs respective actions */
		(strcmp(star, job->minute) == 0) ? (pJob->minute = current_min) : (pJob->minute = atoi(job->minute));
		snprintf(pJob->fire_task, MAX_STR, "%s", job->fire_task);
		insert_parsed(pJob, dest);

		/* points next pointer to current job */
		job = job->next;
	}
}

void print_parsed(parsed_jobs *pJobs) {
	parsed_job *pJob = pJobs->first;
	if (pJob == NULL) {
		perror("No parsed jobs found\n");
		exit(1);
	}
	while (pJob != NULL) {
		printf("%02d:%02d %s - %s\n", pJob->hour, pJob->minute, pJob->day, pJob->fire_task);
		pJob = pJob->next;
	}
}
