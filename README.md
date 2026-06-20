# Computacao Grafica - Exercicios 2026/1

Repositorio com os exercicios da disciplina de Computacao Grafica (modalidade hibrida) da Unisinos.

Cada exercicio gera um executavel independente na pasta `build/`.

## Exercicios

| Exercicio | Descricao | Topicos |
|-----------|-----------|---------|
| Hello3D | Triangulo basico | OpenGL, shaders, VAO/VBO |
| Hello3DCamera | Camera simples | Camera, view/projection matrix |
| TriangleTex | Triangulo texturizado | Coordenadas UV, stb_image |
| SpherePhong | Esfera com iluminacao | Normais, modelo Phong (Ka/Kd/Ks) |
| CuboIluminacao | Cubo iluminado | Iluminacao pontual, 3 fontes de luz |
| TrajectoryDemo | Trajetorias de objetos | Interpolacao linear, curvas de Bezier |

## Como Compilar

```bash
# Clonar
git clone https://github.com/amarqs182/CGCCHibrido.git
cd CGCCHibrido

# Compilar todos
mkdir build && cd build
cmake ..
cmake --build .

# Compilar um exercicio especifico
cmake --build . --target SpherePhong
```

## Pre-requisitos

- Compilador C++17 (GCC/MinGW ou Clang)
- CMake 3.10+
- Git (para FetchContent baixar dependencias)
- OpenGL 3.3+

## GLAD

Os arquivos da GLAD devem ser baixados manualmente:
1. Acesse [GLAD Generator](https://glad.dav1d.de/)
2. Configuracao: OpenGL 3.3+, Core Profile, C/C++
3. Coloque `glad.h` em `include/glad/`, `khrplatform.h` em `include/glad/KHR/`, `glad.c` em `Common/`

## Estrutura

```
CGCCHibrido/
├── src/                # Codigo-fonte dos exercicios
├── include/            # Cabecalhos (Camera, Trajectory)
├── Common/             # glad.c
├── assets/             # Modelos .obj e texturas
├── CMakeLists.txt
└── GettingStarted.md   # Tutorial de setup do ambiente
```

## Autor

- amarqs182 - Unisinos 2026/1
