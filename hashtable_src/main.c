#include <stdio.h>
#include <stdlib.h>

#include <readline/readline.h>
#include <readline/history.h>

#include <time.h>

#include "structs.h"
#include "commands.h"
#include "utils.h"

int main(void)
{
    hashtable_t ht;
    char        *line = NULL;
    char        *tmp = NULL;
    int         r;

    memset(&ht, 0, sizeof(ht));
    while (1)
    {
        line = readline("> ");
        if (!line)
            return fprintf(stderr, "CRITICAL ERROR REALIND FAIL\n"), free_hashtable(&ht), EXIT_FAILURE;
        // execute cmd modifies line to parse argument
        tmp = strdup(line);
        if (tmp == NULL)
            return fprintf(stderr, "CRITICAL ERROR OUT OF RAM\n"), free_hashtable(&ht), free(line), EXIT_FAILURE;
        r = execute_cmd(&ht, line);
        if (r == 0)
            return printf("Exiting...\n"), free_hashtable(&ht), free(tmp), free(line), EXIT_SUCCESS;
        add_history(tmp);
        free(line);
        free(tmp);
    }

    return 0;
}