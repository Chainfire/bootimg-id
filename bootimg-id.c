#include <stdio.h>
#include <fcntl.h>

#define BLOCK_SIZE 4096
#define ID_INDEX 576
#define ID_LENGTH 32

int read_block(char* blockdev, unsigned char* buffer) {
	int fd = open(blockdev, O_RDONLY);
	if (fd <= 0) {
		fprintf(stderr, "File open failure\n");
		return 1;
	}

	int ret = 0;

	int r = read(fd, buffer, BLOCK_SIZE);
	if (r != BLOCK_SIZE) {
		fprintf(stderr, "Block read failure\n");
		ret = 1;
		goto cleanup;
	}

	if (strncmp((char*)buffer, "ANDROID!", 8)) {
		fprintf(stderr, "Header magic mismatch");
		ret = 1;
		goto cleanup;
	}

cleanup:
	close(fd);
	return ret;
}

int write_block(char* blockdev, unsigned char* buffer) {
	int fd = open(blockdev, O_RDWR);
	if (fd <= 0) {
		fprintf(stderr, "File open failure\n");
		return 1;
	}

	int ret = 0;

	int w = write(fd, buffer, BLOCK_SIZE);
	if (w != BLOCK_SIZE) {
		fprintf(stderr, "Block write failure\n");
		ret = 1;
		goto cleanup;
	}

cleanup:
	close(fd);
	return ret;
}

void print_id(unsigned char* buffer, int index) {
	printf("0x");
	int i;
	for (i = 0; i < ID_LENGTH; i++) {
		printf("%02x", buffer[index + i]);
	}
	printf("\n");
}

int main(int argc, char *argv[]) {
	fprintf(stderr, "bootimg-id by Chainfire\n\n");
	if ((argc == 2) || (argc == 3)) {
		unsigned char buffer[BLOCK_SIZE];
		if (read_block(argv[1], buffer)) return 1;
		print_id(buffer, ID_INDEX);
		
		if (argc == 2) return 0;
		
		char* parse = argv[2];
		if (!strncmp(parse, "0x", 2)) parse = &parse[2];
		if (strlen(parse) != ID_LENGTH * 2) {
			fprintf(stderr, "id length mismatch, expected %d, got %d (excluding 0x)\n", ID_LENGTH, strlen(parse));
		}

		unsigned char id[ID_LENGTH];
		char hex[3];
		int i;
		for (i = 0; i < ID_LENGTH; i++) {
			memcpy(hex, &parse[i * 2], 2);
			id[i] = strtol(hex, NULL, 16);
		}
		print_id(id, 0);
		
		memcpy(&buffer[ID_INDEX], id, ID_LENGTH);
		
		if (write_block(argv[1], buffer)) return 1;
		
		return 0;
	} else {
		fprintf(stderr, "bootimg-id blockdev       # get\n");
		fprintf(stderr, "bootimg-id blockdev id    # set\n");
		return 0;
	}
}