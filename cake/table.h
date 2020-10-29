const max_table_key_length = 20;

typedef char *STRING;
typedef void *THING;

class table
{
 private:
  int size, count;
  STRING *index;
  THING *item;
  int getloc( char *s, int left = 0, int right = 99999 );
  int lastloc;
 public:
  table( int s );
  void *lookup( char *k );
  void insert( char *k, void *p );
  void *first();
  void *next();
  int max() { return (count); }
  char *str( int i ) { return (index[i]); }
  void *obj( int i ) { return (item[i]); }
};
