# sys-httpd

This is a Nintendo Switch system module which provides an HTTP 1.1 server in the background (on port 80.)

By default, it serves files from `/srv` on the memory card. It will not automatically serve an index file in a directory - you'll have to append `/index.html` to the URL if you don't want the generated listing.

It does not support fancy server-side languages. Do not complain about lack of support for things like PHP or Ruby.

Attempting to serve files that are huge may cause issues. I haven't tested this.

Unfortunately, due to how little memory is available to system modules, this cannot coexist with sys-ftpd at the moment. Perhaps someday.

Why?
 * Once homebrew developers figure out how to spawn the browser manually from homebrew, a server on localhost can provide a pseudo-browser with minimal effort.
 * Dunno, maybe HTTP is too much of a pain for you?
 * If appstore-nx ever gets support for multiple repositories, this could be repurposed to serve software to other switches from a switch.
 * Why not?

This contains code from multiple projects:
 * sys-ftpd (which is in turn derived from a long list of ftpd forks) - GPLv3
 * sendfile example implementation (from a book's provided solutions) - GPLv3, see header of sendfile.c
 * tiny http server (I cannot for the life of me remember where this came from - aside from it originating from /g/ about two years ago. I've heavily modified it. If this is your code and you're upset about me using it, contact me and I'll port my codebase to another tiny http server)
