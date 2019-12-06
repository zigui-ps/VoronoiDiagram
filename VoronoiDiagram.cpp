#include<vector>
#include<tuple>
#include<queue>
#include<cmath>
#include<algorithm>

using namespace std;

typedef pair<int,int> pii;
typedef pair<double, double> pdd;

const double EPS = 1e-9;
int dcmp(double x){ return x < -EPS? -1 : x > EPS ? 1 : 0; }

double operator / (pdd a,    pdd b){ return a.first * b.second - a.second * b.first; }
pdd    operator * (double b, pdd a){ return pdd(b * a.first, b * a.second); }
pdd    operator + (pdd a,    pdd b){ return pdd(a.first + b.first, a.second + b.second); }
pdd    operator - (pdd a,    pdd b){ return pdd(a.first - b.first, a.second - b.second); }

double sq(double x){ return x*x; }
double size(pdd p){ return hypot(p.first, p.second); }
double sz2(pdd p){ return sq(p.first) + sq(p.second); }
pdd r90(pdd p){ return pdd(-p.second, p.first); }

pdd line_intersect(pdd a, pdd b, pdd u, pdd v){ return u + (((a-u)/b) / (v/b))*v; }
pdd get_circumcenter(pdd p0, pdd p1, pdd p2){
	return line_intersect(0.5 * (p0+p1), r90(p0-p1), 0.5 * (p1+p2), r90(p1-p2));
}

// https://www.youtube.com/watch?v=h_vvP4ah6Ck
double parabola_intersect(pdd left, pdd right, double sweepline){
	if(dcmp(left.second - right.second) == 0) return (left.first + right.first) / 2;
	int sign = -1;
	if(left.second > right.second) swap(left, right), sign = 1;
	pdd v = line_intersect(left, right-left, pdd(0, sweepline), pdd(1, 0));
	double d1 = sz2(0.5 * (left+right) - v), d2 = sz2(0.5 * (left-right));
	return v.first + sign * sqrt(d1 - d2);
}

class Beachline{
	public:
		struct node{
			node(){}
			node(pdd point, int idx, int end):point(point), idx(idx), end(end), 
				link{0, 0}, par(0), prv(0), nxt(0) {}
			pdd point; int idx; int end;
			node *link[2], *par, *prv, *nxt;
		};
		node *root;
		double sweepline;

		Beachline() : sweepline(-1e10), root(NULL){ }
		inline int dir(node *x){ return x->par->link[0] != x; }

		//     p        n          p            n
		//    / \      / \        / \          / \
		//   n   d => a   p   or a   n   =>   p   d
		//  / \          / \        / \      / \
		// a   b        b   d      c   d    a   c

		void rotate(node *n){
			node *p = n->par;         int d = dir(n);
			p->link[d] = n->link[!d]; if(n->link[!d]) n->link[!d]->par = p;
			n->par = p->par;          if(p->par) p->par->link[dir(p)] = n;
			n->link[!d] = p;          p->par = n;
		}

		void splay(node *x, node *f = NULL){
			while(x->par != f){
				if(x->par->par == f);
				else if(dir(x) == dir(x->par)) rotate(x->par);
				else rotate(x);
				rotate(x);
			}
			if(f == NULL) root = x;
		}

		void insert(node *n, node *p, int d){
			splay(p); node* c = p->link[d];
			n->link[d] = c; if(c) c->par = n;
			p->link[d] = n; n->par = p;

			node *prv = !d?p->prv:p, *nxt = !d?p:p->nxt;
			n->prv = prv;   if(prv) prv->nxt = n;
			n->nxt = nxt;   if(nxt) nxt->prv = n;
		}

		void erase(node* n){
			node *prv = n->prv, *nxt = n->nxt;
			if(!prv && !nxt){ if(n == root) root = NULL; return; }
			n->prv = NULL;   if(prv) prv->nxt = nxt;
			n->nxt = NULL;   if(nxt) nxt->prv = prv;
			splay(n);
			if(!nxt){
				root->par = NULL; n->link[0] = NULL;
				root = prv;
			}
			else{
				splay(nxt, n);     node* c = n->link[0];
				nxt->link[0] = c;  c->par = nxt;         n->link[0] = NULL;
				n->link[1] = NULL; nxt->par = NULL;
				root = nxt;
			}
		}
		bool get_event(node* cur, double &next_sweep){
			if(!cur->prv || !cur->nxt) return false;
			pdd u = r90(cur->point - cur->prv->point);
			pdd v = r90(cur->nxt->point - cur->point);
			if(dcmp(u/v) != 1) return false;
			pdd p = get_circumcenter(cur->point, cur->prv->point, cur->nxt->point);
			next_sweep = p.second + size(p - cur->point);
			return true;
		}
		pair<node*, node*> find_beachline(double x){
			node* cur = root;
			pair<node*, node*> answer;
			while(cur){
				double left = cur->prv ? parabola_intersect(cur->prv->point, cur->point, sweepline) : -1e10;
				double right = cur->nxt ? parabola_intersect(cur->point, cur->nxt->point, sweepline) : 1e10;
				if(dcmp(x - left) == 0){ answer = make_pair(cur->prv, cur); break; }
				if(dcmp(right - x) == 0){ answer = make_pair(cur, cur->nxt); break; }
				if(dcmp(x - left) == 1 && dcmp(right - x) == 1){ answer = make_pair(cur, (node*)NULL); break; }
				if(dcmp(x - left) == -1) cur = cur->link[0];
				else cur = cur->link[1];
			}
			splay(cur);
			return answer;
		}
}; using BeachNode = Beachline::node;

BeachNode* arr;
int sz;
BeachNode* new_node(pdd point, int idx, int end){
	arr[sz] = BeachNode(point, idx, end);
	return arr + (sz++);
}

struct event{
	event(double sweep, int idx):type(0), sweep(sweep), idx(idx){}
	event(double sweep, BeachNode* cur):type(1), sweep(sweep), prv(cur->prv->idx), cur(cur), nxt(cur->nxt->idx){}
	int type, idx, prv, nxt;
	BeachNode* cur;
	double sweep;
	bool operator>(const event &l)const{ return sweep > l.sweep; }
};

void VoronoiDiagram(vector<pdd> &input, vector<pdd> &vertex, vector<pii> &edge, vector<pii> &area){
	auto write_edge = [&](int idx, int v){ 
		if(idx%2 == 0) edge[idx/2].first = v;
		else edge[idx/2].second = v; 
	};

	int n = input.size(), cnt = 0;
	Beachline beachline = Beachline();
	arr = new BeachNode[n*4]; sz = 0;

	priority_queue<event, vector<event>, greater<event>> events;
	sort(input.begin(), input.end(), [](const pdd &l, const pdd &r){
		return l.second != r.second ? l.second < r.second : l.first < r.first;
	});
	
	BeachNode* tmp = beachline.root = new_node(input[0], 0, -1);
	for(int i = 1; i < n; i++){
		if(dcmp(input[i].second - input[0].second) == 0){
			BeachNode* t2 = new_node(input[i], i, edge.size()*2 + 1);
			edge.emplace_back(-1, -1);
			area.emplace_back(i-1, i);
			beachline.insert(t2, tmp, 1);
			tmp = t2;
		}
		else events.emplace(input[i].second, i);
	}
	while(events.size()){
		event q = events.top(); events.pop();
		beachline.sweepline = q.sweep;
		if(q.type == 0){
			int idx = q.idx;
			pdd point = input[idx];
			BeachNode *cur, *nxt;
			tie(cur, nxt) = beachline.find_beachline(point.first);
			if(nxt){
				int v = vertex.size();
				write_edge(cur->end, v);
				vertex.push_back(get_circumcenter(cur->point, nxt->point, point));

				BeachNode* site = new_node(point, idx, edge.size()*2 + 1);
				beachline.insert(site, nxt, 0);
				edge.emplace_back(v, -1);
				area.emplace_back(idx, nxt->idx);

				cur->end = edge.size()*2 + 1;
				edge.emplace_back(v, -1);
				area.emplace_back(cur->idx, idx);
			}
			else{
				BeachNode* site = new_node(point, idx, edge.size()*2);
				BeachNode* cur2 = new_node(cur->point, cur->idx, edge.size()*2 + 1);
				beachline.insert(site, cur, 0);
				beachline.insert(cur2, site, 0);

				edge.emplace_back(-1, -1);
				area.emplace_back(cur->idx, idx);

				double next_sweep;
				if(beachline.get_event(cur, next_sweep)) events.emplace(next_sweep, cur);
				if(beachline.get_event(cur2, next_sweep)) events.emplace(next_sweep, cur2);
			}
		}
		else{
			BeachNode* cur = q.cur, *prv = cur->prv, *nxt = cur->nxt;
			if(!prv || !nxt || prv->idx != q.prv || nxt->idx != q.nxt) continue;

			int v = vertex.size();
			write_edge(prv->end, v);
			write_edge(cur->end, v);
			vertex.push_back(get_circumcenter(prv->point, nxt->point, cur->point));

			prv->end = edge.size()*2 + 1;
			edge.emplace_back(v, -1);
			area.emplace_back(prv->idx, nxt->idx);

			beachline.erase(cur);
			double next_sweep;
			if(beachline.get_event(prv, next_sweep)) events.emplace(next_sweep, prv);
			if(beachline.get_event(nxt, next_sweep)) events.emplace(next_sweep, nxt);
		}
	}
	delete arr;
}
