#define MAX_BUF 1023
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

void init_jobs(cron_jobs *jobs);
void init_parsed_jobs(parsed_jobs *pJobs);
void insert_jobs(cron_job *job, cron_jobs *jobs);
void insert_parsed(parsed_job *pJob, parsed_jobs *pJobs);
void read_input(char *filename, cron_jobs *jobs);
void free_jobs(cron_jobs *jobs); 
void free_parsed(parsed_jobs *pJobs); 
void print_jobs(cron_jobs *jobs);
void parse_jobs(char *current_time, cron_jobs *src, parsed_jobs *dest);
void print_parsed(parsed_jobs *pJobs);
