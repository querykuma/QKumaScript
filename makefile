CC = gcc
CPP = g++

ifeq ($(OS),Windows_NT)
 WFLAG = -Wall -Wextra -Wl,--disable-dynamicbase
else
 WFLAG = -Wall -Wextra
endif

CFLAGS = -g $(WFLAG)
CPPFLAGS = -g $(WFLAG)
OBJS = lex.yy.o y.tab.o main.o str.o malloc.o map.o util.o node.o debug.o d_malloc.o
PROGRAM = qks.exe

all: $(PROGRAM)

dmain: d_malloc.o d_malloc_test.o
	$(CC) $(CFLAGS) $^ -o $@.exe
# $@ : ターゲットファイル名
# $^ : すべての依存するファイルの名前

$(PROGRAM): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(PROGRAM) -lstdc++

$(OBJS): y.tab.c lex.yy.c node.h mylib.h

d_malloc_test.o: y.tab.c node.h mylib.h

y.tab.c: bison.y node.h
	bison -d -y -t -l -r all --report-file=y.output.log $<
# $< : 最初の依存するファイルの名前
#  -l, --no-lines             `#line' ディレクティブを生成しない
#  -t, --debug                構文解析のデバッグ用の手段
#  -y, --yacc                 POSIX Yacc をエミュレートする
# Output Files:
#      --defines[=FILE]          also produce a header file
#  -d                            likewise but cannot specify FILE (for POSIX Yacc)
#  -r, --report=THINGS           also produce details on the automaton
# THINGS is a list of comma separated words that can include:
#  all               include all the above information
# 生成ファイル：　y.output.log y.tab.c y.tab.h

lex.yy.c: flex.l node.h
	flex -l $<
#  -l, --lex-compat        maximal compatibility with original lex
#  -L,  --noline            suppress #line directives in scanner
# 生成ファイル：　lex.yy.c

clean:
	rm -f $(PROGRAM) $(OBJS) lex.yy.c y.tab.c y.tab.h lex.yy.c y.output.log dmain.exe d_malloc_test.o
