# Kernel CI demo

Demo for the talk *Building Your Own Linux Kernel CI on Commodity Platforms*.

A GitHub Actions workflow that builds the **latest stable Linux kernel**, boots
it in a VM with [vmtest](https://github.com/danobi/vmtest), and runs a tiny C
program inside it. The program prints `uname -r` and checks that it matches the
version we meant to test, which proves the VM really booted the new kernel.

## Workflow steps and the five stages

Everything lives in one job in [`.github/workflows/kernel-ci-demo.yml`](.github/workflows/kernel-ci-demo.yml).

| # | Stage | Workflow step | What it does |
|---|-------|---------------|--------------|
| 1 | Trigger | `on:` block | Runs on push, on `workflow_dispatch`, and daily (cron). |
| 2 | Get the kernel | `2. Get the kernel (latest stable)` | Shallow-clones (depth 1) the stable branch named in `KERNEL_BRANCH` from Google's kernel.org mirror, reads the exact release from the kernel's `Makefile`, and prints the version and commit. |
| 3 | Build | `3. Build the kernel and the test` | Small config (`defconfig` + `kvm_guest.config` + [`ci/kernel.config`](ci/kernel.config), the options vmtest needs), builds `bzImage`, then compiles the test. |
| 4 | Boot and test | `4. Boot the new kernel in vmtest and run the test` | `libbpf/ci/run-vmtest` boots the kernel under QEMU and runs [`ci/run-in-vm.sh`](ci/run-in-vm.sh) inside the VM. |
| 5 | Result | `5. Result (pass/fail from the test's exit code)` | Reads the test's exit code from `exitstatus`; zero is PASS, anything else (or nothing) is FAIL. |

The test is [`test/check_kernel.c`](test/check_kernel.c). It exits 0 when
`uname -r` equals the expected release, 1 on mismatch, 2 on a usage error.

## Swap in your own test

1. Put your program or script in `test/`, and build it in step 3 (next to the `gcc` line).
2. In [`ci/run-in-vm.sh`](ci/run-in-vm.sh), replace the line marked `YOUR TEST` with your command.
   It runs inside the VM, from the repository root.
3. Keep `echo "<name>:${rc}" >> "${STATUS_FILE}"`: that is how the exit code gets out of the VM.
   If you rename `<name>`, change the `check_kernel:` match in step 5 too.

## Good to know

- **GitHub only runs `schedule:` from the default branch.** The cron trigger does
  nothing until this workflow file is on the default branch. Push and manual runs work anywhere.
- vmtest needs KVM. GitHub's standard `ubuntu-24.04` runners provide it.
- "Latest stable" is whatever series `KERNEL_BRANCH` (top of the workflow) names, for
  example `linux-7.1.y`. Bump it by hand when a new series is released, or point it at an
  LTS series such as `linux-6.12.y`.
