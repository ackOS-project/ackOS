#pragma once

enum
{
    EPERM = 1,       /* operation not permitted (not root) */
    ENOENT,          /* no such file or directory */
    ESRCH,           /* no such process */
    EINTR,           /* interrupted syscall */
    EIO,             /* input/output error */
    ENXIO,           /* no such device or address */
    E2BIG,           /* argument list too long */
    ENOEXEC,         /* executable format error */
    EBADF,           /* invalid file descriptor */
    ECHILD,          /* no child processes */
    EAGAIN,          /* try again */
    ENOMEM,          /* out of memory */
    EACCES,          /* access denied */
    EFAULT,          /* invalid address */
    ENOTBLK,         /* block device required */
    EBUSY,           /* device or resource busy */
    EEXIST,          /* file already exists */
    EXDEV,           /* cross device link */
    ENODEV,          /* no such device */
    ENOTDIR,         /* not a directory */
    EISDIR,          /* is a directory */
    EINVAL,          /* invalid argument */
    ENFILE,          /* file table overflow */
    EMFILE,          /* too many open files */
    ENOTTY,          /* not in tty mode */
    ETXTBSY,         /* text file busy */
    EFBIG,           /* file too large */
    ENOSPC,          /* no disk space left */
    ESPIPE,          /* illegal seek */
    EROFS,           /* read-only file system */
    EMLINK,          /* too many symlinks */
    EPIPE,           /* broken pipe */
    EDOM,            /* numerical argument out of domain */
    ERANGE,          /* numerical result out of range */
    EDEADLK,         /* resource deadlock */
    ENAMETOOLONG,    /* file name too long */
    ENOLCK,          /* no record locks available */
    ENOSYS,          /* invalid syscall number */
    ENOTEMPTY,       /* directory not empty */
    ELOOP,           /* too many symlinks encountered */
    ENOMSG,          /* no message of desired type */
    EIDRM,           /* identifier removed */
    ECHRNG,          /* channel number out of range */
    EL2NSYNC,        /* level 2 not synchronised */
    EL3HLT,          /* level 3 halted */
    EL3RST,          /* level 3 reset */
    ELNRNG,          /* link number out of range */
    EUNATCH,         /* protocol driver not attached */
    ENOCSI,          /* no CSI structure available */
    EL2HLT,          /* level 2 halted */
    EBADE,           /* invalid exchange */
    EBADR,           /* invalid request descriptor */
    EXFULL,          /* exchange full */
    ENOANO,          /* no anode */
    EBADRQC,         /* invalid request code */
    EBADSLT,         /* invalid slot */
    EBFONT,          /* invalid font file format */
    ENOSTR,          /* device not a stream */
    ENODATA,         /* no data available */
    ETIME,           /* timer expired */
    ENOSR,           /* out of streams resources */
    ENONET,          /* machine is not on the network */
    ENOPKG,          /* package not installed */
    EREMOTE,         /* object is remote */
    ENOLINK,         /* link has been served */
    EADV,            /* advertise error */
    ESRMNT,          /* srmount error */
    ECOMM,           /* communication error on send */
    EPROTO,          /* protocol error */
    EMULTIHOP,       /* multihop attempted */
    EBADMSG,         /* not a data message */
    EOVERFLOW,       /* value too large for defined data type */
    ENOTUNIQ,        /* name not unique on network */
    EBADFD,          /* file descriptor in bad state */
    EREMCHG,         /* remote address changed */
    ELIBACC,         /* cannot access a needed shared library */
    ELIBBAD,         /* corrupted shared library */
    ELIBSCN,         /* .lib section in a.out corrupted */
    ELIBMAX,         /* too many shared libraries */
    ELIBEXEC,        /* cannot execute a shared libary directly */
    EILSEQ,          /* illegal byte sequence */
    ERESTART,        /* interrupted syscall should be restarted */
    ESTRPIPE,        /* streams pipe error */
    EUSERS,          /* too many users */
    ENOTSOCK,        /* socket operation on non-socket */
    EDESTADDRREQ,    /* destination address required */
    EMSGSIZE,        /* message too long */
    EPROTOTYPE,      /* protocol wrong type for socket */
    ENOPROTOOPT,     /* protocol not available */
    EPROTONOSUPPORT, /* protocol not supported */
    ESOCKTNOSUPPORT, /* socket type unsupported */
    EOPNOTSUPP,      /* operation not supported on transport endpoint */
    EPFNOSUPPORT,    /* protocol family not supported */
    EAFNOSUPPORT,    /* address family not supported by protocol */
    EADDRINUSE,      /* address already in use */
    EADDRNOTAVAIL,   /* cannot assign rquested adress */
    ENETDOWN,        /* network is down */
    ENETUNREACH,     /* network is unreachable */
    ENETRESET,       /* network dropped connection because of reset */
    ECONNABORTED,    /* connection aborted */
    ECONNRESET,      /* connection reset by peer */
    ENOBUFS,         /* no buffer space available */
    EISCONN,         /* socket is already connected */
    ENOTCONN,        /* socket is not connected */
    ESHUTDOWN,       /* cannot send after socket shutdown */
    ETOOMANYREFS,    /* too many references */
    ETIMEDOUT,       /* connection timed out */
    ECONNREFUSED,    /* connection refused */
    EHOSTDOWN,       /* host is down */
    EHOSTUNREACH,    /* host is unreachable */
    EALREADY,        /* operation is already in progress */
    EINPROGRESS,     /* operation is now in progress */
    ESTALE,          /* stale file handle */
    EDQUOT,          /* quota exceeded */
    ECANCELED,       /* operation canceled */
    EOWNERDEAD,      /* owner died */
    ENOTRECOVERABLE, /* state not recoverable */
    ENOTSUP,         /* not supported */

    EWOULDBLOCK = EAGAIN /* operation would block */
};

extern int errno;
