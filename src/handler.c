/* handler.c: HTTP Request Handlers */

#include "spidey.h"

#include <errno.h>
#include <limits.h>
#include <string.h>

#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

/* Internal Declarations */
Status handle_browse_request(Request *request);
Status handle_file_request(Request *request);
Status handle_cgi_request(Request *request);
Status handle_error(Request *request, Status status);

/**
 * Handle HTTP Request.
 *
 * @param   r           HTTP Request structure
 * @return  Status of the HTTP request.
 *
 * This parses a request, determines the request path, determines the request
 * type, and then dispatches to the appropriate handler type.
 *
 * On error, handle_error should be used with an appropriate HTTP status code.
 **/
Status  handle_request(Request *r) {
    Status result;

    /* Parse request */
<<<<<<< HEAD
    r = parse_request(r);
=======
    parse_request(r);


    /* HANDLER TEST*/
    fprintf(r->stream, "HTTP/1.0 200 ok\r\n");
    fprintf(r->stream, "Content-Type: text/html\r\n");
    fprintf(r->stream, "\r\n");

    fprintf(r->stream, "<h1>I fart in your general direction! Your mother was a hamster and your father smelt of elderberries! Monty Python </h1>\n");
>>>>>>> ef5500683265661839cf3adf237c48db3571aaf4

    /* Determine request path */
    r->path = determine_request_path( r->uri );
    debug("HTTP REQUEST PATH: %s", r->path);

    /* Dispatch to appropriate request handler type based on file type */
    log("HTTP REQUEST STATUS: %s", http_status_string(result));

    if(access((r->path), X_OK) == 0 ){
      result = handle_cgi_request( r );
    }
    else{
      struct stat s;
      if( stat(r->path, &s) || !S_ISDIR(s.st_mode)){
        result = handle_browse_request( r );
      }
      if(stat(r->path, &s) || !S_ISREG(s.st_mode)){
        result = handle_file_request( r );
      }
      else{
        result = handle_error( r , result );
      }
    }


    return result;
}

/**
 * Handle browse request.
 *
 * @param   r           HTTP Request structure.
 * @return  Status of the HTTP browse request.
 *
 * This lists the contents of a directory in HTML.
 *
 * If the path cannot be opened or scanned as a directory, then handle error
 * with HTTP_STATUS_NOT_FOUND.
 **/
Status  handle_browse_request(Request *r) {
<<<<<<< HEAD
  struct dirent **entries;
  int n;

  /* Open a directory for reading or scanning */
  n = scandir(r->path, &entries, 0, alphasort);
  if (!n) {
    debug("Could not scan directory: %s", strerror(errno));
    return HTTP_STATUS_NOT_FOUND;
  }

  /* Write HTTP Header with OK Status and text/html Content-Type */
  fprintf(r->stream, "HTTP/1.0 200 ok\r\n");
  fprintf(r->stream, "Content-Type: text/html\r\n");
  fprintf(r->stream, "\r\n");

  /* For each entry in directory, emit HTML list item */
  fprintf(r->stream, "<ul>\n");
  for( int i = 0; i < n; i++){
    if ((streq(entries[i]->d_name, ".")) || (streq(entries[i]->d_name, ".."))) {
        free(entries[i]);
        continue;
        }
    fprintf(r->stream, "<li>%s</li>\n", entries[i]->d_name);
    free(entries[i]);
  }
  free(entries);
  fprintf(r->stream, "<ul>\n");
  /* Return OK */
  return HTTP_STATUS_OK;
=======
    struct dirent **entries;
    int n;

    /* Open a directory for reading or scanning */
    if((n = scandir(".", &entries, 0, alphasort)) < 0)
        debug("scandir failed: %s", strerror(errno));
    
    /* Write HTTP Header with OK Status and text/html Content-Type */
    fprintf(r->stream, "HTTP/1.0 200 OK \r\n");
    fprintf(r->stream, "Content-Type: text/html\r\n");
    fprintf(r->stream, "\r\n");

    /* For each entry in directory, emit HTML list item */
    fprintf(r->stream, "<ol>\n");
    for(int i = 0; i < n; i++) {
        fprintf(r->stream, "<li>%s</li>", entries[i]->d_name);
    }
    fprintf(r->stream, "</ol>\n");

    /* Return OK */
    return HTTP_STATUS_OK;
>>>>>>> ef5500683265661839cf3adf237c48db3571aaf4
}

/**
 * Handle file request.
 *
 * @param   r           HTTP Request structure.
 * @return  Status of the HTTP file request.
 *
 * This opens and streams the contents of the specified file to the socket.
 *
 * If the path cannot be opened for reading, then handle error with
 * HTTP_STATUS_NOT_FOUND.
 **/
Status  handle_file_request(Request *r) {
    char *mimetype = NULL;
    size_t nread;

    /* Open file for reading */
    FILE *fs = fopen( r->path , "r");   // TODO:  make sure you wanna open r
    if( !fs ){
      debug("Unable to open file: %s", strerror(errno));
      goto fail;
    }

    /* Determine mimetype */
    mimetype =  determine_mimetype(r->path);

    /* Write HTTP Headers with OK status and determined Content-Type */
    fprintf(r->stream, "HTTP/1.0 200 ok\r\n");
    fprintf(r->stream, "Content-Type: %s\r\n", mimetype);
    fprintf(r->stream, "\r\n");

    /* Read from file and write to socket in chunks */
    char buffer[BUFSIZ];

    while ((nread = fread(buffer, 1, BUFSIZ, fs)) > 0 ){
      fwrite(buffer, 1, nread, r->stream);
    }

    /* Close file, deallocate mimetype, return OK */
    free(mimetype);
    fclose(fs);
    return HTTP_STATUS_OK;

fail:
    /* Close file, free mimetype, return INTERNAL_SERVER_ERROR */
    free(mimetype);
    fclose(fs);
    return HTTP_STATUS_INTERNAL_SERVER_ERROR;
}

/**
 * Handle CGI request
 *
 * @param   r           HTTP Request structure.
 * @return  Status of the HTTP file request.
 *
 * This popens and streams the results of the specified executables to the
 * socket.
 *
 * If the path cannot be popened, then handle error with
 * HTTP_STATUS_INTERNAL_SERVER_ERROR.
 **/
Status  handle_cgi_request(Request *r) {
    FILE *pfs;
    char buffer[BUFSIZ];

    /* Export CGI environment variables from request:
     * http://en.wikipedia.org/wiki/Common_Gateway_Interface */
    setenv("QUERY_STRING", r->query, 1);

    /* Export CGI environment variables from request headers */

    /* POpen CGI Script */
    pfs = popen("www/scripts/cowsay.sh" ,"r");

    /* Copy data from popen to socket */
    size_t nread = fread(buffer, 1, BUFSIZ, pfs);
    while(nread > 0) {
        fwrite(buffer, 1, nread, r->stream);
        nread = fread(buffer, 1, BUFSIZ, pfs);
    }

    /* Close popen, return OK */
    pclose(pfs);
    return HTTP_STATUS_OK;
}

/**
 * Handle displaying error page
 *
 * @param   r           HTTP Request structure.
 * @return  Status of the HTTP error request.
 *
 * This writes an HTTP status error code and then generates an HTML message to
 * notify the user of the error.
 **/
Status  handle_error(Request *r, Status status) {
    const char *status_string = http_status_string(status);

    /* Write HTTP Header */
    fprintf(r->stream, "HTTP/1.0 200 ok\r\n");
    fprintf(r->stream, "Content-Type: text/html\r\n");
    fprintf(r->stream, "\r\n");

    /* Write HTML Description of Error*/
    fprintf(r->stream, "%s\n", status_string);


    /* Return specified status */
    return status;
}

/* vim: set expandtab sts=4 sw=4 ts=8 ft=c: */
