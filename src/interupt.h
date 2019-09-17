void DISABLE_INT_FIRST(void)  
{
	asm volatile( "pushf");

/*
	asm volatile("push %rax");
	asm volatile("push %rbx");
	asm volatile("push %rcx");
	asm volatile("push %rdx");
	asm volatile("push %rsi");
	asm volatile("push %rdi");
	asm volatile("push %rbp");
	asm volatile("push %rsp");

	asm volatile("push %r8");
	asm volatile("push %r9");
	asm volatile("push %r10");
	asm volatile("push %r11");
	asm volatile("push %r12");
	asm volatile("push %r13");
	asm volatile("push %r14");
	asm volatile("push %r15");
*/
	asm volatile( "cli" );
}
void ENABLE_INT(void)
{
	asm volatile("sti");
/*
	asm volatile("pop %r15");
	asm volatile("pop %r14");
	asm volatile("pop %r13");
	asm volatile("pop %r12");
	asm volatile("pop %r11");
	asm volatile("pop %r10");
	asm volatile("pop %r9");
	asm volatile("pop %r8");

	asm volatile("pop %rsp");
	asm volatile("pop %rbp");
	asm volatile("pop %rdi");
	asm volatile("pop %rsi");
	asm volatile("pop %rdx");
	asm volatile("pop %rcx");
	asm volatile("pop %rbx");
	asm volatile("pop %rax");
*/
	asm volatile("popf");
	
}
void DISABLE_INT_SECOND(void) 
{
	asm  volatile("cli");
}
static inline void writecr3(unsigned long val)
{
	asm volatile("mov %0,%%cr3": : "r" (val), "m" (__force_order));
}
