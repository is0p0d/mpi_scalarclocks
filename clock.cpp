#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "mpi.h"

void process(int id);
std::vector<std::string> read_events(int id);

int main(int argc, char *argv[])
{
    int maxRank = 0,
        pid = 0;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &maxRank);
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    if (pid == 0)
    {
        std::cout << "Clock program is running across " << maxRank << " processors, initializing..." << std::endl;
    }
    MPI_Barrier(MPI_COMM_WORLD);
    process(pid);
    MPI_Finalize();
    return 0;
}

void process (int id)
{
    MPI_Status status;
    int c = 0;
    std::string outName = "proccess" + std::to_string(id) + ".out";
    std::ofstream outFile (outName, std::ofstream::out);
    std::vector<std::string> events = read_events(id);
    outFile << "proc " << id << " event vector:\n";
    for (int i = 0; i < events.size(); i++)
    {
        outFile << events[i] << " ";
    }
    outFile << std::endl;

    for (int i = 0; i < events.size()-1; i++)
    {
        if (events[i][0] == 'e')
        {
            outFile << "Process " << id << " internal\n";

            c++; // this is like when they say the name of the movie in the movie
            outFile << "\tClock is " << c << std::endl;
        }
        else if (events[i][0] == 's')
        {
            int sendTo = atoi(&(events[i][1]));
            MPI_Send(&c, 1, MPI_INT, sendTo, 0, MPI_COMM_WORLD);
            
            outFile << "Process " << id << " sent: ";
            outFile << c << " to process " << sendTo << "\n";
            
            c++;
            outFile << "\tClock is " << c << std::endl;
        }
        else if (events[i][0] == 'r')
        {
            int recvFrom = atoi(&(events[i][1]));
            int recvClock = 0;
            MPI_Recv(&recvClock, 1, MPI_INT, recvFrom, 0, MPI_COMM_WORLD, &status);
            
            outFile << "Process " << id << " received: ";
            outFile << recvClock << " from process " << recvFrom << "\n";
            if (recvClock > c) c = recvClock;
            c++;
            outFile << "\tClock is " << c << std::endl;
        }
        else
        {
            outFile << "Process " << id << " error, event not recognized" << std::endl;
            break;
        }
    }
    outFile.close();
}

std::vector<std::string> read_events(int id)
{
    std::vector<std::string> eventVector;
    std::ifstream eventsFile ("events.dat", std::ifstream::in);
    std::string temp = "\0";

    eventsFile >> temp;
    while ( std::string::npos == temp.find(std::to_string(id)))
    {
        //std::cout << temp << " is not " << id << "; skipping." << std::endl;
        eventsFile.ignore(1024, '\n');
        eventsFile >> temp;
    }
    //std::cout << temp << " found, vectorizing..." << std::endl;
    std::getline(eventsFile, temp);
    //std::cout << temp << std::endl;
    std::istringstream ss(temp);
    while (ss)
    {
        std::string token;
        ss >> token;
        //std::cout << "Token: " << token << std::endl;
        eventVector.push_back(token);
    }
    eventsFile.close();
    return eventVector;
}
