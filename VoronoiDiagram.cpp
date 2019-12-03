#include<vector>

using namespace std;

typedef pair<int,int> pii;
typedef pair<double, double> pdd;

class Beachline{

};

struct event{
	event(int type, int point, double sweep):type(type), point(point), sweep(sweep){}
	int type, point;
	double sweep;
	bool operator>(events &l)const{ return sweep > l.sweep; }
};

void VoronoiDiagram(vector<pdd> input, vector<pdd> vertex, vector<pii> edge, vector<pii> area){
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
