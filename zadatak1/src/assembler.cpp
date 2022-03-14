#include <iostream>
#include <iomanip>
#include "../inc/include.h"


int main(int argc,  char **argv)
{
    //Inicijalizacija
    ifstream in;
    ofstream out;
    vector<string> input;
    string line;
    Simbol::TabelaSimbola.push_back(Simbol("undefined","undefined",0,0));
    outputText.push_back(output("undefined"));
    //Namestanje argumenata
    if(argc==4)
    {
        in.open(argv[3]);
        out.open(argv[2]);
    }
    else
    {
        in.open(argv[1]);
        out.open("izlaz.o");
    }
    //Input
    while(getline(in,line)){
        input.push_back(line);
    }
    //Ciscenje belina
    ClearWhite(input);
    //Prvi prolaz
    FirstPass(input);
    //Drugi prolaz
    SecondPass();
    if(Simbol::fail==0){
    out<<"Tabela simbola"<<endl<<endl;
    out<<setw(15)<<"ime"<<setw(15)<<"sekcija"<<setw(15)<<"offset/val"<<setw(10)<<"domen"<<setw(5)<<"r.br"<<endl;
    for(int i=0; i<Simbol::TabelaSimbola.size(); i++)
    {
        out<<setw(15)<<Simbol::TabelaSimbola.at(i).ime<<setw(15)<<Simbol::TabelaSimbola.at(i).sekcija<<setw(15)<<Simbol::TabelaSimbola.at(i).offset_value<<setw(10)<<(Simbol::TabelaSimbola.at(i).domen==1?"global":"local")<<setw(5)<<Simbol::TabelaSimbola.at(i).rednibroj<<endl;
        //cout<<Simbol::TabelaSimbola.at(i).ime<<' '<<Simbol::TabelaSimbola.at(i).sekcija<<' '<<Simbol::TabelaSimbola.at(i).offset_value<<' '<<(Simbol::TabelaSimbola.at(i).domen==1?"global":"local")<<' '<<Simbol::TabelaSimbola.at(i).rednibroj<<endl;
    }
    //cout<<endl<<endl;
    out<<endl<<endl;
    out<<"Relokaciona tabela"<<endl<<endl;
    out<<setw(15)<<"sekcija"<<setw(8)<<"offset"<<setw(12)<<"tip"<<setw(11)<<"r.br u t.s"<<endl;
    for(int i=0; i<RelTabela.size(); i++)
    {
        out<<setw(15)<<RelTabela.at(i).sekcija<<setw(8)<<RelTabela.at(i).offset<<setw(12)<<RelTabela.at(i).tip<<setw(11)<<RelTabela.at(i).brsimbola<<endl;
        //cout<<RelTabela.at(i).sekcija<<' '<<RelTabela.at(i).offset<<' '<<RelTabela.at(i).tip<<' '<<RelTabela.at(i).brsimbola<<endl;
    }
    //cout<<endl<<endl;
    out<<endl<<endl;
    out<<"Sadrzaj sekcija"<<endl<<endl;
    for(int i=0; i<outputText.size(); i++)
    {
        out<<outputText.at(i).sectionName<<' '<<outputText.at(i).sectionSize<<endl;
        //cout<<outputText.at(i).sectionName<<endl;
        for(int j=0; j<outputText.at(i).sectionData.size(); j++)
        {
            out<<outputText.at(i).sectionData.at(j)<<endl;
            //cout<<outputText.at(i).sectionData.at(j)<<endl;
        }
        out<<endl;
    }}
    in.close();
    out.close();
    return 0;
}