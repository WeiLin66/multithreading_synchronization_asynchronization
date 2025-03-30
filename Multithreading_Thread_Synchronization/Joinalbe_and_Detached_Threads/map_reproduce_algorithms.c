#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#define FILENAME		"story_novel.txt"
#define PROCESS_LINE	200
#define MAX_LINE_LENGTH	1024

typedef struct {
	const char* filename;
	int start_offset;
	int end_offset;
	int total_characters;
} thread_data;

void* worker_thread (void* arg) {

	thread_data* data = arg;
	int pcurrent = data->start_offset;
	FILE* fd = fopen(data->filename, "r");

	data->total_characters = 0;

	if (fd == NULL) {
		printf("Error opening file\n");
		pthread_exit(NULL);
	}

	fseek(fd, data->start_offset, SEEK_SET);
	char buffer[MAX_LINE_LENGTH];

	while (fgets(buffer, MAX_LINE_LENGTH, fd)) {
		int len = strlen(buffer);
		
		if (buffer[len - 1] == '\n') {
			--len;
		}

		data->total_characters += len;

		if ((pcurrent += ftell(fd)) >= data->end_offset) {
			break;
		}
	}

	fclose(fd);
	pthread_exit;
}

int main (int argc, char* argv[]) {

	FILE* fd 						= NULL;
	pthread_t* thread_pool 			= NULL;
	thread_data* thread_datas 		= NULL;
	int lines = 0, num_of_workers 	= 0;
	int (*content_range)[2];
	int start = 0, end = PROCESS_LINE - 1;
	char ch;

	fd = fopen(FILENAME, "r");

	if (fd == NULL) {
		printf("Error opening file\n");
		return -1;
	}

	while ((ch = fgetc(fd)) != EOF) {
		if (ch == '\n') {
			++lines;
		}
	}

	printf("this file contain %d lines\n", lines);

	if (lines == 0) {
		printf("No content in this file\n");
		return 0;
	}

	if (lines <= end) {
		end = lines - 1;
	}

	num_of_workers = lines / PROCESS_LINE;
	num_of_workers = lines % PROCESS_LINE ? num_of_workers + 1 : num_of_workers;

	if ((thread_pool = malloc(sizeof(pthread_t) * num_of_workers)) == NULL) {
		printf("malloc failed : run out of memory\n");
	}

	if ((thread_datas = malloc(sizeof(thread_data) * num_of_workers)) == NULL) {
		printf("malloc failed : run out of memory\n");
	}	

	if ((content_range = malloc(sizeof(int) * 2 * num_of_workers)) == NULL) {
		printf("malloc failed : run out of memory\n");
	}

	for (int i = 0; i < num_of_workers; ++i) {
		content_range[i][0] = start;
		content_range[i][1] = end;

		start += PROCESS_LINE;
		end = end + PROCESS_LINE >= lines ? lines : end + PROCESS_LINE;

		thread_datas[i].filename 	= FILENAME;
		thread_datas[i].start_offset = start;
		thread_datas[i].end_offset 	= end;

		pthread_create(&thread_pool[i], NULL, worker_thread, (void*)&thread_datas[i]);
	}

	int total_characters = 0;

	for (int i = 0; i < num_of_workers; ++i) {
		pthread_join(thread_pool[i], NULL);
		printf("worker thread %d : %d\n", i, thread_datas[i].total_characters);
		total_characters += thread_datas[i].total_characters;
	}

	printf("Total characters in the file: %d\n", total_characters);

	return 0;
}