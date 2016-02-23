FreeBSD notes for GT.M
======================
(some parts are similar or same as on OpenIndiana)


NOTE: 22 FEB 2016
--------

As provided, this implementation will complain if you have `ncurses 6.0` on your system instead of 5.9


Prereqs:
--------
- FreeBSD 9.0/amd64
- `cmake 2.8.5` or newer, ncurses, libexecinfo, libicu (optional?),
  libelf (be careful about which libelf you're going to use!!)
- gcc 4.x, gmake


Known potentially hazardous hacks:
----------------------------------
Patch notes 8, 11, 9, 17, where the first is most critical and the last is least important.


Patch notes:
------------
1. I don't know how it happened that the same `SET(CMAKE_*` blocks appears two times in `CMakeLists.txt`.
For sure, this is needed for both OpenIndiana and FreeBSD, because we need:
  - enforce x86_64/amd64 defs
  - have the right `CMAKE*SIZEOF*` values

  1. EX: On OpenIndiana, cmake doesn't set these `CMAKE_*` variables with respect on hardware configuration automatically :-(

2. FreeBSD and OpenIndiana has nonstandard include/library paths, so we need to get some `-I/-L` args passed to a compiler. Since my goal was to get it working, I was not so hesitant about puting all these extra arguments into `add_definitions()` function.

3. Since we started out with Linux version, we expected that FreeBSD build should behave as Linux with only few exceptions. 
This is the reason why we use `-D__linux__` in conjunction with `-D__freebsd__` even if we don't build for Linux.  
To avoid some indications that we may be on ix86 box (for example, on OpenIndiana, where part of the userspace is 32bit and we have to force even the compiler to explicitly run in 64b mode), we used `-U__i386`, just to be sure.  
For similar reasons, we added `-D__amd64__` and `-D__x86_64__`.

4. On FreeBSD, we need `libexecinfo` to cover internal backtracing facilities.

5. Linked complained that we didn't supplied `environ` and `__progname` symbols within version script of `gtmexe.export` and probably others.  
So we've added it and it works :)

6. `^GDEINIT` routine was missing FreeBSD definitions, so it's also added, with similar params like Linux and others.

7. Although we have (in the time I was not sure if we'll be able to get a complete build) configured face `M_MMAP_MAX` macro, it should not be merged anywhere because in the final effect, it is not used anywhere (because the function that used it
will not be called in the case of FreeBSD).  
This affected `sr_port/gtm_malloc.h` and `sr_port/gtm_malloc_src.h` files.

8. In the time when we were not sure if we will be able to reach 100% of build procedure, we did a big hack in `sr_unix/extract_signal_info.c` by adding fake `#define EIP` which is not used in final effect too because the line of code that uses it is commented.  
This is a big `XXX`, because I was not able to find how to do the same task in FreeBSD (there is `uc_mcontext`, but looks differently).

9. `sr_unix/gtmio.h` has added `OPENFILE_SYNC` support for `__freebsd__`,
but we should make a revision it can be used like this or changed.
I am not aware of O_DASYNC (and friends) equivalent on FBSD.

10. FreeBSD has it's own "union semun" structure in `/usr/include` headers, and it is defined exactly in the same way, so we wrapped it by `#ifndef __freebsd__`.

11. `sr_unix/gt_timers.c`: I have hacked their `'#define HZ 100'` but I have absolutely no idea where to get this value on FreeBSD and if `HZ=100` is the right value! :-(

12. FreeBSD does not know SIGCLD signal, so we defined it as SIGCHLD.

13. FreeBSD does not know `setpgrp` of zero parity, so we changed all the `setpgrp()` calls to `setpgrp(0,0)`.

14. FreeBSD has `sigsetjmp` instead of `__sigsetjmp`. But I have read somewhere that they think about renaming it to `__sigsetjmp` in future.

15. `sr_x86_64/obj_filesp.c` for FreeBSD, we want ELF OS ABI to be
FreeBSD instead of Linux.

16. FreeBSD should have `KEY_T_LONG` according to `<sys/_types.h>`.
That fact was reflected to `sr_unix/mdefsp.h`.

17. FreeBSD's IPC implementation has permission flags incompatible with most of the rest of UNIX systems and that's why we need to change `sr_unix/iosp.h` file to get semaphores working well without `Permission denied (errno=13)` errors for concurrent DB access.

**WARNING**: we changed `RALL`/`RWALL`/`RWDALL` definitions which may be (correctly and without penalties) used for another syscalls like
`open()` on many UNIX systems.
These arguments are currently used only for `semget()` calls, but it would be better to prefix them so they will start with `SEM_` what will prevent people to use them outside the world of semaphores.
