#include "std.h"
#include "get_word.h"
#include "table.h"
#include "cset.h"
#include "rule.h"

const char *rule_file = "phonology";
const int max_csets = 50;
const int max_rules = 200;

istream *rule_in;

int count;
// the_list is the list of rules to apply.
rule *the_list[max_rules];
// finish is a pointer to a fuction.  This is called each time a
// viable form is generated.  It is set by viable().
void (*finish)(char *s);
// cset_table is a table of csets.  This is where definitions like
// {aeiou} V go.
table cset_table( max_csets );

// add() adds a rule to the_list.
void add( rule *r )
{
  if (count < max_rules)
    the_list[count++] = r;
  else
    cout << "** rule_keeper full: too many rules\n";
}

// show_phonology() shows all the cset definitions and the_list of rules.
void show_phonology()
{
  cset *c = (cset *)cset_table.first();
  do
    {
      c->show();
      cout << "\n";
    }
  while ( (c=(cset *)cset_table.next()) != NULL );

  for ( int i = 0; i < count ; ++i )
    {
      the_list[i]->show();
    }
}

// apply() applies all the rules to a string.
void apply( char *s )
{
  for (int i = 0; i < count; ++i)
    the_list[i]->apply( s );
}

// viable() is used to generate possible allomorphs.  The function
// pointer finish is set to point to the parameter f and then the
// rules are called to generate viable forms.  Only the first rule is
// called here, but it will call continue_viable(), below, which calls
// the next one (and it goes on...).
void viable( char *form, void (*f)(char *s) )
{
  finish = f;
  if (count > 0)
    the_list[0]->viable( form, 0 );
  finish = NULL;
}

// continue_viable() is called by a rule each time the rule produces a
// viable form.  The rule number is checked to see if it was the last
// rule that just called.  If it is then the function pointed to by
// finish is called.  (This will usually be put_form().)  If it is not
// the last rule then the next rule is called.
void continue_viable( char *s, int rule_num )
{
  rule_num++;
  if (rule_num == count)
    (*finish)( s );
  else
    the_list[rule_num]->viable( s, rule_num );
}

// build_rule() is given the left side of a change and tries to get a
// rule built out of it.
rule *build_rule( char *left_side )
{
  rule *x = new rule( left_side );
  if (x->good())
    return (x);
  delete x;
  return (NULL);    
}

// read_rules() reads the rule file.  There are three things that may
// appear in the rule file: cset definitions, the word "show" and rule
// definitions.  This function just reads words looking for "{" and
// "show".  If it finds "{" it calls build_cset and puts the product
// into the cset_table.  If it finds "show" then it shows all the
// cset_definitions and all the rules.  Otherwise it tries to get a rule.
void read_rules()
{
  filebuf rule_in_buf;
  if (rule_in_buf.open(rule_file, input) == 0)
    {
      cerr << "cannot open rule file: " << rule_file << "\n";
      exit(1);
    }
  rule_in = new istream(&rule_in_buf);

  char *w;
  char hold[max_string];
  while ( w = get_word(rule_in) )
    {
      if (strcmp(w, cset_border) == 0)
	{
	  cset *c = build_cset(rule_in);
	  cset_table.insert( c->name, c );
	}
      else if (strcmp(w, "show") == 0)
	show_phonology();
      else
	{
	  strcpy( hold, w );
	  w = get_word(rule_in);
	  if ( strcmp(w, "->") == 0 )
	    add(build_rule( hold ));
	  else
	    {
	      cerr << "** expected: ->  found: " << w << "\n";
	    }
	}
    }
}

// init_phonology() sets count and reads the rule file.
void init_phonology()
{
  count = 0;
  read_rules();
}
