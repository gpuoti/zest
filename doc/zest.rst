===============================
ZEST - the optimiZed filEsySTem
===============================

In few words
------------

ZEST is a virtual compressed filesystem implementation aiming to be a drop in 
replacement for raw file access through std::filesystem.

ZEST stores multiple virtual files into a single one into your real filesystem,
allowing for fast access of small related files. Accessing a thousand files each
smaller than 4k in size, can be slow even on a moder machine equiped with SSD. 
ZEST optimize the multiple access and OS context switch packaging everything 
in a single file that than is processed as a filesystem in user space.

Using an interface compatible with std::filesystem, ZEST makes this not new 
idea, easy and cheap to adopt.


Limitations
-----------

Being the very first implementation, this will leave many details commong FS
usually deal with. ZEST, by design, does not have support for:

  - file access restriction
  - creation date statistics
  - last update date statistics

while it have support for:

  - real time file compression
  - lean file operations: read/update/insertion/delete 

The latter deserve a little explanation. Many archive formats, remarkably 
compressed tarballs, require the whole archive to be read and write each time 
a file need to be accessed. This is visible when try to access a single file in 
an archive or when you try to update one file in an archive. Different formats 
have different trade-off between operation and compression ratio, ZEST is very
much focused on operation and threats compression as a tool to make it goal 
affordable.

ZEST is a filesystem of the FAT type. The file allocation table is located at
the very beginning of the storage file.


Allocation strategy
===================

File allocation strategy is absolutelly trivial. The file content is stored 
in contiguous memory whose size is a multiple of x_kB (4kB by default). In the
following these chuncks of contigous memory in the storage file will be 
referenced as *segments*. The segments base size parameter will trade internal 
fragmentation for better chance of chunks reuse.
Segments from deleted files will be reused for newly created file or to support 
the grow of other files but, since ZEST support multiple file compression 
strategy, it is possible also to trade compression/decompression speed to try 
to reuse the same segment when the file increase its size.


The ZEST FAT (aka the ZAT)
==========================

This section describes the ZEST file Allocation Table format (ZAT) as it is 
stored into the storage file. The ZAT as it is in memory and defined by 
zest::ZAT is slightly different because of the runtime data structures used.
The ZAT will contains minimal informations needed to retrieve the file structure
stored in the filesystem and the location of the file content.
The ZAT is stored as a forward linked list of ZAT fragments each 4090 byte long. 

+------------------------------------------------------------------------------+
| next fragment offset  |    4  |                                              |
+------------------------------------------------------------------------------+
| ZAT fragment          | 4092  |                                              |
+------------------------------------------------------------------------------+


Each fragment is composed of ZAT entry structures defined as follows:

+------------------------------------------------------------------------------+
| entry size (SZ) |    2  |                                                    |
+------------------------------------------------------------------------------+
| File name       |   SZ  |  a string SZ long (SZ artificially limited to 512  |
+------------------------------------------------------------------------------+
| Segment start   |    4  |  in segment base size unit. Multiply by x_kB to the|
|                 |       |  absolute position into the storage.               |
+------------------------------------------------------------------------------+

The fragment is eventually filled with empty ZAT entry so that any fragment is 
always full. To have this behaviour, the unused part of the fragment is zero 
filled.

The filename in the ZAT entry are absolute path, i.e. folders are implicitelly
defined by means of file name conventions. 

.. note:: 
	
	This choise makes the overhead for each entry potentially quite big. 

