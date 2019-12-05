#include <GL/glut.h>
#include<algorithm>
#include<vector>
#include<algorithm>
#include <stdio.h>

typedef std::pair<double, double> pdd;
typedef std::pair<int, int> pii;

static double dx = 0, dy = 0, scale = 1e-2;
std::vector<pdd> vertex;
std::vector<pii> edge;
std::vector<pii> area;

void VoronoiDiagram(std::vector<pdd> input, std::vector<pdd> &vertex, std::vector<pii> &edge, std::vector<pii> &area);

void display() {
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	printf("%lf %lf %lf\n", dx, dy, scale);
	glPushMatrix();

	glTranslated(dx, -dy, 0);
	glScaled(scale, scale, 1);

	glColor3f(1.0f, 0.0f, 0.0f); // Red
	for(int i = 0; i < edge.size(); i++){
		pdd u = vertex[edge[i].first];
		pdd v = vertex[edge[i].second];
		
		glBegin(GL_LINES);
		glVertex2d(u.first, u.second);
		glVertex2d(v.first, v.second);
		glEnd();
	}

	glPopMatrix();
	glFlush();
}

int Ox = 0, Oy = 0;
void mouse(int button, int state, int x, int y){
	printf("%d %d %d %d\n", button, state, x, y);
	if(state == GLUT_UP) return;
	if(button == 0) Ox = x, Oy = y;
	else if(button == 3) scale *= 1.1;
	else if(button == 4) scale /= 1.1;
}

void motion(int x, int y){
	dx += (x - Ox) / 1080.0 * 2; Ox = x;
	dy += (y - Oy) / 1080.0 * 2; Oy = y;
}

void nextTimestep(int time){
	glutTimerFunc(1000.0 / 30, nextTimestep, 0);
	glutPostRedisplay();
}

int main(int argc, char** argv) {
	if(argc != 2) return !printf("usage : ./voronoi (input file)\n");
	freopen(argv[1], "r", stdin);

	int N;
	std::vector<pdd> input;
	scanf("%d", &N);
	for(int i = 0; i < N; i++){
		double a, b;
		scanf("%lf%lf", &a, &b);
		input.emplace_back(a, b);
	}
	VoronoiDiagram(input, vertex, edge, area);

	glutInit(&argc, argv);
	glutInitWindowSize(1080, 1080);
	glutCreateWindow("OpenGL Setup Test");
	glutDisplayFunc(display);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutTimerFunc(1000.0 / 30, nextTimestep, 0);
	glutMainLoop();
	return 0;
}
