#include "contur.h" 
#define INF 1000000

using namespace std;

List Contur::CreateMST(List g)
{
	
	int n=g.size();
	
	List res(n);
	vector<int> min_e (n, INF);
	vector<int> sel_e (n, -1);
	min_e[0] = 0;
	set < pair<int,int> > q;
	q.insert (make_pair (0, 0));
	for (int i=0; i<n; ++i) {
		if (q.empty()) {
			cout << "No MST!";
			return res;
		}
		int v = q.begin()->second;
		q.erase (q.begin());
 /*
		if (sel_e[v] != -1){
			wt=
			for(i=0;i<g[v].size();i++) if (g[v].first=sel_e[v]) int w=
			res[v].push_back(pair(sel_e[v],g[v][sel_e[v]].second));
			cout << v << " " << sel_e[v] << endl;
		};
		*/
		for (size_t j=0; j<g[v].size(); ++j) {
			if (sel_e[v] != -1 && g[v][j].first == sel_e[v] ){
			res[v].push_back(pair<int,int>(sel_e[v],g[v][j].second));
			//cout << v << " " << sel_e[v] << endl;
		};
			
			
			int to = g[v][j].first,
				cost = g[v][j].second;
			if (cost < min_e[to]) {
				q.erase (make_pair (min_e[to], to));
				min_e[to] = cost;
				sel_e[to] = v;
				q.insert (make_pair (min_e[to], to));
			}
		}
	}
	cout << res.size();
	return res;
};


vector<int> Contur::Find_Shortest_Paths(List g,int start,int end)
{
	int n=g.size();
	int s=start;
	//vector<int> d (n, INF),  p (n);
	vector<int> d (n, 0),  p (n);
	d[s] = INF;
	set < pair<int,int> > q;
	q.insert (make_pair (d[s], s));
	while (!q.empty()) {
		int v = q.begin()->second;
		q.erase (q.begin());
 
		for (size_t j=0; j<g[v].size(); ++j) {
			int to = g[v][j].first,
				len = g[v][j].second;
			//if (d[v] + len < d[to]) {
			if (d[v] + len > d[to]) {
				q.erase (make_pair (d[to], to));
				d[to] = d[v] + len;
				p[to] = v;
				q.insert (make_pair (d[to], to));
			}
		}
	}
	vector<int> path;
	for (int v=end; v!=s; v=p[v]) 	path.push_back (v);
	path.push_back (s);
	reverse (path.begin(), path.end());
	return path;

}