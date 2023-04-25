
1. 安装nasm  
apt install nasm  

2. 编译运行asm程序  
nasm -f elf64 -o main.o main.asm  
ld -o main main.o  

