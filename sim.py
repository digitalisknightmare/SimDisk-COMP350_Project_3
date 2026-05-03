class FileSystem:
    def __init__(self):
#The disk has 100 blocks, 512 bytes each
        self.TOTAL_BLOCKS = 100
        self.BLOCK_SIZE = 512
        self.disk = bytearray(self.TOTAL_BLOCKS * self.BLOCK_SIZE)

#Free Map and File Table arrays
        self.free_map = [0] * self.TOTAL_BLOCKS
        self.file_table = [None] * self.TOTAL_BLOCKS

#Automatically format on startup
        self.format(suppress_message=True)

    def format(self, suppress_message=False):
        """Initialize the entire disk, setting blocks to free and clearing the file table."""
        self.disk = bytearray(self.TOTAL_BLOCKS * self.BLOCK_SIZE)

#Initialize Free Map : First 10 blocks are reserved
        for i in range(10):
            self.free_map[i] = 1
        for i in range(10, self.TOTAL_BLOCKS):
            self.free_map[i] = 0

#Clear the File Table
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

#Find the first free block(skipping the 10 reserved blocks)
        block_idx = -1
        for i in range(10, self.TOTAL_BLOCKS):
            if self.free_map[i] == 0:
                block_idx = i
                break

        if block_idx == -1:
            print("Error: Disk is full. No free blocks available.")
            return

#Allocate in map and update file table
        self.free_map[block_idx] = 1
        self.file_table[block_idx] = filename

#Write the filename to the first 32 bytes of the physical block
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

#Data starts at byte 32 within the block
        start_byte = block_idx * self.BLOCK_SIZE + 32
        end_byte = (block_idx + 1) * self.BLOCK_SIZE

#Read data and strip any trailing null bytes
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

#Clear any existing data in the data segment of the block
        self.disk[block_idx * self.BLOCK_SIZE + 32 : (block_idx + 1) * self.BLOCK_SIZE] = b'\x00' * 480

#Write the new data
        self.disk[start_byte:end_byte] = data_bytes
        print(f"Success: Data written to '{filename}'.")

    def delete(self, filename):
        """Free the block in the Free Map and remove the file from the file table."""
        if filename not in self.file_table:
            print(f"Error: File '{filename}' does not exist.")
            return

        block_idx = self.file_table.index(filename)

#Free the block and remove from table
        self.free_map[block_idx] = 0
        self.file_table[block_idx] = None

#Overwrite the physical disk block with null bytes
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

        #Split into command, filename, and data(max 3 parts)
            cmd_parts = user_input.split(maxsplit = 2)
            operation = cmd_parts[0].lower()

            if operation == "exit" :
                print("Exiting File System...")
                break
            elif operation == "format" :
                fs.format()
            elif operation == "ls" :
                fs.ls() 
            elif operation == "create" :
                if len (cmd_parts) < 2 :
                    print("Usage: create <filename>")
                else : fs.create(cmd_parts[1]) 
            elif operation == "read" :
                if len (cmd_parts) < 2 :
                    print("Usage: read <filename>")
                else : fs.read(cmd_parts[1])
            elif operation == "write" :
                if len (cmd_parts) < 3 : 
                    print("Usage: write <filename> <data string>")
                else :
                    fs.write(cmd_parts[1], cmd_parts[2])
            elif operation =="delete" :
                if len (cmd_parts) < 2 : 
                    print("Usage: delete <filename>") 
                else: fs.delete(cmd_parts[1])
            else : 
                print("Unknown command. Please use one of the specified system calls.")
                                                                                                                                                    
        except KeyboardInterrupt :
            print("\nExiting File System...")
            break
        except Exception as e :
            print(f"An unexpected error occurred: {e}")

if __name__ == "__main__" :
    main()
