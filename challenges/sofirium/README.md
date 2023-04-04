# Sofirium - idekctf 2022*

## Protections 
    SMEP, SMAP, KASLR, KPTI

## tl;dr

* UAF in kmalloc-512 slab
* Get heap leak after delete and read an NFT
* Spray msg_msg on kmalloc-512
* Edit next pointer and size of message in msg_msg header. \
    Note: data pointed by new nextpointer should start with a null qword
* Get kleak with msgrcv. Calculate kernel base address and modprobpath address
* Use setxattr the change the next address of an sofirium entry to modprobpath

\
Later I came to know that we could've only used setxattr to get both aaw & aar.\
Exploit: [hack.c](./hack.c)