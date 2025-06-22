#include <sys/termios.h>
#include <sys/unistd.h>
#include <sys/ioctl.h>
#include <errno.h>

/* Remove the __errno_location implementation from here */
extern int *__errno_location(void);

speed_t cfgetispeed(const struct termios *termios_p)
{
    if (!termios_p) {
        errno = EINVAL;
        return (speed_t)-1;
    }
    return termios_p->c_ispeed;
}

speed_t cfgetospeed(const struct termios *termios_p)
{
    if (!termios_p) {
        errno = EINVAL;
        return (speed_t)-1;
    }
    return termios_p->c_ospeed;
}

int cfsetispeed(struct termios *termios_p, speed_t speed)
{
    if (!termios_p) {
        errno = EINVAL;
        return -1;
    }
    
    /* Validate speed */
    switch (speed) {
        case B0: case B50: case B75: case B110: case B134:
        case B150: case B200: case B300: case B600: case B1200:
        case B1800: case B2400: case B4800: case B9600:
        case B19200: case B38400:
            termios_p->c_ispeed = speed;
            return 0;
        default:
            errno = EINVAL;
            return -1;
    }
}

int cfsetospeed(struct termios *termios_p, speed_t speed)
{
    if (!termios_p) {
        errno = EINVAL;
        return -1;
    }
    
    /* Validate speed */
    switch (speed) {
        case B0: case B50: case B75: case B110: case B134:
        case B150: case B200: case B300: case B600: case B1200:
        case B1800: case B2400: case B4800: case B9600:
        case B19200: case B38400:
            termios_p->c_ospeed = speed;
            return 0;
        default:
            errno = EINVAL;
            return -1;
    }
}

int tcdrain(int fd)
{
    if (fd < 0) {
        errno = EBADF;
        return -1;
    }
    return ioctl(fd, TIOCDRAIN);
}

int tcflow(int fd, int action)
{
    if (fd < 0) {
        errno = EBADF;
        return -1;
    }

    switch (action) {
        case TCOOFF:
            return ioctl(fd, TIOCSTOP, 1);
        case TCOON:
            return ioctl(fd, TIOCSTART, 1);
        case TCIOFF:
            return ioctl(fd, TIOCSTOP, 0);
        case TCION:
            return ioctl(fd, TIOCSTART, 0);
        default:
            errno = EINVAL;
            return -1;
    }
}

int tcflush(int fd, int queue_selector)
{
    if (fd < 0) {
        errno = EBADF;
        return -1;
    }

    switch (queue_selector) {
        case TCIFLUSH:
            return ioctl(fd, TIOCFLUSH, (void *)1);  /* flush input */
        case TCOFLUSH:
            return ioctl(fd, TIOCFLUSH, (void *)2);  /* flush output */
        case TCIOFLUSH:
            return ioctl(fd, TIOCFLUSH, (void *)3);  /* flush both */
        default:
            errno = EINVAL;
            return -1;
    }
}

int tcgetattr(int fd, struct termios *termios_p)
{
    if (fd < 0 || !termios_p) {
        errno = EINVAL;
        return -1;
    }
    return ioctl(fd, TIOCGETA, termios_p);
}

pid_t tcgetsid(int fd)
{
    pid_t sid;
    if (fd < 0) {
        errno = EBADF;
        return (pid_t)-1;
    }
    if (ioctl(fd, TIOCGSID, &sid) == -1) {
        return (pid_t)-1;
    }
    return sid;
}

int tcsendbreak(int fd, int duration)
{
    if (fd < 0) {
        errno = EBADF;
        return -1;
    }

    /* For duration == 0, send break for 0.25-0.5 seconds */
    if (duration == 0) {
        return ioctl(fd, TIOCSBRK);
    }
    
    /* For non-zero duration, we'll implement a simple loop */
    if (duration > 0) {
        if (ioctl(fd, TIOCSBRK) == -1)
            return -1;
        
        /* Simple delay - in a real system we'd use proper timing */
        for (volatile int i = 0; i < duration * 10000; i++);
        
        return ioctl(fd, TIOCCBRK);
    }
    
    /* Negative duration is invalid */
    errno = EINVAL;
    return -1;
}

int tcsetattr(int fd, int optional_actions, const struct termios *termios_p)
{
    if (fd < 0 || !termios_p) {
        errno = EINVAL;
        return -1;
    }

    switch (optional_actions) {
        case TCSANOW:
            return ioctl(fd, TIOCSETA, termios_p);
        case TCSADRAIN:
            return ioctl(fd, TIOCSETAW, termios_p);
        case TCSAFLUSH:
            return ioctl(fd, TIOCSETAF, termios_p);
        default:
            errno = EINVAL;
            return -1;
    }
}
