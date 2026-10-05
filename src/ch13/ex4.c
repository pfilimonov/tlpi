/*
* Explain why the output of the following code differs depending on whether
standard output is redirected to a terminal or to a disk file.

printf("If I had more time, \n");
write(STDOUT_FILENO, "I would have written you a shorter letter.\n", 43);
*/

/* ANSWER
 * When using terminal the first line goes straight to the kernel buffer (since
 * it has \n on the end) and next the second line also goes to the kernel
 * buffer. Then the kernel writes this data to the file (terminal) in order.
 *
 * When using disk file, first line will be stored in user-space buffer whereas
 * the second line will go to the kernel buffer first. So the kernel buffer has
 * the second line and the first line and will write them to the disk in that
 * exact order.
 */
