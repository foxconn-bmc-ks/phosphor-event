#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include "event_messaged_sdbus.h"

using namespace std;

static const char *LOCK_PATH = "/var/lib/obmc/events.lock";
static const char *LOG_DIR_PATH = "/var/lib/obmc/events";
static const char *METADATA_PATH = "/var/lib/obmc/events.metadata";
static const char *PID_PATH = "/run/obmc-phosphor-event.pid";

static void print_usage (void)
{
    cerr << "[-s <x>] : Maximum bytes to use for event logger" << endl;
    cerr << "[-t <x>] : Maximum number of logs" << endl;
}

static void save_pid (void)
{
    pid_t pid;
    ofstream f;
    pid = getpid();
    f.open(PID_PATH);
    if (!f) {
        cerr << "DEBUG: failed to open pidfile" << endl;
        return;
    }
    f << pid << endl;
    f.close();
}

int main (int argc, char *argv[])
{
    size_t max_size;
    uint16_t max_logs;
    int c;
    int err;
    max_size = 0;
    max_logs = 0;
    while ((c = getopt(argc, argv, "s:t:")) != -1) {
        switch (c) {
            case 's':
                max_size = (size_t) strtoul(optarg, NULL, 10);
                break;
            case 't':
                max_logs = (uint16_t) strtoul(optarg, NULL, 10);
                break;
            case 'h':
            case '?':
                print_usage();
                return 1;
        }
    }
    save_pid();
    EventManager em(LOG_DIR_PATH, LOCK_PATH, METADATA_PATH, max_logs, max_size);
    if ((err = bus_build(&em)) == 0) {
        bus_mainloop();
        bus_cleaup();
    }
    return err;
}
