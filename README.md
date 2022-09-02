# Process_memory_sharing_in_xv6OS
Implementation of physical memory sharing between processes.
Original project text is in the "OS Domaci 3" file.

This was a university project we had to do.
The point of this project is that two processes can share memory.
In the xv6OS we have virtual addresses that point to physical addresses.
This is made by the whole operation of "pageing" which was the whole
point of our project.
We made changes to the following functions fork(),exec().
We also added system calls to share memory and to get that same
shared memory (share_mem(), get_shared()). The struct for holding these
shared memory structures is in the proc.h file. It holds a name, a size
and the virtual address for the shared variable.
There are simple user programs that demonstrate the work of sharring a
array of 100 000 ints, an two more integers, one as a counter for the
array, and the other as a command indicator.

To see this program work you can start the xv6OS on a Linux machine by
going to the folder where you clone these project folders through
the terminal and by typing "make qemu". The xv6OS will bootup and you can
start the program primestart which will automatically start the other two
primecom and primecalc. The idea is that primestart is used to start
the other two programs, primecom for getting user input and primecalc
calculates first 100 000 prime numbers. You can type "prime <n>" to 
find the nth prime number if it is calculated, "latest" to get the
latest prime number calculated, "pause" to pause the primecalc calculation,
"resume" to continue primecalc calculation and end which is supposed to end
the program but actually crashes for unknown resons to me, that's the only
flaw in my implementation. 
  
So you can see the work of process memory sharing where primestart, primecom,
primecalc share memory between themselves.
