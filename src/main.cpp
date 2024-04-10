#include <GL/freeglut.h>
#include <math.h>

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

struct Node {
  vector<vector<int>> board;
  int value;

  vector<Node *> children;
};
Node *tree = nullptr;
Node *root = nullptr;
vector<vector<int>> board = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
vector<vector<int>> bestMove = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
int moves = 0;
int n_value = 3;
int profundidad = 9;
bool boardvisible = false;
// Función para convertir una variable a string
template <typename T>
string toString(T val) {
  ostringstream oss;
  oss << val;
  return oss.str();
}
// Texto ingresado por el usuario
string userInput = "";
// Caja de texto
string percentageInput = "";
int textBoxX = 20;
int textBoxY = 25;
int textBoxWidth = 200;
int textBoxHeight = 30;
// Colores
float colors[5][3] = {
    {1.0f, 1.0f, 1.0f},  // Blanco
    {0.5f, 0.5f, 0.5f},  // Gris oscuro
    {1.0f, 0.0f, 0.0f},  // Rojo
    {0.0f, 0.0f, 1.0f},  // Azul
    {0.0f, 1.0f, 0.0f}   // Verde
};

// Boton de envío
int submitButtonX = textBoxX + textBoxWidth + 10;
int submitButtonY = 10;
int submitButtonWidth = 100;
int submitButtonHeight = 30;

vector<vector<vector<int>>> generatePossibleMoves(vector<vector<int>> board, bool player) {
  vector<vector<vector<int>>> possibleMoves;
  for (int i = 0; i < n_value; i++) {
    for (int j = 0; j < n_value; j++) {
      if (board[i][j] == 0) {  // Si la casilla está vacía
        vector<vector<int>> newBoard = board;
        if (player)
          newBoard[i][j] = 1;
        else
          newBoard[i][j] = 2;
        possibleMoves.push_back(newBoard);
      }
    }
  }
  return possibleMoves;
}

int calculatePossibilities(Node &node, int player) {
  int possibilities = 0;
  int boardSize = node.board.size();
  // Filas
  for (int i = 0; i < boardSize; i++) {
    bool possible = true;
    for (int j = 0; j < boardSize; j++) {
      if (node.board[i][j] != player && node.board[i][j] != 0) {
        possible = false;
        break;
      }
    }
    if (possible) {
      possibilities++;
    }
  }
  // Columnas
  for (int i = 0; i < boardSize; i++) {
    bool possible = true;
    int countPlayer = 0;
    for (int j = 0; j < boardSize; j++) {
      if (node.board[j][i] != player && node.board[j][i] != 0) {
        possible = false;
        break;
      }
    }
    if (possible) {
      possibilities++;
    }
  }
  // Diagonales
  bool possible = true;
  int countPlayer = 0;
  for (int i = 0; i < boardSize; i++) {
    if (node.board[i][i] != player && node.board[i][i] != 0) {
      possible = false;
      break;
    }
  }
  if (possible) {
    possibilities++;
  }
  possible = true;
  countPlayer = 0;
  for (int i = 0; i < boardSize; i++) {
    if (node.board[i][boardSize - i - 1] != player && node.board[i][boardSize - i - 1] != 0) {
      possible = false;
      break;
    }
  }
  if (possible) {
    possibilities++;
  }

  return possibilities;
}
int checkWinner(const vector<vector<int>> &board) {
  int n = board.size();
  for (int i = 0; i < n; i++) {
    if (all_of(board[i].begin(), board[i].end(),
               [&](int j) { return j == board[i][0] && j != 0; })) {
      return board[i][0];
    }
  }
  for (int i = 0; i < n; i++) {
    if (all_of(board.begin(), board.end(),
               [&](const vector<int> &row) { return row[i] == board[0][i] && row[i] != 0; })) {
      return board[0][i];
    }
  }
  if (all_of(board.begin(), board.end(), [&](const vector<int> &row) {
        int i = &row - &board[0];
        return row[i] == board[0][0] && row[i] != 0;
      })) {
    return board[0][0];
  }
  if (all_of(board.begin(), board.end(), [&](const vector<int> &row) {
        int i = &row - &board[0];
        return row[n - 1 - i] == board[0][n - 1] && row[n - 1 - i] != 0;
      })) {
    return board[0][n - 1];
  }
  return 0;
}
Node *createBinaryTree(vector<vector<int>> board, bool player, int depth) {
  Node *node = new Node();
  node->board = board;
  node->value = calculatePossibilities(*node, 1) - calculatePossibilities(*node, 2);
  if (depth == 0 || checkWinner(board) != 0) {
    return node;
  }

  // node->value = calculateValue(*node, 1) - calculateValue(*node, 2);
  vector<vector<vector<int>>> possibleMoves = generatePossibleMoves(board, player);
  for (const auto &move : possibleMoves) {
    Node *childNode = createBinaryTree(move, !player, depth - 1);
    node->children.push_back(childNode);
  }
  return node;
}
void deleteTree(Node *node) {
  if (node == nullptr) {
    return;
  }
  for (Node *child : node->children) {
    deleteTree(child);
  }
  delete node;
}
int minMaxAlgorithm(Node *node, int depth, bool isMaximizingPlayer) {
  if (depth == 0 || node->children.empty()) {
    return node->value;
  }
  if (isMaximizingPlayer) {
    int maxEval = -10000;
    for (Node *child : node->children) {
      int eval = minMaxAlgorithm(child, depth - 1, false);
      if (eval > maxEval) {
        maxEval = eval;
        if (depth == profundidad) {
          bestMove = child->board;
        }
      }
    }

    return maxEval;
  } else {
    int minEval = 10000;
    for (Node *child : node->children) {
      int eval = minMaxAlgorithm(child, depth - 1, true);
      minEval = min(minEval, eval);
    }
    return minEval;
  }
}
int windowWidth = 1600;
int windowHeight = 900;
int boardSize = windowWidth / 2;
int startX = windowWidth - boardSize - (windowWidth - 2 * boardSize) / 2 - 100;
int startY = (windowHeight - boardSize) / 2;
int cellSize = boardSize / n_value;

// Agregar una variable para controlar si los botones están visibles
bool buttonsVisible = true;
bool computerstarts = false;

// Botones
int button1X = 50;
int button1Y = windowHeight / 2 - 150;
int button1Width = windowWidth / 4;
int button1Height = 100;
int button2X = 50;
int button2Y = windowHeight / 2 + 50;
int button2Width = windowWidth / 4;
int button2Height = 100;
const char *button1Text = "Player starts";
const char *button2Text = "Computer starts";
const char *buttonSubmit = "Submit";
// Define the positions, dimensions, and texts of the new buttons
int button3X = 50;
int button3Y = 100;
int button3Width = 100;
int button3Height = 50;
const char *button3Text = "3x3";
int button4X = 200;
int button4Y = 100;
int button4Width = 100;
int button4Height = 50;
const char *button4Text = "4x4";
int button5X = 350;
int button5Y = 100;
int button5Width = 100;
int button5Height = 50;
const char *button5Text = "5x5";
int button6X = 50;
int button6Y = 200;
int button6Width = 100;
int button6Height = 50;
const char *button6Text = "6x6";
int button7X = 200;
int button7Y = 200;
int button7Width = 100;
int button7Height = 50;
const char *button7Text = "7x7";
int button8X = 350;
int button8Y = 200;
int button8Width = 100;
int button8Height = 50;
const char *button8Text = "8x8";
void drawText(float x, float y, string text, float color[3]) {
  glColor3fv(color);
  glRasterPos2f(x, y);
  for (char &c : text) {
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
  }
}
void keyboard(unsigned char key, int x, int y) {
  if (key == 13) {
    percentageInput += "\n";
  } else if (key == 8) {
    if (!percentageInput.empty()) {
      percentageInput.pop_back();
    }
  } else {
    percentageInput += key;
  }
  glutPostRedisplay();
}

void drawButton(int x, int y, int width, int height, const char *text, float color[3]) {
  glColor3f(color[0], color[1], color[2]);
  glBegin(GL_QUADS);
  glVertex2f(x, y);
  glVertex2f(x + width, y);
  glVertex2f(x + width, y + height);
  glVertex2f(x, y + height);
  glEnd();

  // Draw text
  glColor3f(1.0, 1.0, 1.0);
  int textWidth = strlen(text) * 10;
  int textX = x + (width - textWidth) / 2;
  int textY = y + height / 2;
  glRasterPos2f(textX, textY);
  for (const char *c = text; *c != '\0'; c++) {
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
  }
}
void drawBoard(vector<vector<int>> board) {
  glLineWidth(3.0);
  for (int i = 0; i < n_value; i++) {
    for (int j = 0; j < n_value; j++) {
      int x = startX + j * cellSize;
      int y = startY + i * cellSize;
      // Dibujar celda
      glColor3f(1.0, 1.0, 1.0);
      glBegin(GL_LINE_LOOP);
      glVertex2f(x, y);
      glVertex2f(x + cellSize, y);
      glVertex2f(x + cellSize, y + cellSize);
      glVertex2f(x, y + cellSize);
      glEnd();

      if (board[i][j] == 1) {
        glColor3f(1.0, 0.0, 0.0);
        glLineWidth(3.0);
        glBegin(GL_LINES);
        glVertex2f(x + cellSize / 4, y + cellSize / 4);
        glVertex2f(x + 3 * cellSize / 4, y + 3 * cellSize / 4);
        glVertex2f(x + 3 * cellSize / 4, y + cellSize / 4);
        glVertex2f(x + cellSize / 4, y + 3 * cellSize / 4);
        glEnd();
      } else if (board[i][j] == 2) {
        glColor3f(0.0, 0.0, 1.0);
        glLineWidth(3.0);
        glBegin(GL_LINE_LOOP);
        for (int ii = 0; ii < 100; ii++) {
          float theta = 2.0f * 3.1415926f * float(ii) / float(100);
          float dx = cellSize / 3 * cosf(theta);
          float dy = cellSize / 3 * sinf(theta);
          glVertex2f(x + cellSize / 2 + dx, y + cellSize / 2 + dy);
        }
        glEnd();
      }
    }
  }
}
bool areEqual(const std::vector<std::vector<int>> &vec1,
              const std::vector<std::vector<int>> &vec2) {
  if (vec1.size() != vec2.size()) {
    return false;
  }
  for (size_t i = 0; i < vec1.size(); i++) {
    if (vec1[i].size() != vec2[i].size()) {
      return false;
    }
    for (size_t j = 0; j < vec1[i].size(); j++) {
      if (vec1[i][j] != vec2[i][j]) {
        return false;
      }
    }
  }
  return true;
}

void mouseClick(int button, int state, int x, int y) {
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
    if (x >= submitButtonX && x <= submitButtonX + submitButtonWidth && y >= submitButtonY &&
        y <= submitButtonY + submitButtonHeight) {
      profundidad = stoi(percentageInput);
      glutPostRedisplay();
    } else if (buttonsVisible) {
      if (x >= button1X && x <= button1X + button1Width && y >= button1Y &&
          y <= button1Y + button1Height) {
        // Player 1 starts
        buttonsVisible = false;
        glutPostRedisplay();
      } else if (x >= button2X && x <= button2X + button2Width && y >= button2Y &&
                 y <= button2Y + button2Height) {
        // Computer starts
        root = createBinaryTree(board, true, profundidad);
        buttonsVisible = false;
        computerstarts = true;
        int temp = minMaxAlgorithm(root, profundidad, true);
        board = bestMove;
        moves++;
        glutPostRedisplay();
      } else if (x >= button3X && x <= button3X + button3Width && y >= button3Y &&
                 y <= button3Y + button3Height) {
        n_value = 3;
        boardvisible = true;
        cellSize = boardSize / n_value;
        board.clear();
        for (int i = 0; i < n_value; i++) {
          board.push_back(vector<int>(n_value, 0));
        }
        glutPostRedisplay();
      }
      // Check if 4x4 button is clicked
      else if (x >= button4X && x <= button4X + button4Width && y >= button4Y &&
               y <= button4Y + button4Height) {
        n_value = 4;
        boardvisible = true;
        cellSize = boardSize / n_value;
        board.clear();
        for (int i = 0; i < n_value; i++) {
          board.push_back(vector<int>(n_value, 0));
        }
        glutPostRedisplay();
      }
      // Check if 5x5 button is clicked
      else if (x >= button5X && x <= button5X + button5Width && y >= button5Y &&
               y <= button5Y + button5Height) {
        n_value = 5;
        boardvisible = true;
        cellSize = boardSize / n_value;
        board.clear();
        for (int i = 0; i < n_value; i++) {
          board.push_back(vector<int>(n_value, 0));
        }
        glutPostRedisplay();
      }
      // Check if 6x6 button is clicked
      else if (x >= button6X && x <= button6X + button6Width && y >= button6Y &&
               y <= button6Y + button6Height) {
        n_value = 6;
        boardvisible = true;
        cellSize = boardSize / n_value;
        board.clear();
        for (int i = 0; i < n_value; i++) {
          board.push_back(vector<int>(n_value, 0));
        }
        glutPostRedisplay();
      }
      // Check if 7x7 button is clicked
      else if (x >= button7X && x <= button7X + button7Width && y >= button7Y &&
               y <= button7Y + button7Height) {
        n_value = 7;
        boardvisible = true;
        cellSize = boardSize / n_value;
        board.clear();
        for (int i = 0; i < n_value; i++) {
          board.push_back(vector<int>(n_value, 0));
        }
        glutPostRedisplay();
      }
      // Check if 8x8 button is clicked
      else if (x >= button8X && x <= button8X + button8Width && y >= button8Y &&
               y <= button8Y + button8Height) {
        n_value = 8;
        boardvisible = true;
        cellSize = boardSize / n_value;
        board.clear();
        for (int i = 0; i < n_value; i++) {
          board.push_back(vector<int>(n_value, 0));
        }
        glutPostRedisplay();
      }
    } else {
      int boardX = (x - startX) / cellSize;
      int boardY = (y - startY) / cellSize;
      if (boardX >= 0 && boardX < n_value && boardY >= 0 && boardY < n_value) {
        if (board[boardY][boardX] == 0) {
          // Actualizar la celda con una O
          if (!checkWinner(board)) {
            board[boardY][boardX] = 2;
            moves++;
          }
          glutPostRedisplay();
        }
      }
    }
  }
}

void display() {
  glClear(GL_COLOR_BUFFER_BIT);
  // Jugadas de la computadora
  if (moves % 2 != 0 && !buttonsVisible && !computerstarts && moves < n_value * n_value &&
      !checkWinner(board)) {
    root = createBinaryTree(board, true, profundidad);
    int temp = minMaxAlgorithm(root, profundidad, true);
    board = bestMove;
    moves++;
  } else if (moves % 2 == 0 && !buttonsVisible && computerstarts && moves < n_value * n_value &&
             !checkWinner(board)) {
    root = createBinaryTree(board, true, profundidad);
    int temp = minMaxAlgorithm(root, profundidad, true);
    board = bestMove;
    moves++;
  }
  if (moves == n_value * n_value || checkWinner(board) != 0) {
    if (!checkWinner(board))
      drawText(button2X, button2Y, "Draw", colors[0]);
    else
      drawText(button2X, button2Y, checkWinner(board) == 1 ? "Computer Wins" : "Player Wins",
               colors[0]);
  }
  if (boardvisible) {
    drawBoard(board);
  }

  if (buttonsVisible) {
    drawButton(button1X, button1Y, button1Width, button1Height, button1Text, colors[1]);
    drawButton(button2X, button2Y, button2Width, button2Height, button2Text, colors[1]);
  }
  // Draw the new buttons
  drawButton(button3X, button3Y, button3Width, button3Height, button3Text, colors[1]);
  drawButton(button4X, button4Y, button4Width, button4Height, button4Text, colors[1]);
  drawButton(button5X, button5Y, button5Width, button5Height, button5Text, colors[1]);
  drawButton(button6X, button6Y, button6Width, button6Height, button6Text, colors[1]);
  drawButton(button7X, button7Y, button7Width, button7Height, button7Text, colors[1]);
  drawButton(button8X, button8Y, button8Width, button8Height, button8Text, colors[1]);
  drawText(textBoxX, textBoxY + 50, "Profundidad: " + toString(profundidad), colors[0]);
  drawButton(submitButtonX, submitButtonY, submitButtonWidth, submitButtonHeight, buttonSubmit,
             colors[1]);
  drawText(textBoxX + 5, textBoxY + 5, "Ingrese: " + percentageInput, colors[0]);
  glutSwapBuffers();
}

int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(1600, 900);
  glutCreateWindow("3 en raya");
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, 1600, 900, 0);
  glutDisplayFunc(display);
  glutKeyboardFunc(keyboard);
  glutMouseFunc(mouseClick);
  glutMainLoop();
  return 0;
}