#include <stdio.h>
#include <ctype.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

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
	trie *r;

	if((r = pre->succ[c]) == NULL){
		if( (r = malloc(sizeof(*r))) ){
			memset(r,0,sizeof(*r));
			pre->succ[c] = r;
		}
	}
	return r;
}

#define VALTYPE unsigned

static inline int
inc_obj(trie *obj){
	if(obj->obj == NULL){
		if((obj->obj = malloc(sizeof(VALTYPE))) == NULL){
			fprintf(stderr,"Allocation failure\n");
			return -1;
		}
		*(VALTYPE *)(obj->obj) = 1;
	}else{
		++*(VALTYPE *)(obj->obj);
	}
	return 0;
}

// recursive prefix dfs of the trie, ho-hum
static int
dump_trie_rec(const trie *c,unsigned char *pre,int prelen){
	unsigned z;

	if(c->obj){
		fprintf(stderr,"%.*s: {%u}\n",prelen,pre,*(VALTYPE *)(c->obj));
	}
	for(z = 0 ; z < sizeof(c->succ) / sizeof(*c->succ) ; ++z){
		if(c->succ[z]){
			pre[prelen] = z;
			dump_trie_rec(c->succ[z],pre,prelen + 1);
		}
	}
}

// single call-site driver of dump_trie_rec()'s DFS
static inline int
dump_trie(const trie *ccdance,size_t maxlen){
	unsigned char *b;
	int r;

	if((b = malloc(maxlen)) == NULL){
		fprintf(stderr,"Allocation failure\n");
		return -1;
	}
	r = dump_trie_rec(ccdance,b,0);
	free(b);
	return r;
}

// We assume that "words" are all the longest substrings of non-whitespace,
// non-punctuation characters within the text. For the bonus, we assume that
// "sentences" are the longest substrings of non-[.?!] characters. We read the
// text file on stdin (a character at a time, ugh -- we'd do something more
// reasonable in real life), and write the concordance to stdout. Pure ANSI C
// excludes strerror() and friends, so our diagnostics are a bit terse. We
// don't bother to clean up the trie on exit, whether exceptional or otherwise.
int main(int argc,const char **argv){
	size_t maxlen = 0,wl;
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
	wl = 0;
	// invariant: cur points at a valid trie node, corresponding precisely
	// to the word as read thus far.
	while((c = fgetc(stdin)) != EOF){
		switch(state){
		case 0: if(!isspace(c) && !ispunct(c)){
				state = 1;
			} // intentional fall-through
		case 1: if(!state){
				break;
			}
			if(isspace(c) || ispunct(c)){
				state = 0; // assert(cur != &ccdance)
				if(inc_obj(cur)){
					return EXIT_FAILURE;
				}
				cur = &ccdance;
				maxlen = wl <= maxlen ? maxlen : wl;
				wl = -1;
			}else if((cur = trie_next(cur,c)) == NULL){
				return EXIT_FAILURE;
			}
			++wl;
			break;
		default:
			fprintf(stderr,"Unknown state %u\n",state);
			return EXIT_FAILURE;
		}
		fprintf(stderr,"WL: %zu MAX: %zu\n",wl,maxlen);
	}
	if(!feof(stdin)){
		fprintf(stderr,"Error reading input\n");
		return EXIT_FAILURE;
	}
	dump_trie(&ccdance,maxlen);
	return EXIT_SUCCESS;
}
