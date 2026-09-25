/*
 * Is a process with the following user IDs privileged? Explain your answer.

      real=0 effective=1000 saved=1000 file-system=1000
*/

/* ANSWER
 * not privileged since effective user id is not 0, so it can't execute
 * privileged actions. Moreover file-system uid is not 0 so even on legacy
 * systems it will not be able to execute privileged actions on filesystem
 */
