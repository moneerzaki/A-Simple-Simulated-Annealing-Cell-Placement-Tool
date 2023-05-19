#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <random>
#include <iomanip>


using namespace std; 

// Structure to represent a cell/component
struct Cell {
    int number;
    int row;
    int column;
};

// Structure to represent a net
struct Net {
    std::vector<int> connectedComponents;
};

// function that prints the grid out
void printGrid(const std::vector<Cell>& initialPlacement, int numRows, int numCols, int cellWidth) {
    std::vector<std::vector<int>> grid(numRows, std::vector<int>(numCols, -1));

    for (const Cell& cell : initialPlacement) {
        grid[cell.row][cell.column] = cell.number;
    }

    // Print the grid
    for (const std::vector<int>& row : grid) {
        for (const int& number : row) {
            if (number != -1) {
                std::cout << std::setw(cellWidth) << number << " ";
            } else {
                std::cout << std::setw(cellWidth) << "-" << " ";
            }
        }
        std::cout << std::endl;
    }
}


// Function to calculate the wirelength of a net
int calculateNetWirelength(const std::vector<Cell>& cells, const Net& net) {
    int minX = cells[net.connectedComponents[0]].column;
    int maxX = minX;
    int minY = cells[net.connectedComponents[0]].row;
    int maxY = minY;
    
    cout << cells[net.connectedComponents[0]].number << " ";

    for (size_t i = 1; i < net.connectedComponents.size(); ++i) {
        int col = cells[net.connectedComponents[i]].column;
        int row = cells[net.connectedComponents[i]].row;
        minX = std::min(minX, col);
        maxX = std::max(maxX, col);
        minY = std::min(minY, row);
        maxY = std::max(maxY, row);
        
        cout << cells[net.connectedComponents[i]].number << " "; 
    }

    cout << endl    << "-------------- maxX - minX ="<< maxX-minX << endl;
    cout            << "-------------- maxY - minY ="<< maxY-minY << endl;
    return (maxX - minX) + (maxY - minY);
}

// Function to calculate the total wirelength of the placement
int calculateTotalWirelength(const std::vector<Cell>& cells, const std::vector<Net>& nets) {
    int totalWirelength = 0;
    for (const Net& net : nets) {
        totalWirelength += calculateNetWirelength(cells, net);
    }
    return totalWirelength;
}

// Function to perform a random initial placement
std::vector<Cell> performInitialPlacement(int numRows, int numCols, int numCells) {
    std::vector<Cell> cells;
    cells.reserve(numCells);

    std::random_device rd;
    std::mt19937 generator(rd());

    std::vector<int> indices(numRows * numCols);
    for (int i = 0; i < numRows * numCols; ++i) {
        indices[i] = i;
    }
    std::shuffle(indices.begin(), indices.end(), generator);

    for (int i = 0; i < numCells; ++i) {
        int row = indices[i] / numCols;
        int col = indices[i] % numCols;
        cells.push_back({ i, row, col });
    }

    return cells;
}

// Function to perform simulated annealing-based placement
// Function to perform simulated annealing-based placement
// Function to perform simulated annealing-based placement
void performPlacement(std::vector<Cell>& currentPlacement, const std::vector<Net>& nets,                      double initialTemperature, double finalTemperature, double coolingRate) {
    int currentWirelength = calculateTotalWirelength(currentPlacement, nets);
    int numCells = currentPlacement.size();

    double currentTemperature = initialTemperature;

    while (currentTemperature > finalTemperature) {
        int movesPerTemperature = 10 * numCells;

        for (int i = 0; i < movesPerTemperature; ++i) {
            std::random_device rd;
            std::mt19937 generator(rd());
            std::uniform_int_distribution<int> cellDistribution(0, numCells - 1);

            int randomCellIndex = cellDistribution(generator);
            Cell& cell = currentPlacement[randomCellIndex];

            std::uniform_int_distribution<int> rowDistribution(0, numCells - 1);
            std::uniform_int_distribution<int> colDistribution(0, numCells - 1);

            int oldRow = cell.row;
            int oldCol = cell.column;

            int newRow = rowDistribution(generator);
            int newCol = colDistribution(generator);

            cell.row = newRow;
            cell.column = newCol;

            int newWirelength = calculateTotalWirelength(currentPlacement, nets);

            // Calculate the change in wirelength
            int deltaWirelength = newWirelength - currentWirelength;

            // Accept the new placement if it improves the wirelength or based on the acceptance probability
            if (deltaWirelength < 0 || std::exp(-deltaWirelength / currentTemperature) > static_cast<double>(rand()) / RAND_MAX) {
                currentWirelength = newWirelength;
            } else {
                // Revert the placement change
                cell.row = oldRow;
                cell.column = oldCol;
            }
        }

        // Cool down the temperature
        currentTemperature *= coolingRate;
    }
}



int main(void) {
    int numCells, numNets, numRows, numCols;
    std::cin >> numCells >> numNets >> numRows >> numCols;

    std::vector<Cell> cells;
    cells.reserve(numCells);
    // Read cell information
    for (int i = 0; i < numCells; ++i) {
        int cellNum;
        std::cin >> cellNum;

        Cell cell;
        cell.number = cellNum;
        cells.push_back(cell);
    }

    std::vector<Net> nets;
    nets.reserve(numNets);
    // Read net information
    for (int i = 0; i < numNets; ++i) {
        int numComponents;
        std::cin >> numComponents;

        Net net;
        net.connectedComponents.resize(numComponents);
        for (int j = 0; j < numComponents; ++j) {
            std::cin >> net.connectedComponents[j];
        }

        nets.push_back(net);
    }

    // // Calculate the initial and final temperatures based on the given formula
    // double initialTemperature = 500 * calculateTotalWirelength(cells, nets);
    // double finalTemperature = 5e-6 * calculateTotalWirelength(cells, nets) / numNets;

    // // Define the cooling rate
    // double coolingRate = 0.95;

    // Perform the initial random placement 
    std::vector<Cell> initialPlacement = performInitialPlacement(numRows, numCols, numCells);


    
    // display the grid 
    int cellWidth = 4;
    printGrid(initialPlacement, numRows, numCols, cellWidth);
    
    // Perform the simulated annealing-based placement
    // performPlacement(initialPlacement, nets, initialTemperature, finalTemperature, coolingRate);

    // // Print the final placement
    // for (const Cell& cell : initialPlacement) {
    //     std::cout << "-- " << cell.number << " ";
    //     std::cout << cell.row << " ";
    //     std::cout << cell.column << std::endl;
    // }

    // // Print the final wirelength
    // int totalWirelength = calculateTotalWirelength(initialPlacement, nets);
    // std::cout << "Total wire length = " << totalWirelength << std::endl;



    return 0;
}







