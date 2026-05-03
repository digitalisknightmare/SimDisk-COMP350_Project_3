#include <stdio.h>
#include <string.h>

int const BLOCK_SIZE = 544;
int const NUM_BLOCKS = 100;

// Here's how the filesystem works... it's just a long list of chars.
// Each 'block' is 544 characters, 32 for name, 512 for information. They
// aren't inforced in any particular data structure. You simply need arthmatic
// to move around and compute where things should be.

// The reason for that is the first 10 blocks (the freemap) won't adhere to the
// same data structure as cleanly. The freemap range is just true or false, 1 or 0
// if a certain block is free... the rest of the data has structure, the format being
// [name 32 chars][data 512 chars] but to get them to work together I decided too use
// chars for everything.

// the freemap is simple. The character in the 10th character of the filesystem (1 or 0)
// tells you if the 10th BLOCK (so much further down the character chain) is available.
// The 11th position character of filesystem tells you if the 11th block is available and
// so on.

char filesystem[BLOCK_SIZE * NUM_BLOCKS];
char *filetable[NUM_BLOCKS];

void user_prompts()
{
    printf("Available commands:\n"
           "--> format\n"
           "--> create <filename>\n"
           "--> read <filename>\n"
           "--> write <filename> <type the content after the prompt>\n"
           "--> delete <filename>\n"
           "--> ls\n"
           "--> exit\n"
           "--> help\n");
}

int match(char *first, char *second)
{

    // Compares 2 strings to see if they're identical.
    // strcmp I keep forgetting needs to be checked against 0,
    // (return 0 meaning they match) so this makes it easier.

    return strcmp(first, second) == 0;
}

void format()
{

    // Sets first 10 blocks (freemap range) all to 0. Meaning
    // everything is free.

    for (int i = 0; i < BLOCK_SIZE * 10; i++)
    {
        filesystem[i] = '0';
    }
}

int find_first_empty_block()
{

    // Returns a block number (1, 2, 3 etc) thats
    // currently free. Returns -1 if none are.

    int empty_block_nbr;
    int out_of_room_flag = 1;
    for (int i = 10; i < BLOCK_SIZE * 10; i++)
    {
        if (filesystem[i] == '0')
        {
            empty_block_nbr = i;
            out_of_room_flag = 0;
            return empty_block_nbr;
        }
    }
    return -1;
}

void write_name_to_block(int block_nbr, char *filename)
{

    // Given a block to write to (i.e. block 4) write a filename
    // to that block. This does NOT handle filetable updates.

    for (int i = 0; i < 32; i++)
    {
        filesystem[block_nbr * BLOCK_SIZE + i] = filename[i];
    }
}

void write_data_to_block(int block_nbr, char *data)
{

    // Given a block to write to (i.e. block 4) write a data
    // to that block.

    for (int i = 0; i < 512; i++)
    {
        filesystem[block_nbr * BLOCK_SIZE + 32 + i] = data[i];
    }
}

int get_block_number(char *filename)
{

    // Given a filename (ie. file2) get the blocknumber (i.e. 4)
    // associated with it, return -1 if no match.

    int block_nbr;
    int file_not_found_flag = 1;
    for (int i = 0; i < NUM_BLOCKS; i++)
    {
        if (filetable[i] != NULL && match(filetable[i], filename))
        {
            block_nbr = i;
            file_not_found_flag = 0;
            return block_nbr;
        }
    }
    return -1;
}

void print_block_contents(int block_nbr)
{

    // Given a block number print all its contents to the terminal.

    for (int i = 0; i < 512; i++)
    {
        if (filesystem[block_nbr * BLOCK_SIZE + 32 + i] == '\0')
        {
            break;
        }
        printf("%c", filesystem[block_nbr * BLOCK_SIZE + 32 + i]);
    }
    printf("\n");
}

void create(char *filename)
{
    int empty_block_nbr = find_first_empty_block();
    write_name_to_block(empty_block_nbr, filename);
    char data[544];
    printf("Data: ");
    scanf(" %[^\n]", data);
    write_data_to_block(empty_block_nbr, data);
    filetable[empty_block_nbr] = strdup(filename);
    filesystem[empty_block_nbr] = '1';
}
void read(char *filename)
{
    int block_nbr = get_block_number(filename);
    print_block_contents(block_nbr);
}
void write(char *filename)
{
    int block_nbr = get_block_number(filename);
    char data[544];
    printf("Data: ");
    scanf(" %[^\n]", data);
    write_data_to_block(block_nbr, data);
}
void delete(char *filename)
{
    int block_nbr = get_block_number(filename);
    filetable[block_nbr] = NULL;
    filesystem[block_nbr] = '1';
}
void ls()
{
    for (int i = 0; i < NUM_BLOCKS; i++)
    {
        if (filetable[i] == NULL)
        {
            continue;
        }
        printf("%s\n", filetable[i]);
    }
}

void execute_command(char *command)
{
    // Can use hashmap here, but wanna keep it simple.
    char *token = strtok(command, " ");
    if (match(command, "format"))
    {
        format();
    }
    else if (match(token, "create"))
    {
        char *filename = strtok(NULL, " ");
        create(filename);
    }
    else if (match(token, "read"))
    {
        char *filename = strtok(NULL, " ");
        read(filename);
    }
    else if (match(token, "write"))
    {
        char *filename = strtok(NULL, " ");
        write(filename);
    }
    else if (match(token, "delete"))
    {
        char *filename = strtok(NULL, " ");
        delete (filename);
    }
    else if (match(token, "ls"))
    {
        ls();
    }
    else if (match(command, "help"))
    {
        user_prompts();
    }
    else
    {
        printf("Command not recognised.\n");
    }
}

void main_loop()
{
    while (1)
    {
        char command[256];
        printf("> ");
        scanf(" %[^\n]", command);
        if (match(command, "exit"))
        {
            break;
        }
        execute_command(command);
    }
}

int main()
{
    printf("Formmatting disk...\n");
    format();
    printf("Disk formatted successfully. FreeMap blocks 0-9 are now allocated.\n");
    printf("Loading file table from disk...\n");
    printf("File table loaded successfully.\n");
    printf("Welcome to the simple file system simulator.\n");
    user_prompts();
    main_loop();
    return 0;
}
