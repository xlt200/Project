#include "header/uart.h"
#include "header/shell.h"
#include "header/dtb.h"
#include "header/utils.h"
#include "header/cpio.h"
#include "header/timer.h"
#include "header/allocator.h"

void test_read_command(char* buffer) {
	int index = 0;
	while(1) {
		buffer[index] = uart_get_char();
		if(buffer[index] == '\n') {
			buffer[index] = '\0';
			buffer[index+1] = '\n';
			uart_send_string("\r\n");
			break;
		}
		else 
		{
			uart_send_char(buffer[index]);
		}
		index++;
	}
}


extern void *_dtb_ptr;
void main()
{

    // set up serial console
    uart_init();

	// pre test ///////////////////////////////
	char buffer[256];
	while(1)    
    {
        uart_send_string("# ");
	    test_read_command(buffer);
        char * input_string = buffer;
        if(utils_string_compare(input_string,"in"))
        {
            break;
        }
        else 
        {
            // uart_send_hex((uintptr_t) _dtb_addr);
            uart_send_string("The instruct is not exist.\n");
        }
    }
	//////////////////////////////////////////////
	
	uart_init();
	// device tree traverse
	fdt_traverse(get_cpio_addr,_dtb_ptr);
    traverse_file();
	
	// buddy system init
	buddy_system_init(_dtb_ptr);

	unsigned long el = 0;
	asm volatile ("mrs %0, CurrentEL":"=r"(el));
	uart_send_string("Current exception level: ");
	uart_hex(el>>2); // CurrentEL store el level at [3:2]
	uart_send_string("\n");

	asm volatile("mov %0, sp"::"r"(el));
	uart_send_string("Current stack pointer address: ");
	uart_hex(el);
	uart_send_string("\n");
	
	// say hello
	uart_send_string("Type in `help` to get instruction menu!\n");
	
	uart_enable_interrupt();
	

	while(1){
		asm volatile("nop");
	}
	//echo everything back
	//shell();
}


/*
void except_handle() {
	uart_send_string("In exception");
	
	unsigned long long esr_el1 = 0;
    asm volatile ("mrs %0, esr_el1" :: "r" (esr_el1));
    unsigned ec = (esr_el1 >> 26) & 0x3F; //0x3F = 0b111111(6)
    unsigned imm16 = esr_el1 & 65535; //65535 = 0x1111111111111111(16)

	uart_send_string("\nec: ");
    uart_hex(ec);
    uart_send_string("\n");
    uart_send_string("imm16: ");
    uart_hex(imm16);
    uart_send_string("\n");

    if (ec != 0x15){ // SVC from AArch64
		while(1) nop;
	}
    
    switch (imm16) {
		case 0x1337: { // exit from EL0
			// update spsr to make eret jumps back to loop
			// set EL1h and mask all interrupts
			uart_send_string("In the svc execution");	
			return;
		}
    }
}
*/
