// #include <GL/freeglut.h>

#include <algorithm>
#include <iostream>
#include <vector>
using namespace std;
vector<vector<vector<int>>> generatePossibleMoves(vector<vector<int>> board,
                                                  int player) {
  vector<vector<vector<int>>> possibleMoves;
  for (int i = 0; i < board.size(); i++) {
    for (int j = 0; j < board[i].size(); j++) {
      if (board[i][j] == 0) {  // Si la casilla está vacía
        vector<vector<int>> newBoard = board;
        newBoard[i][j] = player;  // Colocar la ficha del jugador en la casilla
        possibleMoves.push_back(newBoard);
      }
    }
  }
  return possibleMoves;
}
struct Node {
  vector<vector<int>> board;
  int value;
  vector<Node*> children;
};

int calculatePossibilities(std::vector<std::vector<int>> board, int player) {
  int possibilities = 0;
  // Check rows
  for (int i = 0; i < 3; i++) {
    if (board[i][0] == player && board[i][1] == player && board[i][2] == 0)
      possibilities++;
    if (board[i][0] == player && board[i][2] == player && board[i][1] == 0)
      possibilities++;
    if (board[i][1] == player && board[i][2] == player && board[i][0] == 0)
      possibilities++;
  }
  // Check columns
  for (int i = 0; i < 3; i++) {
    if (board[0][i] == player && board[1][i] == player && board[2][i] == 0)
      possibilities++;
    if (board[0][i] == player && board[2][i] == player && board[1][i] == 0)
      possibilities++;
    if (board[1][i] == player && board[2][i] == player && board[0][i] == 0)
      possibilities++;
  }
  // Check diagonals
  if (board[0][0] == player && board[1][1] == player && board[2][2] == 0)
    possibilities++;
  if (board[0][0] == player && board[2][2] == player && board[1][1] == 0)
    possibilities++;
  if (board[1][1] == player && board[2][2] == player && board[0][0] == 0)
    possibilities++;
  if (board[0][2] == player && board[1][1] == player && board[2][0] == 0)
    possibilities++;
  if (board[0][2] == player && board[2][0] == player && board[1][1] == 0)
    possibilities++;
  if (board[1][1] == player && board[2][0] == player && board[0][2] == 0)
    possibilities++;
  return possibilities;
}

Node* createBinaryTree(vector<vector<int>> board, int player, int depth) {
  Node* node = new Node();
  node->board = board;
  node->value =
      calculatePossibilities(board, 1) - calculatePossibilities(board, 2);
  if (depth > 0) {
    // Generar todas las posibles jugadas y crear nodos hijos
    vector<vector<vector<int>>> possibleMoves =
        generatePossibleMoves(board, player);
    for (const auto& move : possibleMoves) {
      Node* childNode = createBinaryTree(move, player, depth - 1);
      node->children.push_back(childNode);
    }
  }
  return node;
}

int minMaxAlgorithm(Node* node, int depth, bool isMaximizingPlayer) {
  if (depth == 0 || node->children.empty()) {
    return node->value;
  }
  if (isMaximizingPlayer) {
    int maxEval = -10000;
    for (Node* child : node->children) {
      int eval = minMaxAlgorithm(child, depth - 1, false);
      maxEval = max(maxEval, eval);
    }
    return maxEval;
  } else {
    int minEval = 10000;
    for (Node* child : node->children) {
      int eval = minMaxAlgorithm(child, depth - 1, true);
      minEval = min(minEval, eval);
    }
    return minEval;
  }
}
/*
void gameLogic() {
  // Implementación de la función
  // ...
}

void display() {
  glClear(GL_COLOR_BUFFER_BIT);
  gameLogic();
  glutSwapBuffers();
}
*/
int main(/*int argc, char** argv*/) {
  vector<vector<int>> board = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
  Node* root = createBinaryTree(board, 1, 3);
  int result = minMaxAlgorithm(root, 3, true);
  cout << "Resultado: " << result << endl;
  /*glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(500, 500);
  glutCreateWindow("3 en raya");
  glutDisplayFunc(display);
  glutMainLoop();*/
  return 0;
}
