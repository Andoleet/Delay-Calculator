#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <vector>


using namespace std;

struct gates{
    string type;
    string name;
    vector<string> privateConnections;  // our reference connections it is not changed along our code
    vector <string> connections ;       // the used connection we will erase from it.
    string output = "NULL";
    string privateoutput;
    int n=0;
};

struct flipflops{
    string name;
    string input;
    string output;
};

struct pathdel{ //struct for the path and del and stage for each cell
    string path="NULL";
    int delay;
    int stage;
};
int n=0; // n = count FF
int m=0; // m= conut others
int countff () { // counting the number of flip flops and gates
    string line;
    int z=0;//z= the number of lines
    ifstream myfile ("netlist.txt");
    if (myfile.is_open())
        {
            while ( getline (myfile,line) ){
                z++;
            if (!line.find("Flipflop")){
                n++;
            }
            }
        }
    m=z-n;
    return n, m;
    myfile.close();
}
int delfunc (string word, int n) { // calculating the delay for each cell
int delay;
    if (word=="And"){
        delay=(n/2)+1;
    }else if(word=="Inv"){
         delay=1;
    }else if(word=="Or"){
         delay=n+1;
    }else if(word=="Xor"){
         delay=n;
    }else if(word=="Flipflop"){
         delay=2;
    }else if(word=="Nand"){
         delay=n/2;
    }else if(word=="Nor"){
         delay=n;
    }else if(word=="Xnor"){
         delay=n;
    }
   return delay;
}

///Function to get a cell's information
void getInfo(string netlist, gates gatesInstance[], flipflops ffInstance[]);

void tracing(gates gatesInstance[], flipflops ffInstance[], pathdel pathint,pathdel pathmax);

int main()
{
    string netlist = "netlist.txt";
    countff();

    gates gatesInstance[m];
    flipflops ffInstance[n];

    getInfo(netlist, gatesInstance, ffInstance); // calling the function

    pathdel pathint; //the path and delay for each path
    pathdel pathmax; // struct for max path only.

    tracing(gatesInstance, ffInstance, pathint, pathmax);

    return 0;
}


//Function to get a cell's information
void getInfo(string netlist, gates gatesInstance[], flipflops ffInstance[]){

    int m=0, n=0; ///used as indices for the structs
    bool twoInputs = false;
    string currentString, dummyString, output="NULL";
    // Calling the file
    ifstream myfile;
    myfile.open("netlist.txt");

    while(myfile)//getting the cell's information
    {
        myfile >> currentString;

        if(currentString == output){break;} ///if the output is the same as current string, then the file has ended
        // getting the information of the flipflop
        if(currentString == "Flipflop")
        {
            myfile >> ffInstance[n].name;
            myfile >> dummyString;
            myfile >> currentString;
            currentString.erase(remove(currentString.begin(), currentString.end(), ','), currentString.end());
            ffInstance[n].input = currentString;
            myfile >> dummyString;
            myfile >> dummyString;
            myfile >> dummyString;
            myfile >> currentString;
            currentString.erase(remove(currentString.begin(), currentString.end(), ')'), currentString.end());
            currentString.erase(remove(currentString.begin(), currentString.end(), ';'), currentString.end());
            ffInstance[n].output = currentString;
            output = currentString; /// saving the cell output

            n++;
        }
        else //getting the information of the gate
        {
            gatesInstance[m].type = currentString;
            myfile >> currentString;
            gatesInstance[m].name = currentString;
            bool inputs=true;
            int here=0;
            /// a loop to take any number of inputs///
            while (inputs)
            {
                inputs=false;
                myfile >> currentString;
                currentString.erase(remove(currentString.begin(),currentString.end(),'('),currentString.end());
                if(currentString == "in") //checking if there is an input for the gate
                {
                    myfile >> currentString;
                    currentString.erase(remove(currentString.begin(), currentString.end(), ','), currentString.end());
                    gatesInstance[m].privateConnections.push_back(currentString);
                    gatesInstance[m].connections.push_back(currentString);
                    gatesInstance[m].n ++;
                    inputs=true;
                }
                else if (currentString=="out")
                {
                    myfile >>currentString;
                    currentString.erase(remove(currentString.begin(), currentString.end(), ')'), currentString.end());
                    currentString.erase(remove(currentString.begin(), currentString.end(), ';'), currentString.end());
                    gatesInstance[m].output = currentString;
                    gatesInstance[m].privateoutput = currentString;
                    output = currentString; /// saving the cell output
                }
            }

            m++;
        }
    }

    myfile.close();
}

//function to trace the pathes and get the maximum acceptable path.
// the function trace each path from back to front of the path eg:starts from F4 to reach F1 and F2.
void tracing(gates gatesInstance[],flipflops ffInstance[], pathdel pathint,pathdel pathmax)
{
    string input;
    string arrow =" <-- ";

    int k;
    bool connected;
    bool gates;
    pathmax.delay=0;

    for (int i=0;i<n;i++)
    {
        connected=true;

        while(connected) // loop means that this FF doesn't have any available traces.
        {

            connected=false;
            gates=true;

            pathint.delay=2;                    //starting with the delay of FF and its input
            pathint.path=ffInstance[i].input;
            pathint.stage=0;

            input=ffInstance[i].input; // input is the current path im in it
            while (gates)   // loop to exit the gates meaning that this gates has no inputs or out puts( virtual this means I passed from all its possible ways with certain FF)
            {
                gates=false;
                for(int j=0; j<m; j++)
                {
                    if(input==gatesInstance[j].output)
                    {
                        connected=true;
                        gates=true;

                        pathint.delay += delfunc(gatesInstance[j].type,gatesInstance[j].n);

                        pathint.path = pathint.path+ arrow+ gatesInstance[j].connections[0];

                        if (gatesInstance[j].connections.empty())//this means that this cell is not needed so it will be erased and the path to it will be erased.
                        {
                        for (int z=0; z<m;z++)
                        {
                            if (gatesInstance[j].output==gatesInstance[z].connections[0])
                            {
                                gatesInstance[z].connections.erase(gatesInstance[z].connections.begin());
                                break;
                            }
                        }

                        gatesInstance[j].output=" ";
                        gates=false;
                        break;
                        }
                        else
                        {
                            k=j;
                            input=gatesInstance[j].connections[0];

                        }
                    }
                }

            for (int x=0; x<n; x++)
            {

                if (input== ffInstance[x].output)
                {
                    gates=false;
                    gatesInstance[k].connections.erase(gatesInstance[k].connections.begin());

                    if (x<n/3) //calculating the stage
                    {
                        pathint.stage=1;
                    }
                    else
                    {
                        pathint.stage=2;
                    }

                    if (pathint.delay>pathmax.delay)
                    {
                        pathmax.delay=pathint.delay;
                        pathmax.path=pathint.path;
                        pathmax.stage=pathint.stage;
                    }
                }
            }
            }

        }
        for (int l=0; l<m; l++) // restoring all the inputs as they were to check for the next flip flop
        {
            gatesInstance[l].connections=gatesInstance[l].privateConnections;
            gatesInstance[l].output=gatesInstance[l].privateoutput;
        }
    }
    cout<<"Critical Path is in stage:  " <<pathmax.stage<<endl;
    cout<<"The Maximum Delay Value is:  " <<pathmax.delay<<"  ns"<<endl;
    cout<<"Critical Path is: "<<pathmax.path<<endl<<endl;
    cout<<"Minimum Achievable Clock Period is:  "<< pathmax.delay<<"  ns"<<endl;
    cout<<"----------------------------------------------------------------------";
    return;
}

