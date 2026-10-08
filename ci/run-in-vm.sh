#!/bin/bash
# Runs INSIDE the VM that vmtest boots with the freshly built kernel.
# vmtest shares the runner's filesystem, so the workspace is right here.
#
# To swap in your own test: replace the line marked "YOUR TEST" below.
# Keep the last two lines: they report the exit code back to the workflow.

STATUS_FILE=${STATUS_FILE:-/mnt/vmtest/exitstatus}

uname -a

./test/check_kernel "$(cat expected-release)"     # <-- YOUR TEST
rc=$?

# The workflow reads "<name>:<exit code>" lines from this file (step 5).
echo "check_kernel:${rc}" >> "${STATUS_FILE}"
exit 0   # the verdict travels via STATUS_FILE, not via this script's own exit code
