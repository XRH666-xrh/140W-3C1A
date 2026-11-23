@echo off
REM 启用延迟变量扩展
setlocal enabledelayedexpansion

REM 遍历当前目录下的所有 PNG 文件
for %%F in (*.jpg) do (

        REM 使用magick命令进行填充：
        REM -background black 设置填充颜色为黑色
        REM -gravity center 将原图置于画布中心，即上下居中填充
        REM -extent !width!x12 设置新的画布大小，宽度保持不变，高度设为12像素
        magick "%%F" -background black -gravity center -extent 18x26 "%%~nF.png"
)

echo 完成所有 PNG 图片的处理。
pause
