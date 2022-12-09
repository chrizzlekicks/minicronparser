#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../lib/minicron.h"

void init_jobs(cron_jobs *jobs) {
	jobs->first = NULL;
	jobs->last = NULL;
}

void init_parsed_list(parsed_jobs *parsed) {
	parsed->first = NULL;
	parsed->last = NULL;
}

void insert_jobs(cron_job *job, cron_jobs *jobs) {
	if(jobs->first == NULL) {
		jobs->first = job;
		jobs->last = job;
		job->next = NULL;
		return;
	}
	jobs->last->next = job;
	jobs->last = job;
	job->next = NULL;
}

void read_input(char *filename, cron_jobs *jobs) {
	FILE *fp = fopen(filename, "r");
	char *buf = malloc(sizeof(char) * MAX_STR);
	cron_job *job;
	while(fgets(buf, MAX_STR, fp) != NULL) {
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
	while(job != NULL) {
		tmp = job->next;
		free(job);
		job = tmp;
	}
}

void free_parsed(parsed_jobs *jobs) {
	parsed_job *parsed = jobs->first;
	parsed_job *tmp;
	while(parsed != NULL) {
		tmp = parsed->next;
		free(parsed);
		parsed = tmp;
	}
}

void print_jobs(cron_jobs *jobs) {
	cron_job *job = jobs->first;
	while(job != NULL) {
		printf("%s %s %s\n", job->minute, job->hour, job->fire_task);
		job = job->next;
	}
}

void parse_jobs(char *current_time, cron_jobs *src, parsed_jobs *dest) {
	char cpy[MAX_STR];
	strcpy(cpy, current_time);
	char delim[] = ":";
	char star[] = "*";
	char *token = strtok(cpy, delim);
	int current_hour = atoi(token);
	printf("%d\n", current_hour);
	token = strtok(NULL, ":");
	int current_min = atoi(token);
	printf("%d\n", current_min);
	cron_job *job = src->first;
	parsed_job *parsed_job = dest->first;
	while(job != NULL) {
		if(strcmp(star, job->hour) == 0) {
			parsed_job->hour = current_hour;
			snprintf(parsed_job->day, MAX_STR, "%s", "today");
		} else {
			int new_hour = atoi(job->hour);
			if(new_hour < current_hour) {
				parsed_job->hour = new_hour;
				snprintf(parsed_job->day, MAX_STR, "%s", "tomorrow");
			}
		}
		if(strcmp(star, job->minute) == 0) {
			parsed_job->minute = current_min;
		} else {
			parsed_job->minute = atoi(job->minute);
		}
		snprintf(parsed_job->fire_task, MAX_STR, "%s", job->fire_task);
		job = job->next;
		parsed_job = parsed_job->next;
	}
}
