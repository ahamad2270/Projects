#include "ournix.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* 
Name: Ahmed Hamad
Directory ID: ahamad1
UID: 118035857
Section:0105
 
This project is supposed to act as a small simulation replicating the filesystem
component of the Ournix operating system. It contains functions that create
files and folders/directories. There is a cd function that changes the current
directory in the filesystem. There are ls and pwd functions that give
information about the directory and finally  rm and rmfs functions that either 
remove a single file/directory or the whole filesystem while making sure that
there are no memory leaks.
*/
static void pwd_helper(File *curr, Ournix *const filesystem);
static void rmfs_helper(File *curr);
static void rm_helper(File *curr);
static void allo_helper(File *curr);
/*
The mkfs function is meant to take in a pointer to a ournix filesystem as a
 parameter and initilize it without returning anything. If the parameter is NULL
nothing will happen.
 */
void mkfs(Ournix *const filesystem) {
  File *curr; 
 if (filesystem != NULL) {
   curr = filesystem -> curr;
   filesystem -> curr = malloc(sizeof(*curr));
   filesystem -> curr -> sub_dir = NULL;
   filesystem -> curr -> parent_dir = NULL;
   filesystem -> curr -> next_file = NULL;
   filesystem -> curr -> prev_file = NULL;
   filesystem -> curr -> name = NULL;
   filesystem -> curr -> timestamp = 1;
  }
       
}
/*
The allo_helper function is meant to act as a helper function to other functions
such as touch and mkdir and initilize any new files or directorys to be added
to the filesystem. It takes in a parameter of pointer to the file to be 
initilized.
 */ 
static void allo_helper(File *curr) {
    curr -> next_file = NULL;
    curr -> prev_file = NULL;
    curr -> parent_dir = NULL;
    curr -> sub_dir = NULL;
    curr -> name = NULL;
}
/*
The touch function takes in a pointer to the filesystem and a string name as
parameters. If the filesystem or name are null or if it contains a / and does
not only consist of /  or an empty string is inserted then the function will 
not do anything and return 0. Otherwise the function will either update the 
timestamp of an existing file with the same name or create a new one
(ordered insert) and return 1. 
 */
int touch(Ournix *const filesystem, const char name[]) {
  int returnvalue = 1;
  int found = 0;
  File *newfile;
  File *curr;
  /* Error cases */
  if (filesystem == NULL || name == NULL || (strlen(name) > 1 && 
					     strchr(name,'/') != NULL ) ||
      strlen(name) == 0) {
    returnvalue = 0;
  }
  else {
    curr = filesystem -> curr;
    /* Special cases */
    if (strcmp(name,".") == 0  || strcmp(name,"/") == 0 ||
	strcmp(name,"..") == 0 ) {
      found  = 1;
    }
    else {
      /* First file in filesystem */
      if (curr -> name == NULL) {
	found = 1;
	curr -> file_type = FILETYPE;
	curr -> name = malloc(strlen(name)+1);
	strcpy(curr -> name, name);
	curr -> timestamp = 1;
      }
      /* File doesnt exist so it is inserted(sorted before) */
      else if (strcmp(name, curr -> name) < 0) {
	found = 1;
	newfile = malloc(sizeof(*newfile));
	allo_helper(newfile);
	newfile -> file_type = FILETYPE;
	newfile -> name = malloc(strlen(name)+1);
	strcpy(newfile -> name, name);
	newfile -> timestamp = 1;
	newfile -> next_file = curr;
	curr -> prev_file = newfile;
	if(curr -> parent_dir != NULL){
	  newfile -> parent_dir = curr -> parent_dir;
	}
	curr = newfile;
	filesystem -> curr = curr;
	}
      while (curr -> next_file != NULL && found == 0 ) {
	/* File already exists */
	if (strcmp(name, curr -> name) == 0) {
	  if(curr -> file_type == FILETYPE) {
	    curr -> timestamp++;
	    found = 1;
	  }
	  else{
	    found = 1;
	  }
	}
	/* File doesnt exist so it is inserted(sorted middle) */
	else if ((strcmp(name, curr -> name) > 0) && 
		 (strcmp(name, curr -> next_file -> name) < 0)) {
	  found = 1;
	  newfile = malloc(sizeof(*newfile));
	  allo_helper(newfile);
	  newfile -> file_type = FILETYPE;
	  newfile -> name = malloc(strlen(name)+1);
	  strcpy(newfile -> name, name);
	  newfile -> timestamp = 1;
	  newfile -> next_file = curr -> next_file;
	  newfile -> prev_file = curr;
	  curr -> next_file = newfile;
	}
        
	else {
	  curr = curr -> next_file;

      }
      }
      /* Evaluates last file */
      if (curr -> next_file == NULL && found == 0) {
	if (strcmp(name, curr -> name) == 0) {
	   if(curr -> file_type == FILETYPE) {
	     curr -> timestamp++;
	     found = 1;
	   }
	  else{
	    found = 1;
	  }
	}
	/* Adds new file to the end */
	else {
	  found = 1;
	  newfile = malloc(sizeof(*newfile));
	  allo_helper(newfile);
	  newfile -> file_type = FILETYPE;
	  newfile -> name = malloc(strlen(name)+1);
	  strcpy(newfile -> name, name);
	  newfile -> timestamp = 1;
	  curr -> next_file = newfile;
	  newfile -> prev_file = curr;
	}
    }
  }
  
  }
  return returnvalue;
}
/*
The mkrdir function takes in a pointer to the filesystem and a string name as
parameters. If the filesystem or name are null or if it contains a /  or is an
 empty string is inserted then the function will not do anything and return 0.
 If the name is either ., .., or the name of an existing directory it will also
 return 0 and not do anything. Otherwise the function will create a new
directory (ordered insert alphabetical) and return 1. 
 */
  int mkdir(Ournix *const filesystem, const char name[]) {
    int returnvalue = 1;
    int found = 0;
    File *newdir;
    File *curr;
    /* Error cases */
    if (filesystem == NULL || name == NULL || strlen(name) == 0 ||
	(strlen(name) > 1 && strchr(name,'/') != NULL) || strcmp(name,"/")
	== 0 || strcmp(name,".") == 0 || strcmp(name,"..") == 0) {
      returnvalue = 0;
    }
    else {
      curr = filesystem -> curr;
      /* First thing in filesystem */
      if (curr -> name == NULL) {
	found = 1;
	curr -> file_type = DIR;
	curr -> name = malloc(strlen(name)+1);
	strcpy(curr -> name, name);
      }
      /* File does not exist, but comes before the first file(alphabetical) */
      else if (strcmp(name,curr -> name) < 0) {
	found = 1;
	newdir =  malloc(sizeof(*newdir));
	allo_helper(newdir);
	newdir -> file_type = DIR;
	newdir -> name = malloc(strlen(name)+1);
	strcpy(newdir -> name, name);
	newdir -> next_file = curr;
	curr -> prev_file = newdir;
	if(curr -> parent_dir != NULL){
	  newdir -> parent_dir = curr -> parent_dir;
	}
	curr = newdir;
	filesystem -> curr = curr;
	}
      while (curr -> next_file != NULL && found  == 0) {
	/* File was found */
	if (strcmp(name, curr -> name) == 0) {
	  found = 1; 
	  returnvalue = 0;
	}
	/* File does not exist, but should be added to middle */
	else if ((strcmp(name, curr -> name) > 0) && 
		 (strcmp(name, curr -> next_file -> name) < 0)) {
	  found = 1;
	  newdir =  malloc(sizeof(*newdir));
	  allo_helper(newdir);
	  newdir -> file_type = DIR;
	  newdir -> name = malloc(strlen(name)+1);
	  strcpy(newdir -> name, name);
	  newdir -> next_file = curr -> next_file;
	  newdir -> prev_file = curr;
	  curr -> next_file = newdir;
	}

	else {
	  curr  = curr -> next_file;
	}
      }
      /* Evaluates last file/dir */
      if (curr -> next_file == NULL && found == 0) {
	if (strcmp(name, curr -> name) == 0) {
	  returnvalue = 0;
	  found = 1;
	}
	/* Adds new file to the end */
	else {
	  found = 1;
	  newdir =  malloc(sizeof(*newdir));
	  allo_helper(newdir);
	  newdir -> file_type = DIR;
	  newdir -> name = malloc(strlen(name)+1);
	  strcpy(newdir -> name, name);
	  newdir -> prev_file = curr;
	  curr -> next_file = newdir;
	}

      }
    }
    return returnvalue;
  }
/* 
The cd function takes in a pointer to a filesystem and a name as parameters. 
If the pointer or name are NULL or if the function contains a / along with other
characters the function will return 0 and not do anything. If the name is empty
or a . then the function will not do anything. If the name is .. then the
function will change to the current directory to the current directory's parent
directory. If the name is / it will change the the current directory to the root
directory. If the name is the name of the directory in the current directory
then the current directory will change to that directory. In all of these cases
the function will return 1.
 */

  int cd(Ournix *const filesystem, const char name[]) {
    int returnvalue = 1;
    int found = 0;
    File *curr;
    File *newdir;
    /* Error case */
    if (filesystem == NULL || name == NULL || (strlen(name) > 1 && 
					     strchr(name,'/') != NULL )) {
      returnvalue = 0;
    }
    else {
      curr = filesystem -> curr;
    /* Name is a . or an empty string */
    if (strcmp(name, ".") == 0 || strlen(name) == 0) {
      found = 1;
    }
    /* Name is .. */
    else if (strcmp(name, "..") == 0) {
      if (curr -> parent_dir == NULL) {
	found = 1;
      }
      else {
	found  = 1;	
	curr = curr -> parent_dir;
	/*Goes back to the beginning of this directory in case it needs to
	  search for a file/Dir in the new directory */
	while (curr -> prev_file != NULL) {
	  curr = curr -> prev_file;
	}
      }
    }
    /* Name is / */ 
    else if (strcmp(name, "/") == 0) {
      while (curr -> prev_file != NULL || 
	     curr -> parent_dir != NULL) {
	while (curr -> prev_file != NULL) {
	  curr = curr -> prev_file; 
	}
	while (curr -> parent_dir != NULL) {
	  curr = curr -> parent_dir;
	}
      }
      found = 1;
    }
    /* Searches for subdirectory of the current directory with name */
    else{
      while (curr != NULL && found == 0) {
	if (strcmp(name,curr -> name) == 0) {
	  found = 1;
	  if(curr -> file_type == FILETYPE) {
	    returnvalue = 0;
	  }
	  else {
	    if (curr -> sub_dir != NULL) {
	      curr = curr -> sub_dir;
	    }
	    else {
	      newdir = malloc(sizeof(*newdir));
	      allo_helper(newdir);
	      curr -> sub_dir = newdir;
	      newdir -> parent_dir = curr;
	      curr = curr -> sub_dir;
	    }
	  }
	}
      
	else { 
	  curr = curr -> next_file;
	}
      }
    }
      if (found == 0) {
	returnvalue = 0;
      }
      filesystem -> curr = curr;
    }
      return returnvalue;
  }
  
/*
The ls function takes in a pointer to a filesystem and a name as parameters. 
If the pointer or name are NULL or if the function contains a / along with other
characters the function will return 0 and not do anything. If the name is empty
or a . then the function will print all files and directories in the current
working directory. If the name is .. the function will print the names of all 
the files and directories in the parent directory. If the name is / then the 
function will print the names of all the files and directories in the root
directory. If the name is the name of a file then it will print the name of that
file along with its timestamp. If the name is the name of a directory then the
function will print the contents of that directory. In all of these cases the
function will return 1.
  */
  int ls(Ournix *const filesystem, const char name[]) {
    int returnvalue = 1;
    int found = 0;
    File *curr;
    /* Error case */
    if (filesystem == NULL || name == NULL || (strlen(name) > 1 && 
					     strchr(name,'/') != NULL )) {
      returnvalue = 0;
      found = 1;
    }
    else {
      curr = filesystem -> curr;
    /* Name is . or empty*/
    if (strcmp(name, ".") == 0 || strlen(name) == 0) {
      if(curr -> name != NULL) {
	while (curr != NULL && curr -> name != NULL) {
	  if (curr -> file_type == FILETYPE) {
	    printf("%s\n", curr -> name);
	  }
	  else {
	    printf("%s/\n",curr -> name);
	  }
	  curr = curr -> next_file;
	}
      }
      found = 1;
    }
    /* Name is .. */
    else if (strcmp(name, "..") == 0) {
      if (curr -> parent_dir != NULL) {
	curr = curr -> parent_dir;
	while (curr -> prev_file != NULL) {
	  curr = curr -> prev_file;
	}
      }
      while (curr != NULL && curr -> name != NULL) {
	if (curr -> file_type == FILETYPE) {
	  printf("%s\n", curr -> name);
	}
	else {
	  printf("%s/\n",curr -> name);
	}
	curr = curr -> next_file;
      }
      found = 1;
    }
    /* name is / */
    else if (strcmp(name, "/") == 0) {
      /* Brings curr back to starting root pos */
      while (curr -> prev_file != NULL || 
	     curr -> parent_dir != NULL) {
	while (curr -> prev_file != NULL) {
	  curr = curr -> prev_file; 
	}
	while (curr -> parent_dir != NULL) {
	  curr = curr -> parent_dir;
	}
      }
      while (curr != NULL && curr -> name != NULL) {
	if (curr -> file_type == FILETYPE) {
	  printf("%s\n", curr -> name);
	}
	else {
	  printf("%s/\n",curr -> name);
	}
	curr = curr -> next_file;
      }
      found = 1;
    }
    /* Searches for specific name in current dir */
    else {
      while (curr != NULL && curr -> name != NULL && found == 0) {
	if (strcmp(name, curr -> name) == 0) {
	  if (curr -> file_type == FILETYPE) {
	    printf("%s %d\n", curr -> name, curr -> timestamp);
	    found = 1;
	}
	  else {
	    found = 1;
	    curr = curr -> sub_dir;
	    while (curr != NULL) {
	      if (curr -> file_type == FILETYPE) {
		printf("%s\n", curr -> name);
	      }
	      else {
		printf("%s/\n", curr -> name);
	      }
	      curr = curr -> next_file;
	    }
	  }
	}
	else {
	curr = curr -> next_file;
	
	}
      }
    }
    
    /* no file/dir with that name and no special case */
    if (found == 0) {
      returnvalue = 0;
    }
    }
    return returnvalue;
  }
/*
Takes in a pointer to the filesystem as a parameter. If filesystem is NULL then
the function will do nothing otherwise it will print the full path from the
 root to the current directory and inputs a new line at the end. Separates 
the directories with / but does not print a / after the last directory. 
 */
void pwd(Ournix *const filesystem) {
  /*
Uses the pwd_helper function to do the majority of the work 
  */
  if (filesystem != NULL) {
    pwd_helper(filesystem -> curr, filesystem);
    printf("\n");
  }

}
/*
pwd_helper takes in a pointer to the filesystem and a pointer that points to
the current directory. It will then recursively traverse up to the root from
the current location printing the names of the directories it is in along the
way.
 */
static void pwd_helper(File *curr, Ournix *const filesystem) {
  /* if statement won't process if it hits root */
  if (curr -> parent_dir == NULL && curr -> prev_file == NULL) {
    printf("/");
  }
  else {
    while (curr -> prev_file != NULL) {
      curr = curr -> prev_file;
    }
    if (curr -> parent_dir != NULL) {
      pwd_helper(curr -> parent_dir, filesystem);
      if(curr == filesystem -> curr){
	printf("%s", curr -> parent_dir -> name);
      }
      else {
      printf("%s/", curr -> parent_dir -> name);
      }
    }
    else {
      printf("/");
    }
  }
}
/*
The rmfs function takes in a pointer to the filesystem as a parameter and
removes all files and directories in the system freeing all the memory allocated
 */
 void rmfs(Ournix *const filesystem) {
   /* Calls the rmfs_helper function to do all of the work */ 
 if (filesystem != NULL) {
       rmfs_helper(filesystem -> curr);
       filesystem -> curr = NULL;
  }
}

/*
The rmfs_helper function takes in the current directory location as a parameter
and recursively removes all the files and directories in the system while 
freeing all the memory allocated. The function will recursively travel in 
every direction until no files remain.
 */
static void rmfs_helper(File *curr) {
    if (curr -> parent_dir != NULL) {
      curr -> parent_dir -> sub_dir = NULL;
      rmfs_helper(curr -> parent_dir);
    }
    if (curr -> sub_dir != NULL) {
      curr -> sub_dir -> parent_dir = NULL;
      rmfs_helper(curr -> sub_dir);
    }
    if (curr -> next_file != NULL) {
      rmfs_helper(curr -> next_file);
    }
    if(curr != NULL) {
      free(curr -> name);
      curr -> name = NULL;
      free(curr);
      curr = NULL;
    }
}
/*
The rm_helper function takes in the current directory location as a parameter
and recursively removes all the files and directories located in a subdirectory
 while freeing all the memory allocated. This function is used in the rm 
function to remove all the subdirectories and files after removing a directory
*/
static void rm_helper(File *curr) {
    if (curr -> sub_dir != NULL) {
      curr -> sub_dir -> parent_dir = NULL;
      rm_helper(curr -> sub_dir);
    }
    if (curr -> next_file != NULL) {
      rm_helper(curr -> next_file);
    }
    free(curr -> name);
    curr -> name = NULL;
    free(curr-> parent_dir);
    free(curr -> sub_dir);
    free(curr);
    curr = NULL;
}
int rm(Ournix *const filesystem, const char name[]) { 
  int found = 0;
  File *curr; 
  int returnvalue = 1;
  /* Error cases */ 
  if (filesystem == NULL || name == NULL || strlen(name) == 0 ||
	(strlen(name) > 1 && strchr(name,'/') != NULL) || strcmp(name,"/")
	== 0 || strcmp(name,".") == 0 || strcmp(name,"..") == 0) {
      returnvalue = 0;
    }
  else {
    curr = filesystem -> curr;
    while (curr != NULL && found == 0) {
      if(strcmp(name, curr -> name) == 0) {
	if (curr -> parent_dir != NULL) {
	  curr -> next_file -> parent_dir = curr -> parent_dir;
	  curr -> parent_dir -> sub_dir = curr -> next_file;
	}
	if (curr -> file_type == FILETYPE) {
	  if (curr -> prev_file == NULL && curr -> parent_dir == NULL &&
	      curr -> next_file != NULL) {
	    filesystem -> curr = curr -> next_file;
	    free(curr -> name);
	    curr -> name = NULL;
	    free(curr);
	    curr = NULL;
	  }
	  else if (curr -> prev_file != NULL && curr -> next_file != NULL) {
	    curr -> prev_file -> next_file = curr -> next_file;
	    curr -> next_file -> prev_file = curr -> prev_file;
	    curr -> prev_file = NULL;
	    curr -> next_file = NULL;
	    free(curr -> name);
	    curr -> name = NULL;
	    free(curr);
	    curr = NULL;
	    }
	  else if(curr -> prev_file == NULL && curr -> next_file == NULL){
	    free(curr -> name);
	    curr -> name = NULL;
	    free(curr);
	    curr = malloc(sizeof(*curr));
	    allo_helper(curr);
	  }
	  else {
	    free(curr -> name);
	    curr -> name = NULL;
	    curr -> prev_file -> next_file = NULL;
	    free(curr);
	    curr = NULL;
	  }
	}
	else {
	  if (curr -> sub_dir != NULL) {
	    rm_helper(curr -> sub_dir);
	  }
	  if (curr -> prev_file == NULL && curr -> parent_dir == NULL &&
	      curr -> next_file != NULL) {
	    filesystem -> curr = curr -> next_file;
	    free(curr -> name);
	    curr -> name = NULL;
	    free(curr);
	    curr = NULL;
	  }
	  else if (curr -> prev_file != NULL && curr -> next_file != NULL) {
	    curr -> prev_file -> next_file = curr -> next_file;
	    curr -> next_file -> prev_file = curr -> prev_file;
	    curr -> prev_file = NULL;
	    curr -> next_file = NULL;
	    free(curr -> name);
	    curr -> name = NULL;
	    free(curr);
	    curr = NULL;
	    }
	  else if(curr -> prev_file == NULL && curr -> next_file == NULL){
	    free(curr -> name);
	    curr -> name = NULL;
	    free(curr);
	    curr = malloc(sizeof(*curr));
	    allo_helper(curr);
	  }
	  else {
	    free(curr -> name);
	    curr -> name = NULL;
	    curr -> prev_file -> next_file = NULL;
	    free(curr);
	    curr = NULL;
	  }

	}
	found = 1;
      }
      else {
	curr = curr -> next_file;
      }
    }
  } 
  if (found == 0) {
    returnvalue = 0;
  } 
  return returnvalue;
}
