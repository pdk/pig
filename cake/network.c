#include "std.h"
#include "table.h"
#include "get_word.h"
#include "lexicon.h"
#include "network.h"

// max_states is the maximum number of states allowed in the network.
const int max_states = 50;
const char *network_file = "network";
// The network is stored in a table of states.  Each is referenced by
// its name. 
table *network;
istream *network_in;

// state::insert() associates an arc with it's tail.
void state::insert( arc *a )
{
  arc_list *al = new arc_list( a );
  al->next = branches;
  branches = al;
}

// get_state() gets a state from the network.  If the input name (s)
// does not refer to a state, then a new one is created and put in the
// table. 
state *get_state( char *s )
{
  state *x = (state *) network->lookup(s);
  if (x == NULL)
    {
      network->insert( s, new state );
      x = (state *) network->lookup(s);
    }
  return (x);
}

// build_arc() reads the network file and gets the parts of an arc.
// An arc has a tail (the state that it comes from), a head (the state
// that it goes to), and a category (what type of morpheme licenses
// the transition).  The order in which the things are defined is not
// important as long as all the pieces are specified.  When the
// semi-colon is found (marking the end of the definition) the head
// and category are associated with the tail by inserting an arc
// structure in the tail state.
void build_arc()
{
  char *w;
  state *to_state = NULL;
  state *from_state = NULL;
  category *by_category = NULL;
  while ( (w=get_word(network_in)) != NULL )
    {
      if (strcmp("to", w)==0)
	to_state = get_state(get_word(network_in));
      else if (strcmp("from", w)==0)
	from_state = get_state(get_word(network_in));
      else if (strcmp("by", w)==0)
	by_category = get_category(get_word(network_in));
      else if (strcmp(";", w)==0)
	{
	  if ( to_state == NULL || from_state == NULL || by_category == NULL )
	    cerr << "incomplete arc definition\n";
	  else
	    from_state->insert( new arc( by_category, to_state ));
	  return;
	}
      else
	cerr << "network input error. need to,from, or by. found "<<w<<"\n";
    }
  cerr << "unexpected end of network file\n";
}

// set_final_states() simply gets states and sets the final flag in
// them until a semi-colon is found to mark the end of the definition.
void set_final_states()
{
  char *w;
  while ( (w=get_word(network_in)) != NULL )
    {
      if (strcmp(";", w)==0)
	return;
      get_state(w)->final = 1;
    }
  cerr << "unexpected end of network file\n";
}
      
// read_network() reads the network file.  It should find either an
// arc definition or a final state definition.  One is signaled by the
// word "arc" and the other by the word "final".
void read_network()
{
  filebuf network_in_buf;
  if (network_in_buf.open( network_file, input ) == 0)
    {
      cerr << "cannot open network file: "<<network_file<<"\n";
      exit(1);
    }
  network_in = new istream( &network_in_buf );

  char *w;
  while ( (w=get_word(network_in)) != NULL )
    {
      if (strcmp("arc", w)==0)
	build_arc();
      else if (strcmp("final", w)==0)
	set_final_states();
      else
	cerr << "unknown network word: "<<w<<"\n";
    }
}

// init_network() initializes the network table and reads the network
// file. 
void init_network()
{
  network = new table(max_states);
  read_network();
}
