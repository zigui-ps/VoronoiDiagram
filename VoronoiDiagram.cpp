#include<vector>
#include<tuple>
#include<queue>
#include<cmath>
#include<algorithm>
#include<assert.h>

using namespace std;

typedef pair<int,int> pii;
typedef pair<double, double> pdd;

const double EPS = 1e-9;
int dcmp(double x){ return x < -EPS? -1 : x > EPS ? 1 : 0; }

double operator/(pdd a, pdd b){ return a.first * b.second - a.second * b.first; }
pdd operator*(double b, pdd a){ return pdd(b * a.first, b * a.second); }
pdd operator+(pdd a, pdd b){ return pdd(a.first + b.first, a.second + b.second); }
pdd operator-(pdd a, pdd b){ return pdd(a.first - b.first, a.second - b.second); }
double size(pdd p){ return hypot(p.first, p.second); }
pdd r90(pdd p){ return pdd(-p.first, p.second); }

pdd intersect(pdd a, pdd b, pdd u, pdd v){
	assert(dcmp(v/b) != 0);
	double d = ((a-u)/b) / (v/b);
	return u + d*v;
}

pdd get_circumcenter(pdd p0, pdd p1, pdd p2){
	pdd a = 0.5 * (p0+p1), b = r90(p0-p1);
	pdd u = 0.5 * (p0+p1), v = r90(p0-p1);
	return intersect(a, b, u, v);
}

class Beachline{
	public:
	struct node{
		node(){}
		node(pdd point, int idx, int *end):point(point), idx(idx), end(end),
			link{0, 0}, par(0), prv(0), nxt(0) {}
		pdd point; int idx; int *end;
		node *link[2], *par;
		node *prv, *nxt;
	};
	node *root;
	double sweepline;

	Beachline() : sweepline(-1e9), root(NULL){ }
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
		assert(x != f);
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
		if(!nxt) n->link[0]->par = NULL, n->link[0] = NULL;
		else{
			splay(nxt, n);     node* c = n->link[0];
			nxt->link[0] = c;  c->par = nxt;         n->link[0] = NULL;
			n->link[1] = NULL; nxt->par = NULL;
		}
	}
	bool empty(){ return root == NULL; }

	bool get_event(node* cur, double &next_sweep){
		if(!cur->prv || !cur->nxt) return false;
		pdd u = r90(cur->point - cur->prv->point);
		pdd v = r90(cur->nxt->point - cur->point);
		if(dcmp(u/v) == 1) return false;
		pdd p = get_circumcenter(cur->point, cur->prv->point, cur->nxt->point);
		next_sweep = p.second + size(p - cur->point);
		return true;
	}

	double intersect(pdd left, pdd right){
		if(dcmp(left.second - right.second) == 0) return (left.first + right.first) / 2;
		assert(false);
	}

	pair<node*, node*> find_lowest(double x){
		node* cur = root;
		while(cur){
			double left = cur->prv ? intersect(cur->prv->point, cur->point) : -1e10;
			double right = cur->nxt ? intersect(cur->point, cur->nxt->point) : 1e10;
			if(dcmp(x - left) == 0) return make_pair(cur->prv, cur);
			if(dcmp(right - x) == 0) return make_pair(cur, cur->nxt);
			if(dcmp(x - left) == 1 && dcmp(right - x) == -1) return make_pair(cur, (node*)NULL);
			if(dcmp(x - left) == -1) cur = cur->link[0];
			else cur = cur->link[1];
		}
		assert(false);
	}
}; using BeachNode = Beachline::node;

BeachNode* arr;
int sz;
BeachNode* new_node(pdd point, int idx, int* end){
	arr[sz] = BeachNode(point, idx, end);
	return arr + (sz++);
}

struct event{
	event(int type, int idx, double sweep):type(type), idx(idx), sweep(sweep){}
	int type, idx;
	double sweep;
	bool operator>(const event &l)const{ return sweep > l.sweep; }
};

void VoronoiDiagram(vector<pdd> input, vector<pdd> &vertex, vector<pii> &edge, vector<pii> &area){
	int n = input.size(), cnt = 0;
	Beachline beachline = Beachline();
	arr = new BeachNode[n*4];

	priority_queue<event, vector<event>, greater<event>> events;
	sort(input.begin(), input.end(), [](const pdd &l, const pdd &r){
		return l.second != r.second ? l.second < r.second : l.first < r.first;
	});
	beachline.root = new_node(input[0], 0, NULL);
	for(int i = 1; i < n; i++) events.emplace(0, i, input[i].second);
	while(events.size()){
		event q = events.top(); events.pop();
		int idx = q.idx; beachline.sweepline = q.sweep;
		if(q.type == 0){
			pdd point = input[idx];
			BeachNode *cur, *nxt;
			tie(cur, nxt) = beachline.find_lowest(point.first);
			if(nxt){
				int v = vertex.size();
				*(cur->end) = v;
				vertex.push_back(get_circumcenter(cur->point, nxt->point, point));

				edge.emplace_back(v, -1);
				area.emplace_back(idx, cur->idx);
				cur->end = &edge.back().second;
				
				edge.emplace_back(v, -1);
				area.emplace_back(nxt->idx, idx);
				cur->end = &edge.back().second;
			}
			else{
				BeachNode* site = new_node(point, idx, NULL);
				BeachNode* cur2 = new_node(cur->point, cur->idx, NULL);
				beachline.insert(site, cur, 0);
				beachline.insert(cur2, site, 0);

				edge.emplace_back(-1, -1);
				area.emplace_back(idx, cur->idx);
				site->end = &edge.back().first;
				cur2->end = &edge.back().second;

				double next_sweep;
				if(beachline.get_event(cur, next_sweep)) events.emplace(1, cur-arr, next_sweep);
				if(beachline.get_event(cur2, next_sweep)) events.emplace(1, cur2-arr, next_sweep);
			}
		}
		else{
			BeachNode* cur = arr + idx;
			beachline.splay(cur);
			if(cur != beachline.root) continue;

			BeachNode* prv = cur->prv, *nxt = cur->nxt;

			int v = vertex.size();
			*(prv->end) = *(cur->end) = v;
			vertex.push_back(get_circumcenter(prv->point, nxt->point, cur->point));

			edge.emplace_back(v, -1);
			area.emplace_back(nxt->idx, prv->idx);
			prv->end = &edge.back().second;
			
			beachline.erase(cur);
			double next_sweep;
			if(beachline.get_event(prv, next_sweep)) events.emplace(1, prv-arr, next_sweep);
			if(beachline.get_event(nxt, next_sweep)) events.emplace(1, nxt-arr, next_sweep);
		}
	}
	delete arr;
}
