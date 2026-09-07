#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

int main()
{
    pid_t pid = getpid();  // Get process ID
    uid_t uid = getuid();  // Get user ID

    // Open (or create) a file for writing
    int fd = open("log.txt", O_CREAT | O_WRONLY | O_APPEND, 0644);

    if (fd < 0)
    {
        write(STDERR_FILENO, "Failed to open file.\n", 22);
        return 1;
    }

    // Prepare output
    char buffer[128];
    int len = snprintf(
        buffer,
        sizeof(buffer),
        "PID: %d \t UID: %d \n",
        pid,
        uid
    );

    write(STDOUT_FILENO, "Enter a message: ", 17);  // Write to file
    char input[64];
    int input_len = read(STDIN_FILENO, input, 63);
    write(fd, "User Input: ", 12);
    write(fd, input, input_len);

    write(
        STDOUT_FILENO,
        "Log written to file.\n",
        22
    );  // Confirmation

    close(fd);

    return 0;
}
