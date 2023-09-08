/* Copyright (c) 2023, Michael Santos <michael.santos@gmail.com>
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
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#if defined(PDEATHSIGEXEC_prctl)
#include <sys/prctl.h>
#elif defined(PDEATHSIGEXEC_procctl)
#include <sys/procctl.h>
#else
#pragma message "unsupported platform"
#endif

void _init(void);

void _init(void) {
  int sig = SIGKILL;
  char *env_sig;

  env_sig = getenv("PDEATHSIGEXEC_SIGNAL");
  if (env_sig)
    sig = atoi(env_sig);

  if (sig < 0 || sig >= NSIG)
    sig = SIGKILL;

#if defined(PDEATHSIGEXEC_prctl)
  if (prctl(PR_SET_PDEATHSIG, sig) != 0) {
    _exit(111);
  }
#elif defined(PDEATHSIGEXEC_procctl)
  if (procctl(P_PID, 0, PROC_PDEATHSIG_CTL, &sig) != 0) {
    _exit(111);
  }
#endif
}
