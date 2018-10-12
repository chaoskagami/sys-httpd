/* sendfile.c

   Implement sendfile() in terms of read(), write(), and lseek().

   The source code file is (C) 2010, Michael Kerrisk, and is licensed under the GNU General Public License, version 3.
*/

#include <arpa/inet.h>          /* inet_ntoa */
#include <signal.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>

#include <switch.h>

#include "misc.h"
#include "minmax.h"
#define BUF_SIZE (size_t)8192

ssize_t
sendfile(int out_fd, int in_fd, off_t *offset, size_t count)
{
    off_t orig = 0;
    char buf[BUF_SIZE];
    size_t toRead, numRead, numSent, totSent;

    if (offset != NULL) {

        /* Save current file offset and set offset to value in '*offset' */

        orig = lseek(in_fd, 0, SEEK_CUR);
        if (orig == -1)
            return -1;
        if (lseek(in_fd, *offset, SEEK_SET) == -1)
            return -1;
    }

    totSent = 0;

    while (count > 0) {
        toRead = min(BUF_SIZE, count);

        numRead = read(in_fd, buf, toRead);
        if (numRead == -1UL)
            return -1;
        if (numRead == 0)
            break;                      /* EOF */

        numSent = write(out_fd, buf, numRead);
        if (numSent == -1UL)
            return -1;
        if (numSent == 0)               /* Should never happen */
            fatalLater(3); // Transferred zero bytes.

        count -= numSent;
        totSent += numSent;
    }

    if (offset != NULL) {

        /* Return updated file offset in '*offset', and reset the file offset
           to the value it had when we were called. */

        *offset = lseek(in_fd, 0, SEEK_CUR);
        if (*offset == -1)
            return -1;
        if (lseek(in_fd, orig, SEEK_SET) == -1)
            return -1;
    }

    return totSent;
}
