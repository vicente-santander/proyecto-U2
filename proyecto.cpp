#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;

struct Node {
    string nombre;
    int poder;
    string maestro;
    string aldea;
    int aldeasRec = 0;
    Node* left;
    Node* right;
};

Node* createNode(string nombre, int poder, string maestro, string aldea) {
    Node* newNode = new Node;
    newNode->nombre = nombre;
    newNode->poder = poder;
    newNode->maestro = maestro;
    newNode->aldea = aldea;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

Node* insert(Node* root, string nombre, int poder, string maestro, string aldea) {
    if (root == NULL) {
        root = createNode(nombre, poder, maestro, aldea);
    } else {
        if (root->poder < poder) {
            root->left = insert(root->left, nombre, poder, maestro, aldea);
        } else {
            root->right = insert(root->right, nombre, poder, maestro, aldea);
        }
    }
    return root;
}

void printInOrder(Node* root) {
    if (root == NULL) {
        return;
    }
    printInOrder(root->left);
    cout << root->nombre << " " << root->poder << " " << root->maestro << " " << root->aldea << endl;
    printInOrder(root->right);
}

int main() {
    ifstream file("guardianes.txt");
    if (!file.is_open()) {
        cout << "Error al abrir el archivo." << endl;
        return 0;
    }

    Node* root = NULL;
    string line;
    while (getline(file, line)) {
        stringstream borrar(line);
        string nombre;
        int poder;
        string maestro;
        string aldea;

        borrar >> nombre >> poder >> maestro >> aldea;
        root = insert(root, nombre, poder, maestro, aldea);
    }

    file.close();

    cout << "Guardianes en orden jerarquico de poder:" << endl;
    printInOrder(root);

    return 0;
}
