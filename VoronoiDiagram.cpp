#include<vector>
#include<tuple>
#include<queue>
#include<cmath>
#include<algorithm>
#include <GL/glut.h>

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
	/*
	if(dcmp(left.second - right.second) == 0) return (left.first + right.first) / 2.0; /*/
	auto f2 = [](pdd left, pdd right, double sweepline){
		int sign = left.first < right.first ? 1 : -1;
		pdd m = 0.5 * (left+right);
		pdd v = line_intersect(m, r90(right-left), pdd(0, sweepline), pdd(1, 0));
		pdd w = line_intersect(m, r90(left-v), v, left-v);
		double l1 = size(v-w), l2 = sqrt(sq(sweepline-m.second) - sz2(m-w)), l3 = size(left-v);
		return v.first + (m.first - v.first) * l3 / (l1 + sign * l2);
	};
	if(fabs(left.second - right.second) < fabs(left.first - right.first) * EPS) return f2(left, right, sweepline);// */
	int sign = left.second < right.second ? -1 : 1;
	pdd v = line_intersect(left, right-left, pdd(0, sweepline), pdd(1, 0));
	double d1 = sz2(0.5 * (left+right) - v), d2 = sz2(0.5 * (left-right));
	return v.first + sign * sqrt(d1 - d2);
}

class Beachline{
	public:
		struct node{
			node(){}
			node(pdd point, int idx):point(point), idx(idx), end(0), 
				link{0, 0}, par(0), prv(0), nxt(0) {}
			pdd point; int idx; int end;
			node *link[2], *par, *prv, *nxt;
		};
		node *root;
		double sweepline;

		Beachline() : sweepline(-1e20), root(NULL){ }
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
		node* find_beachline(double x){
			node* cur = root;
			while(cur){
				double left = cur->prv ? parabola_intersect(cur->prv->point, cur->point, sweepline) : -1e30;
				double right = cur->nxt ? parabola_intersect(cur->point, cur->nxt->point, sweepline) : 1e30;
				if(left <= x && x <= right){ splay(cur); return cur; }
				cur = cur->link[x > right];
			}
		}
}; using BeachNode = Beachline::node;

static BeachNode* arr;
static int sz;
static BeachNode* new_node(pdd point, int idx){
	arr[sz] = BeachNode(point, idx);
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

static priority_queue<event, vector<event>, greater<event>> events;
static Beachline beachline;
void VoronoiDiagram(vector<pdd> &input, vector<pdd> &vertex, vector<pii> &edge, vector<pii> &area){
	while(events.size()) events.pop();
	beachline = Beachline();
	auto add_edge = [&](int u, int v, int a, int b, BeachNode* c1, BeachNode* c2){
		if(c1) c1->end = edge.size()*2;
		if(c2) c2->end = edge.size()*2 + 1;
		edge.emplace_back(u, v);
		area.emplace_back(a, b);
	};
	auto write_edge = [&](int idx, int v){ idx%2 == 0 ? edge[idx/2].first = v : edge[idx/2].second = v; };
	auto add_event = [&](BeachNode* cur){ double nxt; if(beachline.get_event(cur, nxt)) events.emplace(nxt, cur); };

	int n = input.size(), cnt = 0;
	arr = new BeachNode[n*4]; sz = 0;
	sort(input.begin(), input.end(), [](const pdd &l, const pdd &r){
			return l.second != r.second ? l.second < r.second : l.first < r.first;
			});

	BeachNode* tmp = beachline.root = new_node(input[0], 0), *t2;
	for(int i = 1; i < n; i++){
		if(dcmp(input[i].second - input[0].second) == 0){
			add_edge(-1, -1, i-1, i, 0, tmp);
			beachline.insert(t2 = new_node(input[i], i), tmp, 1);
			tmp = t2;
		}
		else events.emplace(input[i].second, i);
	}
	while(events.size()){
		event q = events.top(); events.pop();
		BeachNode *prv, *cur, *nxt, *site;
		int v = vertex.size(), idx = q.idx;
		beachline.sweepline = q.sweep;
		if(q.type == 0){
			pdd point = input[idx];
			cur = beachline.find_beachline(point.first);
			beachline.insert(site = new_node(point, idx), cur, 0);
			beachline.insert(prv = new_node(cur->point, cur->idx), site, 0);
			add_edge(-1, -1, cur->idx, idx, site, prv);
			add_event(prv); add_event(cur);
		}
		else{
			cur = q.cur, prv = cur->prv, nxt = cur->nxt;
			if(!prv || !nxt || prv->idx != q.prv || nxt->idx != q.nxt) continue;
			vertex.push_back(get_circumcenter(prv->point, nxt->point, cur->point));
			write_edge(prv->end, v); write_edge(cur->end, v);
			add_edge(v, -1, prv->idx, nxt->idx, 0, prv);
			beachline.erase(cur);
			add_event(prv); add_event(nxt);
		}
	}
	delete arr;
}

void VoronoiDiagram_init(vector<pdd> &input, vector<pdd> &vertex, vector<pii> &edge, vector<pii> &area){
	auto add_edge = [&](int u, int v, int a, int b, BeachNode* c1, BeachNode* c2){
		if(c1) c1->end = edge.size()*2;
		if(c2) c2->end = edge.size()*2 + 1;
		edge.emplace_back(u, v);
		area.emplace_back(a, b);
	};
	while(events.size()) events.pop();
	beachline = Beachline();
	
	int n = input.size(), cnt = 0;
	if(arr) delete arr;
	arr = new BeachNode[n*4]; sz = 0;
	sort(input.begin(), input.end(), [](const pdd &l, const pdd &r){
			return l.second != r.second ? l.second < r.second : l.first < r.first;
			});

	BeachNode* tmp = beachline.root = new_node(input[0], 0), *t2;
	for(int i = 1; i < n; i++){
		if(dcmp(input[i].second - input[0].second) == 0){
			add_edge(-1, -1, i-1, i, 0, tmp);
			beachline.insert(t2 = new_node(input[i], i), tmp, 1);
			tmp = t2;
		}
		else events.emplace(input[i].second, i);
	}
}

bool VoronoiDiagram_step(vector<pdd> &input, vector<pdd> &vertex, vector<pii> &edge, vector<pii> &area){
	auto add_edge = [&](int u, int v, int a, int b, BeachNode* c1, BeachNode* c2){
		if(c1) c1->end = edge.size()*2;
		if(c2) c2->end = edge.size()*2 + 1;
		edge.emplace_back(u, v);
		area.emplace_back(a, b);
	};
	auto write_edge = [&](int idx, int v){ idx%2 == 0 ? edge[idx/2].first = v : edge[idx/2].second = v; };
	auto add_event = [&](BeachNode* cur){ double nxt; if(beachline.get_event(cur, nxt)) events.emplace(nxt, cur); };

	while(events.size()){
		event q = events.top(); events.pop();
		BeachNode *prv, *cur, *nxt, *site;
		int v = vertex.size(), idx = q.idx;
		beachline.sweepline = q.sweep;
		if(q.type == 0){
			pdd point = input[idx];
			cur = beachline.find_beachline(point.first);
			beachline.insert(site = new_node(point, idx), cur, 0);
			beachline.insert(prv = new_node(cur->point, cur->idx), site, 0);
			add_edge(-1, -1, cur->idx, idx, site, prv);
			add_event(prv); add_event(cur);
		}
		else{
			cur = q.cur, prv = cur->prv, nxt = cur->nxt;
			if(!prv || !nxt || prv->idx != q.prv || nxt->idx != q.nxt) continue;
			vertex.push_back(get_circumcenter(prv->point, nxt->point, cur->point));
			write_edge(prv->end, v); write_edge(cur->end, v);
			add_edge(v, -1, prv->idx, nxt->idx, 0, prv);
			beachline.erase(cur);
			add_event(prv); add_event(nxt);
		}
		break;
	}
	return events.empty();
}

void draw_beachline(double st, double en, double scale){
	double yd = beachline.sweepline;
	glColor3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex2d(st, beachline.sweepline);
	glVertex2d(en, beachline.sweepline);

	glColor3f(0.0f, 0.0f, 1.0f);
	for(int i = 0; i <= 1080; i++){
		double x = st + (en-st)/1080.0 * i;
		BeachNode *cur = beachline.find_beachline(x);
		double xf = cur->point.first, yf = cur->point.second;
		double y;
		if(dcmp(yf-yd) == 0) y = yd;
		else y = sq(x-xf) / 2.0 / (yf-yd) + (yf+yd)/2;

		int c2 = 1;
		if(i != 0 && i != 1080) c2 = 2;
		for(int i = 0; i < c2; i++) glVertex2d(x, y);
	}

	BeachNode* cur = beachline.root;
	while(cur->prv) cur = cur->prv;
	do{
		if(dcmp(cur->point.second - beachline.sweepline) == 0){
			double xf = cur->nxt->point.first, yf = cur->nxt->point.second;
			double x = cur->point.first;
			double y = sq(x-xf) / 2.0 / (yf - beachline.sweepline) + (yf+yd)/2;
			glVertex2d(x, y);
			glVertex2d(x, beachline.sweepline);
		}
		cur = cur->nxt;
	}while(cur);
	glEnd();
}

void draw_event(vector<pdd> &input, double st, double en, double scale){
	priority_queue<event, vector<event>, greater<event>> tmp;
	glBegin(GL_LINES);
	glColor3f(0.0f, 1.0f, 0.0f);
	while(events.size()){
		auto p = events.top(); events.pop();
		tmp.push(p);
		glVertex2d(st, p.sweep);
		glVertex2d(en, p.sweep);
	}
	glEnd();
	events.swap(tmp);
}

bool is_end(){ return events.empty(); }
