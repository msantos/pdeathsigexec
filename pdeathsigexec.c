/* Copyright (c) 2023-2024, Michael Santos <michael.santos@gmail.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#include <err.h>
#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <signal.h>

#if defined(PDEATHSIGEXEC_prctl)
#include <sys/prctl.h>
#elif defined(PDEATHSIGEXEC_procctl)
#include <sys/procctl.h>
#else
#pragma message "unsupported platform"
#endif

#define PDEATHSIGEXEC_VERSION "0.4.0"

extern char *__progname;

static void usage(void);

static const struct option long_options[] = {
    {"help", no_argument, NULL, 'h'},
    {"signal", required_argument, NULL, 's'},
    {NULL, 0, NULL, 0}};

int main(int argc, char *argv[]) {
  int ch;
  int sig = SIGKILL;
  char *env_sig;

  env_sig = getenv("PDEATHSIGEXEC_SIGNAL");
  if (env_sig)
    sig = atoi(env_sig);

  while ((ch = getopt_long(argc, argv, "+hs:", long_options, NULL)) != -1) {
    switch (ch) {
    case 's':
      sig = atoi(optarg);
      break;
    case 'h':
    default:
      usage();
      exit(2);
    }
  }

  if (sig < 0 || sig >= NSIG) {
    (void)fprintf(stderr, "invalid signal: %d\n", sig);
    usage();
    exit(2);
  }

  argc -= optind;
  argv += optind;

  if (argc < 1) {
    usage();
    exit(2);
  }

#if defined(PDEATHSIGEXEC_prctl)
  if (prctl(PR_SET_PDEATHSIG, sig) != 0) {
    err(111, "prctl");
  }
#elif defined(PDEATHSIGEXEC_procctl)
  if (procctl(P_PID, 0, PROC_PDEATHSIG_CTL, &sig) != 0) {
    err(111, "procctl");
  }
#else
  errno = EPERM;
  err(111, "unsupported platform");
#endif

  (void)execvp(argv[0], argv);

  err(errno == ENOENT ? 127 : 126, "%s", argv[0]);
}

static void usage(void) {
  (void)fprintf(stderr,
                "%s [OPTION] <COMMAND> <...>\n"
                "version: %s\n"
                "-s, --signal              termination signal\n"
                "-h, --help                usage summary\n",
                __progname, PDEATHSIGEXEC_VERSION);
}
