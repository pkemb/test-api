# 针对指定目标修改变量
nomain: LDFLAGS := -static -e nomain -lc

# 增加文件到清理列表
clean_files += nomain.o

# 始终编译
.PHONY: nomain.o

# 重新定义规则
nomain.o: nomain.c
	$(CC) $(CFLAGS) -c $< -o $@

nomain: nomain.o
	$(LD) $< -o $@ $(LDFLAGS)
