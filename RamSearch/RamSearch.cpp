// RamSearch.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ramsearch.h"
#include <omp.h>

/*#define CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>*/

#define gr_nv(gr) ((gr->n)*(gr->n-1)/2)

void gr_init(t_gr *gr, int n)
{
	if (!gr || n <= 0) return;
	gr->n = n;
	gr->adjarr = new bool[n*(n-1)/2];
	for (int i=0; i<gr_nv(gr); i++)
		gr->adjarr[i] = 0;
	gr->foundki = NONE;
	//gr->subgraph = NULL;
}
void gr_init_given(t_gr *gr, int n, int firstnum, int totalbits)
{
	if (!gr || n <= 0) return;
	gr->n = n;
	gr->adjarr = new bool[n*(n-1)/2];

	int i;
	for (i = 0; i < totalbits; i++, firstnum >>= 1)
		gr->adjarr[i] = firstnum & 1;

	for ( ; i<gr_nv(gr); i++)
		gr->adjarr[i] = 0;
	gr->foundki = NONE;
	//gr->subgraph = NULL;
}
void gr_free(t_gr *gr)
{
	if (!gr) return;
	delete[] gr->adjarr; gr->adjarr = NULL;
	//if (gr->subgraph) { delete[] gr->subgraph; gr->subgraph = NULL; }
}
void gr_print(const t_gr *gr) {
	//std::cout << gr->n << ':';
	for (int i=0; i<gr_nv(gr); i++)
		std::cout << gr->adjarr[i];
	std::cout << std::endl;
}
void gr_print_all(const t_gr *gr) {
	//std::cout << gr->n << ':';
	for (int i=2; i<=gr->n; i++)
		for (int j=1; j < i; j++)
			std::cout << "a"<<i<<j<<'='<<gr->adjarr[i]<<" ";
	std::cout << std::endl;
}
void gr_incr(t_gr *gr) {
	if (!gr) return;
	for (int i=0; i<gr_nv(gr); i++)
		if (gr->adjarr[i] == 0) {
			gr->adjarr[i] = 1;
			return;
		} else {
			gr->adjarr[i] = 0;
		}
	// if we made it here, the entire array is 0s now; reset to 0
}
// ignore first ignore bits
void gr_incr_ignore(t_gr *gr, int ignore) {
	if (!gr) return;
	for (int i=ignore; i<gr_nv(gr); i++)
		if (gr->adjarr[i] == 0) {
			gr->adjarr[i] = 1;
			return;
		} else {
			gr->adjarr[i] = 0;
		}
	// if we made it here, the entire array is 0s now; reset to 0
}
bool gr_allzero(const t_gr *gr) {
	if (!gr) return false;
	for (int i=0; i<gr_nv(gr); i++)
		if (gr->adjarr[i] != 0)
			return false;
	return true;
}
// see if we're done, ignoreing first ignore bits
bool gr_allzero_ignore(const t_gr *gr, int ignore) {
	if (!gr) return false;
	for (int i=ignore; i<gr_nv(gr); i++)
		if (gr->adjarr[i] != 0)
			return false;
	return true;
}

void gr_search(t_gr *gr, int qboth) {
	//gr_print_all(gr);
	bool **choicearr = new bool*[qboth];
	int *choices = new int[qboth];
	gr->foundki = NONE;

	for (int i=gr->n; i >= qboth; i--) {
		choicearr[0] = (gr->adjarr + (i-2)*(i-1)/2); // pointer to subarray starting at node i
		choices[0] = i;
		gr_search_rec(gr, qboth-1, 1, choices, choicearr);
		if (gr->foundki != NONE) // if we didn't find an indep set or clique subgraph yet, keep looking
				return;
	}
	// if there is NO indep set or clique subgraph at this point, then this graph is a counterexample

	delete[] choicearr; choicearr = NULL;
	delete[] choices; choices = NULL;
}

void gr_search_rec(t_gr *gr, int qtogo, int qsofar, int *choices, bool **choicearr) {
	if (qtogo == 0) { // base case -- we chose a subset of qboth nodes, now test to see if it is a clique or not
		/*std::cout<<"choices: ";
		for (int i=0; i<qsofar; i++)
			std::cout<<choices[i]<<' ';
		std::cout<<"\n";*/
		bool indep = true, clique = true;
		for (int i = qsofar-1; i >= 1; i--) {
			for (int j=i; j < qsofar; j++) {
				if (choicearr[i-1][choices[j]] ) {
					indep = false;
					if (clique==false) return; // not a clique, not an independent set
				} else {
					clique = false;
					if (indep == false) return;
				}
			}
		}
		// went through all the edges -- if they are all 1 or all 0, then we have a clique or an indep
		gr->foundki = clique ? CLIQUE : INDEP;

	} else { // recursive case
		for (int i = choices[qsofar-1]-1; i >= qtogo; i--) {
			/*std::cout<<"i="<<i<<" choices: ";
			for (int i=0; i<qsofar; i++)
				std::cout<<choices[i]<<' ';
			std::cout<<"\n";*/
			choices[qsofar] = i;
			choicearr[qsofar] = (gr->adjarr + (i-2)*(i-1)/2); 
			gr_search_rec(gr, qtogo-1, qsofar+1, choices, choicearr); // recurse
			if (gr->foundki != NONE) // if we didn't find an indep set or clique subgraph yet, keep looking
				return;
		}
	}
}

void bigfind(int qboth, int lb_start) {
	t_gr gr;
	int n = lb_start;
	bool found_ub = false;

	// until we find an upper bound
	while (!found_ub) {

		// look through graphs of size n for a counterexample graph.  If all contain a clique/indep subgraph, then we found an ub
		gr_init(&gr, n);
		do {
			gr_search(&gr, qboth);
			gr_incr(&gr);
		} while (gr.foundki != NONE && !gr_allzero(&gr));

		if (gr.foundki != NONE) {
			found_ub = true;
			std::cout << "FOUND UB R(" << qboth<<','<<qboth << ")<=" << gr.n << std::endl;
			std::cout << "\t ------\n";
		} else {
			std::cout << "FOUND LB R(" << qboth<<','<<qboth << ")>" << gr.n << ' ';
			gr_print(&gr);
			std::cout << "\t" << gr.foundki << '\n';
		}
		gr_free(&gr);

		n++;
	}
}

void bigfind_multi(const int qboth, const int lb_start) {

	omp_set_num_threads(8);
	bool found_ub = false;
	bool n_lb = false;
	int n = lb_start;

	while (!found_ub)
	{
		n_lb = false;
		std::cout << "\n\n";
		#pragma omp parallel default(none) shared(n,qboth,n_lb,std::cout)
		{
			std::cout << "a"<< omp_get_thread_num() <<" ";
			t_gr gr;

			gr_init_given(&gr, n, omp_get_thread_num(), 3); // 2^3 = 8

			do {
				gr_search(&gr, qboth);
				gr_incr_ignore(&gr,3);
				// check to see if another thread found a lb bad graph
				// if so, stop work

				if (n_lb)
					break;
			} while (gr.foundki != NONE && !gr_allzero_ignore(&gr,3));

			if (gr.foundki == NONE) {
				// just one thread do this
				#pragma omp critical
					if (!n_lb) { // only run on first thread, even if others simulatneously found a bad graph
						std::cout << "f"<< omp_get_thread_num() <<"\n";
						n_lb = true; // tell others to stop; I found a bad graph
						std::cout << "FOUND LB R(" << qboth<<','<<qboth << ")>" << gr.n << ' ';
						gr_print(&gr);
						std::cout << "\t" << gr.foundki << '\n';
					}
			}

			gr_free(&gr);
		}
		// only one thread here (master thread 0)
		//std::cout << "d"<< omp_get_thread_num() <<" ";

		if (!n_lb) {
			// no thread found a bad graph, so all the graphs must have been tried among the threads
			found_ub = true;
			std::cout << "FOUND UB R(" << qboth<<','<<qboth << ")<=" << n << std::endl;
			std::cout << "\t ------\n";
		}
	
		n++;
		
	}
}

int test_main() {
	t_gr gr;

	gr_init(&gr, 3);
	/*gr.adjarr[0] = 1;
	gr.adjarr[2] = 1;
	gr_search(&gr, 3);
	std::cout<<gr.foundki;
	gr_print(&gr);*/

	// test t_gr functions
	/*gr_init(&gr,3);
	std::cout << gr_allzero(&gr) << " : ";
	gr_print(&gr);
	gr_incr(&gr);
	std::cout << gr_allzero(&gr) << " : ";
	gr_print(&gr);
	gr_incr(&gr);
	std::cout << gr_allzero(&gr) << " : ";
	gr_print(&gr);
	gr_incr(&gr);
	std::cout << gr_allzero(&gr) << " : ";
	gr_print(&gr);*/

	gr_free(&gr);

	/*#pragma omp parallel
	{
		int ID = omp_get_thread_num();
		std::cout << "hello world ID "<< ID <<"\n";
	}*/

	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	//_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

	//if (test_main() != 0)
		//return 1;

	bigfind_multi(4,3);
	//_CrtDumpMemoryLeaks();

	char c; std::cin >> c;
	return 0;
}

