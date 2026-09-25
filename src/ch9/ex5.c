/*
* Repeat the previous exercise for a process executing a set-user-ID-root
program, which has the following initial set of process credentials:

    real=X effective=0 saved=0
*/

/* ANSWER is the same though reasoning is a bit different
 * a)
 *
 * uid_t X = getuid();
 * uid_t Y = geteuid();
 *
 * 1. seteuid(X); seteuid(Y);
 * 2. setreuid(-1, X); setresuid(-1, Y, -1);
 *
 * b)
 * setuid(getuid());
 */
