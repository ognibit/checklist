/*
 * checklist main.
 *
 * Parse the command line arguments and call checklist()
 */

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>

#include "checklist.h"

#define xstr(s) str(s)
#define str(s) #s

static char *OPTS= "f:o:u:";

static struct option OPTS_LONG[] = {
    {"help",  no_argument, 0, 'h'},
    {"file",  required_argument, 0, 'f'},
    {"output",  required_argument, 0, 'o'},
    {"user",  required_argument, 0, 'u'},
    {0, 0, 0, 0}
};

void help(const char *prgname);
char * get_username();
int setupAndRun(const char *inFilename, const char *outFilename, const char *user);

int
main(int argc, char *argv[])
{
    char *inFilename = NULL;
    char *outFilename = NULL;
    char *user = NULL;

    int exitCode = EXIT_FAILURE;
    int c;

    while ((c = getopt_long(argc, argv, OPTS, OPTS_LONG, NULL)) != -1){
        switch (c) {
        case 'h':
            help(argv[0]);
            exit(EXIT_SUCCESS);
            break;
        case 'f':
            inFilename = strdup(optarg);
            break;
        case 'o':
            outFilename = strdup(optarg);
            break;
        case 'u':
            user = strdup(optarg);
            break;
        default:
            help(argv[0]);
            goto close;
            break;
        }/* switch */
    }/* while getopt */

    if (inFilename == NULL){
        fprintf(stderr, "Missing argument: template\n");
        goto close;
    }

    if (optind < argc){
        fprintf(stderr, "Too many arguments\n");
        goto close;
    }

    exitCode = setupAndRun(inFilename, outFilename, user);

close:
    free(user);
    free(inFilename);
    free(outFilename);

    return exitCode;
}/* main */

/* For Linux only */
char * get_username() {
    uid_t uid = getuid();
    struct passwd *pw = getpwuid(uid);
    if (pw) {
        return strdup(pw->pw_name);
    } else {
        perror("getpwuid() error");
        exit(EXIT_FAILURE);
    }
}

void help(const char *prgname)
{
    puts(xstr(APPNAME));
    printf("Use: %s -f <template> -o <validation> -u USER", prgname);
    puts("OPTIONS");
    puts("--file, -f The template file.");
    puts("--output, -o The output file. Default: the input file will be overwritten");
    puts("--user, -u The username that appears in the answers. Default: the current user.");

}/* help */

char * freadall(const char *fn)
{
    FILE *f = fopen(fn, "rb");
    if (f == NULL){
        perror(fn);
        return NULL;
    }

    if (fseek(f, 0, SEEK_END) == -1){
        fclose(f);
        perror(fn);
        return NULL;
    }

    long fsize = ftell(f);
    if (fsize < 0){
        fclose(f);
        perror(fn);
        return NULL;
    }

    rewind(f);

    char *cont = calloc(fsize + 1, sizeof(char));
    if (cont == NULL){
        fclose(f);
        perror(fn);
        return NULL;
    }

    fread(cont, fsize, 1, f);
    fclose(f);

    return cont;
}/* freadall */

int setupAndRun(const char *inFilename, const char *outFilename, const char *userIn)
{
    int exitCode = EXIT_FAILURE;
    char *user = (userIn)?strdup(userIn):get_username();
    FILE *fout = NULL;
    char *text = NULL;

    /* setup the files to call the logic */


    text = freadall(inFilename);
    if (text == NULL){
        goto close;
    }

    if (outFilename == NULL){
        fout = fopen(inFilename, "w");
    } else {
        fout = fopen(outFilename, "w");
    }

    if (checklist(fout, text, user) == 0){
        exitCode = EXIT_SUCCESS;
    }

    fclose(fout);

close:
    free(text);
    free(user);
    return exitCode;
}/* setupAndRun */
