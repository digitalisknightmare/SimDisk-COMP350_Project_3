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
    class FileSystem:
    def __init__(self):
        # The disk has 100 blocks, 512 bytes each
        self.TOTAL_BLOCKS = 100
        self.BLOCK_SIZE = 512
        self.disk = bytearray(self.TOTAL_BLOCKS * self.BLOCK_SIZE)
        
        # Free Map and File Table arrays
        self.free_map = [0] * self.TOTAL_BLOCKS
        self.file_table = [None] * self.TOTAL_BLOCKS
        
        # Automatically format on startup
        self.format(suppress_message=True)

    def format(self, suppress_message=False):
        """Initialize the entire disk, setting blocks to free and clearing the file table."""
        self.disk = bytearray(self.TOTAL_BLOCKS * self.BLOCK_SIZE)
        
        # Initialize Free Map: First 10 blocks are reserved
        for i in range(10):
            self.free_map[i] = 1
        for i in range(10, self.TOTAL_BLOCKS):
            self.free_map[i] = 0
            
        # Clear the File Table
        for i in range(self.TOTAL_BLOCKS):
            self.file_table[i] = None
            
        if not suppress_message:
            print("Success: Disk formatted successfully.")

    def create(self, filename):
        """Find the first available free block, allocate it, and associate a new filename."""
        if len(filename) > 32:
            print("Error: Filename exceeds the 32-byte limit.")
            return
        if filename in self.file_table:
            print(f"Error: A file named '{filename}' already exists.")
            return

        # Find the first free block (skipping the 10 reserved blocks)
        block_idx = -1
        for i in range(10, self.TOTAL_BLOCKS):
            if self.free_map[i] == 0:
                block_idx = i
                break

        if block_idx == -1:
            print("Error: Disk is full. No free blocks available.")
            return

        # Allocate in map and update file table
        self.free_map[block_idx] = 1
        self.file_table[block_idx] = filename

        # Write the filename to the first 32 bytes of the physical block
        start_byte = block_idx * self.BLOCK_SIZE
        name_bytes = filename.encode('utf-8')
        self.disk[start_byte : start_byte + len(name_bytes)] = name_bytes

        print(f"Success: File '{filename}' created at block {block_idx}.")

    def read(self, filename):
        """Find the file's block and print its data contents to the screen."""
        if filename not in self.file_table:
            print(f"Error: File '{filename}' does not exist.")
            return

        block_idx = self.file_table.index(filename)
        
        # Data starts at byte 32 within the block
        start_byte = block_idx * self.BLOCK_SIZE + 32
        end_byte = (block_idx + 1) * self.BLOCK_SIZE

        # Read data and strip any trailing null bytes
        data_bytes = self.disk[start_byte:end_byte].rstrip(b'\x00')
        print(f"\n--- Contents of {filename} ---")
        print(data_bytes.decode('utf-8', errors='replace'))
        print("------------------------------\n")

    def write(self, filename, data):
        """Find the file's block and write the new data to it."""
        if filename not in self.file_table:
            print(f"Error: File '{filename}' does not exist.")
            return

        data_bytes = data.encode('utf-8')
        if len(data_bytes) > 480:
            print("Error: Data exceeds the maximum file size of 480 bytes.")
            return

        block_idx = self.file_table.index(filename)
        start_byte = block_idx * self.BLOCK_SIZE + 32
        end_byte = start_byte + len(data_bytes)

        # Clear any existing data in the data segment of the block
        self.disk[block_idx * self.BLOCK_SIZE + 32 : (block_idx + 1) * self.BLOCK_SIZE] = b'\x00' * 480
        
        # Write the new data
        self.disk[start_byte:end_byte] = data_bytes
        print(f"Success: Data written to '{filename}'.")

    def delete(self, filename):
        """Free the block in the Free Map and remove the file from the file table."""
        if filename not in self.file_table:
            print(f"Error: File '{filename}' does not exist.")
            return

        block_idx = self.file_table.index(filename)
        
        # Free the block and remove from table
        self.free_map[block_idx] = 0
        self.file_table[block_idx] = None
        
        # Overwrite the physical disk block with null bytes
        start_byte = block_idx * self.BLOCK_SIZE
        end_byte = (block_idx + 1) * self.BLOCK_SIZE
        self.disk[start_byte:end_byte] = b'\x00' * self.BLOCK_SIZE
        
        print(f"Success: File '{filename}' deleted.")

    def ls(self):
        """List all the files currently stored in the file system."""
        files_found = False
        print("\nFiles in root directory:")
        for i in range(10, self.TOTAL_BLOCKS):
            if self.file_table[i] is not None:
                print(f" - {self.file_table[i]} (Stored in Block {i})")
                files_found = True
        
        if not files_found:
            print(" (Empty)")
        print()

def main():
    fs = FileSystem()
    print("Welcome to the Simulated File System!")
    print("Available commands: format, create <file>, read <file>, write <file> <data>, delete <file>, ls, exit")
    
    while True:
        try:
            user_input = input("fs> ").strip()
            if not user_input:
                continue
                
            # Split into command, filename, and data (max 3 parts)
            cmd_parts = user_input.split(maxsplit=2)
            operation = cmd_parts[0].lower()

            if operation == "exit":
                print("Exiting File System...")
                break
            elif operation == "format":
                fs.format()
            elif operation == "ls":
                fs.ls()
            elif operation == "create":
                if len(cmd_parts) < 2:
                    print("Usage: create <filename>")
                else:
                    fs.create(cmd_parts[1])
            elif operation == "read":
                if len(cmd_parts) < 2:
                    print("Usage: read <filename>")
                else:
                    fs.read(cmd_parts[1])
            elif operation == "write":
                if len(cmd_parts) < 3:
                    print("Usage: write <filename> <data string>")
                else:
                    fs.write(cmd_parts[1], cmd_parts[2])
            elif operation == "delete":
                if len(cmd_parts) < 2:
                    print("Usage: delete <filename>")
                else:
                    fs.delete(cmd_parts[1])
            else:
                print("Unknown command. Please use one of the specified system calls.")
        except KeyboardInterrupt:
            print("\nExiting File System...")
            break
        except Exception as e:
            print(f"An unexpected error occurred: {e}")

if __name__ == "__main__":
    main()
    
