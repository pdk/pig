#include "std.h"
#include "table.h"

// table::first() returns the first item in the table.
void *table::first()
{
  lastloc = 0;
  if (count==0)
    return (NULL);
  return (item[0]);
}

// table::next() returns the item after the last item returned.
void *table::next()
{
  ++lastloc;
  if (lastloc >= count)
    return (NULL);
  return (item[lastloc]);
}

// table::getloc() is a recursive binary searcher.  left and right
// have default values of 0 and 99999.  The value of this function is
// the the location in the table where everykey to the left is smaller
// than the input string s.  The value of lastloc is consulted before
// doing a full-blown search because often the last place we looked is
// going to be left of or equal to where we want to look now.
int table::getloc( char *s, int left, int right )
{
  // Make sure right side is legal.
  if (right > count)
    right = count;

  // Consult with lastloc.
  if (lastloc > 0 && lastloc < count && left < lastloc
      && strcmp(index[lastloc-1], s) < 0)
     left = lastloc;
  
  // If left and right are suffiently close together just do a linear
  // search. 
  if ( right - left < 5 )
    {
      for ( ; left < right; ++left )
	if (strcmp(index[left], s) >= 0)
	  break;
      lastloc = left;
      return (left);
    }

  // Find the middle of the list and decide if we want to look at the
  // left side or the right side.
  int middle = ( left + (right-left)/2 );
  if (strcmp(s, index[middle]) < 0)
    return (getloc(s, left, middle));
  else
    return (getloc(s, middle, right));
}

// table::lookup() gets the location where the key k should be if it
// is in the table.  If the returned location is out of range or the
// key at the returned location doesn't match the input key, k, then
// NULL is returned.  Otherwise the pointer associated with the key is
// returned. 
void *table::lookup( char *k )
{
  int i = getloc( k );
  if (i == count)
    return (NULL);
  if (strcmp( index[i], k ) == 0)
    return (item[i]);
  else
    return (NULL);
}

// table::insert() puts a new key/pointer in the table.  If the key
// has already been used then the old object is deleted and the new
// put in in its place.
void table::insert( char *k, void *p )
{
  if ( count == size )
    {
      cerr << "** table full trying to insert: " << k << "\n";
      return;
    }
  int i = getloc( k );
  if ( i < count && strcmp( index[i], k ) == 0 )
    delete item[i];
  else
    {
      // Move everything over one place.
      for (int j = count; j > i; j-- )
	{
	  index[j] = index[j-1];
	  item[j] = item[j-1];
	}
      count++;
      index[i] = new char[max_table_key_length];
    }
  strcpy( index[i], k );
  item[i] = p;
}

// table::table() is the constructor for a table.  Space for the index
// and item arrays of length s are allocated. size, count, and lastloc
// are initialized.
table::table( int s )
{
  index = new STRING[s];
  item = new THING[s];
  size = s;
  count = 0;
  lastloc = 0;
}
