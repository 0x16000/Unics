#include <sys/poll.h>
#include <stddef.h>

int
poll(struct pollfd fds[], nfds_t nfds, int timeout)
{
    (void)timeout;
    if (fds == NULL && nfds > 0)
        return -1;

    for (nfds_t i = 0; i < nfds; i++)
        fds[i].revents = 0;

    return 0;
}

#if __POSIX_VISIBLE >= 202405 || __BSD_VISIBLE
int
ppoll(struct pollfd fds[], nfds_t nfds,
      const struct timespec *timeout, const sigset_t *sigmask)
{
    (void)timeout;
    (void)sigmask;

    if (fds == NULL && nfds > 0)
        return -1;

    for (nfds_t i = 0; i < nfds; i++)
        fds[i].revents = 0;

    return 0;
}
#endif
