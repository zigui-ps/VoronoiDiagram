#include <GL/glut.h>
#include<algorithm>
#include<vector>
#include<algorithm>
#include <stdio.h>

typedef std::pair<double, double> pdd;
typedef std::pair<int, int> pii;

static double dx = 0, dy = 0, scale = 1e-8;
std::vector<pdd> input;
std::vector<pdd> vertex;
std::vector<pii> edge;
std::vector<pii> area;

void VoronoiDiagram(std::vector<pdd> &input, std::vector<pdd> &vertex, std::vector<pii> &edge, std::vector<pii> &area);

extern double operator / (pdd a,    pdd b); //{ return a.first * b.second - a.second * b.first; }
extern pdd    operator * (double b, pdd a); //{ return pdd(b * a.first, b * a.second); }
extern pdd    operator + (pdd a,    pdd b); //{ return pdd(a.first + b.first, a.second + b.second); }
extern pdd    operator - (pdd a,    pdd b); //{ return pdd(a.first - b.first, a.second - b.second); }
extern pdd r90(pdd x); //{ return pdd(-x.second, x.first); }

void display() {
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glPushMatrix();

	glScaled(scale, scale, 1);
	glTranslated(dx, -dy, 0);

	glColor3f(1.0f, 0.0f, 0.0f); // Red

	for(int i = 0; i < edge.size(); i++){
		pdd d = r90(input[area[i].second] - input[area[i].first]), u, v;
		pdd m = 0.5 * (input[area[i].second] + input[area[i].first]);
		if(edge[i] == pii(-1, -1)){
			u = m - 100 * d;
			v = m + 100 * d;
		}
		else if(edge[i].first == -1){
			v = vertex[edge[i].second];
			u = v - 100 * d;
		}
		else if(edge[i].second == -1){
			u = vertex[edge[i].first];
			v = u + 100 * d;
		}
		else{
			u = vertex[edge[i].first];
			v = vertex[edge[i].second];
		}
		
		glBegin(GL_LINES);
		glVertex2d(u.first, u.second);
		glVertex2d(v.first, v.second);
		glEnd();
	}
	glColor3f(0.0f, 1.0f, 0.0f);

	for(int i = 0; i < vertex.size(); i++){
		double x = vertex[i].first, y = vertex[i].second;
		glBegin(GL_POLYGON);
		double R = 1. / 1080 * 2;
		glVertex2d(x - R / scale, y);
		glVertex2d(x, y + R / scale);
		glVertex2d(x + R / scale, y);
		glVertex2d(x, y - R / scale);
		glEnd();
	}
	glColor3f(0.0f, 0.0f, 1.0f);

	for(int i = 0; i < input.size(); i++){
		double x = input[i].first, y = input[i].second;
		glBegin(GL_POLYGON);
		double R = 1. / 1080 * 2;
		glVertex2d(x - R / scale, y);
		glVertex2d(x, y + R / scale);
		glVertex2d(x + R / scale, y);
		glVertex2d(x, y - R / scale);
		glEnd();
	}

	glPopMatrix();
	glutSwapBuffers();
}

int Ox = 0, Oy = 0;
void mouse(int button, int state, int x, int y){
	if(state == GLUT_UP) return;
	if(button == 0) Ox = x, Oy = y;
	else if(button == 3) scale *= 1.1;
	else if(button == 4) scale /= 1.1;
}

void motion(int x, int y){
	dx += (x - Ox) / 1080.0 * 2 / scale; Ox = x;
	dy += (y - Oy) / 1080.0 * 2 / scale; Oy = y;
}

void nextTimestep(int time){
	glutTimerFunc(1000.0 / 30, nextTimestep, 0);
	glutPostRedisplay();
}

int main(int argc, char** argv) {
	if(argc != 2) return !printf("usage : ./voronoi (input file)\n");
	freopen(argv[1], "r", stdin);

	int N;
	double lx = 1e10, rx = -1e10, ly = 1e10, ry = -1e10;
	scanf("%d", &N);
	for(int i = 0; i < N; i++){
		double a, b;
		scanf("%lf%lf", &a, &b);
		input.emplace_back(a, b);
		lx = std::min(lx, a);
		rx = std::max(rx, a);
		ly = std::min(ly, b);
		ry = std::max(ry, b);
	}
	dx = -(rx+lx)/2, dy = (ry+ly)/2;
	scale = 1. / std::max(rx-lx, ry-ly);
	printf("start\n");
	VoronoiDiagram(input, vertex, edge, area);
	printf("end\n");
	return 0;

/*
	printf("vertex : \n");
	for(pdd c : vertex) printf("%lf,%lf\n", c.first, c.second);
	printf("edge : \n");
	for(int i = 0; i < edge.size(); i++) printf("%d %d, %d %d\n", edge[i].first, edge[i].second, area[i].first, area[i].second);
*/
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE);
	glutInitWindowSize(1080, 1080);
	glutCreateWindow("OpenGL Setup Test");
	glutDisplayFunc(display);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutTimerFunc(1000.0 / 30, nextTimestep, 0);
	glutMainLoop();
	return 0;
}
