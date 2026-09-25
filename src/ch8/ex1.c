/*
 * When we execute the following code, which attempts to display the usernames
for two different user IDs, we find that it displays the same username twice.
Why is this?

printf("%s %s\n", getpwuid(uid1)->pw_name, getpwuid(uid2)->pw_name);
*/

/* ANSWER
 * getpwuid returns a pointer to a statically allocated buffer. So, the second
 * call to getpwuid in the printf args overwrite the first one, so when printf
 * actually reads the pointer, it contains the same data
 */
