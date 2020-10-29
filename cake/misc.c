#include "std.h"

// max() returns the larger int.
int max( int x, int y )
{
  if (x>y)
    return(x);
  else
    return(y);
}

// spaces() returns a pointer to a string of i spaces.
char *spaces(int i)
{
  static char spc[max_string];
  spc[i] = '\0';
  for ( ; i > 0 ; i-- )
    spc[i-1] = ' ';
  return (spc);
}

// remove_zeroes() removes all '0' characters from a string.
void remove_zeros( char *x )
{
  char *y = x;
  while ( *x )
    if ( *x == '0' )
      ++x;
    else
      *y++ = *x++;
  *y = '\0';
}

// strmatch() is true if all of little is at the start of big.
//  strmatch( "abcd", "abc" ) ==> 1
//  strmatch( "abcd", "abd" ) ==> 0;
int strmatch( char *big, char *little )
{
  while ( *big && *little )
    {
      if ( *big != *little )
	return (0);
      ++big;
      ++little;
    }
  if ( *little == '\0' )
    return (1);
  else
    return (0);
}

// exchange() replaces the first o characters of s with n.
void exchange( char *s, char *n, int o )
{
  int i = strlen( n );
  int d = i - o;
  if (d < 0)
    {
      // move stuff left
      char *l = s+i;
      char *r = s+o;
      while ( *r )
	*l++ = *r++;
      *l = '\0';
    }
  else if (d > 0)
    // move stuff right
    for ( int z = strlen(s);  z >= o ; --z )
	s[z+d] = s[z];
  while (*n)
    *s++ = *n++;
}
