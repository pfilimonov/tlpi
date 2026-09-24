/*
 * Compile the program in Listing 6-1 (mem_segments.c), and list its size using
 * ls –l. Although the program contains an array (mbuf) that is around 10 MB in
 * size, the executable file is much smaller than this. Why is this?
 *
 * Answer: because uninitialized data doesn't take any space, only location and
 * size to allocate at run-time.
 */
