# ZEST - the optimiZed filEsySTem

ZEST is a virtual compressed filesystem implementation aiming to be a drop in 
replacement for raw file access through std::filesystem.

ZEST stores multiple virtual files into a single one into your real filesystem,
allowing for fast access of small related files. Accessing a thousand files each
smaller than 4k in size, can be slow even on a moder machine equiped with SSD. 
ZEST optimize the multiple access and OS context switch packaging everything 
in a single file that than is processed as a filesystem in user space.

Using an interface compatible with std::filesystem, ZEST makes this not new 
idea, easy and cheap to adopt.