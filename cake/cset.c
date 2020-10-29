#include "std.h"
#include "get_word.h"
#include "cset.h"

const char *cset_border = "{";

//   class cset is a set of characters.

//   cset::cset() is the constructor for a cset.  This initializes the
// cset to be empty.

cset::cset()
{
  for (int i=0; i<256; ++i )
    it[i] = '\0';
}

//   cset::member() returns the character if it is in the set
// otherwise it returns '\0';

char cset::member( char c )
{
  return it[c];
}

//  cset::add() put a character into the cset.

void cset::add( char c )
{
  it[c] = c;
}

//   cset::remove() takes a character out of a cset.

void cset::remove( char c )
{
  it[c] = '\0';
}

void cset::show()
{
  char c;
  cout << name << "{";
  for ( int i=0; i<256; i++ )
    if (c = it[i])
      cout << chr(c);
  cout << "}";
}

void cset::set_name( char *s )
{
  strcpy( name = new char[strlen(s)+1], s );
}

// build_cset() reads the stream fi until it finds a right brace
// putting the characters into a new cset.  The it gets the next word
// as the name.
cset *build_cset( istream *fi )
{
  cset *c = new cset;
  char x;
  // read characters into x
  while ((*fi)>>x)
    {
      if (x=='}')
	break;
      if (x == '\\')
	(*fi)>>x;
      c->add(x);
    }
  char *name = get_word(fi);
  c->set_name(name);
  return (c);
}  
