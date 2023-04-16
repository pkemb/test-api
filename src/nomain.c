
int write(int fd, const char *str, int len);
void _Exit(int code);

int nomain()
{
    write(0, __func__, sizeof(__func__));
    write(0, "\n", 1);
    _Exit(47);
}
