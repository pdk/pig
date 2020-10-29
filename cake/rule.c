#include "std.h"
#include "misc.h"
#include "table.h"
#include "get_word.h"
#include "cset.h"
#include "rule.h"

// continue_viable() is the function in phonology.c which goes on to
// the next rule in the list.  
extern void continue_viable( char *s, int rule_num );
extern istream *rule_in;
// cset_table is the cset_table in phonology.c.
extern table cset_table;

// element::contains() tells if the parameter x is in the this element.
int element::contains( char x )
{
  if (kind == 0)
    if (s == x)
      return (1);
    else
      return (0);
  return ( c->member(x) );
}

// element::show() shows an element.
void element::show()
{
  if (kind == 0)
    cout << chr(s);
  else
    cout << c->name;
}

// rule::good() returns the goodness value of a rule.  If there was a
// problem with the building of a rule the value of goodness will be 0.
int rule::good()
{
  return (goodness);
}

// rule::show() shows a rule in mostly the same way it is read.  Since
// zeroes are deleted from the input strings there are strings of
// length 0.  Because of this all the pieces are surrounded by slashes.
void rule::show()
{
  int i;
  cout << "/";
  for ( i = 0; i < alternate_count; ++i )
    cout << alternate[left][i] << "/";
  cout << " -> /";
  for ( i = 0; i < alternate_count; ++i )
    cout << alternate[right][i] << "/";
  cout << "  ";
  for ( i = 0; i < context_count[left]; ++i )
    {
      context[left][i]->show();
      cout << " ";
    }
  cout << "___ ";
  for ( i = 0; i < context_count[right]; ++i )
    {
      context[right][i]->show();
      cout << " ";
    }
  cout << ";\n";
}

// rule::make_alternate() takes a string like "a/b/c/" and separates
// the stuff from the slashes.  Each piece is put into alternate[s].
// The zeroes are removed from each piece.  The number of pieces is
// returned as the value of this function.
int rule::make_alternate( char *form, side s )
{
  int c;
  int a_count;

  if (*form == '/')
    form++;

  for ( a_count = 0 ; *form != '\0' ; a_count++ )
    {
      for ( c = 0 ;
	   form[c] != '/' && form[c] != '\0' ;
	   c++ )
	;
      strncpy( alternate[s][a_count], form, c );
      alternate[s][a_count][c] = '\0';
      remove_zeros( alternate[s][a_count] );
      form += c;
      if ( *form ) form++;
    }
  return (a_count);
}

// rule::make_context() reads the input until it finds a word starting
// with '_' (denoting the break between left and right context) or a
// semi-colon, which denotes the end of the rule.  Each word is
// checked to see if it is a defined cset.  If it is then that cset
// becomes one element of the context.  Otherwise all the letters in
// the word each become an element.
void rule::make_context( side s )
{
  char *x;
  context_count[s] = 0;
  while ( 1 )
    {
      x = get_word( rule_in );
      if ( *x == ';' || *x == '_' )
	return;
      cset *c = (cset *)cset_table.lookup( x );
      if ( c != NULL )
	context[s][context_count[s]++] = new element( c );
      else
	{
	  remove_zeros( x );
	  while (*x)
	    context[s][context_count[s]++] = new element( *x++ );
	}
    }
}

// rule::rule() is the rule constructor.  It takes a string assumed to
// be a left side of rule.  It assumes that the "->" has been read and
// the next word is the right side.  It processes these and then gets
// the left and right context.  Then various restrictions are checked.
rule::rule( char *left_side )
{
  char *right_side = get_word( rule_in );

  context_count[left] = context_count[right] = 0;
  alternate_count = 0;
  pos = 0;
  goodness = 1;

  int l = make_alternate( left_side, left );
  int r = make_alternate( right_side, right );

  make_context( left );
  make_context( right );

  int alt0len = strlen(alternate[left][0]);
  for (int x = 1; x < l; ++x)
    if ( strlen(alternate[left][x]) != alt0len )
      {
	goodness = 0;
	cerr << "** bad rule: length of left alternates must be equal\n";
	break;
      }

  if (l != r)
    {
      cerr << "** bad rule: number of alternates not equal\n";
      goodness = 0;
    }
  alternate_count = l;
}

// rule::apply() applies a rule to a string from left to right.  In
// the style of SPE the underbar move from left to right.  At each
// point the left context is checked to see if it matches.  If it does
// then each of the pieces of the left side is checked for a match.
// If one is found then the right context is checked.  If all the
// parts are matching then the exchange is done.  If an exchange is
// done then the leftmost positions to check is re-calculated and the
// pieces of the left side are checked again.
void rule::apply( char *s )
{
  int leftest = context_count[left];
  int rightest = strlen(s)-context_count[right];
  // Dont bother looking where there is not enough room for all the context.
  for ( pos = leftest; pos < rightest; )
    {
      int a = 0;
      int loop_count = 0;
      if (match_context( &s[pos-context_count[left]], left ))
	while (a < alternate_count)
	  {
	    if (strmatch( &s[pos], alternate[left][a] ) &&
		match_context( &s[pos+strlen( alternate[left][a] )], right ))
	      {
		exchange( &s[pos], alternate[right][a],
			 strlen( alternate[left][a] ));
		a = -1;
		// Re-calculate the rightest position.  The string
		// might be longer than before.
		rightest = strlen(s)-context_count[right];
	      }
	    ++a;
	    ++loop_count;
	    if (loop_count > max_alternates*max_string)
	      {
		cout<<"** infinite phonological rule:\n";
		this->show();
		exit(1);
	      }
	  }
      ++pos;
    }
}

// rule::match_context() checks to see if the string is matched by a
// context.  Each element of the context is checked to see if it
// contains the character in the string.  If they all match then 1 is
// returned.  Otherwise 0 is returned.
int rule::match_context( char *st, side si )
{
  if (strlen(st) < context_count[si])
    return(0);

  int z;
  for (z = 0; z < context_count[si]; ++z )
    if ( ! context[si][z]->contains( st[z] ))
      return (0);
  return (1);
}

// rule::viable() generates viable forms of form.  There are three
// levels of viability: not_possible, mandatory, and possible.
// not_possible means that no exchange should be done.  mandatory
// means that an exchange must be done.  possible means that a form
// with the exchange done and one without the exchange done should be
// generated.  
void rule::viable( char *form, int rule_num, int vpos )
{
  // Make a copy of the string so that if somebody is waiting to
  // continue with the string they have the same thing later.
  char copy[max_string];
  strcpy( copy, form );
  int rightest = strlen(copy);
  if (vpos >= rightest)
    {
      // If this rule has gone all the way thru the string then
      // continue with the next rule.
      continue_viable( copy, rule_num );
      return;
    }
  viability p;
  // Find a viable position.
  for ( ; vpos < rightest; vpos++ )
    if ( (p=viable_match( copy, vpos )) != not_possible )
      break;
  // If it's mandatory then continue only after the exchange.
  // If it's not_possible then continue only with the same string.
  // If it's possible then do both.
  if ( p != mandatory )
    viable( copy, rule_num, vpos+1 );
  if ( p != not_possible )
    {
      viable_exchange( copy, vpos );
      viable( copy, rule_num, vpos );
    }
}

// rule::viable_match() returns not_possible if the string which is
// present does not match the context.  It returns possible if the
// string matches some of the context.  If all the context is matched
// then mandatory is returned.
viability rule::viable_match( char *s, int p )
{
  // Find a viable possible alternate.
  for ( int a = 0 ; a < alternate_count ; ++a )
    if (strmatch( &s[p], alternate[left][a]))
      break;
  if (a == alternate_count)
    return (not_possible);

  viability lv = viable_context( s, p-context_count[left], left );
  if (lv == not_possible)
    return(not_possible);

  viability rv = viable_context( s, p+strlen(alternate[left][0]), right);
  if (rv == not_possible)
    return(not_possible);

  if (rv == mandatory && lv == mandatory)
    return (mandatory);
  return (possible);
}

// rule::viable_exchange() performs the exchange.
void rule::viable_exchange( char *s, int p )
{
  for (int a = 0; a < alternate_count; a++ )
    if (strmatch( &s[p], alternate[left][a] ))
      {
	exchange( &s[p], alternate[right][a], strlen(alternate[left][a]));
	return;
      }
}

// rule::viable_context() checks to see if a context is viable.  If
// all the context is matched then mandatory is returned.  If some is
// matched then possible is returned.  If the string which is present
// is not in the context then not_possible is returned.
viability rule::viable_context( char *form, int p, side si )
{
  viability v = mandatory;
  int rightest = strlen(form);
  for ( int i = 0; i < context_count[si]; ++i, ++p )
    {
      if (p < 0 || p >= rightest)
	v = possible;
      else
	if ( ! context[si][i]->contains(form[p]) )
	  return (not_possible);
    }
  return (v);
}
