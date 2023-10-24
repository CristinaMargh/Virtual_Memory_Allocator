# Virtual_Memory_Allocator
First project (SDA)
#MARGHEANU CRISTINA-ANDREEA 313CA 2022-2023
In the main program, I read the command that will tell us what operation we have to do
we perform. For each entered command, I read the necessary parameters
built subprograms (starting addresses, sizes).

Solving the problem uses the implementation of doubly linked lists and
their specific operations. To simplify the process I have
introduced in the list structure and a *tail node, which will retain the characteristics
the node in the tail of the list.

The functions used that directly concern the list notion are:
- dll_create, which allocates memory for the list and initializes its components
-dll_get_nth_node goes through each node individually as long as it exists,
until it reaches the required index (at the searched position in the list), returning the node
in that position. If there is no next node, the list is empty.
- function for deleting a certain node, which returns a pointer to it.
I checked the error cases, I positioned myself in the list at the given position
parameter n, and depending on the given index we move the elements from the list for
removing the desired one. Similarly, we proceed to add one
node. We modify the next field both for the node after which we will add but
and that of the added node. If we want to add to the first position, this one
becomes the head of the list, and if we add it to the end, it becomes the tail of the list.
The complexity in these special cases is O(1) as opposed to O(n)
when iterating through the list.
-dll_get_size which returns the number of elements in the list in a complexity
by O(1)

The subprograms created that directly concern the commands in the problem are:
-alloc_arena in which we initialize the size of the arena and the list of
blocks to be positioned in the square. Their structure will be of
chained list, so initialization is done using a create subroutine
of the lists.

- for the reallocation of resources within an arena, we position ourselves at
its beginning (at the top of the list) and we go through the entire list of blocks.
When we position ourselves at the beginning of a block, we check if it contains more
many mini blocks. If so, we delete them one by one, being careful
we also remove the memory allocated to the read-write buffer. After removing all
miniblocks within the current block, we release the allocated memory
the list of mini-blocks, but also the block and we go to the next block until
we go through them all. Thus the memory allocated to the elements in the arena was
deallocated and we can end the subprogram.

- for the function of allocating a block, we first check the cases of
error, after which we perform the operations according to the position in the list
in which we want to add a new entity.
Analogous for freeing the memory in the free_block function, we check if the node
is at the beginning or end of the block list. To find out the size
of a block we use the count subprogram that starts from a received node
as a parameter and returns the sum of the sizes for the miniblocks in that one
portion.

- for the reading function, I store the size that I need to read.
In a char we place the information from miniblock->rw_buffer and display it as such
until I reached the end of the text, constantly iterating through
list of miniblocks. Analogous for the write function, we replace the operation by
display with that of memory allocation and copying of the given information.

- the pmap function constitutes a continuous iteration through lists, displaying information
about blocks and miniblocks. At the end of it, I made the necessary comparisons
to display permissions. To find out the number of miniblocks
I used the subroutine mini, which adds the size of each list of
miniblocks, corresponding to each block in the arena.

-in the mprotect function, we iterate through the lists to get to each one
miniblock, whose permissions we change by adding specific values
for each separate permission. Initial permission for miniblocks
is initialized with 0 in order not to exceed the maximum value of 7(RWX).
