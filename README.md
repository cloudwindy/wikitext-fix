# wikitext-fix

用于中文维基百科的简易维基文本修复工具。

## 警告

wikifix 还在开发中，请不要批量用于条目名字空间。如果您要在条目中使用，请一定在提交编辑前检查。

不当使用此工具可能会招致封禁。

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
./wikifix --fix-notes --fix-punc --fix-punc-width --fix-space 上海市
```

wikifix 不是一个自动化的机器人，而是会输出修复后的维基文本。请手动粘贴到对应的维基页面。

## 可用的修复

### --fix-notes

如果脚注不是这一行的末尾，把脚注移动到标点之前。如果脚注前后都有句号，移除脚注前重复的标点。

### --fix-punc

移除重复的标点符号。

### --fix-punc-width

修复错误的标点符号宽度（例如：把“，”写成“,”）。

### --fix-space

移除中英文之间的空格。

## 其他参数

### --render

渲染一个HTML页面用于显示解析器结果。