/*
After each of the calls to write() in the following code, explain what the
content of the output file would be, and why:

fd1 = open(file, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
fd2 = dup(fd1);
fd3 = open(file, O_RDWR);
write(fd1, "Hello,", 6);

> "Hello," (because we start writing to the file from the beginning)

write(fd2, " world", 6);

> "Hello, world" (fd2 has the same open file description, same offset, so this
write continues the previous one)

lseek(fd2, 0, SEEK_SET); // moves fd1, fd2 offset to the beginning
write(fd1, "HELLO,", 6);

> "HELLO, world" (overwrite first hello with the uppercase since we now have
offset at the beginning)

write(fd3, "Gidday", 6);

> "Gidday world" (fd3 has nothing to do with offsets of fd1 and fd2 so it is
zero by default and the write occurs from the beginning of the same file)
*/
