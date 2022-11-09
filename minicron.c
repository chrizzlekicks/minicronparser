#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STR 255

typedef struct _cron_job {
	char minute[MAX_STR];
	char hour[MAX_STR];
	char fire_task[MAX_STR];
	struct _cron_job* next;
} cron_job;

typedef struct _list {
	cron_job *first;
	cron_job *last;
} cron_jobs;

void init_jobs(cron_jobs *jobs) {
	jobs->first = NULL;
	jobs->last = NULL;
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
	while(fgets(buf, 255, fp) != NULL) {
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

void print_jobs(cron_jobs *jobs) {
	cron_job *job = jobs->first;
	while (job != NULL) {
		printf("%s:%s - %s\n", job->hour, job->minute, job->fire_task);
		job = job->next;
	}
}

int main(int argc, char** argv) {
	if(argc > 1) {
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
