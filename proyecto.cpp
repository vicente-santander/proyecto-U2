#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

struct Node {
    string name;
    vector<Node*> neighbors;

    Node(const string& n) : name(n) {}
};

struct Guardian {
    string name;
    int power;
    Node* aldeaMaestro;

    Guardian(const string& n, int p, Node* maestro) : name(n), power(p), aldeaMaestro(maestro) {}
};

Node* findNode(const string& name, unordered_map<string, Node*>& nodes) {
    auto it = nodes.find(name);
    if (it != nodes.end()) {
        return it->second;
    }
    return nullptr;
}

void createGraph(const string& filename, unordered_map<string, Node*>& nodes) {
    ifstream file(filename);
    if (!file) {
        cerr << "No se pudo abrir el archivo " << filename << endl;
        return;
    }

    string line;
    while (getline(file, line)) {
        istringstream iss(line);
        string from, to;
        if (iss >> from >> to) {
            Node* fromNode = findNode(from, nodes);
            if (fromNode == nullptr) {
                fromNode = new Node(from);
                nodes[from] = fromNode;
            }

            Node* toNode = findNode(to, nodes);
            if (toNode == nullptr) {
                toNode = new Node(to);
                nodes[to] = toNode;
            }

            fromNode->neighbors.push_back(toNode);
            toNode->neighbors.push_back(fromNode);  // Permitir ir y volver
        }
    }

    file.close();
}

void loadGuardians(const string& filename, unordered_map<string, Guardian*>& guardians, unordered_map<string, Node*>& nodes) {
    ifstream file(filename);
    if (!file) {
        cerr << "No se pudo abrir el archivo " << filename << endl;
        return;
    }

    string line;
    while (getline(file, line)) {
        istringstream iss(line);
        string name, aldeaMaestroName;
        int power;
        if (iss >> name >> power >> aldeaMaestroName) {
            Node* aldeaMaestro = findNode(aldeaMaestroName, nodes);
            if (aldeaMaestro != nullptr) {
                guardians[name] = new Guardian(name, power, aldeaMaestro);
            }
        }
    }

    file.close();
}

void dfs(Node* node, unordered_map<Node*, bool>& visited) {
    cout << node->name << " ";
    visited[node] = true;

    for (Node* neighbor : node->neighbors) {
        if (!visited[neighbor]) {
            dfs(neighbor, visited);
        }
    }
}

void bfs(Node* node) {
    unordered_map<Node*, bool> visited;
    vector<Node*> queue;

    visited[node] = true;
    queue.push_back(node);

    while (!queue.empty()) {
        Node* current = queue.front();
        queue.erase(queue.begin());

        cout << current->name << " ";

        for (Node* neighbor : current->neighbors) {
            if (!visited[neighbor]) {
                visited[neighbor] = true;
                queue.push_back(neighbor);
            }
        }
    }
}

void showNeighborOptions(Node* node) {
    cout << "Opciones de aldeas vecinas:" << endl;
    for (size_t i = 0; i < node->neighbors.size(); ++i) {
        cout << i + 1 << ". " << node->neighbors[i]->name << endl;
    }
    cout << "0. Volver atrás" << endl;
}



void showGuardianOptions(Node* node, unordered_map<string, Guardian*>& guardians) {
    cout << "Opciones de guardianes en " << node->name << ":" << endl;

    if (node->name != "Tesla") {
        Guardian* weakestGuardian = nullptr;
        int weakestPower = INT_MAX;

        int option = 1;
        for (const auto& entry : guardians) {
            Guardian* guardian = entry.second;
            if (guardian->aldeaMaestro == node && guardian->power < weakestPower) {
                weakestGuardian = guardian;
                weakestPower = guardian->power;
            }
        }

        if (weakestGuardian != nullptr) {
            cout << "Recomendación: El guardián más débil en esta aldea es " << weakestGuardian->name
                 << " (Poder: " << weakestGuardian->power << ")." << endl;
            option++;
        }
    }

    int option = 1;
    for (const auto& entry : guardians) {
        Guardian* guardian = entry.second;
        if (guardian->aldeaMaestro == node) {
            cout << option << ". " << guardian->name << " (Poder: " << guardian->power << ")" << endl;
            option++;
        }
    }
    cout << "0. No luchar" << endl;

    string input;
    int selectedOption = -1;

    while (true) {
        cout << "Seleccione una opción: ";
        getline(cin, input);

        // Verificar que la entrada sea numérica y coincida exactamente con una opción válida
        istringstream iss(input);
        if (iss >> selectedOption && iss.eof() && selectedOption >= 0 && selectedOption <= option - 1) {
            break;
        }

        cout << "Opción inválida. Intente nuevamente." << endl;
    }

    if (selectedOption == 0) {
        cout << "No luchar seleccionado." << endl;
    } else {
        // Procesar la opción seleccionada
        // ...
    }
}




Guardian* findWeakestGuardian(Node* node, unordered_map<string, Guardian*>& guardians) {
    Guardian* weakestGuardian = nullptr;
    int weakestPower = INT_MAX;

    for (const auto& entry : guardians) {
        Guardian* guardian = entry.second;
        if (guardian->aldeaMaestro == node && guardian->power < weakestPower) {
            weakestGuardian = guardian;
            weakestPower = guardian->power;
        }
    }

    return weakestGuardian;
}


Node* choosePath(Node* current, unordered_map<string, Guardian*>& guardians) {
    cout << "Estás en la aldea " << current->name << "." << endl;
    
    

    while (true) {
        showNeighborOptions(current);

        int choice;
        cout << "Elige tu próxima aldea: ";
        cin >> choice;

        if (choice < 0 || choice > static_cast<int>(current->neighbors.size())) {
            cout << "Opción inválida. Inténtalo de nuevo." << endl;
        } else if (choice == 0) {
            return nullptr;  // El jugador eligió volver atrás
        } else {
            Node* next = current->neighbors[choice - 1];

            showGuardianOptions(next, guardians);

            int guardianChoice;
            cout << "Elige el guardián con el que quieres luchar: ";
            cin >> guardianChoice;

            if (guardianChoice == 0) {
                return next;  // El jugador eligió no luchar
            } else {
                Guardian* guardian = nullptr;
                int count = 1;
                for (const auto& entry : guardians) {
                    Guardian* g = entry.second;
                    if (g->aldeaMaestro == next) {
                        if (count == guardianChoice) {
                            guardian = g;
                            break;
                        }
                        count++;
                    }
                }

                if (guardian != nullptr) {
                    cout << "Luchando contra " << guardian->name << " (Poder: " << guardian->power << ")..." << endl;
                    // Aquí puedes implementar la lógica de la pelea con el guardián
                }

                return next;
            }
        }
    }
}

int main() {
    unordered_map<string, Node*> nodes;
    unordered_map<string, Guardian*> guardians;

    createGraph("aldeas.txt", nodes);
    loadGuardians("guardianes.txt", guardians, nodes);

    Node* current = findNode("Capital_City", nodes);
    while (current != nullptr) {
        current = choosePath(current, guardians);
    }

    for (auto& entry : nodes) {
        delete entry.second;
    }

    for (auto& entry : guardians) {
        delete entry.second;
    }

    return 0;
}
