extern const char *cset_border;

//   class cset is a set of characters.

class cset
{
 private:
  char it[256];
 public:
  char *name;
  cset();
  char member( char c );
  void add( char c );
  void remove( char c );
  void show();
  void set_name( char *n );
};

extern cset *build_cset( istream *fi );
