CROSS    :=
CC       := $(CROSS)gcc
CXX      := $(CROSS)g++
LD       := $(CROSS)ld
CFLAGS   += -g -O0 -Wall
CXXFLAGS += -g -O0 -Wall
CPPFLAGS +=
LDFLAGS  +=

srcdir   := src
submkdir := submk

# 根据 .mk 文件自动生成变量 progs_re_define

progs_re_define := $(wildcard $(submkdir)/*.mk)
progs_re_define := $(progs_re_define:$(submkdir)/%=%) # 删除前缀
progs_re_define := $(basename $(progs_re_define))     # 删除后缀

progs := $(basename $(wildcard $(srcdir)/*.c $(srcdir)/*.cpp))
progs := $(progs:$(srcdir)/%=%)
progs := $(filter-out $(progs_re_define),$(progs))

.PHONY: all clean c help h

all: $(progs) $(progs_re_define)

# 定义模式规则
%: $(srcdir)/%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(srcdir)/$@.c -o $@ $(LDFLAGS)

%: $(srcdir)/%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(srcdir)/$@.cpp -o $@ $(LDFLAGS)

# 加载重定义mk
include $(addprefix submk/,$(addsuffix .mk,$(progs_re_define)))

clean c:
	rm -rf $(progs) $(progs_re_define) $(clean_files)

help h:
	@echo "编译测试代码为可执行文件"
	@echo ""
	@echo "使用方法："
	@echo ""
	@echo "    make           编译所有文件"
	@echo "    make <target>  编译指定文件"
	@echo "    make clean     清理编译产生的临时文件"
	@echo ""
