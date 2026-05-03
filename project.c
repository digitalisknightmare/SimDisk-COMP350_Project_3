#include <stdio.h>
#include <string.h>

int const BLOCK_SIZE = 544;
int const NUM_BLOCKS = 100;

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
    return strcmp(first, second) == 0;
}

void format()
{
    for (int i = 0; i < BLOCK_SIZE * 10; i++)
    {
        filesystem[i] = '0';
    }
}

int find_first_empty_block()
{
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
    for (int i = 0; i < 32; i++)
    {
        filesystem[block_nbr * BLOCK_SIZE + i] = filename[i];
    }
}

void write_data_to_block(int block_nbr, char *data)
{
    for (int i = 0; i < 512; i++)
    {
        filesystem[block_nbr * BLOCK_SIZE + 32 + i] = data[i];
    }
}

int get_block_number(char *filename)
{
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

void executecommand(char *command)
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

void mainloop()
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
        executecommand(command);
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
    mainloop();
    return 0;
}