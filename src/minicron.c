#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STR 255
#define MAX_HOUR 24
#define MAX_MINUTE 60

typedef struct _cron_job {
	char minute[MAX_STR];
	char hour[MAX_STR];
	char fire_task[MAX_STR];
	struct _cron_job *next;
} cron_job;

typedef struct _parsed_job {
	int minute;
	int hour;
	char day[MAX_STR];
	char fire_task[MAX_STR];
	struct _parsed_job *next;
} parsed_job;

typedef struct _cron_list {
	cron_job *first;
	cron_job *last;
} cron_jobs;

typedef struct _parsed_list {
	parsed_job *first;
	parsed_job *last;
} parsed_jobs;

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
	char *token = strtok(cpy, delim);
	int current_hour = atoi(token);
	printf("%d\n", current_hour);
	token = strtok(NULL, ":");
	int current_min = atoi(token);
	printf("%d\n", current_min);
	cron_job *job = src->first;
	parsed_job *parsed_job = dest->first;
	while(job != NULL) {
		int new_hour = atoi(job->hour);
		if(new_hour < current_hour) {
			parsed_job->hour = new_hour;
			parsed_job->minute = atoi(job->minute);
			snprintf(parsed_job->day, MAX_STR, "%s", "tomorrow");
		}
		job = job->next;
		parsed_job = parsed_job->next;
	}
}

int main(int argc, char** argv) {
	if(argc < 1) {
		printf("config only needed\n");
		return 1;
	}
	cron_jobs jobs;
	init_jobs(&jobs);
	read_input(argv[1], &jobs);
	print_jobs(&jobs);
	free_jobs(&jobs);
	return 0;
}
