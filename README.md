# Computacao Grafica - Diorama 3D

Projeto final da disciplina de Computacao Grafica (2026/1) - Unisinos.
Visualizador 3D completo (diorama) integrando todos os topicos do semestre: OpenGL, camera FPS, iluminacao Phong, curvas de Bezier, texturas, carregamento de modelos .obj e interface ImGui.

## Demonstracao

- Video da demonstracao ao vivo (3-4 min)
- Executavel funcional com todas as features integradas

## Funcionalidades

| Feature | Descricao | Atalho |
|---------|-----------|--------|
| Camera FPS | Movimento livre com WASD/QE + mouse + scroll | WASD/QE, Mouse, Scroll |
| Transformacoes | Translacao, rotacao e escala por objeto | TAB, IJKL, U/O, +/- |
| Iluminacao Phong | 3 luzes pontuais (Key, Fill, Back) | F1/F2/F3 (individual), F4 (todas) |
| Curvas de Bezier | Trajetorias com interpolacao linear e Bezier | SPACE, B, 1-5, R |
| Texturas | Troca automatica (.mtl) e manual via ImGui | Painel ImGui |
| Modelos .obj | Carregamento com materiais e normais | Automatico |
| ImGui | Painel de controle para todos os parametros | Janela flutuante |

## Arquivos

```
CGCCHibrido/
├── src/
│   ├── MainDiorama.cpp        # Aplicacao principal (diorama)
│   ├── Camera.cpp             # Camera FPS
│   ├── ObjParser.cpp          # Parser de .obj/.mtl
│   ├── Trajectory.cpp         # Curvas de Bezier
│   ├── Hello3D.cpp            # Exercicio: triangulo basico
│   ├── Hello3DCamera.cpp      # Exercicio: camera simples
│   ├── TriangleTex.cpp        # Exercicio: textura
│   ├── SpherePhong.cpp        # Exercicio: esfera + Phong
│   ├── CuboIluminacao.cpp     # Exercicio: cubo iluminado
│   └── TrajectoryDemo.cpp     # Exercicio: trajetorias
├── include/
│   ├── Camera.h
│   ├── ObjParser.h
│   ├── Trajectory.h
│   └── SceneParser.h
├── Common/
│   └── glad.c                 # GLAD loader
├── assets/
│   ├── Modelos3D/             # Modelos .obj e .mtl
│   │   ├── Suzanne.obj/.mtl/.png
│   │   └── Cube.obj/.mtl
│   ├── tex/                   # Texturas
│   │   └── pixelWall.png
│   └── scene.json             # Configuracao da cena
├── CMakeLists.txt
└── GettingStarted.md          # Tutorial de setup do ambiente
```

## Como Compilar e Rodar

### Pre-requisitos

- Compilador C++17 (GCC/MinGW ou Clang)
- CMake 3.10+
- Git (para FetchContent baixar dependencias)
- OpenGL 4.1+

### Build

```bash
# Clonar
git clone https://github.com/amarqs182/CGCCHibrido.git
cd CGCCHibrido

# Compilar
mkdir build && cd build
cmake ..
cmake --build . --target MainDiorama

# Rodar
./MainDiorama
```

As dependencias (GLFW, GLM, stb_image, ImGui) sao baixadas automaticamente pelo CMake FetchContent.

### GLAD

Os arquivos da GLAD devem ser baixados manualmente:
1. Acesse [GLAD Generator](https://glad.dav1d.de/)
2. Configuracao: OpenGL 4.1+, Core Profile, C/C++
3. Coloque `glad.h` em `include/glad/`, `khrplatform.h` em `include/glad/KHR/`, `glad.c` em `Common/`

## Dependencias (FetchContent)

| Biblioteca | Versao | Uso |
|------------|--------|-----|
| GLFW | 3.4 | Janela e input |
| GLM | master | Matrizes e math |
| stb_image | master | Carregamento de imagens |
| ImGui | v1.91.8 | Interface grafica |

## Controles Detalhados

### Camera
- **W/S** - Frente/tras
- **A/D** - Esquerda/direita
- **Q/E** - Baixo/cima
- **Mouse** - Olhar em volta
- **Scroll** - Zoom

### Objeto Selecionado
- **TAB** - Trocar objeto
- **I/K** - Transladar Z
- **J/L** - Transladar X
- **U/O** - Rotacionar Y
- **+/--** - Escalar

### Iluminacao
- **F1/F2/F3** - Lig/deslig luz 1/2/3
- **F4** - Lig/deslig todas

### Trajetoria
- **SPACE** - Ativar/desativar trajetoria
- **B** - Alternar modo Bezier
- **1** - Adicionar ponto de controle
- **2** - Remover ultimo ponto
- **3** - Limpar pontos
- **4/5** - Adicionar/remover ponto Bezier
- **R** - Resetar trajetoria
- **UP/DOWN** - Ajustar velocidade

## Tecnologias

- C++17
- OpenGL 4.x (Core Profile)
- GLSL 4.10
- GLFW 3.4
- GLM
- ImGui 1.91.8
- GLAD
- stb_image

## Autor

- aluno (amarqs182) - Unisinos 2026/1

