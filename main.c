/**
 * Simple application to log JACK xruns as they occur
 */
#include <jack/jack.h>
#include <jack/statistics.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

jack_client_t *client;
pthread_mutex_t mx PTHREAD_MUTEX_INITIALIZER;
time_t timestamp;
struct tm *time_info;
unsigned int xrun_count;
char buffer[26];

/**
 * Callback to be executed on an xrun
 */
int on_xrun(void *arg) {
    (void) arg;
    // get the current time
    time(&timestamp);
    time_info = localtime(&timestamp);

    // format the timestamp
    strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", time_info);

    // get milliseconds of delay
    float delay = jack_get_xrun_delayed_usecs(client) / 1000.0f;

    // (safely) increment and get the xrun count
    unsigned int x;
    pthread_mutex_lock(&mx);
    x = ++xrun_count;
    pthread_mutex_unlock(&mx);

    // print to stdout
    fprintf(stdout, "%s xrun (%.2fms) - total xruns: %u\n", buffer, delay, x);
    fflush(stdout);

    return 0;
}

/**
 * Cleanup on JACK shutdown
 * @param arg
 */
void on_jack_shutdown(void *arg) {
    (void) arg;
    pthread_mutex_destroy(&mx);
    exit(1);
}

/**
 * Main
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char *argv[]) {
    const char *name = "xrun-logger";
    jack_status_t status;
    xrun_count = 0;

    client = jack_client_open(name, JackNoStartServer, &status, NULL);

    // make sure our client got created successfully
    if (client == NULL) {
        fprintf(stderr, "jack_client_open failed: 0x%2.0x\n", status);
        if (status & JackServerFailed) {
            fprintf(stderr, "could not connect to JACK server\n");
        }
        exit(1);
    }
    // get a unique name from jack if ours isn't
    if (status & JackNameNotUnique) {
        name = jack_get_client_name(client);
        fprintf(stderr, "unique client name assigned: %s\n", name);
    }

    // register callbacks
    jack_on_shutdown(client, on_jack_shutdown, 0);
    jack_set_xrun_callback(client, on_xrun, stdout);

    // tell jack we're ready
    if (jack_activate(client)) {
        fprintf(stderr, "could not activate client\n");
        exit(1);
    }

    // sleep forever(ish)
    sleep((unsigned)-1);

    return 0;
}