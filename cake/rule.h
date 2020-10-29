const int max_context = 10;      // how many items on each side
const int max_alternates = 10;   // how many things to alternate

class element
{
 private:
  int kind;
  cset *c;
  char s;
 public:
  element( char x )
    {
      kind = 0;
      s = x;
    }
  element( cset *x )
    {
      kind = 1;
      c = x;
    }
  int contains( char x );
  void show();
};

enum side { left, right };
const int side_count = 2;
const int alt_size = 10;
enum viability { not_possible, mandatory, possible };

class rule
{
 private:
  element *context[side_count][max_context];
  int context_count[side_count];
  
  char alternate[side_count][max_alternates][alt_size];
  int alternate_count;

  int pos;
  int goodness;

  int make_alternate( char *form, side s );
  void make_context( side s );

  int match_context( char *st, side si );
  void viable_exchange( char *s, int p );
  viability viable_match( char *s, int p );
  viability viable_context( char *s, int p, side si );

 public:
  void apply( char *s );
  void viable( char *form, int rule_num, int vpos = 0 );
  rule( char *left_alt );
  int good();
  void show();
};
