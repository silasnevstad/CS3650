
/* Compare two (signed) long integers a and b and return:
 * * -1 if a < b
 * * 0 if a == b
 * * 1 if a > b
 */
long compare(long a, long b) {
  if (a < b)
    return -1;
  if (a == b)
    return 0;
  
  return 1;
}
