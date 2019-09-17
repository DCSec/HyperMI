obj-m :=1.o				#产生module_hello模块的目标文件
1-objs := hookcr32.o #gate_function.o
#copy_pg.o gate_function.o sec.monitor.o sethook.o set_pgprot.o native_gate_function.o gate_function.o struct_pt.o find.o
#/home/juku/桌面/d/1/hookcr32.o /usr/src/linux-3.18.1/arch/x86/include/3/copy_pg.o /usr/src/linux-3.18.1/arch/x86/include/3/gate_function.o /usr/src/linux-3.18.1/arch/x86/include/3/sec.monitor.o /usr/src/linux-3.18.1/arch/x86/include/3/sethook.o /usr/src/linux-3.18.1/arch/x86/include/3/set_pgprot.o
	CURRENT_PATH := $(shell pwd) 		#模块所在的当前路径
	LINUX_KERNEL := $(shell uname -r) 	#Linux内核源代码的当前版本
	LINUX_KERNEL_PATH := /usr/src/linux-$(LINUX_KERNEL) 	#Linux内核源代码的绝对路径
all:
	make -C $(LINUX_KERNEL_PATH) M=$(CURRENT_PATH) modules 	#编译模块
clean:
	make -C $(LINUX_KERNEL_PATH) M=$(CURRENT_PATH) clean 	#清理
