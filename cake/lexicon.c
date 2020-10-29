#include "std.h"
#include "get_word.h"
#include "table.h"
#include "lexicon.h"
#include "phonology.h"

// cat_table is a table of categories.
static table *cat_table;
static category *cur_cat;
static morpheme *cur_morph;
const char *lexicon_file = "lexicon";

istream* lexicon_in;

// morpheme::morpheme() is the morpheme constructor.  f is the
// underlying form and g is the gloss.
morpheme::morpheme( char *f, char *g )
{
  if (strlen(f) > max_morph_length)
    cerr << "morphmeme longer than max_morph_length\n";
  if (strlen(g) > max_string)
    cerr << "gloss longer than max_string\n";
  strcpy( form, f );
  strcpy( gloss, g );
}

// morpheme::show() displays a morphemes form and gloss.
void morpheme::show()
{
  cout << "    /" << form << "/   '" << gloss << "'\n";
}

// morpheme_list::morpheme_list() is the morpheme_list constructor.
morpheme_list::morpheme_list( morpheme *m )
{
  data = m;
  next = NULL;
}

// morpheme_list::insert() puts a new morpheme into a morpheme list.
// If the morpheme is already in the list then nothing happens.
void morpheme_list::insert( morpheme *m )
{
  if ( m == data )
    return;
  if ( next == NULL )
    next = new morpheme_list( m );
  else
    next->insert(m);
}

// morpheme_list::show() shows a morpheme_list.
void morpheme_list::show()
{
  if (data != NULL)
    data->show();
  if (next != NULL)
    next->show();
}

// put_form() is used to insert morphemes into a cateogry indexed by
// allomorphs.  A pointer to this function is handed to the phonology
// with a request to produce viable allomorphs.  For each allomorph
// the phonology produces this function is called.
void put_form( char *sf )
{
  cur_cat->inner_put_form( sf );
}

// category::inner_put_form() is called by put_form to put a morpheme
// into a category.  If form_table does not have an entry for this
// surface form (allomorph produced by the phonology, the sf
// parameter) then a new morpheme list is inserted into the
// form_table.  Otherwise the morpheme is just put into the morpheme
// list that is already there.
void category::inner_put_form( char *sf )
{
  morpheme_list *ml = (morpheme_list *)form_table->lookup( sf );
  if ( ml == NULL )
    form_table->insert( sf, new morpheme_list( cur_morph ));
  else
    ml->insert( cur_morph );
}
    
// category::insert() puts a new morpheme into a category or
// morphemes.  The global (to this file) variables cur_morph and
// cur_cat are set and then the phonology is called to generate all
// the possible allomorphs and put them into the form_table.
void category::insert( morpheme *m )
{
  cur_morph = m;
  cur_cat = this;
  viable( m->form, &put_form );
}

// category::matching() returns the list of morphemes which match the
// first len characters of sf.  
morpheme_list *category::matching( char *sf, int len )
{
  char x[max_morph_length];
  strncpy( x, sf, len );
  x[len] = '\0';
  return ( (morpheme_list *)form_table->lookup( x ) );
}

// category::show() goes thru the table of surface forms and shows the
// morpheme_lists.  
void category::show()
{
  for (int i = 0; i < form_table->max(); i++ )
    {
      cout << "  [" << form_table->str(i) << "]\n";
      ( (morpheme_list *) form_table->obj(i) )->show();
    }
}

// get_category() returns the category which the string s refers to or
// puts in a new category and names it s.  
category *get_category( char *s )
{
  category *x = (category *)cat_table->lookup( s );
  if (x == NULL)
    {
      cat_table->insert( s, new category );
      x = (category *)cat_table->lookup( s );
    }
  return (x);
}

// show_lexicon() shows all the categories in the cat_table (the lexicon).
void show_lexicon()
{
  for (int i = 0; i < cat_table->max(); i++ )
    {
      cout << cat_table->str(i) << ":\n";
      ( (category *)cat_table->obj(i) )->show();
    }
}

// read_lexicon() reads the lexicon file and puts the morpheme
// definitions into cat_table.  Each morpheme has three aspects: the
// underlying form, the category, and the gloss.  A morpheme struct
// containing the form and category is inserted into it's category.
void read_lexicon()
{
  filebuf lexicon_in_buf;
  if (lexicon_in_buf.open(lexicon_file, input) == 0)
    {
      cerr << "cannot open lexicon file: " << lexicon_file << "\n";
      exit(1);
    }
  lexicon_in = new istream(&lexicon_in_buf);

  char *w;
  char ur[max_string];
  char cat[max_string];
  char gloss[max_string];
  while ( w = get_word(lexicon_in) )
    {
      strcpy( ur, w );
      if ( (w=get_word(lexicon_in)) == NULL )
	{
	  cerr << "unexpected end of lexicon: expected category\n";
	  return;
	}
      strcpy( cat, w );
      if ( (w=get_word(lexicon_in)) == NULL )
	{
	  cerr << "unexpected end of lexicon: expected gloss\n";
	  return;
	}
      strcpy( gloss, w );
      while ( w = get_word(lexicon_in) )
	if ( *w == ';' )
	  break;
	else
	  {
	    cerr << "expected semicolon in lexicon, found: " << w << "\n";
	  }
      // Do the insertion.
      get_category( cat )->insert( new morpheme( ur, gloss ));
    }
}

// init_lexicon() initialized the cat_table (table of categories) and
// reads the lexicon file.
void init_lexicon()
{
  cat_table = new table( max_categories );
  read_lexicon();
};
