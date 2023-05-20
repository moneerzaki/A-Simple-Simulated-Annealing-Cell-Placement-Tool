#include <iostream>
#include <vector>   // using a vector of struct cells and nets
#include <algorithm>// sort ...
#include <cmath>    // using math functions
#include <random>   // for random generating cells positions
#include <iomanip>  //
#include <fstream>  // For file input/output

// for flushing the console out and reprint the outut in the same line of code..
#include <chrono>
#include <thread>



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

// clearing the outupt of the grid
void clearGrid(int ROWS)
{
    // Clear the grid portion of the console
    for (int i = 0; i < ROWS; ++i) {
        std::cout << "\033[1A\033[2K";
    }
}

// function that prints the grid out
void printGrid(const std::vector<Cell>& initialPlacement, int numRows, int numCols, int cellWidth) {
    std::vector<std::vector<int>> grid(numRows, std::vector<int>(numCols, -1));

    
    for (const Cell& cell : initialPlacement) {
        grid[cell.row][cell.column] = cell.number;
    }

    // Print the grid
    cout << endl << endl << " ------------- start  grid ----------------" << endl;
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
    cout << " ------------- end  grid ----------------" << endl;

}

//function that prints the grid out in binary
void printGridBinary(const std::vector<Cell>& initialPlacement, int numRows, int numCols, int cellWidth) {
    std::vector<std::vector<int>> grid(numRows, std::vector<int>(numCols, -1));


    for (const Cell& cell : initialPlacement) {
        grid[cell.row][cell.column] = cell.number;
    }

    // Print the grid
    cout << endl << endl << " ------------- start  grid ----------------" << endl;
    for (const std::vector<int>& row : grid) {
        for (const int& number : row) {
            if (number != -1) {
                std::cout << std::setw(cellWidth) << "0" << " ";
            } else {
                std::cout << std::setw(cellWidth) << "1" << " ";
            }
        }
        std::cout << std::endl;
    }
    cout << " ------------- end  grid ----------------" << endl;

}


// Function to calculate the wirelength of a net
int calculateNetWirelength(const std::vector<Cell>& cells, const Net& net) {
    int minX = cells[net.connectedComponents[0]].column;
    int maxX = minX;
    int minY = cells[net.connectedComponents[0]].row;
    int maxY = minY;
    
    // cout << cells[net.connectedComponents[0]].number << " of col ("<< cells[net.connectedComponents[0]].column
    // << ") and row (" << cells[net.connectedComponents[0]].column << ")" << endl;

    for (size_t i = 1; i < net.connectedComponents.size(); ++i) {
        int col = cells[net.connectedComponents[i]].column;
        int row = cells[net.connectedComponents[i]].row;
        minX = std::min(minX, col);
        maxX = std::max(maxX, col);
        minY = std::min(minY, row);
        maxY = std::max(maxY, row);
        
    //     cout << cells[net.connectedComponents[i]].number << " of col ("<< cells[net.connectedComponents[i]].column
    // << ") and row (" << cells[net.connectedComponents[i]].column << ")" << endl;
    }

    // cout << endl    << "-------------- maxX (" << maxX << ") - minX ("<< minX << ") ="<< maxX-minX << endl;
    // cout            << "-------------- maxY (" << maxY << ") - minY ("<< minY << ") ="<< maxY-minY << endl;
    return (maxX - minX) + (maxY - minY);
}

// Function to calculate the total wirelength of the placement
int calculateTotalWirelength(const std::vector<Cell>& cells, const std::vector<Net>& nets) {
    
    int totalWirelength = 0;
    // cout << endl << "number of nets = " << nets.size() << endl;
    for (const Net& net : nets) {
        
        // cout << endl;
        // for (int i =0; i<net.connectedComponents.size(); i++)
        // {
        //     cout << net.connectedComponents[i] << " ";
        // }
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
void performPlacement(  std::vector<Cell>& currentPlacement, const std::vector<Net>& nets, double initialTemperature, double finalTemperature, double coolingRate,int numRows, int numCols) {
    int currentWirelength = calculateTotalWirelength(currentPlacement, nets);
    int numCells = currentPlacement.size();
    // int numRows = /* number of rows in the grid */;
    // int numCols = /* number of columns in the grid */;


    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<int> cellDistribution(0, numCells - 1);
    std::uniform_int_distribution<int> rowDistribution(0, numRows - 1);
    std::uniform_int_distribution<int> colDistribution(0, numCols - 1);


    double currentTemperature = initialTemperature;

    cout << endl;
    auto startTime = std::chrono::high_resolution_clock::now();
    int counter =0;
    while (currentTemperature > finalTemperature) {
        int movesPerTemperature = 10 * numCells;

        for (int i = 0; i < movesPerTemperature; ++i) {
            

            int randomCellIndex = cellDistribution(generator);
            Cell& cell = currentPlacement[randomCellIndex];

            

            int oldRow = cell.row;
            int oldCol = cell.column;

            int newRow = rowDistribution(generator);
            int newCol = colDistribution(generator);

            // Check if the new placement is within the grid boundaries
            if (newRow >= 0 && newRow < numRows && newCol >= 0 && newCol < numCols) {
                // Check if the new placement overlaps with any other cell
                bool overlap = false;
                for (const Cell& otherCell : currentPlacement) {
                    if (&otherCell != &cell && otherCell.row == newRow && otherCell.column == newCol) {
                        overlap = true;
                        break;
                    }
                }

                if (!overlap) {
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
            }
        }
        

        // Cool down the temperature
        currentTemperature *= coolingRate;
        
        // current time
        auto endTime = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(endTime - startTime).count();
        
        if (counter)
        clearGrid(numRows+4);
        
        cout << "\r                        iteration: " << counter << "    current temp: " << currentTemperature  << "         elapsed time: " << duration << std::flush;
        printGrid(currentPlacement, numRows, numCols, 4);
        
        counter ++;
    }
    //printGridBinary(currentPlacement, numRows, numCols, 4);
    
    cout << endl;
    auto endTime = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(endTime - startTime).count();
    
    
    cout << "                        Execution time: " << duration << " milliseconds" << endl;
     

}


int main(void) {
    int numCells, numNets, numRows, numCols;
    int cellWidth = 4;

    std::ifstream inputFile("/Users/ahmedgouda/Desktop/t3.txt");
    if (inputFile.is_open()) {
        inputFile >> numCells >> numNets >> numRows >> numCols;

        std::vector<Cell> cells;
        cells.reserve(numCells);
        // Read cell information
        // for (int i = 0; i < numCells; ++i) {
        //     int cellNum = i;
        //     // inputFile >> cellNum;

        //     Cell cell;
        //     cell.number = cellNum;
        //     cells.push_back(cell);
        // }

        std::vector<Net> nets;
        nets.reserve(numNets);
        // Read net information
        for (int i = 0; i < numNets; ++i) {
            int numComponents;
            inputFile >> numComponents;
            // cout << endl << " number of components: "<< numComponents;

            Net net;
            net.connectedComponents.resize(numComponents);
            for (int j = 0; j < numComponents; ++j) {
                inputFile >> net.connectedComponents[j];
            }

            nets.push_back(net);
        }
        inputFile.close();


        // Perform the initial random placement
        vector<Cell> initialPlacement = performInitialPlacement(numRows, numCols, numCells);
        // Display the grid
        
        //printGrid(initialPlacement, numRows, numCols, cellWidth);

        // checking the nets.
        /*
        // cout << "number of nets =" << nets.size();
        // for (int i =0; i<nets.size(); i++)
        // {
        //     cout << endl << " net (" << i << ") of size (" << nets[i].connectedComponents.size() << ") and components:";
        //     for (int j = 0; j<nets[i].connectedComponents.size(); j++)
        //     {
        //         cout << nets[i].connectedComponents[j] << " ";
        //     }
        // }
        */

        // Calculate the initial and final temperatures based on the given formula
        double total_wire_length = calculateTotalWirelength(initialPlacement, nets);
            cout << endl << " total wire length is: " << total_wire_length;
        double initialTemperature = 500 * total_wire_length;
            cout << endl << " inital temperature = " << total_wire_length << " * 500 = " <<  initialTemperature;
        double finalTemperature = 5e-6 * total_wire_length / numNets;
            cout << endl << " final temperature is: " << finalTemperature;

        // Define the cooling rate
        double coolingRate = 0.9;

        // Perform the simulated annealing-based placement
        vector<Cell>finalPlacement = initialPlacement;
        performPlacement(finalPlacement, nets, initialTemperature, finalTemperature, coolingRate, numRows, numCols);
        
        //printGrid(finalPlacement, numRows, numCols, cellWidth);
        printGridBinary(finalPlacement, numRows, numCols, cellWidth);
                
                cout << endl << " inital temperature = " << total_wire_length << " * 500 = " <<  initialTemperature;
                cout << endl << " final temperature is: " << finalTemperature;
                
                total_wire_length = calculateTotalWirelength(finalPlacement, nets);
                cout << endl << " total wire length is: " << total_wire_length << endl;
                
                cout << "cooling rate is: " << coolingRate << endl;
        
        // cellWidth = 4;
        // for (int i = 0; i < finalPlacement.size(); i++)
        // {
        //     cout << endl << " cell " << i << " of row (" << finalPlacement[i].row << ") of col (" << finalPlacement[i].column << ") .";
        // }
        // cout << endl << "numRows =" << numRows << "    numCols =" << numCols << " cellWidth =" << cellWidth ;
        // printGrid(finalPlacement, numRows, numCols, cellWidth);
        
        // Save the final placement and wirelength to an output file
        // std::ofstream outputFile("output.txt");
        // if (outputFile.is_open()) {
        //     // Print the final placement
        //     for (const Cell& cell : initialPlacement) {
        //         outputFile << "-- " << cell.number << " ";
        //         outputFile << cell.row << " ";
        //         outputFile << cell.column << std::endl;
        //     }

        //     // Print the final wirelength
        //     int totalWirelength = calculateTotalWirelength(initialPlacement, nets);
        //     outputFile << "Total wire length = " << totalWirelength << std::endl;

        //     outputFile.close();
        // } else {
        //     std::cout << "Failed to open output file." << std::endl;
        //     return 1; // Exit the program with an error code
        // }
    } else {
        std::cout << "Failed to open input file." << std::endl;
        return 1; // Exit the program with an error code
    }

    return 0; // Exit the program successfully
}







