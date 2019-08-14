A simple client/server shellcode execution harness. The C server will execute bytes streamed over the TCP socket, from the client, which in this case is a Python harness.

To use:

1. Read the `README`.
1. Build the server component with `make`.
1. Run the server `./bin/shellcode-server`
1. Write x86 assembly to meet the requirements of the server.
1. Assemble and link x86 into an ELF object file, using `build.sh`, which does a `nasm -f elf hello.asm` followed by a `ld -m elf_i386 -o pewpew hello.o`
1. Dump out the bytes as hex, of the ELF packed shellcode with `objdump -d pewpew`
1. Put these bytes into the Python test client, e.g. `shellcode = "\xDE\xAD\xBE\xEF"`
1. Run the Python client, and observe any output made by the server.
1. Hit head against wall.
1. Run the server with GDB, optionally with the [peda](https://github.com/longld/peda) extension installed to make life better. Set some breaks at the `CALL EAX` instruction in the `test()` function, responsible for loading in the bytes that poor in over the socket and executing them. Step each instruction through observing the state of each register.



# Resources

## x86 Assembly

Lots of resources out there, but these one pagers keep it simple:

* [NASM Intel x86 Assembly Language Cheat Sheet](https://www.bencode.net/blob/nasmcheatsheet.pdf)
* [Intel Code Table](https://www.bencode.net/blob/intel_code_table.pdf) is a consise one page on the transfer, arithmetic, logic, jumps and misc instructions provided by the 80186 instruction set and higher.
* [GCC x86 Assembly Quick Reference](https://www.bencode.net/blob/att_x86.pdf) is another, but shows off the AT&T style assembly sytnax, which `gdb` uses. 


## GDB PEDA

The GNU debugger, the gold standard when it comes to debugging. Make sure the [peda extension](https://github.com/longld/peda) is installed, which add some insanely useful feature to GDB, like better colorisation and visual (still CLI!) display of memory and instructions, and commands like `context`, `context_stack` and `jmpall` to name but a few. PEDA = Python Exploit Development Assistance for GDB

    git clone https://github.com/longld/peda.git ~/peda
    echo "source ~/peda/peda.py" >> ~/.gdbinit

Don't forget `help peda` has always got your back.


## GDB Cheatsheet

* `break foo` set breakpoint of function `foo`
* `break *0xf7cd6ef8` set breakpoint on specific instruction
* `break main:66`
* `x/i foo` to examine each instruction of `foo`
* `r` run program (once all breaks in place)
* `c` continue
* `stepi` step next instruction
* `nexti` step over any nested subs or instructions
* `info reg` dump out all registers
* `x/s $esp` print `esp` as string
* `print "%d\n", 0xf7cd6edc` printf style output of memory or registers

