# wikitext-fix

用于中文维基百科的简易维基文本修复工具。

## 警告

wikifix 还在开发中，请不要用于条目名字空间。

## 编译

```sh
git clone https://github.com/cloudwindy/wikitext-fix
cd wikitext-fix
mkdir build && cd build
cmake ..
make
cd bin && ./wikifix --help
```

## 例子

```sh
./wikifix --fix-note --fix-punc --fix-space 上海市
```

wikifix 不是一个自动化的机器人，而是会输出修复后的维基文本。请手动粘贴到对应的维基页面。

## 可用的修复

以下修复不会作用于模板、表格、链接、参考文献等维基标记中。

### --fix-note

如果脚注不是这一行的末尾，把脚注移动到句号之前。如果脚注前后都有句号，移除脚注前重复的句号。

### --fix-punc

移除重复的标点符号。

### --fix-space（开发中）

移除中英文之间的空格。
