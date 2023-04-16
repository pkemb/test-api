API测试小程序。

## 默认编译选项

```
CFLAGS   += -g -O0 -Wall
CXXFLAGS += -g -O0 -Wall
CPPFLAGS +=
LDFLAGS  += -pthread
```

## 默认编译规则

```
%: $(srcdir)/%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(srcdir)/$@.c -o $@ $(LDFLAGS)

%: $(srcdir)/%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(srcdir)/$@.cpp -o $@ $(LDFLAGS)
```

## 使用方法

在`src/`目录创建一个`.c`或`.cpp`文件，例如[getpid.c](src/getpid.c)，执行`make getpid`编译。不需要修改`Makefile`文件。

```shell
pk@pkdev:test-api (master)$ make getpid
gcc  -g -O0 -Wall src/getpid.c -o getpid -pthread
pk@pkdev:test-api (master)$ ./getpid
pid = 1532
```

**注意：**
* 一个源代码文件编译成一个可执行文件，不支持多文件编译链接。
* 不要创建多个基本名相同的文件，例如`getpid.c`和`getpid.cpp`，这会导致有一个会被覆盖。

### 修改编译选项

在`submk/`目录创建`.mk`文件，可以给同名的测试程序修改编译选项。例如[src/hello.cpp](src/hello.cpp)和[submk/hello.mk](submk/hello.mk)。`hello.cpp`需要宏`HELLO`，但是默认的编译选项没有定义，所以需要额外添加。借助Makefile目标变量的特性，只为`hello`目标定义宏`HELLO`。

```makefile
hello: CXXFLAGS += -DHELLO=\"hello\ cpp,\ this\ string\ define\ in\ hello.mk\"
```

> 关于目标变量的更多信息，请阅读[目标变量](https://pkemb.com/2021/07/write-makefile-with-me/#%E7%9B%AE%E6%A0%87%E5%8F%98%E9%87%8F)。

### 修改编译规则

默认的编译规则适用于绝大多数的程序，但极少数特殊程序无法编译通过，需要重写编译规则。例如[src/nomain.c](src/nomain.c)没有`main()`函数，如果使用`gcc`一步完成编译链接，会链接失败。所以为了编译`nomain.c`，需要在[submk/nomain.mk](submk/nomain.mk)重写编译规则。以下是`nomain.mk`的内容。

```makefile
# 针对指定目标修改变量
nomain: LDFLAGS := -static -e nomain -lc

# 增加文件到清理列表
clean_files += nomain.o

# 重新定义规则
nomain.o: $(srcdir)/nomain.c
	$(CC) $(CFLAGS) -c $< -o $@

nomain: nomain.o
	$(LD) $< -o $@ $(LDFLAGS)
```

## submk常用变量

| 变量名 | 说明 |
| - | - |
| clean_files | 额外需要清理的文件。`make clean`只会删除编译产生的可执行文件。如果额外的文件需要清理，例如`.o`文件，追加到这个变量。 |
