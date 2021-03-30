#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void panic(char *s);
char* fmtname(char *s);

void find(char *path, const char *filename)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if ((fd = open(path, O_RDONLY)) < 0) {
      fprintf(2, "find: cannot open %s\n", path);
      return;
  }
  
  if (fstat(fd, &st) < 0) {
      fprintf(2, "find: cannot open stat %s\n", path);
      close(fd);
      return;
  }

  switch(st.type) {
    case T_DEVICE:
    case T_FILE:
      if (strcmp(fmtname(path), filename) == 0) {
       printf("%s\n", path);
      }
      break;
    case T_DIR:
      strcpy(buf, path);
      p = buf + strlen(buf);
      *p++ = '/';
      while (read(fd, &de, sizeof(de)) == sizeof(de)) {
        if (de.inum == 0 || strcmp(".", de.name) == 0 || strcmp("..", de.name) == 0) {
          continue;
        }
        strcpy(p, de.name);
        find(buf, filename);
      }
      break;
  }
  close(fd);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
        panic("Usage: find path filename");

    if (argc == 3) {
        find(argv[1], argv[2]);
    } else {
        find(".", argv[1]);
    }
    exit(0);
}

void panic(char *s)
{
    fprintf(2, "%s\n", s);
    exit(1);
}

char* fmtname(char *path)
{
  // static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;
  return p;
}
