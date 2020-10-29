const int max_morph_length = 15;
const int max_categories = 50;
const int max_surface_forms = 15000;

extern void show_lexicon();
extern void init_lexicon();

struct morpheme
{
  char form[max_morph_length];
  char gloss[max_string];

  morpheme( char *f, char *g );
  void show();
};

struct morpheme_list
{
  morpheme *data;
  morpheme_list *next;

  morpheme_list( morpheme *m );
  void insert( morpheme *m );
  void show();
};

class category
{
  friend void put_form( char *sf );
 private:
  table *form_table;
  void inner_put_form( char *sf );
 public:
  morpheme_list *matching( char *sf, int len );
  void insert( morpheme *m );
  category() { form_table = new table(max_surface_forms); }
  void show();
};

extern category *get_category( char *s );
