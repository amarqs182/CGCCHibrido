/**
 * TrajectoryDemo.cpp
 *
 * Demonstração do sistema de trajetórias para objetos 3D.
 *
 * Tarefa: "Definindo trajetórias para alguns objetos"
 *   - Adicionar pontos no espaço (teclado/mouse)
 *   - Translação cíclica pelos pontos
 *   - Sem interpolação por curva cúbica (será na Atividade Vivencial)
 *
 * Controles:
 *   W / S     - mover frente / trás (câmera)
 *   A / D     - mover lateral (câmera)
 *   Q / E     - mover baixo / cima (câmera)
 *   Mouse     - olhar em volta (yaw / pitch)
 *   Scroll    - zoom (altera FOV)
 *
 *   1         - Adicionar ponto na posição atual do objeto
 *   2         - Remover último ponto
 *   3         - Limpar todos os pontos
 *   SPACE     - Ativar/desativar trajetória
 *   R         - Resetar trajetória para o início
 *   UP/DOWN   - Aumentar/diminuir velocidade
 *   S         - Salvar pontos em arquivo
 *   L         - Carregar pontos de arquivo
 *   ESC       - sair
 *
 * Autor: aluno (amarqs182) - Unisinos 2026/1
 */

#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <assert.h>

using namespace std;

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Nossas classes
#include "Camera.h"
#include "Trajectory.h"

// Protótipos
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
int  setupShader();
int  setupGeometry();
void drawCube(GLuint shaderID, GLuint VAO, glm::mat4 model, GLint modelLoc);
void drawControlPoints(GLuint shaderID, GLuint VAO, const Trajectory& trajectory, GLint modelLoc);

// Dimensões da janela
const GLuint WIDTH = 1000, HEIGHT = 1000;

// Câmera (global, referenciada pelas callbacks)
Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));

// Trajetória (global)
Trajectory trajectory;

// Controle de deltaTime
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Controle de primeira captura do mouse
bool  firstMouse = true;
float lastX = WIDTH / 2.0f;
float lastY = HEIGHT / 2.0f;

// Posição do objeto (global para poder ser modificada pelas callbacks)
glm::vec3 objectPosition(0.0f, 0.0f, 0.0f);

// Vertex Shader: agora aceita uniform "view" e "projection"
const GLchar* vertexShaderSource = "#version 450\n"
"layout (location = 0) in vec3 position;\n"
"layout (location = 1) in vec3 color;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"out vec4 finalColor;\n"
"void main()\n"
"{\n"
"    gl_Position = projection * view * model * vec4(position, 1.0);\n"
"    finalColor  = vec4(color, 1.0);\n"
"}\0";

const GLchar* fragmentShaderSource = "#version 450\n"
"in vec4 finalColor;\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"    color = finalColor;\n"
"}\n\0";


int main()
{
    // Inicializa GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Janela
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT,
        "CG 2026/1 - Tarefa - Trajetória de Objetos", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    // Callbacks
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // Captura e esconde o cursor (modo FPS)
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* version  = glGetString(GL_VERSION);
    cout << "Renderer: " << renderer << endl;
    cout << "OpenGL version: " << version << endl;

    // Viewport
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    // Inicializa parâmetros de projeção da câmera com o aspect da janela
    camera.setProjectionParams(45.0f, (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);

    // Shader e geometria
    GLuint shaderID = setupShader();
    GLuint VAO      = setupGeometry();

    glUseProgram(shaderID);

    // Localizações dos uniforms
    GLint modelLoc      = glGetUniformLocation(shaderID, "model");
    GLint viewLoc       = glGetUniformLocation(shaderID, "view");
    GLint projectionLoc = glGetUniformLocation(shaderID, "projection");

    glEnable(GL_DEPTH_TEST);

    // Adiciona alguns pontos iniciais de exemplo
    trajectory.addPoint(glm::vec3(-2.0f, 0.0f, 0.0f));
    trajectory.addPoint(glm::vec3(2.0f, 0.0f, 0.0f));
    trajectory.addPoint(glm::vec3(0.0f, 2.0f, 0.0f));
    trajectory.addPoint(glm::vec3(0.0f, -2.0f, 0.0f));
    trajectory.setSpeed(1.5f);

    cout << "\n=== Controles ===" << endl;
    cout << "WASD/QE - Mover câmera" << endl;
    cout << "Mouse - Olhar em volta" << endl;
    cout << "1 - Adicionar ponto na posição do objeto" << endl;
    cout << "2 - Remover último ponto" << endl;
    cout << "3 - Limpar todos os pontos" << endl;
    cout << "SPACE - Ativar/desativar trajetória" << endl;
    cout << "R - Resetar trajetória" << endl;
    cout << "UP/DOWN - Aumentar/diminuir velocidade" << endl;
    cout << "S - Salvar pontos em arquivo" << endl;
    cout << "L - Carregar pontos de arquivo" << endl;
    cout << "ESC - Sair" << endl;
    cout << "==================\n" << endl;

    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        // DeltaTime
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();

        // Atualiza a trajetória
        trajectory.update(deltaTime);
        objectPosition = trajectory.getCurrentPosition();

        // Limpa buffers
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);  // fundo escuro pra dar contraste
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Atualiza matrizes view e projection a partir da câmera
        glm::mat4 view       = camera.getViewMatrix();
        glm::mat4 projection = camera.getProjectionMatrix();

        glUniformMatrix4fv(viewLoc,       1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // Desenha os pontos de controle como pequenos cubos
        drawControlPoints(shaderID, VAO, trajectory, modelLoc);

        // Desenha o cubo principal
        drawCube(shaderID, VAO, glm::mat4(1.0f), modelLoc);

        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &VAO);
    glfwTerminate();
    return 0;
}


// === Callbacks ===

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    // Só processa no KEY_PRESS ou KEY_REPEAT (movimentação contínua)
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        // Movimentação da câmera
        if (key == GLFW_KEY_W) camera.processKeyboard(Camera::FORWARD,  deltaTime);
        if (key == GLFW_KEY_S) camera.processKeyboard(Camera::BACKWARD, deltaTime);
        if (key == GLFW_KEY_A) camera.processKeyboard(Camera::LEFT,      deltaTime);
        if (key == GLFW_KEY_D) camera.processKeyboard(Camera::RIGHT_DIR, deltaTime);
        if (key == GLFW_KEY_Q) camera.processKeyboard(Camera::DOWN,      deltaTime);
        if (key == GLFW_KEY_E) camera.processKeyboard(Camera::UP,        deltaTime);

        // Controles da trajetória
        if (key == GLFW_KEY_1 && action == GLFW_PRESS)
        {
            // Adiciona ponto na posição atual do objeto
            trajectory.addPoint(objectPosition);
            cout << "Ponto adicionado: (" << objectPosition.x << ", " 
                 << objectPosition.y << ", " << objectPosition.z << ")" << endl;
            cout << "Total de pontos: " << trajectory.getPointCount() << endl;
        }

        if (key == GLFW_KEY_2 && action == GLFW_PRESS)
        {
            // Remove último ponto
            if (trajectory.getPointCount() > 0)
            {
                trajectory.removePoint(trajectory.getPointCount() - 1);
                cout << "Último ponto removido. Total: " << trajectory.getPointCount() << endl;
            }
        }

        if (key == GLFW_KEY_3 && action == GLFW_PRESS)
        {
            // Limpa todos os pontos
            trajectory.clearPoints();
            cout << "Todos os pontos foram removidos." << endl;
        }

        if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
        {
            // Ativa/desativa trajetória
            trajectory.setActive(!trajectory.isActive());
            cout << "Trajetória " << (trajectory.isActive() ? "ATIVADA" : "DESATIVADA") << endl;
        }

        if (key == GLFW_KEY_R && action == GLFW_PRESS)
        {
            // Reseta trajetória
            trajectory.reset();
            cout << "Trajetória resetada." << endl;
        }

        if (key == GLFW_KEY_UP && action == GLFW_PRESS)
        {
            // Aumenta velocidade
            trajectory.setSpeed(trajectory.getSpeed() + 0.5f);
            cout << "Velocidade: " << trajectory.getSpeed() << endl;
        }

        if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
        {
            // Diminui velocidade
            float newSpeed = trajectory.getSpeed() - 0.5f;
            if (newSpeed < 0.1f) newSpeed = 0.1f;
            trajectory.setSpeed(newSpeed);
            cout << "Velocidade: " << trajectory.getSpeed() << endl;
        }

        if (key == GLFW_KEY_S && action == GLFW_PRESS)
        {
            // Salva pontos em arquivo
            if (trajectory.saveToFile("trajectory.txt"))
                cout << "Pontos salvos em 'trajectory.txt'" << endl;
            else
                cout << "Erro ao salvar pontos!" << endl;
        }

        if (key == GLFW_KEY_L && action == GLFW_PRESS)
        {
            // Carrega pontos de arquivo
            if (trajectory.loadFromFile("trajectory.txt"))
                cout << "Pontos carregados de 'trajectory.txt'. Total: " << trajectory.getPointCount() << endl;
            else
                cout << "Erro ao carregar pontos!" << endl;
        }
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = (float)xpos;
        lastY = (float)ypos;
        firstMouse = false;
    }

    float xoffset = (float)xpos - lastX;
    float yoffset = lastY - (float)ypos;  // invertido: y cresce pra baixo
    lastX = (float)xpos;
    lastY = (float)ypos;

    camera.processMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.processMouseScroll((float)yoffset);
}


// === Shaders ===

int setupShader()
{
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return shaderProgram;
}


// === Geometria: cubo unitário (12 triângulos = 36 vértices) ===

int setupGeometry()
{
    // 6 faces, cada uma com 4 vértices e 2 triângulos
    // 36 vértices no total, 6 floats por vértice (x,y,z,r,g,b)
    GLfloat vertices[] = {
        // x     y     z     r    g    b

        // Face de cima (y = +0.5) - ciano
        -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
         0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
         0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
         0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 1.0f,

        // Face de baixo (y = -0.5) - magenta
        -0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 1.0f,
         0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 1.0f,
         0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 1.0f,
         0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 1.0f,

        // Face da frente (z = +0.5) - amarelo
        -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 0.0f,
         0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 0.0f,

        // Face de trás (z = -0.5) - vermelho
         0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
         0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f,

        // Face da direita (x = +0.5) - verde
         0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
         0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
         0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
         0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
         0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f,

        // Face da esquerda (x = -0.5) - azul
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
        -0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
    };

    GLuint VBO, VAO;

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Posição
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // Cor
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
                          (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    return VAO;
}


// === Funções auxiliares ===

void drawCube(GLuint shaderID, GLuint VAO, glm::mat4 model, GLint modelLoc)
{
    glBindVertexArray(VAO);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

void drawControlPoints(GLuint shaderID, GLuint VAO, const Trajectory& trajectory, GLint modelLoc)
{
    if (trajectory.getPointCount() == 0)
        return;

    // Desenha cada ponto de controle como um cubo menor
    for (size_t i = 0; i < trajectory.getPointCount(); ++i)
    {
        glm::vec3 point = trajectory.getPoint(i);

        // Cria uma matriz de modelo para o ponto
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, point);
        model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));  // Cubo menor

        // Se for o ponto atual, destaca com uma cor diferente
        if (i == trajectory.getCurrentIndex())
        {
            // Escala um pouco mais para destacar
            model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
        }

        // Desenha o cubo do ponto de controle
        glBindVertexArray(VAO);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
    }
}