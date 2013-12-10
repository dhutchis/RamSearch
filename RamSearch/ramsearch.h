#include <iostream>
#include <map>

enum t_found { NONE=0, CLIQUE, INDEP };
// trick to output string name
std::ostream& operator<<(std::ostream& out, const t_found value){
    const char* s = 0;
#define PROCESS_VAL(p) case(p): s = #p; break;
    switch(value){
        PROCESS_VAL(NONE);     
        PROCESS_VAL(CLIQUE);     
        PROCESS_VAL(INDEP);
    }
#undef PROCESS_VAL

    return out << s;
}

typedef struct {
	bool *adjarr;
	int n; // total # edges = n(n-1)/2
	t_found	foundki;
	//int *subgraph;
} t_gr;

typedef struct {
	std::map<int,t_gr*> countermap;
} t_ss;

void gr_init(t_gr *gr, int n);
void gr_free(t_gr *gr);
void gr_print(const t_gr *gr);
void gr_incr(t_gr *gr);
bool gr_allzero(const t_gr *gr);

// given a graph, search for a clique subgraph size qboth OR an independent set subgraph size qboth
void gr_search(t_gr *gr, int qboth); // updates gr-> foundki and perhaps subgraph
void gr_search_rec(t_gr *gr, int qtogo, int qsofar, int *choices, bool **choicearr); // private helper recursive method

// search for a lower bound, starting n at the given number
void bigfind(int qboth, int lb_start);

// OpenMP parallel
void gr_init_given(t_gr *gr, int n, int firstnum, int totalbits);
void gr_incr_ignore(t_gr *gr, int ignore);
bool gr_allzero_ignore(const t_gr *gr, int ignore);
void bigfind_multi(const int qboth, const int lb_start);