#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>
#include <stdio.h>
#include "visualizacao.h"

// Variáveis globais para a visualização
static const Trelica* trelica_global = NULL;
static int window_width = 800;
static int window_height = 600;
static float scale = 50.0f;
static float offset_x = 0.0f;
static float offset_y = 0.0f;
static HDC hDC;
static HGLRC hRC;
static HWND hWnd;

void draw_arrow(float x1, float y1, float x2, float y2) {
    glBegin(GL_LINES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glEnd();
    
    // Desenhar ponta da seta
    float angle = atan2(y2 - y1, x2 - x1);
    float length = sqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1));

    // Tamanho da ponta: 10% do comprimento ou no mínimo 8 px
    float arrow_size = fmax(8.0f, 0.1f * length);

    glBegin(GL_TRIANGLES);
    glVertex2f(x2, y2);
    glVertex2f(x2 - arrow_size * cos(angle - 0.3), y2 - arrow_size * sin(angle - 0.3));
    glVertex2f(x2 - arrow_size * cos(angle + 0.3), y2 - arrow_size * sin(angle + 0.3));
    glEnd();
}


// Função para desenhar a treliça
void draw_trelica() {
    if (!trelica_global) return;
    
    // Encontrar a força máxima para normalizar a visualização
    float max_forca = 1.0f;
    for (int i = 0; i < trelica_global->node_count; i++) {
        Node* node = trelica_global->nodes[i];
        float forca_applied = sqrt(node->force_x * node->force_x + node->force_y * node->force_y);
        float forca_reaction = sqrt(node->reacao_x * node->reacao_x + node->reacao_y * node->reacao_y);
        
        if (forca_applied > max_forca) max_forca = forca_applied;
        if (forca_reaction > max_forca) max_forca = forca_reaction;
    }
    
    // Calcular fator de escala para as setas
    float scale_factor = fmin(window_width, window_height) * 0.2f / max_forca;
    
    // Desenhar barras com cores diferentes para tração e compressão
    glLineWidth(3.0f);
    for (int i = 0; i < trelica_global->componente_count; i++) {
        Componente* comp = trelica_global->componentes[i];
        Node* node1 = NULL;
        Node* node2 = NULL;

        for (int j = 0; j < trelica_global->node_count; j++) {
            if (trelica_global->nodes[j]->nome == comp->node1_nome) node1 = trelica_global->nodes[j];
            if (trelica_global->nodes[j]->nome == comp->node2_nome) node2 = trelica_global->nodes[j];
        }

        if (node1 && node2) {
            // Escolher cor
            if (comp->forca_reacao > 0) {
                // Compressão → vermelho
                glColor3f(1.0f, 0.0f, 0.0f);
            } else if (comp->forca_reacao < 0) {
                // Tração → azul
                glColor3f(0.0f, 0.0f, 1.0f);
            } else {
                // Sem força → cinza
                glColor3f(0.5f, 0.5f, 0.5f);
            }

            glBegin(GL_LINES);
            glVertex2f(node1->x * scale + offset_x, node1->y * scale + offset_y);
            glVertex2f(node2->x * scale + offset_x, node2->y * scale + offset_y);
            glEnd();
        }
    }

    
    // Desenhar nós
    glPointSize(8.0f);
    glBegin(GL_POINTS);
    glColor3f(0.0f, 0.0f, 1.0f);
    
    for (int i = 0; i < trelica_global->node_count; i++) {
        Node* node = trelica_global->nodes[i];
        glVertex2f(node->x * scale + offset_x, node->y * scale + offset_y);
    }
    glEnd();
    
    // Desenhar forças aplicadas (verde)
    glColor3f(0.0f, 1.0f, 0.0f);
    for (int i = 0; i < trelica_global->node_count; i++) {
        Node* node = trelica_global->nodes[i];
        if (fabs(node->force_x) > 1e-5 || fabs(node->force_y) > 1e-5) {
            float x1 = node->x * scale + offset_x;
            float y1 = node->y * scale + offset_y;
            float x2 = x1 + node->force_x * scale_factor;
            float y2 = y1 + node->force_y * scale_factor;
            draw_arrow(x1, y1, x2, y2);
        }
    }
    
    // Desenhar reações (preto)
    glColor3f(0.0f, 0.0f, 0.0f);
    for (int i = 0; i < trelica_global->node_count; i++) {
        Node* node = trelica_global->nodes[i];
        if (fabs(node->reacao_x) > 1e-5 || fabs(node->reacao_y) > 1e-5) {
            float x1 = node->x * scale + offset_x;
            float y1 = node->y * scale + offset_y;
            float x2 = x1 + node->reacao_x * scale_factor;
            float y2 = y1 + node->reacao_y * scale_factor;
            draw_arrow(x1, y1, x2, y2);
        }
    }

    
    // Mostrar informações no título da janela
    char title[100];
    sprintf(title, "Visualizacao da Trelica - Escala: 1:%.0f (Maior força: %.0f N)", 
            1.0/scale_factor, max_forca);
    SetWindowText(hWnd, title);
}

// Procedimento da janela
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            BeginPaint(hWnd, &ps);
            
            glClear(GL_COLOR_BUFFER_BIT);
            glLoadIdentity();
            draw_trelica();
            SwapBuffers(hDC);
            
            EndPaint(hWnd, &ps);
            return 0;
        }
        
        case WM_KEYDOWN: {
            switch (wParam) {
                case VK_ESCAPE:
                case 'Q':
                    PostQuitMessage(0);
                    break;
                case VK_ADD:
                case VK_OEM_PLUS:
                    scale *= 1.1f;
                    InvalidateRect(hWnd, NULL, TRUE);
                    break;
                case VK_SUBTRACT:
                case VK_OEM_MINUS:
                    scale /= 1.1f;
                    InvalidateRect(hWnd, NULL, TRUE);
                    break;
                case VK_UP:
                    offset_y += 10.0f;
                    InvalidateRect(hWnd, NULL, TRUE);
                    break;
                case VK_DOWN:
                    offset_y -= 10.0f;
                    InvalidateRect(hWnd, NULL, TRUE);
                    break;
                case VK_LEFT:
                    offset_x -= 10.0f;
                    InvalidateRect(hWnd, NULL, TRUE);
                    break;
                case VK_RIGHT:
                    offset_x += 10.0f;
                    InvalidateRect(hWnd, NULL, TRUE);
                    break;
            }
            return 0;
        }
        
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

// Inicializar OpenGL
BOOL InitGL() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, window_width, 0, window_height);
    glMatrixMode(GL_MODELVIEW);
    return TRUE;
}

// Função principal de visualização
void visualizar_trelica(const Trelica* trelica) {
    trelica_global = trelica;
    
    // Registrar a classe da janela
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = "TrelicaWindow";
    wc.style = CS_OWNDC;
    
    if (!RegisterClass(&wc)) return;
    
    // Criar a janela
    hWnd = CreateWindow("TrelicaWindow", "Visualizacao da Trelica", 
                        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                        window_width, window_height, NULL, NULL, 
                        GetModuleHandle(NULL), NULL);
    
    if (!hWnd) return;
    
    // Obter o contexto de dispositivo
    hDC = GetDC(hWnd);
    
    // Configurar o pixel format
    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR), 1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 0, 0, 
        PFD_MAIN_PLANE, 0, 0, 0, 0
    };
    
    int format = ChoosePixelFormat(hDC, &pfd);
    SetPixelFormat(hDC, format, &pfd);
    
    // Criar o contexto de renderização
    hRC = wglCreateContext(hDC);
    wglMakeCurrent(hDC, hRC);
    
    // Inicializar OpenGL
    InitGL();
    
    // Calcular offset para centralizar
    if (trelica && trelica->node_count > 0) {
        float min_x = trelica->nodes[0]->x;
        float max_x = trelica->nodes[0]->x;
        float min_y = trelica->nodes[0]->y;
        float max_y = trelica->nodes[0]->y;
        
        for (int i = 1; i < trelica->node_count; i++) {
            if (trelica->nodes[i]->x < min_x) min_x = trelica->nodes[i]->x;
            if (trelica->nodes[i]->x > max_x) max_x = trelica->nodes[i]->x;
            if (trelica->nodes[i]->y < min_y) min_y = trelica->nodes[i]->y;
            if (trelica->nodes[i]->y > max_y) max_y = trelica->nodes[i]->y;
        }
        
        offset_x = window_width / 2.0f - (min_x + max_x) * scale / 2.0f;
        offset_y = window_height / 2.0f - (min_y + max_y) * scale / 2.0f;
    }
    
    // Mostrar a janela
    ShowWindow(hWnd, SW_SHOW);
    UpdateWindow(hWnd);
    
    // Loop de mensagens
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    // Limpar
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hWnd, hDC);
}