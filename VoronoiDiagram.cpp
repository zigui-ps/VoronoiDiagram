#include<vector>

using namespace std;

typedef pair<int,int> pii;
typedef pair<double, double> pdd;

class Beachline{
	struct node{
		pii point; int idx;
		node *link[2], *par;
		node *prv, *nxt;
	};
	node *arr;
	double sweepline;

	Beachline(int sz) : sweepline(-1e9){
		arr = new node[sz];
		for(int i = 0; i < sz; i++){
			arr[i].link[0] = arr[i].link[1] = NULL;
			arr[i].par = arr[i].prv = arr[i].nxt = NULL;
		}
	}
	~Beachline(){ delete arr; }
	node* operator[](int idx){ return arr[node]; }
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

	void splay(node *x, node *f){
		assert(x != f);
		while(x->par != f){
			if( x->par->par == f );
			else if(dir(x) == dir(x->par)) rotate(x->par);
			else rotate(x);
			rotate(x);
		}
	}
	
	void insert(node *n, node *p, int d){
		splay(p, NULL); node* c = p->link[d];
		n->link[d] = c; if(c) c->par = n;
		p->link[d] = n; n->par = p;

		node *prv = !d?p->prv:p, *nxt = !d?p:p->nxt;
		n->prv = prv;   if(prv) prv->nxt = n;
		n->nxt = nxt;   if(nxt) nxt->prv = n;
	}

	void connect(node *n, node *p, int d){
		assert(!p->link[d]); assert(!n->par);
		p->link[d] = n;      n->par = p;
	}

	void erase(node* n){
		node *prv = n->prv, *nxt = n->nxt;
		if(!prv && !nxt) return;
		n->prv = NULL;   if(prv) prv->nxt = nxt;
		n->nxt = NULL;   if(nxt) nxt->prv = prv;
		splay(n, NULL);
		if(!nxt) n->link[0]->par = NULL, n->link[0] = NULL;
		else{
			splay(nxt, n);     node* c = n->link[0];
			nxt->link[0] = c;  c->par = nxt;         n->link[0] = NULL;
			n->link[1] = NULL; nxt->par = NULL;
		}
	}
}; using BeachNode = Beachline::node;

struct event{
	event(int type, int point, double sweep):type(type), point(point), sweep(sweep){}
	int type, point;
	double sweep;
	bool operator>(events &l)const{ return sweep > l.sweep; }
};

void VoronoiDiagram(vector<pdd> input, vector<pdd> &vertex, vector<pii> &edge, vector<pii> &area){
	Beachline beachline;
	double sweepline = -1e10;
	priority_queue<event, vector<event>, greater<event>> events;
	for(int i = 0; i < input.size(); i++) events.emplace_back(0, i, input[i].second);
	while(events.size()){
		event q = events.top(); events.pop();
		int point = q.u;
		if(q.type == 0){
			int idx = beachline.find_lowest_x(input[point].first);
			int left_idx = , right_idx = ;
			double left_event = , right_event = ;
			if(left_event < 1e-9 || right_event < 1e-9){
				int u, v;
				if(left_event < 1e-9) u = left_idx, v = idx;
				if(right_event < 1e-9) u = idx, v = right_idx;
				// TODO
			}
			edge.emplace_back(-1, -1);
			area.emplace_back(idx, point); // TODO order
			beachline.insert(point, &edge.back().first, idx);
			beachline.insert(idx, &edge.back().second, point);
		}
		else{
			auto node = nodeList[point], left = node, right = node; --left, ++right;
			pdd v = intersect(left, right);
			int idx = vertex.size(); vertex.push_back(v);
			*left->remain = idx;
			edge.emplace_back(idx, -1);
			area.emplace_back(left->point, right->point); // TODO
			left->remain = &edge.back().second;
			beachline.erase(node);
		}
	}
}
