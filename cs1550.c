/*
	FUSE: Filesystem in Userspace
	Copyright (C) 2001-2007  Miklos Szeredi <miklos@szeredi.hu>

	This program can be distributed under the terms of the GNU GPL.
	See the file COPYING.

	gcc -Wall `pkg-config fuse --cflags --libs` cs1550.c -o cs1550
*/

//By Brian Sisco and Dylan White


#define	FUSE_USE_VERSION 26

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

//size of a disk block
#define	BLOCK_SIZE 512

//we'll use 8.3 filenames
#define	MAX_FILENAME 8
#define	MAX_EXTENSION 3

//How many files can there be in one directory?
#define	MAX_FILES_IN_DIR (BLOCK_SIZE - (MAX_FILENAME + 1) - sizeof(int)) / \
	((MAX_FILENAME + 1) + (MAX_EXTENSION + 1) + sizeof(size_t) + sizeof(long))

//How much data can one block hold?
#define	MAX_DATA_IN_BLOCK (BLOCK_SIZE)

struct cs1550_directory_entry
{
	char dname[MAX_FILENAME	+ 1];	//the directory name (plus space for a nul)
	int nFiles;			//How many files are in this directory. 
					//Needs to be less than MAX_FILES_IN_DIR

	struct cs1550_file_directory
	{
		char fname[MAX_FILENAME + 1];	//filename (plus space for nul)
		char fext[MAX_EXTENSION + 1];	//extension (plus space for nul)
		size_t fsize;			//file size
		long nStartBlock;		//where the first block is on disk
	} files[MAX_FILES_IN_DIR];		//There is an array of these
};

int last_allocation_start = 0;
typedef struct cs1550_directory_entry cs1550_directory_entry;
typedef struct cs1550_file_directory cs1550_file_directory;

struct cs1550_disk_block
{
	//All of the space in the block can be used for actual data
	//storage.
	char data[MAX_DATA_IN_BLOCK];
};

typedef struct cs1550_disk_block cs1550_disk_block;


void parse_path(const char* path, char* directory, char* filename, char* extension){
	sscanf(path, "/%[^/]/%[^.].%s", directory, filename, extension);
}

//returns the index of a file in a directory
int find_file(cs1550_directory_entry* dir, char* file_target, char* ext_target){
	int i;
	for (i = 0; i < dir->nFiles; i++){
		cs1550_file_directory cur_file = dir->files[i];
		if (strcmp(file_target, cur_file.fname) == 0
			&& strcmp(ext_target, cur_file.fext) == 0){
			return i;
		}
	}
	return -1;
}

//looks for a directory and returns index of directory entry in .directories
int find_dir(cs1550_directory_entry* dir_struct, char* dir_name){
	FILE* f = fopen(".directories","rb");
	fread(dir_struct, sizeof(*dir_struct), 1, f);
    //while not at end of directories, while name is not correct
    while (strcmp(dir_struct->dname,dir_name)!=0 && !feof(f)){
        int read = fread(dir_struct, sizeof(*dir_struct), 1, f);
        if (read == 0) break;
    }
	//if the name was correct then the directory was found at sizeof(dir_struct) bytes ago
	if (strcmp(dir_struct->dname,dir_name)==0){
		int result = ftell(f) - sizeof(*dir_struct);
		fclose(f);
		return result;
	}
	else{ 
		fclose(f);
		return -1;
	}
}

void touch(char* path){
	FILE* f = fopen(path,"a");
	fclose(f);
}


//for testing
void print_bitmap()
{
	int i = 0;
	//opening the disk
	touch(".disk"); //just in case it wasn't precreated
	char curr_char = 0;
	int size = last_bitmap_index(); //get the number of blocks in bitmap
	for(i = 0; i <= size; i++) //for each byte in each block
	{
		int this_bit = get_state(i);
		printf("%u", this_bit); 
		if(i % 20 == 19){
			//printf("\n");
		}
		if(i % 512 == 511)
		{
			printf("\n");
		}
	}
	printf("\n");
	return;
}
//fn that calculates size (in blocks) of bitmap:
int get_bitmap_size()
{
	//one line oversimplification:
	//return ((x / (512* 8) + 1);
	//x comes from fseek, ftelling the .disk file
	//x = size of .disk in bytes / 512

	touch(".disk"); //just in case it wasn't precreated
	FILE* f = fopen(".disk", "r+b");
	fseek(f, 0, SEEK_END);
	int bytes_on_disk = ftell(f);
	int blocks_on_disk = bytes_on_disk / 512; //keep as is to round down so you don't have a half sized block at end
	int bitmap_bytes_needed = blocks_on_disk / 8 + 1;
	int bitmap_blocks_needed = bitmap_bytes_needed / 512 + 1;
	fclose(f);
	return bitmap_blocks_needed;
}

//Returns 1 if the block is allocated
//Returns 0 otherwise
int get_state(int block_idx){
	FILE* f = fopen(".disk", "r+b");
	int byte_idx = block_idx / 8;
	int bit_idx = block_idx % 8;
	fseek(f,byte_idx,SEEK_SET);
	unsigned char target_byte;
	fread(&target_byte,sizeof(char),1,f);
	fclose(f);
	unsigned char operand = 1 << bit_idx;
	unsigned char result = target_byte & operand;
	result = result >> bit_idx;
	return result;
}


//since set and unset are so similiar, they just call this.
void change_bit(int i, int sign)
{	
	//printf("changing bit %d to %d\n",i,sign);
	//opening the disk
	touch(".disk"); //just in case it wasn't precreated
	FILE* f = fopen(".disk", "r+b");
	//we were given a bit to set. we must adjust this to a bit within a byte
	int byte_to_set = i / 8;
	i = i % 8;
	//now we are setting the ith bit in the byte_to_set byte
	fseek(f, byte_to_set, SEEK_SET);
	//to set a single bit, we must read what is already there and or on the bit we want to set
	unsigned char curr_byte = 0;
	fread(&curr_byte, sizeof(char), 1, f);
	unsigned char operand = 1 << i;
	if(sign == -1) //if we are unsetting the bit
	{
		operand = ~operand;
		curr_byte = curr_byte & operand;
	}
	else //if we are setting the bit
	{
		curr_byte = curr_byte | operand;
	}
	fseek(f, -1, SEEK_CUR);
	fwrite(&curr_byte, sizeof(char), 1, f);
	fclose(f);
	return;
}

//the next two functions are sort of wrappers for the above one

//set the given bit in bit map to 1
void set(int i)
{
	change_bit(i, 1);
	
	return;
}    
//set the given bit in bit map to 0
void unset(int i)
{
	change_bit(i, -1);
	
	return;
}

void allocate(int start_block, int num_blocks){
	int i;
	for (i = start_block; i < start_block + num_blocks; i++){
		set(i);
	}
}

void unallocate(int start_block, int num_blocks){
	int i;
	printf("unallocating %d starting at %d\n",num_blocks,start_block);
	for (i = start_block; i < start_block + num_blocks; i++){
		unset(i);
	}
}


//makes the bitmap if it doesnt exist
void check_bitmap()
{
	int i = 0;
	//opening the disk
	touch(".disk"); //just in case it wasn't precreated
	FILE* f = fopen(".disk", "r+b");
	char first_char = 0;
	fseek(f, 0, SEEK_SET);
	fread(&first_char, sizeof(char), 1, f);
	if(first_char == 0) //then the beginning of the bitmap is zero and thus the bitmap does not exist
	{
		int bitmap_size = get_bitmap_size();
		printf("DEBUG: creating new bitmap of size %d\n",bitmap_size);
		for(i = 0; i < bitmap_size; i++)
		{
			set(i);
		}
	print_bitmap();
	}
	fclose(f);
	return;
}







int last_bitmap_index()
{
    touch(".disk"); //just in case it wasn't precreated
    FILE* f = fopen(".disk", "r+b");
    fseek(f, 0, SEEK_END);
    int bytes_on_disk = ftell(f);
    int blocks_on_disk = bytes_on_disk / 512; //keep as is to round down so you don't have a half sized block at end
    fclose(f);
    return blocks_on_disk - 1;
}


//find num_blocks contiguous free blocks
//we are using the next fit algorithm

int find_free_space(int num_blocks)
{
    //we start by openning up the .disk and reading through it bitwise
    touch(".disk");
    int run_start = -1;
    int run_length = 0;
    int cur_block = 0;//last_allocation_start + 1;
    while(cur_block <= last_bitmap_index())
    {
		int this_bit = get_state(cur_block);
		if(this_bit == 0)
        {
        	if(run_start == -1) //if this is the first in a run of 0s, record this location as the run's start
            {
            	run_start = cur_block; //combining the byte we are in with the bit in that byte to get the overall bit in the map
            }
            run_length++;
            if(run_length >= num_blocks) //we found a big enough run of 0s!
            {
            	last_allocation_start = run_start;
            	return run_start;
            }
        }
        else if(this_bit == 1)
        {
        	run_length = 0;
        	run_start = -1;
        }

		cur_block++;
        /*if(cur_block > last_bitmap_index())	//reached end of bitmap, need to restart at beginning
        {     			               		//the bitmap is not treated as "circular". The last bit is not considered contiguous with the beginning.
            run_start = -1;
            run_length = 0;
            cur_block = 0;
        }*/
    }
    return -1;
}





/*
 * Called whenever the system wants to know the file attributes, including
 * simply whether the file exists or not. 
 *
 * man -s 2 stat will show the fields of a stat structure
 */
static int cs1550_getattr(const char *path, struct stat *stbuf)
{

    int res = 0;

    memset(stbuf, 0, sizeof(struct stat));

	FILE* toucher = fopen(".directories","a");
	fclose(toucher);

    FILE* f = fopen(".directories","r");

	char dir_target[9];
	char file_target[9];
	char ext_target[3];
	dir_target[0] = 0;
	file_target[0] = 0; //initialize file target to null string
	ext_target[0] = 0;
	parse_path(path,dir_target,file_target,ext_target);

  
	struct cs1550_directory_entry cur_dir;
    fread(&cur_dir, sizeof(cur_dir), 1, f);
    //while not at end of directories, while name is not correct
    while (strcmp(cur_dir.dname,dir_target)!=0 && !feof(f)){
        int read = fread(&cur_dir, sizeof(cur_dir), 1, f);
        if (read == 0) break;
    }
    fclose(f);

	//this triggers when we find the directory
    if (strcmp(cur_dir.dname,dir_target)==0 || strcmp(path,"/")==0){
        //if we are looking for directory attributes
		if (strlen(file_target) == 0){
			stbuf->st_nlink = 2;
       		stbuf->st_mode = S_IFDIR | 0755;
		}
		//if we are looking for a file which is there
		else{
			int file_index = find_file(&cur_dir,file_target,ext_target);
			//-1 means not found
			if (file_index == -1){
				res = -ENOENT;
			}
			//Anything else means we found it
			else{
				stbuf->st_mode = S_IFREG | 0666;
				stbuf->st_nlink = 1;
				stbuf->st_size = cur_dir.files[file_index].fsize;
				stbuf->st_blksize = 512;
				stbuf->st_blocks = cur_dir.files[file_index].fsize / 512;
				if (cur_dir.files[file_index].fsize % 512 != 0){ stbuf->st_blocks++;}
			}
		}
    }
    else{
        res = -ENOENT;
    }
    return res;
}

/*d 
 * Called whenever the contents of a directory are desired. Could be from an 'ls'
 * or could even be when a user hits TAB to do autocompletion
 */
static int cs1550_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
			 off_t offset, struct fuse_file_info *fi)
{
	//Since we're building with -Wall (all warnings reported) we need
	//to "use" every parameter, so let's just cast them to void to
	//satisfy the compiler
	(void) offset;
	(void) fi;
	FILE* toucher = fopen(".directories","a");
	fclose(toucher);
	FILE* f = fopen(".directories","r");		
	struct cs1550_directory_entry cur_dir;

	//Check if we want to read the root, if so read it
	if (strcmp(path, "/") == 0){
		filler(buf, ".", NULL, 0);
		filler(buf, "..", NULL, 0);
		while(fread(&cur_dir,sizeof(cur_dir),1,f) > 0 && !feof(f)){
			filler(buf, cur_dir.dname, NULL, 0);
		}

	}
	//if we arent trying to read the root, see if the directory we want to read exists
	else{
		int found = 0;
		while(fread(&cur_dir,sizeof(cur_dir),1,f) > 0 && !feof(f)){
			if (strcmp(cur_dir.dname,path + 1)==0){
				int i;
				for (i = 0; i < cur_dir.nFiles; i++){
					char full_name[13];
					full_name[0] = 0;
					strcat(full_name,cur_dir.files[i].fname);
					if (strlen(cur_dir.files[i].fext) > 0){ strcat(full_name,".");}
					strcat(full_name,cur_dir.files[i].fext);
					filler(buf,full_name,NULL,0);
				}
			found = 1;
			}
		}
		if (!found){
			fclose(f);
			return -ENOENT;
		}
	}
	/*
	//add the user stuff (subdirs or files)
	//the +1 skips the leading '/' on the filenames
	filler(buf, newpath + 1, NULL, 0);
	*/
	fclose(f);
	return 0;
}

/*c 
 * Creates a directory. We can ignore mode since we're not dealing with
 * permissions, as long as getattr returns appropriate ones for us.
 */
static int cs1550_mkdir(const char *path, mode_t mode)
{
	
	//First: check that path is valid
	//The path is at most a slash followed by 8 characters

	char dir[9];
	char file[9];
	file[0] = 0;	//make sure file starts as zero length
	char ext[3];

	sscanf(path, "/%[^/]/%[^.].%s", dir, file, ext); 
	
	if (strlen(dir) > 9) return -ENAMETOOLONG;
	if (strlen(file) > 0) return -EPERM;
	
	//check if directory exists
	FILE* toucher = fopen(".directories","a");
	fclose(toucher);
	FILE* g = fopen(".directories","r");
	struct cs1550_directory_entry cur_dir;
	fread(&cur_dir, sizeof(cur_dir), 1, g);
	while (strcmp(cur_dir.dname,path + 1) != 0 && fread(&cur_dir, sizeof(cur_dir), 1, g) > 0);
	if (strcmp(cur_dir.dname,path + 1) == 0) return -EEXIST;

	
	fclose(g);
    FILE* f = fopen(".directories", "a");
    struct cs1550_directory_entry next_dir;
    strcpy(next_dir.dname, path + 1);
    next_dir.nFiles = 0;

    fwrite(&next_dir, sizeof(next_dir),  1, f);
    fclose(f);

	//Uncomment to delete directories file every time mkdir is called
	//f = fopen(".directories","w");
	//fclose(f);

	return 0;
}

/* w
 * Removes a directory.
 */
static int cs1550_rmdir(const char *path)
{

	//First: check that path is valid
	//The path is at most a slash followed by 8 characters

	char dir[9];
	char file[9];
	file[0] = 0;	//make sure file starts as zero length
	char ext[3];

	sscanf(path, "/%[^/]/%[^.].%s", dir, file, ext); 
	
	if (strlen(file) > 0) return -ENOTDIR;



    //creates the file if it doesn't exist
    FILE* toucher = fopen(".directories", "a");
    fclose(toucher);
    FILE* f = fopen(".directories", "r+b");

    //find the one to get rid of, record its position, and read it
    fseek(f, -sizeof(cs1550_directory_entry), SEEK_END);
    int length = ftell(f);
    struct cs1550_directory_entry temp_entry;
    fread(&temp_entry, sizeof(temp_entry), 1, f);

    //find the one to delete
    fseek(f, 0, SEEK_SET); //go back to begining
    struct cs1550_directory_entry cur_dir;
    fread(&cur_dir, sizeof(cur_dir), 1, f);
   
	//look for deletion target
	while (strcmp(cur_dir.dname, path + 1)!=0 && !feof(f)){
    	fread(&cur_dir, sizeof(cur_dir), 1, f);
    }

	//if we found deletion target, overwrite it with last element and truncate file
    if (strcmp(cur_dir.dname,path + 1) == 0){
 		fseek(f, -sizeof(cur_dir), SEEK_CUR);
 		fwrite(&temp_entry, sizeof(cur_dir), 1, f);
		fseek(f, 0, SEEK_SET);
  		int fterr = ftruncate(fileno(f), length);
		printf("TRUNCATATION LENGTH: %d\n",length);
    }
	//otherwise return ENOENT
	else{
		fclose(f);
		return -ENOENT;
	}

	fclose(f);
	return 0;
}

/* 
 * Does the actual creation of a file. Mode and dev can be ignored.
 * V1: sets file number zero to a new file starting at disk block 0
 */
static int cs1550_mknod(const char *path, mode_t mode, dev_t dev)
{
	(void) mode;
	(void) dev;
	char dir_targ[9];
	char file_targ[9];
	char ext_targ[4];
	ext_targ[0] = 0;
	file_targ[0] = 0;
	dir_targ[0] = 0;

	check_bitmap();

	parse_path(path,dir_targ,file_targ,ext_targ);
	if (strlen(file_targ) > 8 || strlen(ext_targ) > 3 ) return -ENAMETOOLONG;
	if (strlen(file_targ) == 0) return -EPERM;		//if we are trying to create a file in root, parse path returns null for file and ext strings


	cs1550_directory_entry cur_dir;
	
	//find directory, make sure it exists
	int dir_idx = find_dir(&cur_dir,dir_targ);
	if (dir_idx == -1) return -ENOENT;
	
	//make sure file does not exist
	int file_idx = find_file(&cur_dir,file_targ,ext_targ);
	if (file_idx != -1) return -EEXIST;

	//make sure there is room for another file
	//TODO: make sure there is room for another file

	//now actually make the file
	int new_file_idx = cur_dir.nFiles; 
	if (new_file_idx > MAX_FILES_IN_DIR) return -EPERM; //if the directory is full return a permission error

	strcpy(cur_dir.files[new_file_idx].fname,file_targ);
	strcpy(cur_dir.files[new_file_idx].fext,ext_targ);
	cur_dir.files[new_file_idx].fsize = 0;

	int new_file_block = -1;// find_free_space(1);
	cur_dir.files[new_file_idx].nStartBlock = new_file_block;

	cur_dir.nFiles++;

	FILE* f = fopen(".directories","r+b");
	fseek(f,dir_idx,SEEK_SET);
	fwrite(&cur_dir,sizeof(cur_dir),1,f);

	fclose(f);
	print_bitmap();
	return 0;
}

/*
 * Deletes a file
 */
static int cs1550_unlink(const char *path)
{
    char dir_target[9];
    char file_target[9];
    char ext_target[3];
    dir_target[0] = 0;
    file_target[0] = 0; //initialize file target to null string
    ext_target[0] = 0;
    parse_path(path,dir_target,file_target,ext_target);
    if(!(strlen(file_target) > 0))//it is a directory
    {
        return -EISDIR;
    }
    struct stat stbuf;
    int attr = cs1550_getattr(path, &stbuf);
	if(attr != 0)
    {
        return attr; //file was not found, return -ENOENT.
    }
    //so to delete, we must unset the bits it takes up in the bitmap and remove its entry in the listing
    //first we use dir_target to find its directory in .directories, then we find the file in that directory's list of files
    //that we can use and change to delete the file
    //int find_dir(cs1550_directory_entry* dir_struct, char* dir_name){
    cs1550_directory_entry dir_struct;
    int dir_idx = find_dir(&dir_struct, dir_target);
    if(dir_idx == -1)
    {
        return -ENOENT; //somehow missed this earlier, shouldn't happen
    }
    //so we have the directory itself on hand, now to get the file.
    int file_index = find_file(&dir_struct, file_target, ext_target);
    cs1550_file_directory the_file = dir_struct.files[file_index];
    //from the file we use its size to get how many blocks it took up.
    //If it exactly fit in x blocks, we're good, otherwise we add 1 to round up to the nearest whole block since that is what happens in allocation.
    
	//Bitmap management- only neccecary for files that take up non zero space
	if (the_file.fsize > 0){
		printf("Deleting a file of size %d\n",the_file.fsize);
		int num_blocks = the_file.fsize / 512;
    	if(the_file.fsize % 512 != 0)
    	{
        	num_blocks++;
   		}
  		unallocate(the_file.nStartBlock, num_blocks); //free the blocks it used
  	}

	  //removing all references to it in the .directories file
    //its data in .disk will eventually be overwritten since it is no longer represented in the bitmap (see call to unallocate).
    dir_struct.files[file_index] = dir_struct.files[dir_struct.nFiles - 1]; //moves the last file to where the dead one is
    dir_struct.nFiles--;

	FILE* f = fopen(".directories","r+b");
	fwrite(&dir_struct,sizeof(dir_struct),1,f);
	fclose(f);
	print_bitmap();
    return 0;
}

/* 
 * Read size bytes from file into buf starting from offset
 *
 */
static int cs1550_read(const char *path, char *buf, size_t size, off_t offset,
			  struct fuse_file_info *fi)
{
	
	(void) buf;
	(void) offset;
	(void) fi;
	(void) path;

	char dir_targ[9];
	char file_targ[9];
	char ext_targ[3];
	dir_targ[0] = 0;
	file_targ[0] = 0;
	ext_targ[0] = 0;

	touch(".disk");

	parse_path(path, dir_targ, file_targ, ext_targ);

	//check to make sure path exists
	cs1550_directory_entry cur_dir;
	int dir_exists = find_dir(&cur_dir,dir_targ);
	if (dir_exists == -1) return -ENOENT;
	int file_index = find_file(&cur_dir,file_targ,ext_targ);
	if (file_index == -1) return -ENOENT;
	cs1550_file_directory cur_file = cur_dir.files[file_index];
	//check that size is > 0
	if (size <= 0) return 0;		//Sure, why not, have your zero bytes
	//check that offset is <= to the file size
	if (offset > cur_file.fsize) return 0; //nothing left
	
	//figure out where to read from
	int read_index = cur_file.nStartBlock * 512 + offset;
	int max_read = cur_file.fsize - offset;
	if (max_read < size) size = max_read;
	//read in data
	//get bytes read and return, or error
	FILE* f = fopen(".disk","rb");
	fseek(f,read_index,SEEK_SET);
	int bytes_read = fread(buf,1,size,f);
	printf("DBUG Read %d bytes\n",bytes_read);
	fclose(f);
	//if fread returns an error, we end up returning that error.
	//clever, no?
	return bytes_read;
}

/* 
 * Write size bytes from buf into file starting from offset
 *
 */


static int cs1550_write(const char *path, const char *buf, size_t size, 
			  off_t offset, struct fuse_file_info *fi)
{
	char dir_targ[9];
	char file_targ[9];
	char ext_targ[3];
	dir_targ[0] = 0;
	file_targ[0] = 0;
	ext_targ[0] = 0;
	char* touch_targ = ".disk";
	touch(touch_targ);

	parse_path(path, dir_targ, file_targ, ext_targ);

	//check to make sure path exists
	cs1550_directory_entry cur_dir;
	int dir_index = find_dir(&cur_dir,dir_targ);
	if (dir_index == -1) return -ENOENT;
	int file_index = find_file(&cur_dir,file_targ,ext_targ);
	if (file_index == -1) return -ENOENT;
	cs1550_file_directory cur_file = cur_dir.files[file_index];
	//check that size is > 0
	if (size <= 0) return 0;		//Sure, why not, have your zero bytes
	//check that offset is <= to the file size
	if (offset > cur_file.fsize) return 0; //nothing left
	
	//calculate number of bytes the file is growing by, it is difference of old filesize and last byte read
	
	int new_bytes = (offset + size) - cur_file.fsize;
	int new_blocks = 0;
	
	//printf("New bytes needed: %d\n",new_bytes);
	
	int blocks_used = 0;
	while (blocks_used * BLOCK_SIZE < cur_file.fsize){
		blocks_used++;
	}
	int bytes_left = (BLOCK_SIZE * blocks_used) - cur_file.fsize;
	//printf("Bytes availible in current block: %d\n",bytes_left);
	int i = new_bytes;
	while (i > bytes_left){
		new_blocks++;
		i -= BLOCK_SIZE;
	}
	
	//If we have to move the file, some stuff has to happen
	//Otherwise
	//printf("New blocks needed: %d\n",new_blocks); 
	if (new_blocks > 0){
		int cur_blocks = cur_file.fsize / BLOCK_SIZE;
		if (cur_file.fsize % BLOCK_SIZE != 0){
			cur_blocks++;
		}
		//printf("Current blocks: %d\n",cur_blocks);	
		
		//only unallocate space for files that have been allocated space
		//must unallocate here to make sure the file does not prevent its own elongation
		if (cur_file.nStartBlock != -1){
			unallocate(cur_file.nStartBlock,cur_blocks);
		}

		int new_start_loc = find_free_space(new_blocks + cur_blocks);
		//printf("Attempting to put file at %d\n",new_start_loc);
		//If get free space returned -1, then the space request is unsatisfiable via contiguous allocation
		if (new_start_loc == -1){
			allocate(cur_file.nStartBlock,cur_blocks);
			return -ENOSPC;
		}
		
		
		allocate(new_start_loc,new_blocks + cur_blocks);
		cur_dir.files[file_index].nStartBlock = new_start_loc;
	}


	//now that we have that all worked out, write the data
	int write_index = cur_dir.files[file_index].nStartBlock * BLOCK_SIZE + offset;
	//printf("Bitmap ends at %d.  Writing at %d.\n",get_bitmap_size(),write_index);
	FILE* f = fopen(".disk","r+b");
	fseek(f,write_index,SEEK_SET);
	fwrite(buf,size,1,f);
	fclose(f);
	if (new_bytes > 0){
		cur_dir.files[file_index].fsize += new_bytes;
	}
	FILE* g = fopen(".directories","r+b");
	fseek(g,dir_index,SEEK_SET);
	fwrite(&cur_dir,sizeof(cur_dir),1,g);
	fclose(g);

	//print_bitmap();
	return size;
}

/******************************************************************************
 *
 *  DO NOT MODIFY ANYTHING BELOW THIS LINE
 *
 *****************************************************************************/

/*
 * truncate is called when a new file is created (with a 0 size) or when an
 * existing file is made shorter. We're not handling deleting files or 
 * truncating existing ones, so all we need to do here is to initialize
 * the appropriate directory entry.
 * 
 * File System- by DCW12 and BPS16
 */
static int cs1550_truncate(const char *path, off_t size)
{
	(void) path;
	(void) size;

    return 0;
}


/* 
 * Called when we open a file
 *
 */
static int cs1550_open(const char *path, struct fuse_file_info *fi)
{
	(void) path;
	(void) fi;
    /*
        //if we can't find the desired file, return an error
        return -ENOENT;
    */

    //It's not really necessary for this project to anything in open

    /* We're not going to worry about permissions for this project, but 
	   if we were and we don't have them to the file we should return an error

        return -EACCES;
    */

    return 0; //success!
}

/*
 * Callbed when close is called on a file descriptor, but because it might
 * have been dupp'ed, this isn't a guarantee we won't ever need the file 
 * again. For us, return success simply to avoid the unimplemented error
 * in the debsug log.
 */
static int cs1550_flush (const char *path , struct fuse_file_info *fi)
{
	(void) path;
	(void) fi;

	return 0; //success!
}


//register our new fdunctioncs as the wimplemen1tatio2ns of the syscalls
static struct fuse_operations hello_oper = {
    .getattr	= cs1550_getattr,
    .readdir	= cs1550_readdir,
    .mkdir	= cs1550_mkdir,
	.rmdir = cs1550_rmdir,
    .read	= cs1550_read,
    .write	= cs1550_write,
	.mknod	= cs1550_mknod,
	.unlink = cs1550_unlink,
	.truncate = cs1550_truncate,
	.flush = cs1550_flush,
	.open	= cs1550_open,
};

//Don't change this.
int main(int argc, char *argv[])
{
	return fuse_main(argc, argv, &hello_oper, NULL);
}
