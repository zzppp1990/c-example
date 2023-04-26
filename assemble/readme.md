
1. 安装nasm  
apt install nasm  

2. 编译运行asm程序  
nasm -f elf64 -o main.o main.asm  
ld -o main main.o  

3. 指令含义  
3.1 x64 架构的汇编  
寄存器种类：  
x64 有 16 个通用寄存器, 分别是 rax, rbx, rcx, rdx, rbp, rsp, rsi, rdi, r8, r9, r10, r11, r12, r13, r14 和 r15. 此外, 其中一些低字节寄存器可以作为 32 位, 16 位或 8 位寄存器独立访问  
8-byte register	Bytes 0-3	Bytes 0-1	Byte 0
rax	            eax	        ax	        al
rcx	            ecx	        cx	        cl
rdx	            edx	        dx	        dl
rbx	            ebx	        bx	        bl
rsi	            esi	        si	        sil
rdi	            edi	        di	        dil
rsp	            esp	        sp	        spl
rbp	            ebp	        bp	        bpl
r8	            r8d	        r8w	        r8b
r9	            r9d	        r9w	        r9b
r10	            r10d	    r10w	    r10b
r11	            r11d	    r11w	    r11b
r12	            r12d	    r12w	    r12b
r13	            r13d	    r13w	    r13b
r14	            r14d	    r14w	    r14b
r15	            r15d	    r15w	    r15b

cmp 指令用于在两个值之间执行比较, 它可与条件跳转指令一起用于流程控制,如： cmp rax, 50
jae / jnb Label	Jump if above or equal (unsigned)
