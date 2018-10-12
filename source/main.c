#include <string.h>
#include <stdio.h>
#include <dirent.h>

#include <errno.h>
#include <malloc.h>
#include <stdarg.h>
#include <unistd.h>
#include "http.h"

// only for mkdir, used when creating the "logs" directory
#include <sys/stat.h>

#include <switch.h>

#include "misc.h"

#define TITLE_ID 0x420000000000000F
#define HEAP_SIZE 0x540000

#define POLL_UNKNOWN (~(POLLIN | POLLPRI | POLLOUT))

#define SOCK_BUFFERSIZE 16384

// we aren't an applet
u32 __nx_applet_type = AppletType_None;

// setup a fake heap
char fake_heap[HEAP_SIZE];

static AppletHookCookie cookie;

// we override libnx internals to do a minimal init
void __libnx_initheap(void)
{
    extern char *fake_heap_start;
    extern char *fake_heap_end;

    // setup newlib fake heap
    fake_heap_start = fake_heap;
    fake_heap_end = fake_heap + HEAP_SIZE;
}

void registerFspLr()
{
    if (kernelAbove400())
        return;

    Result rc = fsprInitialize();
    if (R_FAILED(rc))
        fatalLater(rc);

    u64 pid;
    svcGetProcessId(&pid, CUR_PROCESS_HANDLE);

    rc = fsprRegisterProgram(pid, TITLE_ID, FsStorageId_NandSystem, NULL, 0, NULL, 0);
    if (R_FAILED(rc))
        fatalLater(rc);
    fsprExit();
}

void __appInit(void)
{
    Result rc;
    svcSleepThread(10000000000L);
    rc = smInitialize();
    if (R_FAILED(rc))
        fatalLater(rc);
    rc = fsInitialize();
    if (R_FAILED(rc))
        fatalLater(rc);
    registerFspLr();
    rc = fsdevMountSdmc();
    if (R_FAILED(rc))
        fatalLater(rc);
    rc = timeInitialize();
    if (R_FAILED(rc))
        fatalLater(rc);
    rc = hidInitialize();
    if (R_FAILED(rc))
        fatalLater(rc);
}

void __appExit(void)
{
    fsdevUnmountAll();
    fsExit();
    smExit();
    timeExit();
}

static void applet_hook(__attribute__((unused)) AppletHookType type, __attribute__((unused)) void *closure) {}

void pre_init(void)
{
  static const SocketInitConfig socketInitConfig = {
      .bsdsockets_version = 1,

      .tcp_tx_buf_size = 8 * SOCK_BUFFERSIZE,
      .tcp_rx_buf_size = 8 * SOCK_BUFFERSIZE,
      .tcp_tx_buf_max_size = 16 * SOCK_BUFFERSIZE,
      .tcp_rx_buf_max_size = 16 * SOCK_BUFFERSIZE,

      .udp_tx_buf_size = 0x2400,
      .udp_rx_buf_size = 0xA500,

      .sb_efficiency = 8,
  };

  Result ret = socketInitialize(&socketInitConfig);
  if (ret != 0)
    fatalLater(ret);

  /* register applet hook */
  appletHook(&cookie, applet_hook, NULL);
}

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    FILE *should_log_file = fopen("/logs/httpd_log_enabled", "r");
    if (should_log_file != NULL)
    {
        should_log = true;
        fclose(should_log_file);

        mkdir("/logs", 0700);
        unlink("/logs/httpd.log");

        log_init();
    }

    int status = 0;

    pre_init();
    http_init();
    while (appletMainLoop())
    {
        status = http_loop();
        if (status != 0)
            break;
    }
    http_exit();
    socketExit();

    return 0;
}
