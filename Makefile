CROSS    :=
CC       := $(CROSS)gcc
CXX      := $(CROSS)g++
LD       := $(CROSS)ld
CFLAGS   += -g -O0 -Wall
CXXFLAGS += -g -O0 -Wall
CPPFLAGS +=
LDFLAGS  += -pthread

# 根据 .mk 文件自动生成变量 progs_special
submkdir := submk
progs_special := $(wildcard $(submkdir)/*.mk)
progs_special := $(progs_special:$(submkdir)/%=%) # 删除前缀
progs_special := $(basename $(progs_special))     # 删除后缀

progs_c   := $(basename $(wildcard *.c))
progs_cpp := $(basename $(wildcard *.cpp))
progs_c   := $(filter-out $(progs_special),$(progs_c))
progs_cpp := $(filter-out $(progs_special),$(progs_cpp))

.PHONY: all clean c help h

all: $(progs_c) $(progs_cpp) $(progs_special)

# 默认编译规则
$(progs_c): %: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $@.c -o $@ $(LDFLAGS)

$(progs_cpp): %: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $@.cpp -o $@ $(LDFLAGS)

# 加载特殊测试文件的编译规则
include $(addprefix submk/,$(addsuffix .mk,$(progs_special)))

clean c:
	rm -rf $(progs_c) $(progs_cpp) $(progs_special) $(clean_files)

help h:
	@echo "编译测试代码为可执行文件"
	@echo ""
	@echo "使用方法："
	@echo ""
	@echo "    make           编译所有文件"
	@echo "    make <target>  编译指定文件"
	@echo "    make clean     清理编译产生的临时文件"
	@echo ""
