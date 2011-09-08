#include <stdio.h>
#include <limits.h>
#include <stdlib.h>

#define SIGMA (1u << CHAR_BIT) // size of the alphabet

// We hold the concordance in a classic trie over the native alphabet.
typedef struct trie {
	struct trie *succ[SIGMA];
	void *obj;
} trie;

// ccdance->obj is object corresponding to empty string
static trie ccdance;

static inline trie *
trie_next(trie *pre,int c){
	fprintf(stderr,"FIXME!\n");
	return NULL;
}

// We assume that "words" are all the longest substrings of non-whitespace,
// non-punctuation characters within the text. For the bonus, we assume that
// "sentences" are the longest substrings of non-[.?!] characters. We read the
// text file on stdin (a character at a time, ugh -- we'd do something more
// reasonable in real life), and write the concordance to stdout. Pure ANSI C
// excludes strerror() and friends, so our diagnostics are a bit terse. We
// don't bother to clean up the trie on exit, whether exceptional or otherwise.
int main(int argc,const char **argv){
	struct trie *cur;
	unsigned state;
	int c;

	if(argc != 1){
		fprintf(stderr,"usage: %s\n",*argv);
		return EXIT_FAILURE;
	}
	// We can be in two states:
	//  - looking for next word (state 0, cur = &&ccdance)
	//  - looking for word's end (state 1, cur points at prefix node)
	state = 0;
	cur = &ccdance;
	// invariant: cur points at a valid trie node, corresponding precisely
	// to the word as read thus far.
	while((c = fgetc(stdin)) != EOF){
		switch(state){
		case 0: if(!isspace(c)){
				state = 1;
			} // intentional fall-through
		case 1: if(!state){
				break;
			}
			if(isspace(c)){
				state = 0;
				// FIXME record the word occurence (inc 'obj')
			}else if((cur = trie_next(cur,c)) == NULL){
				return EXIT_FAILURE;
			}
			break;
		default:
			fprintf(stderr,"Unknown state %u\n",state);
			return EXIT_FAILURE;
		}
	}
	if(!feof(stdin)){
		fprintf(stderr,"Error reading input\n");
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
