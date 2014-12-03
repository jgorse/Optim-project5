// Project 5

#include <iostream>
#include <limits.h>
#include "d_except.h"
#include <list>
#include <fstream>
#include "d_matrix.h"
#include "graph.h"

using namespace std;

class maze
{
   public:
      maze(ifstream &fin);
      void print(int,int,int,int);
      bool isLegal(int i, int j);

      void setMap(int i, int j, int n);
      int getMap(int i, int j) const;
      void mapMazeToGraph(graph &g);
	  void findPathNonRecursive(graph &g);

	  bool findPathRecursive(node &n, graph &g);
	  bool findShortestPath1(node &n, graph &g);
	  vector<string> output;
   private:
      int rows; // number of rows in the maze
      int cols; // number of columns in the maze

      matrix<bool> value;
      matrix<int> map;      // Mapping from maze (i,j) values to node index values
};

void maze::setMap(int i, int j, int n)
// Set mapping from maze cell (i,j) to graph node n. 
{
}

int maze ::getMap(int i, int j) const
// Return mapping of maze cell (i,j) in the graph.
{
	return 0;
}

maze::maze(ifstream &fin)
// Initializes a maze by reading values from fin.  Assumes that the
// number of rows and columns indicated in the file are correct.
{
   fin >> rows;
   fin >> cols;

   char x;

   value.resize(rows,cols);
   for (int i = 0; i <= rows-1; i++)
      for (int j = 0; j <= cols-1; j++)
      {
	 fin >> x;
	 if (x == 'O')
            value[i][j] = true;
	 else
	    value[i][j] = false;
      }

   map.resize(rows,cols);
}

void maze::print(int goalI, int goalJ, int currI, int currJ)
// Print out a maze, with the goal and current cells marked on the
// board.
{
   cout << endl;

   if (goalI < 0 || goalI > rows || goalJ < 0 || goalJ > cols)
      throw rangeError("Bad value in maze::print");

   if (currI < 0 || currI > rows || currJ < 0 || currJ > cols)
      throw rangeError("Bad value in maze::print");

   for (int i = 0; i <= rows-1; i++)
   {
      for (int j = 0; j <= cols-1; j++)
      {
	 if (i == goalI && j == goalJ)
	    cout << "*";
	 else
	    if (i == currI && j == currJ)
	       cout << "+";
	    else
	       if (value[i][j])
		  cout << " ";
	       else
		  cout << "X";	  
      }
      cout << endl;
   }
   cout << endl;
}

bool maze::isLegal(int i, int j)
// Return the value stored at the (i,j) entry in the maze.
{
   if (i < 0 || i > rows || j < 0 || j > cols)
      throw rangeError("Bad value in maze::isLegal");

   return value[i][j];
}

void maze::mapMazeToGraph(graph &g)
// Create a graph g that represents the legal moves in the maze m.
{
	int first, second, totalnodes = cols*rows;

	//create all nodes of the graph (one for every maze coordinate)
	for(int x=0; x < totalnodes; x++)
		g.addNode();

	//create all edges. current cell is i, j
	for(int i =0; i<rows; i++)
	{
		for(int j=0; j<cols; j++)
		{
			//If current cell is not empty, no edges can connect here
			if(!value[i][j])
				continue;
			
			
			//Up, (i-1, j)
			if(i != 0)
				if(isLegal(i-1, j))
				{
					if(value[i-1][j])
					{
						first = i*cols + j;
						second = (i-1)*cols + j;
						g.addEdge(first, second);
					}
				}	

			//Down (i+1, j)
			if(i != rows-1)
				if(isLegal(i+1, j))
				{
					if(value[i+1][j])
					{
						first = i*cols + j;
						second = (i+1)*cols + j;
						g.addEdge(first, second);
					}
				}

			//left (i, j-1)
			if(j != 0)
				if(isLegal(i, j-1))
				{
					if(value[i][j-1])
					{
						first = i*cols + j;
						second = i*cols + j-1;
						g.addEdge(first, second);
					}
				}

			//right (i, j+1)
			if(j != cols-1)
				if(isLegal(i, j+1))
				{
					if(value[i][j+1])
					{
						first = i*cols + j;
						second = i*cols + j+1;
						g.addEdge(first, second);
					}
				}

		}
	}
	
}

bool maze::findPathRecursive(node &n, graph &g)
{
	n.visit();

	if (n.getId() == g.numNodes()-1)
	{
		output.push_back("End of maze.\n");
		return true;
	}
	else if (g.allNodesVisited()){
		return false;
	}
	for (int i = 0; i < g.numNodes(); i++){
		if (g.isEdge(n.getId(),i) && !g.isVisited(i) && !g.isMarked(i)){
			if (i == n.getId()+1)
				output.push_back("right");
			if (i == n.getId()-1)
				output.push_back("left");
			if (i == n.getId()+cols)
				output.push_back("down");
			if (i == n.getId()-cols)
				output.push_back("up");
			if (findPathRecursive(g.getNode(i),g))
				return true;
		}
	}
	//dead end?
	n.mark(); //using mark for paths already taken
	n.unVisit();
	//visit adjacent visited but unmarked node
	for(int temp = g.numNodes()-1; temp >= 0; temp--)
	{	
		if(g.isEdge(n.getId(), temp)) //if current has an adjcent node
		{
			if(g.getNode(temp).isVisited() && !g.getNode(temp).isMarked())//if adjacent node is unvisited
			{
				if (!output.empty())
					output.pop_back();
				if (findPathRecursive(g.getNode(temp),g))
					return true;
				else
					return false;
			}
		}
	}
}

void maze::findPathNonRecursive(graph &g)
{
	vector<string> moves;
	bool win = false;

	int current = 0, next = 0, numnodes = g.numNodes();
	g.getNode(current).visit();

	for(next = numnodes-1; next >= 0; next--)//check every node
	{
		if(current == numnodes - 1)
		{
			win = true;
			break;
		}
		if(g.isEdge(current, next)) //if current has an adjcent node
		{
			if(!g.getNode(next).isVisited())//if adjacent node is unvisited
			{
				g.getNode(next).visit();//visit it and restart for loop
				if(next == current+1)
					moves.push_back("right");
				if(next == current-1)
					moves.push_back("left");
				if(next == current+cols)
					moves.push_back("down");
				if(next == current-cols)
					moves.push_back("up");

				current = next;
				next = numnodes;
				continue;
			}
		}
		if(next == 0)//no adjacent unvisited nodes
		{
			g.getNode(current).mark();//mark current node so we don't go back to it
			
			//visit adjacent visited but unmarked node
			for(int temp = numnodes-1; temp >= 0; temp--)
			{	
				if(g.isEdge(current, temp)) //if current has an adjcent node
				{
					if(g.getNode(temp).isVisited() && !g.getNode(temp).isMarked())//if adjacent node is unvisited
					{
						moves.pop_back();
						current = temp;//move back to it
						next = numnodes;//reset for next outer 'for' loop run
						temp = 0;//break out of this 'for' loop
						continue;
					}
				}
			}
		}
	}
	
	if(win)
	{
		int i = 0, j = 0, count = moves.size();
		for(int x = 0; x<count; x++)
		{
			print(7, 10, i, j);
			cout<<"\n\n"<<moves[x]<<"\n";

			if(moves[x] == "up")
				i--;
			if(moves[x] == "down")
				i++;
			if(moves[x] == "left")
				j--;
			if(moves[x] == "right")
				j++;
			
		}
		print(7, 10, i, j);
	}
	else
	{
		cout<<"No solution to this puzzle\n";
	}

}

bool maze::findShortestPath1(node &n, graph &g)
{
	n.visit();

	if (n.getId() == g.numNodes()-1)
	{
		output.push_back("End of maze.\n");
		return true;
	}
	else if (g.allNodesVisited()){
		return false;
	}
	for (int i = 0; i < g.numNodes(); i++){
		if (g.isEdge(n.getId(),i) && !g.isVisited(i) && !g.isMarked(i)){
			if (i == n.getId()+1)
				output.push_back("right");
			if (i == n.getId()-1)
				output.push_back("left");
			if (i == n.getId()+cols)
				output.push_back("down");
			if (i == n.getId()-cols)
				output.push_back("up");
			if (findPathRecursive(g.getNode(i),g))
				return true;
		}
	}
	//dead end?
	n.mark(); //using mark for paths already taken
	n.unVisit();
	//visit adjacent visited but unmarked node
	for(int temp = g.numNodes()-1; temp >= 0; temp--)
	{	
		if(g.isEdge(n.getId(), temp)) //if current has an adjcent node
		{
			if(g.getNode(temp).isVisited() && !g.getNode(temp).isMarked())//if adjacent node is unvisited
			{
				if (!output.empty())
					output.pop_back();
				if (findPathRecursive(g.getNode(temp),g))
					return true;
				else
					return false;
			}
		}
	}
}

int main()
{
	//char x;
	ifstream fin;
   
	// Read the maze from the file.
	string fileName = "maze1.txt";

	fin.open(fileName.c_str());
	if (!fin)
	{
		cerr << "Cannot open " << fileName << endl;
		exit(1);
	}

	try
	{
		while (fin && fin.peek() != 'Z')
		{
			maze m(fin);
			graph g;
			m.mapMazeToGraph(g);
			//cout<<"***********Non-recursive solution**********\n";
			//m.findPathNonRecursive(g);
			
			
			cout<<"\n\n***********Depth-first search**********\n";
			if (m.findShortestPath1(g.getNode(0), g))
			{
				int i = 0, j = 0, count = m.output.size();
				for(int x = 0; x<count; x++)
				{
					m.print(7, 10, i, j);
					cout<<"\n\n"<<m.output[x]<<"\n";

					if(m.output[x] == "up")
						i--;
					if(m.output[x] == "down")
						i++;
					if(m.output[x] == "left")
						j--;
					if(m.output[x] == "right")
						j++;
			
				}
			}
			else
				cout << "No valid path found.\n";
		}



	} 
	catch (indexRangeError &ex) 
	{ 
		cout << ex.what() << endl; exit(1);
	}
	catch (rangeError &ex)
	{
		cout << ex.what() << endl; exit(1);
	}

	system("pause");
}
