#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../lib/minicron.h"

int main(int argc, char** argv) {
	if (argc < 1) {
		printf("config only needed\n");
		return 1;
	}
	cron_jobs jobs;
	init_jobs(&jobs);
	read_input(argv[1], &jobs);
	print_jobs(&jobs);
	parsed_jobs pJobs;
	init_parsed_list(&pJobs);
	parse_jobs(argv[2], &jobs, &pJobs);
	print_parsed(&pJobs);
	free_jobs(&jobs);
	free_parsed(&pJobs);
	return 0;
}
