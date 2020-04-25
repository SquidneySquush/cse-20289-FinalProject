#!/usr/bin/env python3

import concurrent.futures
import os
import requests
import sys
import time

# Functions

def usage(status=0):
    progname = os.path.basename(sys.argv[0])
    print(f'''Usage: {progname} [-h HAMMERS -t THROWS] URL
    -h  HAMMERS     Number of hammers to utilize (1)
    -t  THROWS      Number of throws per hammer  (1)
    -v              Display verbose output
    ''')
    sys.exit(status)

def hammer(url, throws, verbose, hid):
    ''' Hammer specified url by making multiple throws (ie. HTTP requests).

    - url:      URL to request
    - throws:   How many times to make the request
    - verbose:  Whether or not to display the text of the response
    - hid:      Unique hammer identifier

    Return the average elapsed time of all the throws.
    '''
    total_time = 0
    for i in range(throws):
        timer = time.time()
        r = requests.get(url)
        if verbose:
            print(r.text)
        timer = time.time() - timer
        total_time = total_time + timer
        print(f'Hammer: {hid}, Throw:   {i}, Elapsed Time: {timer:.2f}')
    print(f'Hammer: {hid}, AVERAGE   , Elapsed Time: {total_time:.2f}')

    return total_time


def do_hammer(args):
    ''' Use args tuple to call `hammer` '''
    return hammer(*args)

def main():
    hammers = 1
    throws  = 1
    verbose = False
    arguments = sys.argv[1:]

    # Parse command line arguments
    while arguments:
        if arguments[0] == '-h':
            hammers = int(arguments[1])
            arguments.pop(0)
        elif arguments[0] == '-t':
            throws = int(arguments[1])
            arguments.pop(0)
        elif arguments[0] == '-v':
            verbose = True
        else:
            if not len(arguments) == 1:
                usage(1)
            else:
                url = arguments[0]
        arguments.pop(0)

    # Create pool of workers and perform throws
    args = ((url, throws, verbose, hid) for hid in range(hammers)) 

    with concurrent.futures.ProcessPoolExecutor(hammers) as executor:
        executor.map(do_hammer, args)

# Main execution

if __name__ == '__main__':
    main()

# vim: set sts=4 sw=4 ts=8 expandtab ft=python:
