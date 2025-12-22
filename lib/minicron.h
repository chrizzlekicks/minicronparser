#define MAX_BUF 1023
#define MAX_STR 255
#define MAX_HOUR 24
#define MAX_MINUTE 60

/**
 * Defines the structure of a cron job when read from the config (input) file.
 */
typedef struct _cron_job {
	char minute[MAX_STR];
	char hour[MAX_STR];
	char fire_task[MAX_STR];
	struct _cron_job *next;
} cron_job;

/**
 * Defines the strucutre of a cron job after parsing into a human-readable format.
 */
typedef struct _parsed_job {
	int minute;
	int hour;
	char day[MAX_STR];
	char fire_task[MAX_STR];
	struct _parsed_job *next;
} parsed_job;

/**
 * A struct for a cron job when read from stdin
 */
typedef struct _cron_list {
	cron_job *first;
	cron_job *last;
} cron_jobs;

/**
 * A struct for a parsed cron job before displaying
 */
typedef struct _parsed_list {
	parsed_job *first;
	parsed_job *last;
} parsed_jobs;

/**
 * Initializes an empty linked list for all cron jobs from stdin
 * @param reference to cron_list
 * @return
 */
void init_jobs(cron_jobs *jobs);

/**
 * Initializes an empty linked list for all parsed cron jobs.
 * @param reference to parsed_list
 * @return
 */
void init_parsed_jobs(parsed_jobs *pJobs);

/**
 * Inserts a newly created cron job into a linked list and assigns the next pointers correctly.
 * @param reference to cron_job, reference to cron_jobs list.
 * @return
 */ 
void insert_jobs(cron_job *job, cron_jobs *jobs);

/**
 * Inserts a newly created parsed cron job into a linked list and assigns the next pointers correctly.
 * @param reference to parsed_job, reference to parsed_jobs list.
 * @return
 */ 
void insert_parsed(parsed_job *pJob, parsed_jobs *pJobs);

/**
 * Simple helper function which reads from a file input and writes line by line to a buffer.
 * The information in the buffer is then assigned to cron job and inserted into a list of cron jobs.
 * @param reference to input file, empty cron_jons list
 * @return
 */
void read_input(char *filename, cron_jobs *jobs);

/**
 * Frees the allocated memmory of the read cron_jobs list.
 * @param reference to cron_jobs list
 * @return
 */
void free_jobs(cron_jobs *jobs); 

/**
 * Frees the allocated memmory of the parsed cron_jobs list.
 * @param reference to parsed_jobs list
 * @return
 */
void free_parsed(parsed_jobs *pJobs); 

/**
 * Prints the list of cron jobs which is read from stdin
 * @param reference to cron_jobs list
 * @return
 */
void print_jobs(cron_jobs *jobs);

/**
 * Parses all the imported cron jobs into a human readable format.
 * It needs the current time to execute the parsing and determines if the execution of the cron job should happen either today or tomorrow.
 * @param current time, reference to cron_jobs list, reference to parsed_jobs list
 * @return
 */
void parse_jobs(char *current_time, cron_jobs *src, parsed_jobs *dest);

/**
 * Prints the list of parsed cron jobs
 * @param reference to parsed_jobs list
 * @return
 */
void print_parsed(parsed_jobs *pJobs);

/**
 * Cleans up allocated memory and exits the program with the given error code.
 * @param reference to cron_jobs list, reference to parsed_jobs list, error code
 * @return
 */
void cleanup_and_exit(cron_jobs *jobs, parsed_jobs *pJobs, int errcode);
