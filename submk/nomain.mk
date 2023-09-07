# # 针对指定目标修改变量
# nomain: LDFLAGS := -static -e nomain -lc

# # 增加文件到清理列表
# clean_files += nomain.o

# # 重新定义规则
# nomain.o: $(srcdir)/nomain.c
# 	$(CC) $(CFLAGS) -c $< -o $@

# nomain: nomain.o
# 	$(LD) $< -o $@ $(LDFLAGS)

# 或者使用 -nostartfiles 选项，不链接 ctr1.o。这样可以一步完成编译链接，不必拆开编译和链接。
nomain: CFLAGS += -nostartfiles -static -e nomain -lc
