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
	int score; // Puntaje de la aldea
	vector<string> trainingResults; // Historial de resultados de entrenamientos
	
    Node(const string& n) : name(n), score(0) {}
};

struct Guardian {
    string name;
    int powerLevel;
    string mainMaster;
    string village;
    vector<Guardian*> apprentices;

    Guardian(const string& n, int p, const string& mm, const string& v) : name(n), powerLevel(p), mainMaster(mm), village(v) {}
};

// Función para encontrar un nodo en el mapa de nodos dados su nombre
Node* findNode(const string& name, unordered_map<string, Node*>& nodes) {
	
    auto it = nodes.find(name); // realiza una búsqueda en el contenedor unordered_map llamado "nodes" utilizando el valor de "name"
    if (it != nodes.end()) { // busca un elemento en un unordered_map y devuelve su valor si se encuentra
        return it->second;
    }
    return nullptr; // nullptr si no se encuentra
}

// Función para crear el grafo de aldeas a partir de un archivo de texto
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
        // Buscar o crear el nodo "from"
        if (iss >> from >> to) {
            Node* fromNode = findNode(from, nodes);
            if (fromNode == nullptr) {
                fromNode = new Node(from);
                nodes[from] = fromNode;
            }
            
			// Buscar o crear el nodo "to"
            Node* toNode = findNode(to, nodes);
            if (toNode == nullptr) {
                toNode = new Node(to);
                nodes[to] = toNode;
            }
            
            // Agregar conexiones entre los nodos "from" y "to"
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


void showNeighborOptions(Node* node) {
    cout << "Opciones de aldeas vecinas:" << endl;
    for (int i = 0; i < node->neighbors.size(); ++i) {
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
    
    int weakestPower = INT_MAX; // el máximo valor posible para un int
    
	// Encontrar al guardián más debil de cada aldea
    for (const auto& entry : guardians) { // itera a través de cada elemento en la colección guardians, Cada elemento se almacena en la variable entry
    	
        Guardian* guardian = entry.second; // extrae el valor del elemento actual en entry y lo asigna a un puntero a objeto Guardian
        
        // Compara si el nombre del pueblo es igual al nombre del nodo 
        if (guardian->village == node->name && guardian->powerLevel < weakestPower) {  // verifica si el nivel de poder del guardián es más débil que el nivel de poder más débil encontrado hasta el momento.
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

int GetDiceRoll(){
	srand(time(0));
	int dice;
	
	dice = rand()%20+1;
	return dice;
}


Node* choosePath(Node* current, unordered_map<string, Guardian*>& guardians, Guardian* playerGuardian, vector<string>& full_journey, vector<string>& journey) {
	
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
            
            full_journey.push_back(next->name);  // Agregar el nombre de la aldea al recorrido completo
            journey.push_back(next->name);  // Agregar el nombre de la aldea al recorrido
            
            cout << "Score:" << next->score << endl;
            // Actualizar el puntaje de la aldea actual
            next->score += 1;
            playerGuardian->powerLevel++;
            cout << "Score:" << next->score << endl;
			cout << "Guardian: " << playerGuardian->name << " (Power: " << playerGuardian->powerLevel << ")" << endl;
			
			
            if (next->score >= 4) {
                cout << "You have reached the maximum score for this village. You can continue the journey without training." << endl;
                return next;
            }

            
            showGuardianOptions(next, guardians);

            int guardianChoice;
            cout << "Choose the guardian you want to fight: ";
            cin >> guardianChoice;

            if (guardianChoice == 0) {
            	full_journey.push_back(current->name);
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
                    // Verificar si el guardian enemigo es un maestro
					bool isPlayerMaster = (guardian->name.find("maestro") != string::npos);
					int result = 0;
					
					if (isPlayerMaster){
						
						//result = ((static_cast<double>(playerGuardian->powerLevel) / enemyGuardian->powerLevel) * 75);
						result = GetDiceRoll();
						cout << result << endl;
						
						if(result > 12){
							cout << "Successful training" << endl;
							next->trainingResults.push_back("Successful training");
							playerGuardian->powerLevel = playerGuardian->powerLevel + 2;
							next->score += 2;
							cout << "Score:" << next->score << endl;
						}else{
							cout << "Failed training" << endl;
							next->trainingResults.push_back("Failed training");
						}
					}else{
						
						//result = ((static_cast<double>(playerGuardian->powerLevel) / enemyGuardian->powerLevel) * 100);
						result = GetDiceRoll();
						cout << result << endl;
						
						if(result >= 10){
							cout << "Successful training" << endl;
							next->trainingResults.push_back("Successful training");
							playerGuardian->powerLevel++;
							next->score += 1;
							cout << "Score:" << next->score << endl;
						}else{
							cout << "Failed training" << endl;
							next->trainingResults.push_back("Failed training");
						}
						
					}
					 
                }
                
                // Verificar si se alcanzó el puntaje máximo
                if (next->score >= 4) {
                    cout << "Congratulations! You have reached the maximum score for this village." << endl;
                }
				
				Node* next = current->neighbors[choice - 1];
                return next;
            }
        }
    }
}

Guardian* creationPlayerGuardian(const unordered_map<string, Node*>& villages) {
    string playerName;
    cout << "Enter your guardian's name: ";
    cin >> playerName;

    cout << "Available villages to choose from:" << endl;
    int count = 1;
    for (const auto& entry : villages) {
        cout << count << ". " << entry.first << endl;
        count++;
    }

    int villageChoice;
    cout << "Choose the village for your guardian: ";
    cin >> villageChoice;

    count = 1;
    Node* chosenVillage = nullptr;
    for (const auto& entry : villages) {
        if (count == villageChoice) {
            chosenVillage = entry.second;
            break;
        }
        count++;
    }

    if (chosenVillage == nullptr) {
        cout << "Invalid village choice. Guardian creation failed." << endl;
        return nullptr;
    }
	
    int powerLevel = 50;
    string master = "";
    
    Guardian* playerGuardian = new Guardian(playerName, powerLevel, master, chosenVillage->name);
    
    return playerGuardian;
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

void alquimista(unordered_map<string, Node*>& nodes, Guardian* playerGuardian) {
    srand(time(0)); // Inicializar la semilla para generar números aleatorios

    cout << "Welcome to the Alchemists' Path Creation!" << endl << endl;

    while (true) {
        cout << "Available villages to choose from:" << endl;
        int count = 1;
        for (const auto& entry : nodes) {
            cout << count << ". " << entry.first << endl;
            count++;
        }
        
        

        int villageChoice;
        cout << "Choose the village to create a path from: ";
        cin >> villageChoice;

        count = 1;
        Node* fromVillage = nullptr;
        for (const auto& entry : nodes) {
            if (count == villageChoice) {
                fromVillage = entry.second;
                break;
            }
            count++;
        }

        if (fromVillage == nullptr) {
            cout << "Invalid village choice. Path creation canceled." << endl;
            break;
        }

        cout << "Choose the village to create a path to: ";
        cin >> villageChoice;

        count = 1;
        Node* toVillage = nullptr;
        for (const auto& entry : nodes) {
            if (count == villageChoice) {
                toVillage = entry.second;
                break;
            }
            count++;
        }

        if (toVillage == nullptr) {
            cout << "Invalid village choice. Path creation canceled." << endl;
            break;
        }

        // Verificar si ya existe un camino entre las aldeas seleccionadas
        bool pathExists = false;
        for (Node* neighbor : fromVillage->neighbors) {
            if (neighbor == toVillage) {
                pathExists = true;
                break;
            }
        }

        if (pathExists) {
            cout << "There is already a path between " << fromVillage->name << " and " << toVillage->name << "." << endl;
            break;
        }

        cout << "Path created successfully between " << fromVillage->name << " and " << toVillage->name << "!" << endl;

        // Reducir el nivel de poder del jugador en el camino
        int sacrifice = rand() % 3 + 2; // Generar un número aleatorio entre 2 y 4
        playerGuardian->powerLevel -= sacrifice;
        if (playerGuardian->powerLevel < 0) {
            playerGuardian->powerLevel = 0; // Asegurarse de que el nivel de poder no sea negativo
        }

        // Establecer la conexión entre las aldeas
        fromVillage->neighbors.push_back(toVillage);
        toVillage->neighbors.push_back(fromVillage);

        break;
    }
}




int main() {
    unordered_map<string, Node*> nodes;
    unordered_map<string, Guardian*> guardians;
    vector<string> full_journey;  // Vector para almacenar el recorrido completo del guardián
    vector<string> journey;  // Vector para almacenar el recorrido del guardián
    Guardian* playerGuardian;
	int op;
	
    createGraph("aldeas.txt", nodes);
    
	cout << "Welcome to the Guardian Journey" << endl << endl;
	
	cout << "Menu" << endl;
	cout << "1. creation Player" << endl;
	cout << "2. choose Player" << endl;
	
	cin >> op;
	// Validar la elección del jugador
    while (op < 0 || op > 2) {
        cout << "Invalid choice. Enter a valid number: ";
        cin >> op;
    }
	
	switch(op){
		case 1:
			playerGuardian = creationPlayerGuardian(nodes);
			if (playerGuardian == nullptr) {
                cout << "Guardian creation failed. Exiting the program." << endl;
                return 0;
            }
			loadGuardians("guardianes.txt", guardians, nodes);
			
			break;
		case 2:
			loadGuardians("guardianes.txt", guardians, nodes);
			playerGuardian = choosePlayerGuardian(guardians, nodes);
			removeGuardianFromVillage(playerGuardian->name, guardians, nodes);
			break;
	}    
    
    

	// Actualizar los nombres de los guardianes más fuertes de cada aldea
    updateStrongestGuardian(guardians);
	//cout << playerGuardian->village;
	
    Node* current = findNode(playerGuardian->village, nodes);
    full_journey.push_back(current->name);
    
    
    while (current != nullptr) {
        current = choosePath(current, guardians, playerGuardian, full_journey, journey);
        
        cout << "You want to use the alchemist function?" << endl;
        cout << "1. Yes" << endl << "2. No" << endl;
        int al;
        cout << "Option: ";
        cin >> al;
        switch(al){
        	case 1:
        		alquimista(nodes, playerGuardian);
        		break;
        	case 2:
        		break;
        	default:
        		cout << "Invalid option, closing alchemy" << endl;
        		break;
		}
		
    }
    
    cout << "Guardian " << playerGuardian->name << " completed the journey!" << endl;
    cout << "Full Journey: ";
    for (const auto& village : full_journey) {
        cout << village << " -> ";
    }
    cout << "End" << endl << endl;
    
    
    cout << "Journey: ";
    for (const auto& village : journey) {
        cout << village << " -> ";
    }
    cout << "End" << endl << endl;
    
    cout << "Training history:" << endl;
    for (const string& villageName : full_journey) {
        Node* village = nodes[villageName];
        cout << "Village: " << villageName << endl;
        cout << "Training results: ";
        for (string string : village->trainingResults) {
            cout << string << " ";
        }
        cout << endl;
    }
    

    for (auto& entry : nodes) {
        delete entry.second;
    }

    for (auto& entry : guardians) {
        delete entry.second;
    }

    return 0;
}
