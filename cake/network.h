extern void init_network();

struct state;

struct arc
{
  category *cat;
  state *head;

  arc( category *c, state *h )
    {
      cat = c;
      head = h;
    }
};

struct arc_list
{
  arc *data;
  arc_list *next;

  arc_list( arc *d )
    {
      data = d;
      next = NULL;
    }
};

struct state
{
  arc_list *branches;
  int final;

  state()
    {
      branches = NULL;
      final = 0;
    }
  void insert( arc *a );
};

extern state *get_state( char *s );
