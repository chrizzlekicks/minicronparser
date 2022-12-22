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
	FILE *fp = fopen(filename, "r");
	if (fp == NULL) {
		perror("Could not find input file\n");
		exit(1);
	}
	char *buf = malloc(sizeof(char) * MAX_BUF);
	cron_job *job;
	while (fgets(buf, MAX_BUF, fp) != NULL) {
		job = malloc(sizeof(cron_job));
		sscanf(buf, "%s %s %s", job->minute, job->hour, job->fire_task);
		insert_jobs(job, jobs);
	}
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
	if (current_time == NULL) {
		perror("Current time missing. Could not execute parsing\n");
		exit(1);
	}
	char cpy[MAX_STR];
	strcpy(cpy, current_time);
	char delim[] = ":";
	if (strstr(cpy, delim) == NULL) {
		perror("Wrong time format. Could not execute parsing\n");
		exit(1);
	}
	char star[] = "*";
	char *token = strtok(cpy, delim);
	int current_hour = atoi(token);
	token = strtok(NULL, delim);
	int current_min = atoi(token);
	cron_job *job = src->first;
	parsed_job *pJob = dest->first;
	while (job != NULL) {
		pJob = malloc(sizeof(parsed_job));
		if (strcmp(star, job->hour) == 0) {
			pJob->hour = current_hour;
			snprintf(pJob->day, MAX_STR, "%s", "today");
		} else {
			pJob->hour = atoi(job->hour);
			(pJob->hour < current_hour) 
				? snprintf(pJob->day, MAX_STR, "%s", "tomorrow") 
				: snprintf(pJob->day, MAX_STR, "%s", "today");
		}
		(strcmp(star, job->minute) == 0) ? (pJob->minute = current_min) : (pJob->minute = atoi(job->minute));
		snprintf(pJob->fire_task, MAX_STR, "%s", job->fire_task);
		insert_parsed(pJob, dest);
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
