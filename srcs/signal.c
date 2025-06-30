#include "../lib/ping.h"

void handle_sigint(int sig)
{
    if (sig == SIGINT)
        g_loop_ping = false;
}
