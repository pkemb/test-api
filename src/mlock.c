/*
 * https://gist.github.com/pkemb/a2667efdbc2056129b594da555126c8d
 */

#define _GNU_SOURCE
#include <sys/mman.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <link.h>

#define CMD_LEN  128

static int callback(struct dl_phdr_info *info, size_t size, void *data)
{
    int p_type;
    void *p_vaddr;
    int p_memsz;
    int ret;
    int pagesize = getpagesize();
    int pagemask = pagesize - 1;
    char *basename = strrchr(info->dlpi_name, '/');
    if (!basename)
        basename = "";
    else
        basename += 1;

    // printf("name = %s, basename = %s\n", info->dlpi_name, basename);
    // 只mlock libc.so.6
    if (strcmp(basename, "libc.so.6") != 0) {
        return 0;
    }
    for (int j = 0; j < info->dlpi_phnum; j++) {
        p_type = info->dlpi_phdr[j].p_type;
        if (p_type != PT_LOAD) {
            continue;
        }

        p_vaddr = (void *) (info->dlpi_addr + info->dlpi_phdr[j].p_vaddr);
        p_memsz = info->dlpi_phdr[j].p_memsz;

        p_memsz = (p_memsz + pagemask) & (~pagemask);
        p_vaddr = (void *)((unsigned long)p_vaddr & (~pagemask));

        ret = mlock((const void *)p_vaddr, p_memsz);
        if (ret == 0) {
            printf("mlock %p (size: 0x%x) success\n", p_vaddr, p_memsz);
        } else {
            printf("mlock %p (size: 0x%x) fail: %s\n", p_vaddr, p_memsz, strerror(errno));
        }
    }

    return 0;
}

int main(int argc, char *argv[])
{
    char *mlock_cmd;
    int ret = 0;
    char cmd[CMD_LEN];

    if (argc >= 2) {
        mlock_cmd = argv[1];
    } else {
        mlock_cmd = NULL;
    }

    // 强制打开 libpthread，用于参考对比
    dlopen("libpthread.so.0", RTLD_LAZY);

    if (mlock_cmd) {
        if (strcmp(mlock_cmd, "mlockall") == 0) {
            printf("mlockall test\n");
            ret = mlockall(MCL_CURRENT | MCL_FUTURE);
            if (ret != 0) {
                printf("mlockall fail: %s\n", strerror(errno));
            } else {
                printf("mlockall success\n");
            }
        } else if (strcmp(mlock_cmd, "mlock") == 0) {
            printf("mlock test\n");
            dl_iterate_phdr(callback, NULL);
        }
    }

    // 通过 procmem 观察内存占用情况
    snprintf(cmd, CMD_LEN, "procmem %d", getpid());
    system(cmd);
    return 0;
}
