/*
 * What is the effect of the following statements?

fflush(fp);
fsync(fileno(fp));
*/

/*
 * fflush forces the data from user-space buffer to be written into the
 * kernel-space buffer fsync flushes the kernel-space buffer to the disk
 * (including all the metadata)
 *
 * So this to statements ensure that all the data written to fp will be written
 * to the disk
 */
