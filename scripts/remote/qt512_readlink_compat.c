/*
 * Compatibility shim for the Qt 5.12 build bundled with AFSIM 2.9.
 *
 * On current glibc, Qt's QLockFile process-name lookup can call the fortified
 * readlink helper with PATH_MAX even when its actual stack buffer is smaller.
 * Clamp the requested length to the compiler-reported object size. The shim is
 * loaded only by the remote Warlock launcher.
 */

#define _GNU_SOURCE

#include <stddef.h>
#include <sys/syscall.h>
#include <unistd.h>

ssize_t __readlink_chk(const char* path, char* buffer, size_t length, size_t buffer_length)
{
   const size_t safe_length = length < buffer_length ? length : buffer_length;
   return syscall(SYS_readlink, path, buffer, safe_length);
}
