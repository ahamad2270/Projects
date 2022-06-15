typedef struct file {
  enum {DIR, FILETYPE } file_type;
  struct file *sub_dir;
  struct file *parent_dir;
  struct file *next_file;
  struct file *prev_file;
  char * name;
  int timestamp;
} File;

typedef struct ournix {
  File *curr;
} Ournix;
