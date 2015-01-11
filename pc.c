#define _GNU_SOURCE
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

extern char **environ;

#define PN "pchroot"
#define PN_ PN": "

static void die(const char *txt, ...) __attribute__((format (printf, 1, 2)));
static void die(const char *txt, ...)
{
    va_list ap;
    va_start(ap, txt);
    vfprintf(stderr, txt, ap);
    va_end(ap);

    exit(1);
}

int main(int argc, char*const*argv)
{
    if (argc < 2)
        die("Usage: "PN" <name> [<command> ...]\n");

    if (*argv[1] == '.')
        die(PN_"chroot name can't start with a dot.\n");

    if (strchr(argv[1], '/'))
        die(PN_"only chroots directly in /srv/chroots/ are allowed.\n");

    const char *path = get_current_dir_name();

    if (chdir("/srv/chroots"))
        die(PN_"can't chdir to /srv/chroots: %s\n", strerror(errno));

    if (chroot(argv[1]))
        die(PN_"chroot(\"/srv/chroots/%s\") failed: %s\n", argv[1], strerror(errno));

    setuid(getuid());

    if (chdir(path))
    {
        if (chdir(getenv("HOME")))
            fprintf(stderr, PN_"couldn't chdir to either %s nor $HOME.\n", path);
        else
            fprintf(stderr, PN_"couldn't chdir to %s, going to $HOME.\n", path);
    }

    setenv("SCHROOT_CHROOT_NAME", argv[1], 1);

    if (argv[2])
        execvpe(argv[2], argv+2, environ);
    else
    {
        char*const args[] = { "bash", 0 };
        execve("/bin/bash", args, environ);
    }

    die(PN_"couldn't exec %s: %s\n", argv[2] ? argv[2] : "/bin/bash", strerror(errno));
}
