#include "std.h"
#include "misc.h"
#include "table.h"
#include "phonology.h"
#include "lexicon.h"
#include "network.h"
#include "parse.h"

// Here is the general parsing strategy:  examine the surface forms
// looking for allomorphs while making transitions in the network and
// building a tree containing all the morphemes.

// parse::insert_sibling() inserts a morpheme in a parse tree as a
// sibling node.  It recursively traces the sibling list looking to
// see if the morpheme being inserted is already included in the list.
// If it is a duplicate then nothing is done.  If it isn't a duplicate
// then a new parse node is created and put at the end of the list.
// The parse tree node containing the morpheme being inserted is
// returned from the function so that the analysis can continue with
// that node.
parse *parse::insert_sibling( morpheme *x )
{
  if (m == NULL)
    {
      m = x;
      return (this);
    }
  if (m == x)
    {
      count++;
      return (this);
    }
  if (sibling == NULL)
    return (sibling = new parse(x));
  return (sibling->insert_sibling(x));
}

// parse::~parse() is the destructor for a parse tree.  This is called
// when a parse tree is deleted (the memory is deallocated).  It
// deletes whatever stuff it may be pointing to.
parse::~parse()
{
  if (child!=NULL)
    delete child;
  if (sibling!=NULL)
    delete sibling;
}

// build_parse is the most critical function.  It examines the surface
// form looking for allomorphs and building a parse tree.  It works
// recursively.
parse *build_parse( char *f, state *s )
{
  // Get a blank parse tree to start with.
  parse *np = new parse(NULL);
  // Get the list of arcs out of the state we're in.
  arc_list *arcl = s->branches;
  // Examine all the arcs leaving this state.
  while ( arcl != NULL )
    {
      // Get the arc from the list and shorten the arc_list for next
      // time thru the loop.
      arc *a = arcl->data;
      arcl = arcl->next;
      // Find out how much surface form we need to deal with.
      int max = strlen(f);
      // Look at all substrings starting at the beginning of the form.
      for (int len = 0; len <= max; len++)
	{
          // Get the list of morphemes matching the current substring.
	  morpheme_list *ml = a->cat->matching(f,len);
          // Posit all the morphemes in the list.
	  while (ml!=NULL)
	    {
              // Get the sibling parse node which contains the
              // morpheme being posited.
	      parse *p = np->insert_sibling(ml->data);
              // If we've reached the end of the string and the head
              // of this arc is a final state set the parse node as a
              // final node. 
	      if (len == max)
		p->final = a->head->final;
              // Recursively move thru the string and go to the next
              // state.  Store the result of that exploration as the
              // child of the posited parse node.
	      p->child = build_parse( &f[len], a->head );
              // Go to the next morpheme in the list.
	      ml = ml->next;
	    }
	}
    }
  // If we found no allomorphs then return NULL for this parse tree.
  // Otherwise return the new parse tree.
  if (np->m == NULL)
    return (NULL);
  else
    return (np);
}

// display_parse() traces the tree displaying all the morphemes of the
// allomorphs found.  
void display_parse( parse *p, int l = 0 )
{
  while (p!=NULL)
    {
      if (p->final)
	cout << "  *";
      else
	cout << "   ";
      cout<<spaces(l)<<"["<<p->m->form<<"] '"
	<<p->m->gloss<<"'   "<<p->count<<"\n";
      display_parse( p->child, l+2 );
      p = p->sibling;
    }
}

// sf is the surface form being examined.
static char sf[max_string];
// wur is the working underlying representation.
static char wur[max_string];
// disp_form is the displayable string containing all the morpheme
// underlying forms.
static char disp_form[max_string];
// disp_gloss is the displayable string containing all the morpheme
// glosses.  disp_form and disp_gloss have the UR and the glosses
// lined up together so that the user can see them in parallel.
static char disp_gloss[max_string];

// check_phonology() is the bottom of the parse checker.  Once a path
// thru the parse tree ending with a final parse node has been found
// then the phonology is applied here.  A copy of wur (the working
// underlying form) is made and the phonology is applied (using
// apply()).  If the result matches the surface form then we have a
// good parse and the displayable strings are written out.
void check_phonology()
{
  char worker[max_string];
  // Make a copy for the phonology.
  strcpy( worker, wur );
  // Apply the phonology.
  apply( worker );
  // Does it match?
  if (strcmp( worker, sf )==0)
    {
      cout << "  " << disp_form << "\n";
      cout << "  " << disp_gloss << "\n\n";
    }
}

// copy_morpheme() copies the form of the morpheme of the current
// parse node into wur (the working underlying form) and into
// disp_form.  The gloss of the morpheme is copied into disp_gloss.
// Spaces are put on the end of disp_form and disp_gloss so that
// they'll line up neatly.
void copy_morpheme( parse *p, int pos, int disp_pos )
{
  strcpy( &wur[pos], p->m->form );
  strcpy( &disp_form[disp_pos], p->m->form );
  strcpy( &disp_gloss[disp_pos], p->m->gloss );
  // Put padding on the display strings.
  int f = strlen(p->m->form)+disp_pos;
  int g = strlen(p->m->gloss)+disp_pos;
  while(f<g) disp_form[f++]=' ';
  while(g<f) disp_gloss[g++]=' ';
  disp_form[f++]=' ';
  disp_gloss[g++]=' ';
  disp_form[f]='\0';
  disp_gloss[g]='\0';
}  

// check_parse() traverses the parse tree trying all the different
// paths, and thus all the sequences of allomorphs found.
void check_parse( parse *p, int pos = 0, int disp_pos = 0 )
{
  while (p != NULL)
    {
      // Update wur, disp_form and disp_gloss.
      copy_morpheme( p, pos, disp_pos );
      // If this is a final node in the tree, try the phonology and
      // print it if so. (Printing is done by check_phonology().)
      if (p->final)
	check_phonology();
      check_parse(p->child, pos+strlen(p->m->form),
		  disp_pos+strlen(&disp_form[disp_pos]));
      // Go to the next sibling.
      p = p->sibling;
    }
}

// parse_word() is the interface function.  It puts the word into sf
// (the surface form storage place).  A parse tree is built from the
// start state 'start'.  The word is displayed and the parse tree
// built for it.  check_parse() is called to finalize the parsing and
// prints the good analyses.  The parse tree is deleted to return the
// allocated memory.
void parse_word( char *s )
{
  strcpy( sf, s );
  parse *p = build_parse(sf,get_state( "start" ));
  cout << "\n--- " << s << " ---\n";
  check_parse(p);
  display_parse(p);
  delete p;
}
