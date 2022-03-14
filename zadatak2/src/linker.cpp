#include <iomanip>
#include <iostream>
#include <algorithm>
#include <vector>
#include "../../zadatak1/inc/include.h"
#include <string.h>

using namespace std;
struct outputStruct{
    string sectionName;
    string sectionStart;
    int sectionLength;
    vector<string> sectionData;
    outputStruct(string s1, string s2)
    {
        sectionName=s1;
        sectionStart=s2;
        sectionLength=0;
        sectionData=vector<string>();
    }
};

bool comp(outputStruct o1,outputStruct o2)
{
    return HexToInt(o1.sectionStart)<HexToInt(o2.sectionStart);
}

int main(int argc, char** argv)
{
    string s=argv[1];
    bool hex=false;
    if(s=="-hex")
        hex=true;
    ifstream in;
    ofstream out;
    Simbol::TabelaSimbola.push_back(Simbol("undefined","undefined",0,0));
    vector<outputStruct> podaci=vector<outputStruct>();
    vector<Simbol> globalnaTabela=vector<Simbol>();
    vector<relocation> globalnaRelTa=vector<relocation>();
    for(int i=2; i<argc; i++)
    {
        if(argv[i][0]=='-')
        {
            string s1=argv[i];
            if(s1=="-o")
            {
                if(!out.is_open())
                {
                    i++;
                    out.open(argv[i]);
                }
            }
            else
            {
                s1="";
                for(int j=0; j<7; j++)
                    s1+=argv[i][j];
                if(s1!="-place=")
                    break;
                string s2;
                s1="";
                int j;
                for(j=7; j<strlen(argv[i]); j++)
                    if(argv[i][j]=='@')
                    {
                        j++;
                        break;
                    }
                    else
                        s1+=argv[i][j];
                for(; j<strlen(argv[i]); j++)
                    s2+=argv[i][j];
                if(hex)
                    podaci.push_back(outputStruct(s1,s2));
            }
        }
        else
        {
            in.open(argv[i]);
            vector<Simbol> lokalnaTabela=vector<Simbol>();
            string line;
            getline(in,line);
            getline(in,line);
            getline(in,line);
            while(getline(in,line))
            {
                if(line.empty()) break;
                string s;
                string s1;
                string s2;
                int o;
                int d;
                int rb;
                stringstream stream(line);
                stream>>s;
                stream>>s1;
                stream>>o;
                for(int j=0; j<podaci.size(); j++)
                {
                    if(podaci.at(j).sectionName==s && s!=s1)
                    {
                        o+=podaci.at(j).sectionLength;
                        break;
                    }
                }
                stream>>s2;
                stream>>rb;
                if(s2=="local")
                    d=0;
                else d=1;
                Simbol temp(s,s1,o,d,rb);
                lokalnaTabela.push_back(temp);
            }
            getline(in,line);
            getline(in,line);
            getline(in,line);
            getline(in,line);
            vector<relocation> lokalnaRelTa=vector<relocation>();
            while(getline(in,line))
            {
                if(line.empty()) break;
                string s;
                stringstream stream(line);
                stream>>s;
                int o;
                stream>>o;
                for(int j=0; j<podaci.size(); j++)
                {
                    if(podaci.at(j).sectionName==s)
                    {
                        o+=podaci.at(j).sectionLength;
                        break;
                    }
                }
                string s1;
                stream>>s1;
                int rb;
                stream>>rb;
                relocation temp(s,o,rb,s1);
                lokalnaRelTa.push_back(temp);
            }
            for(int j=0; j<lokalnaTabela.size(); j++)
            {
                bool t=true;
                for(int k=0; k<globalnaTabela.size(); k++)
                {
                    if(lokalnaTabela.at(j).ime==globalnaTabela.at(k).ime)
                    {
                        if(lokalnaTabela.at(j).sekcija!="undefined" && globalnaTabela.at(k).sekcija!="undefined")
                        {
                            if(lokalnaTabela.at(j).sekcija==lokalnaTabela.at(j).ime && globalnaTabela.at(k).ime==globalnaTabela.at(k).sekcija)
                            {
                                
                                t=false;
                                break;
                            }
                            else
                            {
                                cout<<"Duplo definisan simbol "<<lokalnaTabela.at(j).ime<<endl;
                                return 0;
                            }
                        }
                        if(lokalnaTabela.at(j).sekcija!="undefined")
                        {
                            globalnaTabela.at(k).sekcija=lokalnaTabela.at(j).sekcija;
                            globalnaTabela.at(k).offset_value=lokalnaTabela.at(j).offset_value;
                        }
                        if(lokalnaTabela.at(j).domen==1)
                            globalnaTabela.at(k).domen=1;
                        for(int l=0; l<lokalnaRelTa.size(); l++)
                            if(lokalnaRelTa.at(l).brsimbola==lokalnaTabela.at(j).rednibroj)
                                {
                                    lokalnaRelTa.at(l).brsimbola=-globalnaTabela.at(k).rednibroj-1;
                                }
                        t=false;
                        break;
                    }
                }
                if(t)
                {
                    int rbr=globalnaTabela.size();
                    for(int l=0; l<lokalnaRelTa.size(); l++)
                        if(lokalnaRelTa.at(l).brsimbola==lokalnaTabela.at(j).rednibroj)
                        {
                            lokalnaRelTa.at(l).brsimbola=-rbr-1;
                        }
                    lokalnaTabela.at(j).rednibroj=rbr;
                    globalnaTabela.push_back(lokalnaTabela.at(j));
                }
            }
            for(int j=0; j<lokalnaRelTa.size(); j++)
            {
                if(lokalnaRelTa.at(j).brsimbola<0)
                    lokalnaRelTa.at(j).brsimbola=-1-lokalnaRelTa.at(j).brsimbola;
                globalnaRelTa.push_back(lokalnaRelTa.at(j));
            }
            getline(in,line);
            getline(in,line);
            getline(in,line);
            while(1)
            {
                if(!getline(in,line))
                    break;
                string s;
                int n;
                int ind=-1;
                stringstream stream(line);
                stream>>s;
                stream>>n;
                for(int j=0; j<podaci.size(); j++)
                    if(podaci.at(j).sectionName==s)
                    {
                        ind=j;
                        podaci.at(j).sectionLength+=n;
                    }
                if(ind==-1)
                {
                    ind=podaci.size();
                    podaci.push_back(outputStruct(s,"0x"));
                    podaci.at(podaci.size()-1).sectionLength+=n;
                }
                bool t=false;
                while(1)
                {
                    if(getline(in,line))
                    {
                        if(line=="")
                        {
                            break;
                        }
                        else 
                        {
                            if(hex)
                            {
                                for(int j=0; j<line.size(); j++)
                                {
                                    string ss="";
                                    ss+=line[j];
                                    podaci.at(ind).sectionData.push_back(ss);
                                }
                            }
                            else //link
                            {
                                podaci.at(ind).sectionData.push_back(line);
                            }
                        }
                                
                    }
                    else
                        t=true;
                }
                if(t) break;
            }
            in.close();
        }
    }
    if(hex)
    {
        int max=0;
        int start=0;
        for(int i=0; i<podaci.size(); i++)
        {
       
            string s="";
            s+=podaci.at(i).sectionStart[2];
            s+=podaci.at(i).sectionStart[3];
            s+=podaci.at(i).sectionStart[4];
            s+=podaci.at(i).sectionStart[5];
            if(HexToInt(s)>max)
            {
                max=HexToInt(s);
                start=max+podaci.at(i).sectionLength;
            }
        }
        for(int i=0; i<podaci.size(); i++)
        {
            if(podaci.at(i).sectionStart=="0x")
            {
                podaci.at(i).sectionStart+=DecimalToHex(start);
                start+=podaci.at(i).sectionLength;
            }
        }
        sort(podaci.begin(),podaci.end(),comp);
        for(int i=0; i<globalnaRelTa.size(); i++)
        {
            int value=0;
            string sek;
            for(int j=0; j<globalnaTabela.size(); j++)
            {
                if(globalnaTabela.at(j).rednibroj==globalnaRelTa.at(i).brsimbola)
                {
                    value=globalnaTabela.at(j).offset_value;
                    sek=globalnaTabela.at(j).sekcija;
                    if(globalnaRelTa.at(i).tip=="PC_Rel")
                        value-=2;
                }
            }
            if(sek!="absolute")
            {
                for(int j=0; j<podaci.size(); j++)
                    if(podaci.at(j).sectionName==sek)
                    {
                        string s="";
                        s+=podaci.at(j).sectionStart[2];
                        s+=podaci.at(j).sectionStart[3];
                        s+=podaci.at(j).sectionStart[4];
                        s+=podaci.at(j).sectionStart[5];
                        value+=HexToInt(s);
                    }

            }
            string s=DecimalToHex(value);
            for(int j=0; j<podaci.size(); j++)
            {
                if(podaci.at(j).sectionName==globalnaRelTa.at(i).sekcija)
                {
                    podaci.at(j).sectionData[globalnaRelTa.at(i).offset*2]=s[0];
                    podaci.at(j).sectionData[globalnaRelTa.at(i).offset*2+1]=s[1];
                    podaci.at(j).sectionData[globalnaRelTa.at(i).offset*2+2]=s[2];
                    podaci.at(j).sectionData[globalnaRelTa.at(i).offset*2+3]=s[3];
                }
            }
        }
        for(int i=1; i<globalnaTabela.size(); i++)
            if(globalnaTabela.at(i).sekcija=="undefined")
            {
                cout<<"Simbol "<<globalnaTabela.at(i).ime<<" nije definisan!"<<endl;
                return 0;
            }
        for(int i=0; i<podaci.size()-1; i++)
        {
            string s1="";
            s1+=podaci.at(i).sectionStart[2];
            s1+=podaci.at(i).sectionStart[3];
            s1+=podaci.at(i).sectionStart[4];
            s1+=podaci.at(i).sectionStart[5];
            string s2="";
            s2+=podaci.at(i+1).sectionStart[2];
            s2+=podaci.at(i+1).sectionStart[3];
            s2+=podaci.at(i+1).sectionStart[4];
            s2+=podaci.at(i+1).sectionStart[5];
            if(HexToInt(s1)+podaci.at(i).sectionLength>HexToInt(s2))
            {
                cout<<"Preklapaju se sekcije "<<podaci.at(i).sectionName<<" i "<<podaci.at(i+1).sectionName<<"!"<<endl;
                return 0;
            }
        
        }
    }
    out<<"Tabela simbola"<<endl<<endl;
    out<<setw(15)<<"ime"<<setw(15)<<"sekcija"<<setw(15)<<"offset/val"<<setw(10)<<"domen"<<setw(5)<<"r.br"<<endl;
    for(int i=0; i<globalnaTabela.size(); i++)
    {
        out<<setw(15)<<globalnaTabela.at(i).ime<<setw(15)<<globalnaTabela.at(i).sekcija<<setw(15)<<globalnaTabela.at(i).offset_value<<setw(10)<<(globalnaTabela.at(i).domen==1?"global":"local")<<setw(5)<<globalnaTabela.at(i).rednibroj<<endl;
        //cout<<globalnaTabela.at(i).ime<<' '<<globalnaTabela.at(i).sekcija<<' '<<globalnaTabela.at(i).offset_value<<' '<<(globalnaTabela.at(i).domen==1?"global":"local")<<' '<<globalnaTabela.at(i).rednibroj<<endl;
    }
    //cout<<endl<<endl;
    out<<endl<<endl;
    out<<"Relokaciona tabela"<<endl<<endl;
    out<<setw(15)<<"sekcija"<<setw(8)<<"offset"<<setw(12)<<"tip"<<setw(11)<<"r.br u t.s"<<endl;
    for(int i=0; i<globalnaRelTa.size(); i++)
    {
        out<<setw(15)<<globalnaRelTa.at(i).sekcija<<setw(8)<<globalnaRelTa.at(i).offset<<setw(12)<<globalnaRelTa.at(i).tip<<setw(11)<<globalnaRelTa.at(i).brsimbola<<endl;
        //cout<<globalnaRelTa.at(i).sekcija<<' '<<globalnaRelTa.at(i).offset<<' '<<globalnaRelTa.at(i).tip<<' '<<globalnaRelTa.at(i).brsimbola<<endl;
    }
    //cout<<endl<<endl;
    out<<endl<<endl;
    out<<"Sadrzaj sekcija"<<endl<<endl;
    if(hex)
    {
        for(int i=0; i<podaci.size(); i++)
        {
            out<<podaci.at(i).sectionName<<" ";
            string ss="";
            ss+=podaci.at(i).sectionStart[2];
            ss+=podaci.at(i).sectionStart[3];
            ss+=podaci.at(i).sectionStart[4];
            ss+=podaci.at(i).sectionStart[5];
            int temp=HexToInt(ss);
            out<<podaci.at(i).sectionLength<<endl;
            int help=0;
            if(temp%8!=0){
                out<<endl<<"0x"<<DecimalToHex(temp-temp%8)<<" ";
                for(int i=0; i<temp%8; i++)
                    out<<"  ";
                help=temp%8*2;
                temp-=temp%8;
                temp+=8;
            }
            for(int j=0; j<podaci.at(i).sectionData.size(); j++)
            {
                if(help%16==0)
                {
                    out<<endl<<"0x"<<DecimalToHex(temp)<<" ";
                    temp+=8;
                }
                out<<podaci.at(i).sectionData.at(j);
                help++;
            }
            out<<endl<<endl;
        }
    }
    else //link
    {
        for(int i=0; i<podaci.size(); i++)
        {
            out<<podaci.at(i).sectionName<<' '<<podaci.at(i).sectionLength<<endl;
            //cout<<podaci.at(i).sectionName<<endl;
            for(int j=0; j<podaci.at(i).sectionData.size(); j++)
            {
                out<<podaci.at(i).sectionData.at(j)<<endl;
                //cout<<podaci.at(i).sectionData.at(j)<<endl;
            }
            out<<endl;
        }
    }
    out.close();
    return 0;
}