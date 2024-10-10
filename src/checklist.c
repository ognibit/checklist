/*
 * checklist implementation
 */

#include "checklist.h"

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

#define MARKER_CHECK "!CHECK!"

typedef enum {
    CHK_ANS_KO = -1,   /* error */
    CHK_ANS_OK = 0,    /* success */
    CHK_ANS_EMPTY = 1, /* empty answer, should ask again */
    CHK_ANS_NEG = 2    /* negative answer, should stop */
} chk_ans_t;

/* write the check answer like
 * Checked by John Doe at 2024-09-29T12:30:05.
 * if it is the case.
 */
chk_ans_t checklist_wans(FILE *out, const char *line, const char *usr)
{
    /* trim */
    while (isblank(*line)){
        line++;
    }

    chk_ans_t state = CHK_ANS_KO;
    char a = tolower(line[0]);
    char dt[32];

    if (a == 'y'){
        time_t t = time(NULL);
        if (t == -1){
            perror("time");
            return CHK_ANS_KO;
        }
        struct tm *tmp = localtime(&t);
        if (tmp == NULL){
            perror("localtime");
            return CHK_ANS_KO;
        }
        if (strftime(dt, sizeof(dt), "%Y-%m-%dT%H:%M:%S", tmp) == 0){
            perror("strftime");
            return CHK_ANS_KO;
        }
        if (fprintf(out, "Checked by %s at %s.\n", usr, dt) < 0){
            perror("fprintf");
            return CHK_ANS_KO;
        }
        state = CHK_ANS_OK;
    } else if (a == 'n'){
        state = CHK_ANS_NEG;
    } else if (a == '\n' || a == '\0'){
        state = CHK_ANS_EMPTY;
    }

    return state;
}/* checklist_wans */

int checklist(FILE *fout, char *templ, const char *user)
{
    char * s = templ;
    char * p = strstr(s, MARKER_CHECK);
    char *line = NULL;
    size_t n = 0;
    int rc = -1;

    while (p != NULL){
        /* show the item */
        printf("%.*s", (int)(p-s), s);
        if (fwrite(s, p-s, 1, fout) == 0){
            perror("fwrite");
            goto close;
        }

        /* ask questions */
        chk_ans_t ans = CHK_ANS_EMPTY;
        while (ans == CHK_ANS_EMPTY){
            printf("\n>Checked?[y,n]: ");
            if (getline(&line, &n, stdin) == -1){
                perror("Input error");
                continue;
            }

            ans = checklist_wans(fout, line, user);
        } /* while empty ans */

        if (ans == CHK_ANS_KO){
            goto close;
        }
        if (ans == CHK_ANS_NEG){
            /* copy the marker and the rest of the file */
            s = p;
            break;
        }

        printf("\n>Note: ");
        if (getline(&line, &n, stdin) == -1){
            perror("Input error");
            continue;
        }

        if (fwrite(line, strlen(line), 1, fout) == 0){
            perror("fwrite");
            goto close;
        }
        /* next marker */
        s = p + strlen(MARKER_CHECK);
        p = strstr(s, MARKER_CHECK);
    }/* while */

    if (strlen(s) > 0 && fwrite(s, strlen(s), 1, fout) == 0){
        perror("fwrite");
    }

    rc = 0;
close:
    free(line);

    return rc;
}/* checklist */
