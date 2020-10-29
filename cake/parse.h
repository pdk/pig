struct parse
{
  parse *sibling;
  parse *child;
  morpheme *m;
  int final;
  int count;
  parse( morpheme *x )
    {
      m = x;
      sibling = NULL;
      child = NULL;
      final = 0;
      count = 1;
    }
  parse *insert_sibling( morpheme *x );
  ~parse();
};

extern void parse_word( char *f );
