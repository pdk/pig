#include "std.h"

// ismark() checks to see if a character is a brace or semi-colon.
// Braces are used to define csets.  The semi-colon is used to
// terminate rules and definitions.

int ismark(char c)
{
  switch(c)
    {
    case '{':
    case '}':
    case ';':
      return (1);
    default:
      return (0);
    }
}

// get_word() is used by to read "words".  A "word", here, is either a
// brace or a semi-colon or a sequence of characters which does not
// contain a brace, a semi-colon, or whitespace.  When there are no
// more words in the file NULL is returned.  A # is ignored as well as
// everything upto the end of the line.  This is used for comments.

char *get_word( istream *fi )
{
  // c is static so that if we get a special character (brace or
  // semi-colon) it can be returned next time get_word() is called.
  static char c = ' ';
  // s is static because this is where the word is stored.  A pointer
  // to this is returned if there is a word to return.
  static char s[max_string];

  // First we check to see if we have a brace or semi-colon left over
  // from last time get_word was invoked.  If so put it is s and
  // return it as the next word.
  if (ismark(c))
    {
      s[0] = c;
      s[1] = '\0';
      c = ' ';
      return (s);
    }
  // Well, we didn't have a semi-colon or brace, so we need to read
  // the next word.
  int i = 0;
  while (fi->get(c)) {
    if (isspace(c)||c=='#') {
      // Found whitespace.  If it's a comment then read to the end of
      // the line.  If already have characters then return the word.
      if (c=='#')
	do fi->get(c); while (c!='\n');
      if (i>0) {
        s[i] = '\0';
        return (s);
      }
      continue;
    } else if (ismark(c)) {
      // Found a brace or semi-colon.  If we have stuff already then
      // leave the mark in c for next time and return the stuff we
      // have as the next word.  Otherwise just return the mark as the
      // next word.
      if (i == 0) {
        s[0] = c;
        s[1] = '\0';
        c = ' ';
        return (s);
      } else {
        s[i] = '\0';
        return (s);
      }
    } else {
      // Found a normal character.  If it is a backslash then get the
      // next character and put it in the new word whatever it is.
      // This is useful for getting a # into a word.
      if (c=='\\')
	fi->get(c);
      s[i++] = c;
      continue;
    }
  }
  // We've gotten to the end of the file.  If we have stuff in to
  // return as a word then do so.  Otherwise return NULL.
  if (i>0)
    {
      s[i] = '\0';
      return (s);
    }
  else
    return (NULL);
}
