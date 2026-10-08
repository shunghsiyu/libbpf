// SPDX-License-Identifier: BSD-2-Clause
//
// Print `uname -r` and check it matches the kernel version we meant to test.
// If it does, this program really ran on the kernel we just built, not on
// the host's own kernel.
//
// usage: check_kernel <expected-release>
// exit:  0 = match, 1 = mismatch, 2 = usage or syscall error

#include <stdio.h>
#include <string.h>
#include <sys/utsname.h>

int main(int argc, char **argv)
{
	struct utsname u;

	if (argc != 2) {
		fprintf(stderr, "usage: %s <expected-release>\n", argv[0]);
		return 2;
	}

	/* uname(2) is what `uname -r` calls. */
	if (uname(&u)) {
		perror("uname");
		return 2;
	}

	printf("uname -r : %s\n", u.release);
	printf("expected : %s\n", argv[1]);

	if (strcmp(u.release, argv[1])) {
		puts("FAIL: this is not the kernel we built");
		return 1;
	}

	puts("OK: running on the kernel we built");
	return 0;
}
