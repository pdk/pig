#include "std.h"
#include "get_word.h"
#include "table.h"
#include "phonology.h"
#include "lexicon.h"
#include "network.h"
#include "parse.h"

main()
{
  init_phonology();
  init_lexicon();
  init_network();

  char w[max_string];
  while ( cin >> w )
    {
      if (strcmp(".show", w)==0)
	{
	  cin >> w;
	  cout << w << ":\n";
	  get_category(w)->show();
	}
      else
	parse_word(w);
    }
}
