/* Erik Sherman */

#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <copyfile.h>

#define SOURCE_ROOT "/Users/sherman/Dropbox/"
#define DEST_ROOT "/Users/sherman/Documents/sorted/"

#define MIN(a,b) ((a) < (b) ? (a) : (b))

int string_in_array(const char* string, const char *array[], int length);
void moveFiles(char* curpath);
char *concat_string(const char *str1, const char* str2);

/* return 1 if string is in array (with length length),
   otherwise return 0 */
int string_in_array(const char* string, const char *array[], int length) {
    int i;
    for (i = 0; i < length; i++) {
        if (strcmp(string, array[i]) == 0)
            return 1;
    }

    return 0;
}

/* concat str2 to str1 and return result as new string */
char *concat_string(const char* str1, const char* str2) {
    char *result;
    result = malloc(sizeof(char) * (strlen(str1) + strlen(str2) + 1));
    strcpy(result, str1);
    strcat(result, str2);
    return result;
}

/* recursiely moves all files and directories from
 * SOURCE_ROOT to DEST_ROOT directory, sorting all the files with folders
 * containing the name of the first word of the file name
 */
void movefiles(char * curpath) {
    char *fnames_dont_open[] = {".","..",".DS_Store","Icon\r",".dropbox",
        ".dropbox.cache"};

    /* create curpath string */
    char *path = concat_string(SOURCE_ROOT, curpath);
    printf("%s\n", path);

    /* open directory with path curpath */
    struct dirent *cure;
    DIR *cur = opendir(path);

    if (cur == NULL) {
        printf("movefiles called with a nonvalid directory");
        return;
    }

    /* for every file in curpath (fname):
     *
     * if fname is a file:
     *     1. create  new directory DEST_ROOT/cupath/<first name in fname>
     *     2. copy file into the directory just created
     * if fname is a directory:
     *     1. create the directory at DEST_ROOT/curpath/fname
     *     2. recursively call movefiles, passing curpath/fname/ as curpath
     */
    while((cure = readdir(cur)) != NULL) {
        char *fname = cure -> d_name;
        if (!string_in_array(fname, fnames_dont_open,
                    sizeof(fnames_dont_open)/sizeof(fnames_dont_open[0]))) {
            printf("nested dir name: %s\n", fname);
            struct dirent *neste;
                char *nested_path = concat_string(path, fname);
            DIR *nest = opendir(nested_path);

            /* if fname is a file */
            if (nest == NULL) {
                /* find first word in fname */
                char *space_ptr = strstr(fname, " ");
                char *dot_ptr = strstr(fname, ".");
                /* isolate delim_ptr (pointer to the first instace of a
                   space or dot in fname */
                char *delim_ptr;
                if (space_ptr == NULL && dot_ptr == NULL)
                    delim_ptr = NULL;
                else if (space_ptr == NULL)
                    delim_ptr = dot_ptr;
                else if (dot_ptr == NULL)
                    delim_ptr = space_ptr;
                else
                    delim_ptr = MIN(space_ptr, dot_ptr);
                /* isolate the first word based on delim_ptr */
                char *first_word_fname;
                if (delim_ptr != NULL) {
                    int space_loc = delim_ptr - fname;
                    printf("space loc: %d\n", space_loc);
                    first_word_fname = malloc(sizeof(char)
                            * (space_loc + 1));
                    strlcpy(first_word_fname, fname, space_loc + 1);
                    printf("first_word_fname: %s\n", first_word_fname);
                } else {
                    first_word_fname = malloc(sizeof(char)
                            * (strlen(fname) + 1));
                    strcpy(first_word_fname, fname);
                }
                /* make directory with name of first word in fname
                   (DEST_ROOT/curpath/<first word fname>) */
                char *dest_path = concat_string(concat_string(
                            DEST_ROOT, curpath), first_word_fname);
                mkdir(dest_path, 0777);
                printf("dest path: %s\n", dest_path);

                /* move fname to the directory that was just created
                   (DEST_ROOT/curpath/<first word fname>/fname */
                char *dest_path_file = concat_string(concat_string(
                            dest_path, "/"), fname);
                int file_copied = copyfile(nested_path, dest_path_file,
                        NULL, COPYFILE_DATA);
                if (file_copied == 0)
                    printf("%s copied sucessfully\n", fname);
                else
                    printf("%s was not copied sucessfully\n", fname);
            }
            /* fname is a directory */
            else {
                /* make directory with same name as fname in
                   DEST_ROOT/curpath */
                char *dest_path = concat_string(concat_string(
                            DEST_ROOT, curpath), fname);
                mkdir(dest_path, 0777);

                /* recursively call movefiles, changing curpath to
                   move into the directory fname */
                movefiles(concat_string(concat_string(
                                curpath, fname), "/"));

                closedir(nest);
            }
        }
    }
    closedir(cur);
}

int main(void) {
    movefiles("");
    return 0;
}













