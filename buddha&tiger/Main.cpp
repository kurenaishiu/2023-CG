#include "./Library/SOIL/include/SOIL.h"
#include "./Library/glut/glut.h"
#include "./Library/stdc++.h"

#pragma comment( lib, "./Library/glut/glut32.lib" )
#pragma comment( lib, "./Library/SOIL/lib/SOIL.lib" )
using namespace std;
float angleX = 0, angleY = 0;
float lastX, lastY;
bool Rotate = false;
float maxDimension = 1.0;

// Structure to hold vertex data
struct Vertex {
    float x, y, z;
    Vertex(float x, float y, float z) :x(x), y(y), z(z) {}
};

// Structure to hold face data
struct Face {
    int v[3];
    int t[3];
};
struct Texture {
    float t1, t2;
};
struct Material {
    float ambient[3],diffuse[3],specular[3];
    float illum = 4.60013e-304;
    float Ns = 8;
    string map_Kd;
    GLuint Texture;
};
vector<Vertex> vertices1,vertices2;
vector<Face> faces1,faces2;
vector<Texture>texture1,texture2;
Material material1,material2;
float lightX = 45, lightY = 45;
const float radius = 3.0 * sqrt(2);
int sel = 1;
void loadOBJ(const char* filename) {
    vector<Vertex>*vertices;
    vector<Face> *faces;
    vector<Texture>*texture;
    if (sel == 1) {
        vertices = &vertices1;
        faces = &faces1;
        texture=&texture1;
    }
    else {
        vertices = &vertices2;
        faces = &faces2;
        texture = &texture2;
    }
    
    
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open the OBJ file." << std::endl;
        exit(1);
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string token;
        iss >> token;

        if (token == "v") {
            Vertex vertex(0, 0, 0);
            iss >> vertex.x >> vertex.y >> vertex.z;
            vertices->push_back(vertex);
            maxDimension = max(maxDimension, max(abs(vertex.x), max(abs(vertex.y), abs(vertex.z))));
        }
        else if (token == "vt") {
            Texture t;
            iss >> t.t1 >> t.t2;
            texture->push_back(t);
        }
        else if (token == "g") {
            string trash; iss >> trash;
        }
        else if (token == "f") {
            Face face;
            string s;
            for (int j = 0; j < 3; ++j) {
                iss >> s;
                stringstream ss(s);
                string token;
                getline(ss, token, '/');
                face.v[j] = stoi(token);
                getline(ss, token, '/');
                face.t[j] = stoi(token);
                //getline(ss, token, '/');
            }
            faces->push_back(face);
        }
    }

    file.close();
}
void loadMTL(const char* filename) {
    Material *material;
    if (sel == 1) {
        material = &material1;
    }
    else {
        material = &material2;
    }
    std::ifstream file(filename);
    std::string line;
    std::string currMaterial;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "newmtl") {
            iss >> currMaterial;
        }
        else if (prefix == "Ka") {
            iss >> material->ambient[0] >> material->ambient[1] >> material->ambient[2];
        }
        else if (prefix == "Kd") {
            iss >> material->diffuse[0] >> material->diffuse[1] >> material->diffuse[2];
        }
        else if (prefix == "Ks") {
            iss >> material->specular[0] >> material->specular[1] >> material->specular[2];
        }
        else if (prefix == "Ns") {
            iss >> material->Ns;
        }
        else if (prefix == "illum") {
            iss >> material->illum;
        }
        else if (prefix == "map_Kd") {
            iss >> material->map_Kd;
            
            //material->Texture = LoadTexture(SOIL_load_image( "mkkm.jpg", &width, &height, &channels, SOIL_LOAD_AUTO));
        }
    }
}
int width, height, channels;
GLuint LoadTexture(unsigned char* image) {
    GLuint texture = SOIL_create_OGL_texture(image, width, height, channels, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
    SOIL_free_image_data(image);
    if (!texture) {
        std::cerr << "Error loading texture: " << SOIL_last_result() << std::endl;
    }
    return texture;
}
void init() {
    sel = 1;
    loadOBJ("tiger.obj");
    loadMTL("tiger.mtl");
    sel = 0;
    loadOBJ("buddha.obj");
    loadMTL("buddha.mtl");
    sel = 1;
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glEnable(GL_TEXTURE_2D);
    material1.Texture = LoadTexture(SOIL_load_image("./tiger-atlas.jpg", &width, &height, &channels, SOIL_LOAD_AUTO));
    material2.Texture = LoadTexture(SOIL_load_image("./buddha-atlas.jpg", &width, &height, &channels, SOIL_LOAD_AUTO));
   

}
Vertex computeFaceNormal(const Vertex& v1, const Vertex& v2, const Vertex& v3) {
    Vertex vec1(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
    Vertex vec2(v1.x - v3.x, v1.y - v3.y, v1.z - v3.z);
    float X = vec1.y * vec2.z - vec1.z * vec2.y;
    float Y = -(vec1.x * vec2.z - vec1.z * vec2.x);
    float Z = vec1.x * vec2.y - vec1.y * vec2.x;
    float Q = sqrt(X * X + Y * Y + Z * Z);
    return Vertex(X / Q, Y / Q, Z / Q);
}
void display() {
    Material *material;
    vector<Face>*faces;
    vector<Vertex>*vertices;
    vector<Texture>*texture;
    if (sel == 1) {
        material = &material1;
        faces = &faces1;
        vertices = &vertices1;
        texture = &texture1;
    }
    else {
        material = &material2;
        faces = &faces2;
        vertices = &vertices2;
        texture = &texture2;
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(0, 0, 3.5, 0, 0, 0, 0, 1, 0);
    GLfloat light_diffuse[] = { 0.75, 0.75, 0.75, 1.0 };  // 漫反射光的颜色
    GLfloat light_ambient[] = { 0.5, 0.5, 0.5, 1.0 };  // 环境光的颜色
    GLfloat light_specular[] = { 0.75, 0.75, 0.75, 1.0 }; // 镜面反射光的颜色
    GLfloat light_position[] = { radius * cos(lightX), radius * sin(lightY), 3.0, 1.0 }; // 光源的位置
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0); // 启用光源0
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glRotatef(angleX, 1, 0, 0);
    glRotatef(angleY, 0, 1, 0);
    glScalef(1.0 / maxDimension, 1.0 / maxDimension, 1.0 / maxDimension);
    glEnable(GL_COLOR_MATERIAL);
    GLfloat material_ambient[] = { material->ambient[0],material->ambient[1],material->ambient[2] };//{ 0.2, 0.2, 0.2, 1.0 }; // 对环境光的反应
    GLfloat material_diffuse[] = { material->diffuse[0],material->diffuse[1],material->diffuse[2] }; // 对漫反射光的反应
    GLfloat material_specular[] = { material->specular[0],material->specular[1],material->specular[2] }; // 对镜面反射光的反应
    GLfloat material_shininess = material->Ns; // 反射指数
    //glEnable(GL_COLOR_MATERIAL);
    glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, material_shininess);
    glBindTexture(GL_TEXTURE_2D, material->Texture);
    glBegin(GL_TRIANGLES);
    for (const Face& face : *faces) {
        const Vertex& v1 = vertices->at(face.v[0] - 1);
        const Vertex& v2 = vertices->at(face.v[1] - 1);
        const Vertex& v3 = vertices->at(face.v[2] - 1);
        const Texture& t1 = texture->at(face.t[0] - 1);
        //if (face.t[1] - 1 >= texture.size()) cout << face.t[1] - 1 <<' '<<texture.size()<< endl;
        const Texture& t2 = texture->at(face.t[1] - 1);
        const Texture& t3 = texture->at(face.t[2] - 1);
        Vertex normal = computeFaceNormal(v1, v2, v3);
        glNormal3f(normal.x, normal.y, normal.z);
        glTexCoord2f(t1.t1, t1.t2); glVertex3f(v1.x, v1.y, v1.z);
        glTexCoord2f(t2.t1, t2.t2); glVertex3f(v2.x, v2.y, v2.z);
        glTexCoord2f(t3.t1, t3.t2); glVertex3f(v3.x, v3.y, v3.z);
    }
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
    glutSwapBuffers();
}
void Keyboard(unsigned char key, int x, int y) {
    if (key == 'c') {
        sel = sel == 1 ? 0 : 1;
    }
}
void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, static_cast<double>(w) / static_cast<double>(h), 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
}
void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        Rotate = true;
        lastX = x;
        lastY = y;
    }
    else {
        Rotate = false;
    }
}

void motion(int x, int y) {
    if (Rotate) {
        angleY += (x - lastX);
        angleX += (y - lastY);
        lastX = x;
        lastY = y;
        glutPostRedisplay();
    }
}
void update(int value) {
    lightX = lightX + 0.03;
    lightY = lightY + 0.03;
    glutPostRedisplay();
    glutTimerFunc(10, update, 0);
}
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("OBJ Viewer");

    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutKeyboardFunc(Keyboard);
    glutTimerFunc(10, update, 0);
    glutMainLoop();

    return 0;
}