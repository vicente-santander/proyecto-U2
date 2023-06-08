#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <climits>

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

    Guardian(const string& n, int p, const string& master, const string& v) 
        : name(n), powerLevel(p), mainMaster(master), village(v) {}
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

Node* choosePath(Node* current, unordered_map<string, Guardian*>& guardians) {
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


void dice (){
	
}

Guardian* choosePlayerGuardian(const unordered_map<string, Guardian*>& guardians) {
    cout << "Choose your guardian character from guardian.txt:" << endl;

    int option = 1;
    for (const auto& entry : guardians) {
        Guardian* guardian = entry.second;
        cout << option << ". " << guardian->name << " (Power Level: " << guardian->powerLevel << ")" << endl;
        option++;
    }

    int choice;
    cout << "Enter the number of your character: ";
    cin >> choice;

    // Validate the player's choice
    while (choice < 1 || choice > option - 1) {
        cout << "Invalid choice. Enter a valid number: ";
        cin >> choice;
    }

    // Get the selected character
    option = 1;
    Guardian* playerGuardian = nullptr;
    for (const auto& entry : guardians) {
        Guardian* guardian = entry.second;
        if (option == choice) {
            playerGuardian = guardian;
            break;
        }
        option++;
    }

    cout << "You have chosen " << playerGuardian->name << " as your character." << endl;
    return playerGuardian;
}





int main() {
    unordered_map<string, Node*> nodes;
    unordered_map<string, Guardian*> guardians;

    createGraph("aldeas.txt", nodes);
    loadGuardians("guardianes.txt", guardians, nodes);
    
    // Permitir al jugador elegir su personaje
    Guardian* playerGuardian = choosePlayerGuardian(guardians);

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
