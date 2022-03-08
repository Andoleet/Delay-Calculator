#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <cstdlib>


using namespace std;

struct gates{
    string type;
    string name;
    string privateConnections;
    string connections = "NULL";
    string input1 = "NULL";
    string input2 = "NULL";
    string pinput1 = "NULL";
    string pinput2 = "NULL";
    string output = "NULL";
};

struct flipflops{
    string name;
    string privateConnections;
    string connections = "NULL";
    string pinput;
    string input;
    string output;
};

struct pathdel{
    string path="NULL";
    int delay;
    int stage;
};
int n=0; // x = count FF
int m=0; // y= conut others
int countff () {
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
int delfunc (string word) {
int delay;
    if (word=="And"){
        delay=2;
    }else if(word=="Inv"){
         delay=1;
    }else if(word=="Or"){
         delay=3;
    }else if(word=="Xor"){
         delay=2;
    }else if(word=="Flipflop"){
         delay=2;
    }else if(word=="Nand"){
         delay=1;
    }else if(word=="Nor"){
         delay=2;
    }else if(word=="Xnor"){
         delay=2;
    }
   return delay;
}

///Function to get a cell's information
void getInfo(string netlist, gates gatesInstance[], flipflops ffInstance[]);

int main()
{
    string netlist = "netlist.txt";
    countff();

    gates gatesInstance[m];
    flipflops ffInstance[n];

    getInfo(netlist, gatesInstance, ffInstance); // calling the function

    pathdel pathint;
    pathdel pathmax;
    string input;
    string arrow =" <-- ";
    bool connected= true;
    bool gates=true;
    pathmax.delay=0;
    for (int i=0;i<n;i++){
        connected=true;
        while(connected)
            {
            pathint.delay=2;
            pathint.path=ffInstance[i].name;
            pathint.stage=0;
            input=ffInstance[i].input;
            cout<<input;
            connected=false;
            gates=true;
            while (gates)
            {
                gates=false;
              for(int j=0; j<m; j++)
              {
                if(input==gatesInstance[j].output)
                {
                    connected=true;
                    gates=true;
                    pathint.delay += delfunc(gatesInstance[j].type);
                    cout<<pathint.delay<<endl;
                    pathint.path = pathint.path+ gatesInstance[j].name+ arrow;
                    if (gatesInstance[j].input1!="NULL")
                    {
                        input=gatesInstance[j].input1;
                        gatesInstance[j].input1="NULL";
                        break;
                    }
                    else if (gatesInstance[j].input2!="NULL")
                    {
                        input=gatesInstance[j].input2;
                        gatesInstance[j].input2="NULL";
                        break;
                    }
                }
                }
            for (int x=0; x<n; x++)
            {
                if (input== ffInstance[x].output)
                {
                    gates=false;
                    pathint.path += ffInstance[x].name;
                    if (x<n/3-1)
                    {
                        pathint.stage==1;
                    }
                    else
                    {
                        pathint.stage==2;
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
        for (int l=0; l<m; l++)
        {
            gatesInstance[l].input1=gatesInstance[i].pinput1;
            gatesInstance[l].input2=gatesInstance[i].pinput2;
        }
    }
    cout<<"Critical Path is in stage:  " <<pathmax.stage<<endl;
    cout<<"The Maximum Delay Value is:  " <<pathmax.path<<"  ns"<<endl;

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
            ffInstance[n].pinput = currentString;
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
            myfile >> dummyString;
            myfile >> currentString;
            currentString.erase(remove(currentString.begin(), currentString.end(), ','), currentString.end());
            gatesInstance[m].input1 = currentString;
            gatesInstance[m].pinput1 = currentString;
            myfile >> currentString;
            if(currentString == "in") //checking if there is another input for the gate
            {
                twoInputs = true;
                myfile >> currentString;
                currentString.erase(remove(currentString.begin(), currentString.end(), ','), currentString.end());
                gatesInstance[m].input2 = currentString;
                gatesInstance[m].pinput2 = currentString;

            }

            if(twoInputs == true)
            {
                myfile >> currentString;
                twoInputs = false;
            }
            myfile >> currentString;
            currentString.erase(remove(currentString.begin(), currentString.end(), ')'), currentString.end());
            currentString.erase(remove(currentString.begin(), currentString.end(), ';'), currentString.end());
            gatesInstance[m].output = currentString;
            output = currentString; /// saving the cell output

            m++;
        }
    }

    myfile.close();
}
