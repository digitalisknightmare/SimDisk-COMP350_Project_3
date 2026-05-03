#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// ⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻ Storage Definitions: ⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻

#define BLOCKTOTAL 100 // Total number of blocks that the system has before running out of space.
#define RESERVE 10 // Used for free map
#define DATA_TOTAL 512 // The size of the entire program.
#define FILENAME_SIZE 32 // Used by the free map.
#define DATA_SIZE 480 // 512 minus the space taken by the free map.

#define MAX_Horizontal Line SymbolSIZE 10
#define MAX_LINE 80 // Both used in command processing for the osh shell.

// ⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻ Structures: ⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻

// filename is used to communicate with fileTable, data... stores data.
typedef struct {
    char filename[FILENAME_SIZE];
    char data[DATA_SIZE];
} Block;

int freeMap[BLOCKTOTAL];
char fileTable[BLOCKTOTAL][FILENAME_SIZE];  // filename at each block index

// Our storage disk!
Block disk[BLOCKTOTAL];

// ⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻ Finders: ⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻

// Used to find any file by name. Used by nearly everything!
int findFile(const char *filename) {
    for(int i = 10; i < BLOCKTOTAL; i++){
        if ((strcasecmp(fileTable[i],filename) == 0)) {
            return i;
        }
    }
    printf("That file doesn't exist.");
    return -1; // This is an easy way to make sure it returns an index that doesn't exist. MAKE SURE THAT ALL FUNCTIONS THAT USE THIS (and findFreeBlock) CHECK IF THEY RETURN -1, AND RETURN ERRORS IF SO.
}

// Used to find an empty block. Incredibly useful for creating new files.
int findFreeBlock() {
    for(int i = 10; i < BLOCKTOTAL; i++){
        if (fileTable[i][0] == '\0') { // Looks for a null value.
            return i;
        }
    }
    return -1;
}

// ⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻ Functions: ⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻

// Works as a test command for now, but absolutely no persistance. This is why "Loading file table from disk... File table loaded successfully." is not built in yet.
void format() {
    printf("Formatting free disk...");
    // FORMAT FREE MAP.
    for(int i = 0; i < BLOCKTOTAL; i++) { // First, fresh clear the entire program. This insures that there won't be leftover data from previous iterations.
        freeMap[i] = 0;
    }
    for (int i = 0; i < RESERVE; i++) { // Then reclaim the reserve (Freemap).
        freeMap[i] = 1;
    }
    printf("Disk formatted succesfully. FreeMap blocks 0-9 are now allocated.");
    // FORMAT FILE TABLE
    memset(fileTable, 0, sizeof(fileTable)); // memset is useful to set all of fileTable + disk to NULL.
    // FORMAT DISK
    memset(disk, 0, sizeof(disk));
}

// ALL OF THESE NEED COMPLETING!!!!
void create(const char *filename) {

}

void read(const char *filename) {

}

void write(const char *filename) {

}

void delete(const char *filename) {

}

void ls() {

}

// Used when starting program, and for the help command.
void userPrompts() {
    printf("Available commands:\n"
    "--> format\n"
    "--> create <filename>\n"
    "--> read <filename>\n"
    "--> write <filename> <type the content after the prompt>\n"
    "--> delete <filename>\n"
    "--> ls\n"
    "--> exit\n"
    "--> help");
}

// ⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻ User/Program Interaction: ⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻
// Used to turn a user's prompt into arguments a computer can understand.
char **makeArgs(char *line) {
    int position = 0, maxSize=MAX_LINE;
    char **tokens = malloc(maxSize *sizeof(char *));
    char *token;

    // Keeps making a token until hitting one of the banned letters, at which point it creates a new argument.
    token = strtok(line, " \t\r\n\a");
    while (token!=NULL) {
        tokens[position] = token;
        position++;
        if (position >= maxSize) {
            printf("Too long! Please keep commands to a maximum of 80 characters!");
            exit(EXIT_FAILURE);
        }
        // Null terminated.
        token = strtok(NULL, " \t\r\n\a");
    }
    // Null terminated.
    tokens[position] = NULL;
    return tokens;
}

// Will ensure that the user entered a correct option, and send them to the correct function as needed.
int runIt(char** args) {
    // This will loop through all possible commands and divert the command to the proper function as needed.
    if (strcasecmp(args[0],"exit") == 0) { // strcasecmp allows the program to ignore case sensitivity.
        return 0; // canLoop is now false. Main loop ends.
    } else if (strcasecmp(args[0],"format") == 0) {
        format();
    } else if (strcasecmp(args[0],"create") == 0) {
        create(args[1]);
    } else if (strcasecmp(args[0],"read") == 0) {
        read(args[1]);
    } else if (strcasecmp(args[0],"write") == 0) {
        write(args[1]);
    } else if (strcasecmp(args[0],"delete") == 0) {
        delete(args[1]);
    } else if (strcasecmp(args[0],"ls") == 0) {
        ls();
    } else if (strcasecmp(args[0],"help") == 0) {
        userPrompts();
    } else {
    printf("Not a command! Type 'Help' for a list of available commands.");
    }
    return 1; // Program loops when this method returns 1, making status true.
}
// ⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻ Main Loop/Function: ⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻
void oshLoop (void) {
    char buffer[100];
    int maxSize = MAX_LINE;
    char *line; // Used to save the text and translate it to a program.
    char **args; // Used to execute said program.
    int canLoop = 0; // Used to determine when the program is finished and can loop.

    // Initialize the entire program real fast.
    format();
    userPrompts();
    do {
        printf("> ");
        fflush(stdout);
        fgets(buffer, maxSize, stdin);
        if (strcmp("\n",buffer) ==  0) { // Checks for empty lines, and immediately skips all other processes if so.
            continue;
        }

        line = strdup(buffer); // Line is more practical as a long term option.
        args = makeArgs(line); // Will turn line into a set of arguments for a program.

        canLoop = runIt(args); // Since runIt returns 1 (True), it'll tell the program that it can loop as soon as it finishes.
        // Resets the values before code loops to prevent garbage data.
        free(line);
        free(args);
    } while (canLoop);
}

int main(int argc, char **argv) {
    oshLoop();

    // Preforms any necessary cleanup once the oshLoop ends, by an error or by the exit command.
    return EXIT_SUCCESS;
}







     ---- Saver ----
    void format(FileSystem *fs, bool suppress_message) {
    // Clear the physical disk
    memset(fs->disk, 0, TOTAL_BLOCKS * BLOCK_SIZE);
    
    // Initialize Free Map: First 10 blocks reserved
    for (int i = 0; i < RESERVED_BLOCKS; i++) {
        fs->free_map[i] = 1;
    }
    for (int i = RESERVED_BLOCKS; i < TOTAL_BLOCKS; i++) {
        fs->free_map[i] = 0;
    }
    
    // Clear the File Table
    memset(fs->file_table, 0, sizeof(fs->file_table));
    
    if (!suppress_message) {
        printf("Success: Disk formatted successfully.\n");
    }
}

void init_fs(FileSystem *fs) {
    format(fs, true);
}

void create_file(FileSystem *fs, const char *filename) {
    if (strlen(filename) >= MAX_FILENAME) {
        printf("Error: Filename exceeds the 32-byte limit.\n");
        return;
    }
    if (find_file_index(fs, filename) != -1) {
        printf("Error: A file named '%s' already exists.\n", filename);
        return;
    }

    int block_idx = -1;
    for (int i = RESERVED_BLOCKS; i < TOTAL_BLOCKS; i++) {
        if (fs->free_map[i] == 0) {
            block_idx = i;
            break;
        }
    }

    if (block_idx == -1) {
        printf("Error: Disk is full. No free blocks available.\n");
        return;
    }

    // Allocate in map and update file table
    fs->free_map[block_idx] = 1;
    strncpy(fs->file_table[block_idx], filename, MAX_FILENAME - 1);
    fs->file_table[block_idx][MAX_FILENAME - 1] = '\0'; // Ensure null-termination

    // Write the filename to the first 32 bytes of the physical block
    int start_byte = block_idx * BLOCK_SIZE;
    strncpy((char *)&fs->disk[start_byte], filename, MAX_FILENAME);

    printf("Success: File '%s' created at block %d.\n", filename, block_idx);
}

void read_file(FileSystem *fs, const char *filename) {
    int block_idx = find_file_index(fs, filename);
    if (block_idx == -1) {
        printf("Error: File '%s' does not exist.\n", filename);
        return;
    }

    // Data starts at byte 32 within the block
    int start_byte = block_idx * BLOCK_SIZE + MAX_FILENAME;

    printf("\n--- Contents of %s ---\n", filename);
    // Print bytes until we hit a null terminator or max data limit
    for (int i = 0; i < MAX_DATA; i++) {
        char c = fs->disk[start_byte + i];
        if (c == '\0') break;
        putchar(c);
    }
    printf("\n--\n\n");
}

void write_file(FileSystem *fs, const char *filename, const char *data) {
    int block_idx = find_file_index(fs, filename);
    if (block_idx == -1) {
        printf("Error: File '%s' does not exist.\n", filename);
        return;
    }

    if (strlen(data) > MAX_DATA) {
        printf("Error: Data exceeds the maximum file size of 480 bytes.\n");
        return;
    }

    int start_byte = block_idx * BLOCK_SIZE + MAX_FILENAME;

    /
    memset(&fs->disk[start_byte], 0, MAX_DATA);
    
    // Write the new data
    strncpy((char *)&fs->disk[start_byte], data, MAX_DATA);
    
    printf("Success: Data written to '%s'.\n", filename);
}

void delete_file(FileSystem *fs, const char *filename) {
    int block_idx = find_file_index(fs, filename);
    if (block_idx == -1) {
        printf("Error: File '%s' does not exist.\n", filename);
        return;
    }

    // Free the block and remove from table
    fs->free_map[block_idx] = 0;
    memset(fs->file_table[block_idx], 0, MAX_FILENAME);

    // Overwrite the physical disk block with null bytes
    int start_byte = block_idx * BLOCK_SIZE;
    memset(&fs->disk[start_byte], 0, BLOCK_SIZE);

    printf("Success: File '%s' deleted.\n", filename);
}

void ls(FileSystem *fs) {
    bool files_found = false;
    printf("\nFiles in root directory:\n");
    for (int i = RESERVED_BLOCKS; i < TOTAL_BLOCKS; i++) {
        if (fs->free_map[i] == 1 && fs->file_table[i][0] != '\0') {
            printf(" - %s (Stored in Block %d)\n", fs->file_table[i], i);
            files_found = true;
        }
    }

    if (!files_found) {
        printf(" (Empty)\n");
    }
    printf("\n");
}
