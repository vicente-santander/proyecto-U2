#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <climits>
#include <cstdlib>
#include <ctime>

using namespace std;

struct Node {
    string name;
    vector<Node*> neighbors;

    Node(const string& n) : name(n) {}
};

struct Guardian {
    string name;
    int powerLevel;
    string mainMaster;
    string village;
    vector<Guardian*> apprentices;

    Guardian(const string& n, int p, const string& mm, const string& v) : name(n), powerLevel(p), mainMaster(mm), village(v) {}
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

    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            istringstream iss(line);
            string name;
            int power;
            string masterNodeName;
            string villageName;

            if (iss >> name >> power >> masterNodeName >> villageName) {
                Node* masterNode = nodes[masterNodeName];
                Guardian* guardian = new Guardian(name, power, masterNodeName, villageName);
                guardians[name] = guardian;

                // Add the guardian as an apprentice to its master
                if (masterNode != nullptr) {
                    Guardian* masterGuardian = guardians[masterNode->name];
                    if (masterGuardian != nullptr) {
                        masterGuardian->apprentices.push_back(guardian);
                    }
                }
            }
        }

        file.close();
    }
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
    cout << "0. Volver atras" << endl;
}

void showGuardianOptions(Node* node, unordered_map<string, Guardian*>& guardians) {
    cout << "Options for guardians in " << node->name << ":" << endl;

    if (node->name != "Tesla") {
        Guardian* weakestGuardian = nullptr;
        int weakestPower = INT_MAX;

        int option = 1;
        for (const auto& entry : guardians) {
            Guardian* guardian = entry.second;
            if (guardian->village == node->name && guardian->powerLevel < weakestPower) {
                weakestGuardian = guardian;
                weakestPower = guardian->powerLevel;
            }
        }

        if (weakestGuardian != nullptr) {
            cout << "Recommendation: The weakest guardian in this village is " << weakestGuardian->name
                 << " (Power Level: " << weakestGuardian->powerLevel << ")." << endl;
        }
    }

    int option = 1;
    for (const auto& entry : guardians) {
        Guardian* guardian = entry.second;
        if (guardian->village == node->name) {
            cout << option << ". " << guardian->name << " (Power Level: " << guardian->powerLevel << ")" << endl;
            if (!guardian->apprentices.empty()) {
                cout << "    - Apprentices: ";
                for (Guardian* apprentice : guardian->apprentices) {
                    cout << apprentice->name << ", ";
                }
                cout << endl;
            }
            option++;
        }
    }
    cout << "0. Do not fight" << endl;
}

Guardian* findWeakestGuardian(Node* node, unordered_map<string, Guardian*>& guardians) {
    Guardian* weakestGuardian = nullptr;
    int weakestPower = INT_MAX;

    for (const auto& entry : guardians) {
        Guardian* guardian = entry.second;
        if (guardian->village == node->name && guardian->powerLevel < weakestPower) {
            weakestGuardian = guardian;
            weakestPower = guardian->powerLevel;
        }
    }

    return weakestGuardian;
}

void updateStrongestGuardian(unordered_map<string, Guardian*>& guardians) {
    unordered_map<string, Guardian*> strongestGuardians;

    // Encontrar al guardián más fuerte de cada aldea
    for (const auto& entry : guardians) {
        Guardian* guardian = entry.second;
        if (strongestGuardians.find(guardian->village) == strongestGuardians.end() || guardian->powerLevel > strongestGuardians[guardian->village]->powerLevel) {
            strongestGuardians[guardian->village] = guardian;
        }
    }

    // Agregar "(maestro)" al nombre del guardián más fuerte de cada aldea
    for (const auto& entry : strongestGuardians) {
        Guardian* strongestGuardian = entry.second;
        strongestGuardian->name += " (maestro)";
    }
}


Node* choosePath(Node* current, unordered_map<string, Guardian*>& guardians, Guardian* playerGuardian) {
	cout << "Guardian: " << playerGuardian->name << " (Power: " << playerGuardian->powerLevel << ")" << endl;
    cout << "You are in the village " << current->name << "." << endl;

    while (true) {
        showNeighborOptions(current);

        int choice;
        cout << "Choose your next village: ";
        cin >> choice;

        if (choice < 0 || choice > static_cast<int>(current->neighbors.size())) {
            cout << "Invalid option. Please try again." << endl;
        } else if (choice == 0) {
            return nullptr;  // The player chose to go back
        } else {
            Node* next = current->neighbors[choice - 1];

            showGuardianOptions(next, guardians);

            int guardianChoice;
            cout << "Choose the guardian you want to fight: ";
            cin >> guardianChoice;

            if (guardianChoice == 0) {
                return next;  // The player chose not to fight
            } else {
                Guardian* guardian = nullptr;
                int count = 1;
                for (const auto& entry : guardians) {
                    Guardian* g = entry.second;
                    if (g->village == next->name) {
                        if (count == guardianChoice) {
                            guardian = g;
                            break;
                        }
                        count++;
                    }
                }

                if (guardian != nullptr) {
                    cout << "Fighting against " << guardian->name << " (Power Level: " << guardian->powerLevel << ")..." << endl;
                    // Implement the fight logic with the guardian
                }

                return next;
            }
        }
    }
}

Guardian* creationPlayerGuardian(){
	
	
	
}

Guardian* choosePlayerGuardian(const unordered_map<string, Guardian*>& guardians, unordered_map<string, Node*>& nodes) {
    cout << "Choose your guardian character:" << endl;

    int option = 1;
    vector<Guardian*> guardianOptions; // Vector para almacenar las opciones de guardianes

    for (const auto& entry : guardians) {
        Guardian* guardian = entry.second;
        cout << option << ". " << guardian->name << endl;
        guardianOptions.push_back(guardian); // Agregar el guardián a las opciones
        option++;
    }

    int choice;
    cout << "Enter the number of your character: ";
    cin >> choice;

    // Validar la elección del jugador
    while (choice < 1 || choice > option - 1) {
        cout << "Invalid choice. Enter a valid number: ";
        cin >> choice;
    }

    // Obtener el personaje seleccionado
    Guardian* selectedGuardian = guardianOptions[choice - 1];

    // Crear un duplicado del personaje seleccionado
    Guardian* playerGuardian = new Guardian(*selectedGuardian);
    playerGuardian->powerLevel = 50; // Establecer el nivel de poder mínimo para el personaje duplicado

    string guardianVillage = playerGuardian->village;
    Node* playerVillage = findNode(guardianVillage, nodes);

    return playerGuardian;
}


void removeGuardianFromVillage(const string& guardianName, unordered_map<string, Guardian*>& guardians, unordered_map<string, Node*>& nodes) {
    
	Guardian* guardian = guardians[guardianName];
    
    if (guardian != nullptr) {
        string villageName = guardian->village;
        Node* villageNode = nodes[villageName];

        if (villageNode != nullptr) {
            for (auto it = villageNode->neighbors.begin(); it != villageNode->neighbors.end(); ++it) {
            	
                if ((*it)->name == guardianName) {
                    villageNode->neighbors.erase(it);
                    break;
                }
            }
        }
        
        // Rename Stormheart to Stormheart the Grandmaster
        if (guardianName == "Stormheart") {
            guardian->name = "Stormheart the Grandmaster";
        } else {
            // Remove the guardian from the guardian map
            guardians.erase(guardianName);
        }
    }
}

void fight(Guardian* playerGuardian, Guardian* enemyGuardian) {
	// Verificar si el guardian enemigo es un maestro
	bool isPlayerMaster = (enemyGuardian->name.find("maestro") != string::npos);
	
	 
}


int main() {
    unordered_map<string, Node*> nodes;
    unordered_map<string, Guardian*> guardians;
    Guardian* playerGuardian;

    createGraph("aldeas.txt", nodes);
    loadGuardians("guardianes.txt", guardians, nodes);

	cout << "Welcome to the Guardian Journey" << endl << endl;
	
    playerGuardian = choosePlayerGuardian(guardians, nodes);
    removeGuardianFromVillage(playerGuardian->name, guardians, nodes);

	// Actualizar los nombres de los guardianes más fuertes de cada aldea
    updateStrongestGuardian(guardians);

    Node* current = findNode(playerGuardian->village, nodes);
    
    while (current != nullptr) {
        current = choosePath(current, guardians, playerGuardian);
    }

    for (auto& entry : nodes) {
        delete entry.second;
    }

    for (auto& entry : guardians) {
        delete entry.second;
    }

    return 0;
}
