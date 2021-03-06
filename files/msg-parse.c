#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/**
 * A simple test program that parses a msg_t from an input file and prints
 * the username and address. The parse_msg() function is intentionally
 * written with two unsafe statements:
 *
 *     - if the input file is well-formed (the user tells the truth about
 *       the lengths of their address and username), the program proceeds
 *       as expected without any memory violations
 *
 *     - however, if the length fields of the input file are inaccurate, the
 *       program will attempt may attempt to either read from or write to
 *       unowned memory, depending on the input file fields
 *
 * See the UNSAFE statements for details.
 */
#define MIN_MSG_SIZE 1 + 1 + 1 + 2 + sizeof(char*) + sizeof(char*)
#define MSG_TYPE_CLIENT_HELLO 1

typedef struct {
    uint8_t type;       // What kind of message is this? Only 1 recognized.
    uint8_t addr_len;   // How long, in bytes, is addr field. Big endian.
    uint8_t uname_len;  // How long, in bytes, is uname field.
    uint16_t len;       // Total length, in bytes, of this message.
    char *addr;         // Email address.
    char *uname;        // Username.
} msg_t;

/**
 * Exit nicely and print a message on stderr if we fail.
 */
void
errexit(char *reason) {
    fprintf(stderr, reason);
    exit(1);
}

/**
 * Read a file into memory. Allocates enough memory in buf to hold the
 * file contents and returns size of buf.
 *
 * Fail gracefully and immediately if any system calls return an error.
 */
size_t
read_file(const char *const fname, uint8_t **buf) {
   	FILE *file;
	size_t len;

    if (!(file = fopen(fname, "r"))) {
        errexit("fopen() failed.\n");
    }
    if (fseek(file, 0, SEEK_END)) {
        errexit("fprintf() failed.\n");
    }
    if ((len = ftell(file)) == -1) {
        errexit("ftell() failed.\n");
    }

    rewind(file);

    if ((*buf = malloc(len))) {
        if (fread(*buf, 1, len, file) != len) {
            errexit("fread() failed.\n");
        }
    } else {
        errexit("malloc() failed.\n");
    }
    if (fclose(file)) {
        errexit("fclose() failed.\n");
    }

    return len;
}

/**
 * Parse a msg_t struct from buf.
 *
 * This function is intentionally unsafe, see UNSAFE blocks for more details.
 */
msg_t *
parse_msg(uint8_t *buf, size_t buf_len) {
    size_t idx = 0;
    msg_t *msg;

    if (!(msg = malloc(sizeof(msg_t)))) {
        errexit("malloc() for msg failed.\n");
    }

    // Fail gracefully if file is too short to possibly contain a valid msg.
    if (buf_len < MIN_MSG_SIZE) {
        errexit("Impossibly short input buffer.\n");
    }

    if (buf[idx] != MSG_TYPE_CLIENT_HELLO) {
        errexit("Unrecognized message type.\n");
    }
    // read msg type
    msg->type = buf[idx++];
    // read addr length
    msg->addr_len = buf[idx++];
    // read uname length
    msg->uname_len = buf[idx++];
    // read overall msg len
    msg->len = (buf[idx] << 8) | buf[idx+1];
    idx += 2;

    if (!(msg->addr = malloc(msg->addr_len+1))) {
        errexit("malloc() failed for msg->addr.\n");
    }

    if (!(msg->uname = malloc(msg->uname_len+1))) {
        errexit("malloc() failed for msg->uname.\n");
    }

    // UNSAFE: read of buf[idx] without checking idx+msg->addr_len < buf_len
    memcpy(msg->addr, &(buf[idx]), msg->addr_len);
    idx += msg->addr_len;
    msg->addr[msg->addr_len] = '\0';

    // UNSAFE: read of buf[idx] without checking idx+msg->uname_len < buf_len
    memcpy(msg->uname, &(buf[idx]), msg->uname_len);
    msg->uname[msg->uname_len] = '\0';

    // for user privacy, zero-out message buffer when we're done with it
    // UNSAFE: writes to buf without verifying msg->len <= buf_len
    memset(buf, 0, msg->len);

    return msg;
}

int
main(int argc, char *argv[]) {
    uint8_t *data;
    size_t len;
    
    if (argc != 2) {
        errexit("Usage: msg-parse msg-file\n");
    }

    len = read_file(argv[1], &data);
    msg_t *msg = parse_msg(data, len);    

    printf("Username: %s\n", msg->uname);
    printf("Contact: %s\n", msg->addr);

    free(msg->uname);
    free(msg->addr);
    free(msg);
    free(data);

    return 0;
}
