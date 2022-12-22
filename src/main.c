#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../lib/minicron.h"

int main(int argc, char** argv) {
	if (argc == 0) {
		perror("Cannot run program because input and current time are missing\n");
		return (-1);
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
