/* forking.c: Forking HTTP Server */

#include "spidey.h"

#include <errno.h>
#include <signal.h>
#include <string.h>

#include <unistd.h>

/**
 * Fork incoming HTTP requests to handle the concurrently.
 *
 * @param   sfd         Server socket file descriptor.
 * @return  Exit status of server (EXIT_SUCCESS).
 *
 * The parent should accept a request and then fork off and let the child
 * handle the request.
 **/
int forking_server(int sfd) {
    /* Accept and handle HTTP request */
    while (true) {
    	/* Accept request */
        Request *request = accept_request(sfd);
        if(!(request)) {
            log("Unable to accept request: %s", strerror(errno));
            continue;
        }


	/* Ignore children */
        signal(SIGCHLD, SIG_IGN);

	/* Fork off child process to handle request */
        pid_t pid = fork();
        if(pid < 0) {
            log("Unable to create child process: %s", strerror(errno));
        }
        else if(pid == 0) {
            close(sfd);
            handle_request(request);
            exit(EXIT_SUCCESS);
        }
        else {
            free_request(request);
        }
    }

    /* Close server socket */
    return EXIT_SUCCESS;
}

/* vim: set expandtab sts=4 sw=4 ts=8 ft=c: */
