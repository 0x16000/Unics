#ifndef _TERMIOS_H
#define _TERMIOS_H

#include <sys/cdefs.h>
#include <sys/_types.h>
#include <sys/ttycom.h>
#include <sys/ttydefaults.h>
#include <sys/types.h>

__BEGIN_DECLS

typedef __uint32_t tcflag_t;
typedef __uint8_t cc_t;
typedef __uint32_t speed_t;

#define NCCS 20

struct termios {
    tcflag_t c_iflag;    /* input mode flags */
    tcflag_t c_oflag;    /* output mode flags */
    tcflag_t c_cflag;    /* control mode flags */
    tcflag_t c_lflag;    /* local mode flags */
    cc_t c_cc[NCCS];     /* control characters */
    speed_t c_ispeed;    /* input speed */
    speed_t c_ospeed;    /* output speed */
};

/* c_iflag bits */
#define IGNBRK  0x00000001  /* ignore BREAK condition */
#define BRKINT  0x00000002  /* map BREAK to SIGINTR */
#define IGNPAR  0x00000004  /* ignore (discard) parity errors */
#define PARMRK  0x00000008  /* mark parity and framing errors */
#define INPCK   0x00000010  /* enable checking of parity errors */
#define ISTRIP  0x00000020  /* strip 8th bit off chars */
#define INLCR   0x00000040  /* map NL to CR */
#define IGNCR   0x00000080  /* ignore CR */
#define ICRNL   0x00000100  /* map CR to NL */
#define IUCLC   0x00000200  /* map upper case to lower case */
#define IXON    0x00000400  /* enable output flow control */
#define IXANY   0x00000800  /* any char will restart output */
#define IXOFF   0x00001000  /* enable input flow control */
#define IMAXBEL 0x00002000  /* ring bell on input queue full */

/* c_oflag bits */
#define OPOST   0x00000001  /* enable following output processing */
#define OLCUC   0x00000002  /* map lower case to upper case */
#define ONLCR   0x00000004  /* map NL to CR-NL */
#define OCRNL   0x00000008  /* map CR to NL */
#define ONOCR   0x00000010  /* no CR output at column 0 */
#define ONLRET  0x00000020  /* NL performs CR function */
#define OFILL   0x00000040  /* use fill characters for delay */
#define OFDEL   0x00000080  /* fill is DEL, else NUL */
#define NLDLY   0x00000100  /* \n delay */
#define   NL0   0x00000000
#define   NL1   0x00000100
#define CRDLY   0x00000600  /* \r delay */
#define   CR0   0x00000000
#define   CR1   0x00000200
#define   CR2   0x00000400
#define   CR3   0x00000600
#define TABDLY  0x00001800  /* horizontal tab delay */
#define   TAB0  0x00000000
#define   TAB1  0x00000800
#define   TAB2  0x00001000
#define   TAB3  0x00001800
#define BSDLY   0x00002000  /* \b delay */
#define   BS0   0x00000000
#define   BS1   0x00002000
#define VTDLY   0x00004000  /* vertical tab delay */
#define   VT0   0x00000000
#define   VT1   0x00004000
#define FFDLY   0x00008000  /* form feed delay */
#define   FF0   0x00000000
#define   FF1   0x00008000

/* c_cflag bits */
#define CBAUD   0x0000100F
#define  B0     0x00000000  /* hang up */
#define  B50    0x00000001
#define  B75    0x00000002
#define  B110   0x00000003
#define  B134   0x00000004
#define  B150   0x00000005
#define  B200   0x00000006
#define  B300   0x00000007
#define  B600   0x00000008
#define  B1200  0x00000009
#define  B1800  0x0000000A
#define  B2400  0x0000000B
#define  B4800  0x0000000C
#define  B9600  0x0000000D
#define  B19200 0x0000000E
#define  B38400 0x0000000F
#define EXTA    B19200
#define EXTB    B38400
#define CSIZE   0x00000030  /* character size mask */
#define   CS5   0x00000010  /* 5 bits */
#define   CS6   0x00000020  /* 6 bits */
#define   CS7   0x00000030  /* 7 bits */
#define   CS8   0x00000000  /* 8 bits */
#define CSTOPB  0x00000040  /* send 2 stop bits */
#define CREAD   0x00000080  /* enable receiver */
#define PARENB  0x00000100  /* parity enable */
#define PARODD  0x00000200  /* odd parity, else even */
#define HUPCL   0x00000400  /* hang up on last close */
#define CLOCAL  0x00000800  /* ignore modem status lines */
#define CCTS_OFLOW 0x00001000 /* CTS flow control of output */
#define CRTS_IFLOW 0x00002000 /* RTS flow control of input */
#define MDMBUF  0x00004000  /* flow control output via Carrier */

/* c_lflag bits */
#define ISIG    0x00000001  /* enable signals INTR, QUIT, [D]SUSP */
#define ICANON  0x00000002  /* canonical input (erase and kill processing) */
#define XCASE   0x00000004  /* canonical upper/lower presentation */
#define ECHO    0x00000008  /* enable echo */
#define ECHOE   0x00000010  /* echo ERASE as BS-SPACE-BS */
#define ECHOK   0x00000020  /* echo KILL by starting new line */
#define ECHONL  0x00000040  /* echo NL even if ECHO is off */
#define NOFLSH  0x00000080  /* disable flush after INTR, QUIT, SUSP */
#define TOSTOP  0x00000100  /* send SIGTTOU for background output */
#define ECHOCTL 0x00000200  /* echo control chars as ^X */
#define ECHOPRT 0x00000400  /* visual erase mode for hardcopy */
#define ECHOKE  0x00000800  /* visual kill for hardcopy */
#define FLUSHO  0x00001000  /* output being flushed (state) */
#define PENDIN  0x00002000  /* retype pending input (state) */
#define IEXTEN  0x00004000  /* enable DISCARD and LNEXT */

/* c_cc indices */
#define VINTR    0
#define VQUIT    1
#define VERASE   2
#define VKILL    3
#define VEOF     4
#define VTIME    5
#define VMIN     6
#define VSWTC    7
#define VSTART   8
#define VSTOP    9
#define VSUSP    10
#define VEOL     11
#define VREPRINT 12
#define VDISCARD 13
#define VWERASE  14
#define VLNEXT   15
#define VEOL2    16

/* tcflow() actions */
#define TCOOFF   0  /* suspend output */
#define TCOON    1  /* restart output */
#define TCIOFF   2  /* transmit a STOP character */
#define TCION    3  /* transmit a START character */

/* tcflush() actions */
#define TCIFLUSH  0  /* flush data received but not read */
#define TCOFLUSH  1  /* flush data written but not transmitted */
#define TCIOFLUSH 2  /* flush both */

/* tcsetattr() actions */
#define TCSANOW   0  /* change immediately */
#define TCSADRAIN 1  /* change when output has drained */
#define TCSAFLUSH 2  /* flush buffers and change */

speed_t cfgetispeed(const struct termios *);
speed_t cfgetospeed(const struct termios *);
int cfsetispeed(struct termios *, speed_t);
int cfsetospeed(struct termios *, speed_t);
int tcdrain(int);
int tcflow(int, int);
int tcflush(int, int);
int tcgetattr(int, struct termios *);
pid_t tcgetsid(int);
int tcsendbreak(int, int);
int tcsetattr(int, int, const struct termios *);

__END_DECLS

#endif /* _TERMIOS_H */
