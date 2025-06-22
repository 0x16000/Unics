#include <sys/poll.h>
#include <stddef.h>

/*
 * Stub implementation of poll()
 * Always returns 0 (no events), or -1 on invalid args.
 */
int
poll(struct pollfd fds[], nfds_t nfds, int timeout)
{
    (void)timeout;  // Unused

    if (fds == NULL && nfds > 0)
        return -1;

    for (nfds_t i = 0; i < nfds; i++) {
        fds[i].revents = 0;  // No events
    }

    return 0;  // No file descriptors are ready
}

#if __POSIX_VISIBLE >= 202405 || __BSD_VISIBLE
/*
 * Stub implementation of ppoll()
 * Same as poll() for now; ignores timeout and sigmask.
 */
int
ppoll(struct pollfd fds[], nfds_t nfds, const struct timespec *timeout, const sigset_t *sigmask)
{
    (void)timeout;
    (void)sigmask;

    if (fds == NULL && nfds > 0)
        return -1;

    for (nfds_t i = 0; i < nfds; i++) {
        fds[i].revents = 0;
    }

    return 0;
}
#endif
