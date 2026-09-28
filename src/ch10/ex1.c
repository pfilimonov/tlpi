/*
 * Assume a system where the value returned by the call sysconf(_SC_CLK_TCK) is
 * 100. Assuming that the clock_t value returned by times() is a signed 32-bit
 * integer, how long will it take before this value cycles so that it restarts
 * at 0? Perform the same calculation for the CLOCKS_PER_SEC value returned by
 * clock().
 */

/*
 * a) for times(): 2^32/100 = 21474836.4 seconds = 497 days
 * b) for clock(): 2^32/1000000 = 2147.483647 seconds = 71.58 minutes
 */
