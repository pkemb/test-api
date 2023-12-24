#include <stdio.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>
#include <unistd.h>
#include <libgen.h> // for basename()
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#define PROC_MQUEUE_DIR         "/proc/sys/fs/mqueue"
#define PROC_MSG_DEFAULT        PROC_MQUEUE_DIR"/msg_default"
#define PROC_MSG_MAX            PROC_MQUEUE_DIR"/msg_max"
#define PROC_MSGSIZE_DEFAULT    PROC_MQUEUE_DIR"/msgsize_default"
#define PROC_MSGSIZE_MAX        PROC_MQUEUE_DIR"/msgsize_max"
#define PROC_QUEUES_MAX         PROC_MQUEUE_DIR"/queues_max"

#define MQUEUE_NAME             "/test_posix_message_queue"

#define __log(tag, fmt, args...)    printf("[%s][%s:%04d] "fmt, tag, __func__, __LINE__, ##args)
#define info(fmt, args...)  __log("INFO", fmt, ##args)
#define err(fmt, args...)   __log("ERR", fmt, ##args)

int server(int argc, char *argv[]);
int client(int argc, char *argv[]);
void show_mqueue_info();

struct cmd_map {
    char *cmd;
    int (*func)(int argc, char *argv[]);
};


int main(int argc, char *argv[])
{
    if (argc < 2) {
        err("usage: %s [server|client] ...\n", argv[0]);
        return -1;
    }

    struct cmd_map map[] = {
        {.cmd = "server", .func = server},
        {.cmd = "client", .func = client},
    };
    int map_len = sizeof(map)/sizeof(map[0]);
    int i;

    for (i = 0; i < map_len; i++) {
        if (strcmp(map[i].cmd, argv[1]) == 0)
            return map[i].func(argc-1, argv+1);
    }

    err("未知指令：%s\n", argv[1]);
    return -1;
}

int server(int argc, char *argv[])
{
    struct mq_attr attr;
    char *buffer;
    int buffer_len;
    int ret;
    unsigned int prio;
    int msg_len;
    int no;

    show_mqueue_info();

    mqd_t mqd = mq_open(MQUEUE_NAME, O_RDONLY | O_CREAT, 0666, NULL);
    if (mqd < 0) {
        err("消息队列打开失败：%s\n", strerror(errno));
        return -1;
    }

    if (mq_getattr(mqd, &attr) != 0) {
        err("获取消息队列属性失败：%s\n", strerror(errno));
        ret = -1;
        goto exit;
    }

    buffer_len = attr.mq_msgsize;
    buffer = (char *)malloc(sizeof(char)*buffer_len);
    if (buffer == NULL) {
        err("malloc失败：%s\n", strerror(errno));
        goto exit;
    }

    no = 0;
    ret = 0;
    while(1) {
        memset(buffer, 0, buffer_len);
        msg_len = mq_receive(mqd, buffer, buffer_len, &prio);
        if (msg_len < 0) {
            err("接收消息失败：%s\n", strerror(errno));
            continue;
        }
        no++;

        info("第 %d 个消息\n", no);
        info("收到的消息：%s\n", buffer);
        info("消息长度：%d\n", msg_len);
        info("优先级：%d\n\n", prio);

        if (strcmp("quit", buffer) == 0) {
            info("server退出\n");
            break;
        }
    }

exit:
    if (mqd >= 0) {
        mq_close(mqd);
    }
    mq_unlink(MQUEUE_NAME);

    return ret;
}

int client(int argc, char *argv[])
{
    if (argc < 2) {
        err("Usage: client msg1 msg2 msg3...\n");
        return -1;
    }

    int i;
    int ret;
    mqd_t mqd = mq_open(MQUEUE_NAME, O_WRONLY | O_CREAT, 0666, NULL);
    info("mqd = %d\n", mqd);
    if (mqd < 0) {
        err("消息队列打开失败：%s\n", strerror(errno));
        return -1;
    }

    for (i=1; i<argc; i++) {
        ret = mq_send(mqd, argv[i], strlen(argv[i]), (unsigned int)i);
        if (ret != 0) {
            err("消息发送失败：%s\n", strerror(errno));
        } else {
            info("发送成功：%s\n", argv[i]);
        }
    }

    mq_close(mqd);
    return 0;
}

char *read_one_line_file(char *f)
{
    char buffer[128] = {0};
    char *ret;
    FILE *fp = NULL;

    fp = fopen(f, "r");
    if (fp == NULL) {
        err("打开文件\"%s\"失败：%s\n", f, strerror(errno));
        ret = strdup("");
        goto exit;
    }

    if (buffer != fgets(buffer, sizeof(buffer)-1, fp)) {
        err("读取失败：%s\n", strerror(errno));
        ret = strdup("");
        goto exit;
    }

    buffer[strlen(buffer)-1] = 0; // remove \n
    ret = strdup(buffer);

exit:
    if (fp) {
        fclose(fp);
    }
    return ret;

}

void show_mqueue_info()
{
    char *files[] = {
        PROC_MSG_DEFAULT,
        PROC_MSG_MAX,
        PROC_MSGSIZE_DEFAULT,
        PROC_MSGSIZE_MAX,
        PROC_QUEUES_MAX,
    };
    char *f;
    int number = sizeof(files)/sizeof(files[0]);
    int i = 0;
    char *content;

    for (i = 0; i < number; i++) {
        f = files[i];
        if (access(f, F_OK) != 0) {
            err("文件\"%s\"不存在，忽略！！！\n", f);
            continue;
        }

        content = read_one_line_file(f);
        info("%s: %s\n", f, content);
        if (content) {
            free(content);
        }
    }

    info("SC_MQ_PRIO_MAX: %ld\n", sysconf(_SC_MQ_PRIO_MAX));
}
