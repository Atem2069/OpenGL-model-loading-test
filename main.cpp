#include <glad/glad.h>
#include <glad.c>
#include <glfw/glfw3.h>
#include <SOIL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cstdio>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>

bool loadObj(const char *path, std::vector<glm::vec3> &out_vertices,
                               std::vector<glm::vec2> &out_uvs,
                               std::vector<glm::vec3> &out_normals);
std::string loadShader(std::string path);


GLuint vBuffer = 0;
GLuint uvBuffer = 0;
GLuint nBuffer = 0;
int main()
{
    std::vector <glm::vec3> vertices;
    std::vector <glm::vec2> uvs;
    std::vector <glm::vec3> normals;

    if (!glfwInit())
    {
        std::cout << "GLFW Initialization error";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    GLFWwindow *window = glfwCreateWindow(1024,600,"test",nullptr,nullptr);
    if (window == nullptr)
    {
        std::cout << "OpenGL context creation error";
        return -1;
    }

    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "GLAD initialization error";
        return -1;
    }
    loadObj("suzanne.obj",vertices,uvs,normals);
    std::string vertex  = loadShader("shader.vert");
    std::string fragment = loadShader("shader.frag");

    const char *pVertex = vertex.c_str();
    const char *pFragment = fragment.c_str();

    glGenBuffers(1,&vBuffer);
    glBindBuffer(GL_ARRAY_BUFFER,vBuffer);
    glBufferData(GL_ARRAY_BUFFER,vertices.size() * sizeof(glm::vec3),&vertices[0],GL_STATIC_DRAW);

    glGenBuffers(1,&uvBuffer);
    glBindBuffer(GL_ARRAY_BUFFER,uvBuffer);
    glBufferData(GL_ARRAY_BUFFER,uvs.size() * sizeof(glm::vec2), &uvs[0],GL_STATIC_DRAW);

    glGenBuffers(1,&nBuffer);
    glBindBuffer(GL_ARRAY_BUFFER,nBuffer);
    glBufferData(GL_ARRAY_BUFFER,normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER,vBuffer);
    char buffer[512];
    std::cout << pVertex;
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader,1,&pVertex,NULL);
    glCompileShader(vertexShader);
    glGetShaderInfoLog(vertexShader,512,NULL,buffer);
    std::cout << buffer;

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader,1,&pFragment,NULL);
    glCompileShader(fragmentShader);
    glGetShaderInfoLog(fragmentShader,512,NULL,buffer);
    std::cout << buffer;
    GLuint program = glCreateProgram();
    glAttachShader(program,vertexShader);
    glAttachShader(program,fragmentShader);
    glLinkProgram(program);
    glGetProgramInfoLog(program,512,NULL,buffer);
    std::cout << buffer;
    glUseProgram(program);

    glBindBuffer(GL_ARRAY_BUFFER,vBuffer);
    GLuint posAttrib = glGetAttribLocation(program,"position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib,3,GL_FLOAT,GL_FALSE,0,0);

    glBindBuffer(GL_ARRAY_BUFFER,uvBuffer);
    GLuint uvAttrib = glGetAttribLocation(program,"texcoord");
    glEnableVertexAttribArray(uvAttrib);
    glVertexAttribPointer(uvAttrib,2,GL_FLOAT,GL_FALSE,0,0);

    glBindBuffer(GL_ARRAY_BUFFER,nBuffer);
    GLuint normAttrib = glGetAttribLocation(program,"normal");
    glEnableVertexAttribArray(normAttrib);
    glVertexAttribPointer(normAttrib,3,GL_FLOAT,GL_FALSE,0,0);

    glBindBuffer(GL_ARRAY_BUFFER,vBuffer);
    GLint modelU = glGetUniformLocation(program,"model");
    GLint viewU = glGetUniformLocation(program,"view");
    GLint projU = glGetUniformLocation(program,"proj");

    glm::mat4 view = glm::lookAt
    (
        glm::vec3(2.5f,2.5f,2.5f),
        glm::vec3(0.0f,0.0f,0.0f),
        glm::vec3(0.0f,0.0f,0.5f)
    );
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), 1024.0f / 600.0f, 1.0f, 1000.0f);
    glUniformMatrix4fv(viewU,1,GL_FALSE,glm::value_ptr(view));
    glUniformMatrix4fv(projU,1,GL_FALSE,glm::value_ptr(proj));

    float transX = 0.1, transY = 0, transZ = 0;
    float lkAtY = 0, lkAtZ = 0;
    float rotation = 0;
    glEnable(GL_DEPTH_TEST);
    while (!glfwWindowShouldClose(window))
    {
        rotation += 0.05f;
        glm::mat4 model;
        model = glm::rotate(model,glm::radians(rotation),glm::vec3(0.0f,1.0f,0.0f));
        proj = glm::perspective(glm::radians(45.0f), 1920.0f / 1080.0f, 1.0f, 1000.0f);
        view = glm::lookAt
        (
            glm::vec3(transX,transY,transZ),
            glm::vec3(1,lkAtY,lkAtZ),
            glm::vec3(0.0f,1.0f,0.0f)
        );
        glUniformMatrix4fv(modelU,1,GL_FALSE,glm::value_ptr(model));
        glUniformMatrix4fv(projU,1,GL_FALSE,glm::value_ptr(proj));
        glUniformMatrix4fv(viewU,1,GL_FALSE,glm::value_ptr(view));

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.5f,0.5f,0.5f,1.0f);
        glDrawArrays(GL_TRIANGLES,0,vertices.size());
        glfwSwapBuffers(window);
        glfwPollEvents();


        if (glfwGetKey(window,GLFW_KEY_Q) == GLFW_PRESS)
            transX -= 0.05f;
        if (glfwGetKey(window,GLFW_KEY_W) == GLFW_PRESS)
            transX += 0.05f;
        if (glfwGetKey(window,GLFW_KEY_A) == GLFW_PRESS)
            transY -= 0.05f;
        if (glfwGetKey(window,GLFW_KEY_S) == GLFW_PRESS)
            transY += 0.05f;
        if (glfwGetKey(window,GLFW_KEY_Z) == GLFW_PRESS)
            transZ -= 0.05f;
        if (glfwGetKey(window,GLFW_KEY_X) == GLFW_PRESS)
            transZ += 0.05f;
        if (glfwGetKey(window,GLFW_KEY_I) == GLFW_PRESS)
            lkAtY += 0.05f;
        if (glfwGetKey(window,GLFW_KEY_K) == GLFW_PRESS)
            lkAtY -= 0.05f;
        if (glfwGetKey(window,GLFW_KEY_J) == GLFW_PRESS)
            lkAtZ -= 0.05f;
        if (glfwGetKey(window,GLFW_KEY_L) == GLFW_PRESS)
            lkAtZ += 0.05f;
    }
    glfwTerminate();
    return 0;
}





bool loadObj(const char *path, std::vector<glm::vec3> &out_vertices,
                               std::vector<glm::vec2> &out_uvs,
                               std::vector<glm::vec3> &out_normals)
{
    std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
    std::vector< glm::vec3 > temp_vertices;
    std::vector< glm::vec2 > temp_uvs;
    std::vector< glm::vec3 > temp_normals;

    FILE * file = fopen(path, "r");
    if( file == NULL )
    {
        printf("[ERROR] : File couldn't be opened\n");
        return false;
    }

    while(true)
    {

        char lineHeader[128];
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF)
            break;          //EOF: End of File

        if (strcmp(lineHeader, "v") == 0)
        {
            glm::vec3 vertex;
            fscanf(file,"%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
            temp_vertices.push_back(vertex);
        }else if(strcmp(lineHeader,"vt") == 0)
        {
            glm::vec2 uv;
            fscanf(file, "%f %f\n", &uv.x, &uv.y);
            temp_uvs.push_back(uv);
        }else if(strcmp(lineHeader,"vn") == 0)
        {
            glm::vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
            temp_normals.push_back(normal);
        }else if ( strcmp( lineHeader, "f" ) == 0 && temp_uvs.size() != 0)
        {
            std::string vertex1, vertex2, vertex3;
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
            if (matches != 9)
                {
                printf("[ERROR] : File indices couldn't be parsed\n");
                return false;
            }

            vertexIndices.push_back(vertexIndex[0]);
            vertexIndices.push_back(vertexIndex[1]);
            vertexIndices.push_back(vertexIndex[2]);
            uvIndices    .push_back(uvIndex[0]);
            uvIndices    .push_back(uvIndex[1]);
            uvIndices    .push_back(uvIndex[2]);
            normalIndices.push_back(normalIndex[0]);
            normalIndices.push_back(normalIndex[1]);
            normalIndices.push_back(normalIndex[2]);
        }
        else if(strcmp(lineHeader,"f") == 0 && temp_uvs.size() == 0)
        {
            std::string vertex1,vertex2,verstex3;
            unsigned int vertexIndex[3];
            fscanf(file,"%d %d %d\n", &vertexIndex[0], &vertexIndex[1], &vertexIndex[2]);

            vertexIndices.push_back(vertexIndex[0]);
            vertexIndices.push_back(vertexIndex[1]);
            vertexIndices.push_back(vertexIndex[2]);
        }
    }

    for (unsigned int i = 0; i<vertexIndices.size(); i++)
    {
        unsigned int vertexIndex = vertexIndices[i];
        glm::vec3 vertex = temp_vertices[vertexIndex - 1];
        out_vertices.push_back(vertex);
    }
    for (unsigned int i = 0; i<normalIndices.size(); i++)
    {
        unsigned int normalIndex = normalIndices[i];
        glm::vec3 normal = temp_normals[normalIndex - 1];
        out_normals.push_back(normal);
    }
    for (unsigned int i = 0; i<uvIndices.size(); i++)
    {
        unsigned int uvIndex = uvIndices[i];
        glm::vec2 uv = temp_uvs[uvIndex - 1];
        out_uvs.push_back(uv);
    }
    return true;
}



std::string loadShader(std::string path)
{
    std::ifstream infile(path);
    if (infile.fail())
    {
        std::cout << "Shader load error: " << path << std::endl;
    }
    std::string contents {std::istreambuf_iterator<char>(infile), std::istreambuf_iterator<char>()};
    return contents;
}
