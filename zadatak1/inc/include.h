#include <fstream>
#include <vector>

using namespace std;

class Simbol
{
    public:
        string ime;
        string sekcija;
        int offset_value;
        int domen;
        int rednibroj;
        static vector<Simbol> TabelaSimbola;
        static int gid;
        static int fail;
        static string currentSection;
        static int sectionCnt;
        static int end;

        Simbol(string i,string s, int o, int d,int rb=-1)
        {
            ime=i;
            sekcija=s;
            offset_value=o;
            domen=d;
            if(rb==-1)
                rednibroj=gid++;
            else 
                rednibroj=rb;
        }
};

struct output{
    string sectionName;
    vector<string> sectionData;
    int sectionSize;
    output(string s)
    {
        sectionName=s;
        sectionData=vector<string>();
        sectionSize=0;
    }
};

struct relocation{
    string sekcija;
    int offset;
    int brsimbola;
    string tip;
    relocation(string s,int o,int b,string t)
    {
        sekcija=s;
        offset=o;
        brsimbola=b;
        tip=t;
    }
};

int Simbol::gid=0;
int Simbol::end=0;
int Simbol::fail=0;
int Simbol::sectionCnt=0;
string Simbol::currentSection="undefined";
vector<output> outputText=vector<output>();
vector<Simbol> Simbol::TabelaSimbola;
vector<relocation> RelTabela=vector<relocation>();
////////////////Utility
char toHex(int i)
{
    if(i<10)
        return '0'+i;
    if(i==10)
        return 'A';
    if(i==11)
        return 'B';
    if(i==12)
        return 'C';
    if(i==13)
        return 'D';
    if(i==14)
        return 'E';
    if(i==15)
        return 'F';
    return 'E';
}
int toDecimal(char c)
{
    if(c>='0' && c<='9')
        return c-'0';
    if(c=='A')
        return 10;
    if(c=='B')
        return 11;
    if(c=='C')
        return 12;
    if(c=='D')
        return 13;
    if(c=='E')
        return 14;
    if(c=='F')
        return 15;
    return -1;
}
string parseDecimal(string toParse)
{
    int value=0;
    for(int i=0; i<toParse.length(); i++)
    {
            if(toParse[i]<'0' || toParse[i]>'9')
                return "error"; 
            value=value*10+toParse[i]-'0';
    }
    if(value>65535) return "error";
    string ret="";
    ret+=toHex(value/4096);
    ret+=toHex((value%4096)/256);
    ret+=toHex((value%256)/16);
    ret+=toHex(value%16);
    return ret;
}
string parseHex(string toParse)
{
    int value=0;
    if(toParse[0]!='0' || toParse[1]!='x')
        return "error";
    for(int i=2; i<toParse.length(); i++)
    {
        if((toParse[i]<'0' || toParse[i]>'9') && (toParse[i]<'A' || toParse[i]>'F'))
            return "error";
        value=value*16+toDecimal(toParse[i]);
    }
    if(value>65535) return "error";
    string ret="";
    ret+=toHex(value/4096);
    ret+=toHex((value%4096)/256);
    ret+=toHex((value%256)/16);
    ret+=toHex(value%16);
    return ret;
}
int HexToInt(string hex)
{
    int ret=0;
    for(int i=0; i<hex.length(); i++)
        ret=16*ret+toDecimal(hex[i]);
    return ret;
}
string DecimalToHex(int value)
{
    string ret="";
    ret+=toHex(value/4096);
    ret+=toHex((value%4096)/256);
    ret+=toHex((value%256)/16);
    ret+=toHex(value%16);
    return ret;
}
///////Parsiranje registra
void RegParser(string &base, string toParse, int start, int regNum)
{
    if(regNum==1)
    {
        string temp="";
        for(int i=start; i<toParse.size(); i++)
            temp.push_back(toParse[i]);
        if(temp=="r0")
            base=base+"0F";
        else if(temp=="r1")
        {
            base=base+"1F";
        }
        else if(temp=="r2")
        {
            base=base+"2F";
        }
        else if(temp=="r3")
        {
            base=base+"3F";
        }
        else if(temp=="r4")
        {
            base=base+"4F";
        }
        else if(temp=="r5")
        {
            base=base+"5F";
        }
        else if(temp=="r6" || temp=="sp") 
        {
            base=base+"6F";
        }
        else if(temp=="r7" || temp=="pc")
        {
            base=base+"7F";
        }
        else if(temp=="psw")
        {
            base=base+"8F";
        }
        else base="error";
    }
    else
    {
        string temp="";
        int i=start;
        while(1)
        {
            if(toParse[i]==',') break;
            else temp+=toParse[i];
            i++;
        }
        if(temp=="r0")
            base=base+"0";
        else if(temp=="r1")
        {
            base=base+"1";
        }
        else if(temp=="r2")
        {
            base=base+"2";
        }
        else if(temp=="r3")
        {
            base=base+"3";
        }
        else if(temp=="r4")
        {
            base=base+"4";
        }
        else if(temp=="r5")
        {
            base=base+"5";
        }
        else if(temp=="r6" || temp=="sp") 
        {
            base=base+"6";
        }
        else if(temp=="r7" || temp=="pc")
        {
            base=base+"7";
        }
        else if(temp=="psw")
        {
            base=base+"8";
        }
        else 
        {
            base="error";
            return;
        }
        i++;
        temp="";
        for( ; i<toParse.size(); i++)
            if(toParse[i]==' ') 
                break;
            else
                temp+=toParse[i];
        if(temp=="r0")
            base=base+"0";
        else if(temp=="r1")
        {
            base=base+"1";
        }
        else if(temp=="r2")
        {
            base=base+"2";
        }
        else if(temp=="r3")
        {
            base=base+"3";
        }
        else if(temp=="r4")
        {
            base=base+"4";
        }
        else if(temp=="r5")
        {
            base=base+"5";
        }
        else if(temp=="r6" || temp=="sp") 
        {
            base=base+"6";
        }
        else if(temp=="r7" || temp=="pc")
        {
            base=base+"7";
        }
        else if(temp=="psw")
        {
            base=base+"8";
        }
        else 
        {
            base="error";
            return;
        }
    }
}
//////////////////////Cisti beline
void ClearWhite(vector<string> &input)
{
    for(int i=0; i<input.size(); i++)
    {
        string temp="";
        int j=0;
        while(input.at(i)[j]==' '  || input.at(i)[j]=='\t')
            j++;
        int t=0;
        for( ; j<input.at(i).size(); j++)
        {
            if(input.at(i)[j]=='#') break;
            if(input.at(i)[j]==' ')
                t++;
            else if(input.at(i)[j]==',') t=1;
                 else t=0;
            if(t<=1) temp.push_back(input.at(i)[j]);
        }
        if(temp[temp.length()-1]==' ')
            temp.pop_back();
        input.at(i)=temp;
    }

}
//////////////Operacije koje samo rade sa registrima ne operandima
string OperationCoder(string operation)
{
    string temp="";
    string ret;
    string e="error";
    for(int i=0; i<operation.size(); i++)
    {
        temp+=operation[i];
        if(operation[i]==' ') break;
    }
    if(temp=="halt") 
    {
        ret="00";
        if(operation.size()>4) return e;
        else return ret;
    }
    else if(temp=="int ")
    {
        ret="10";
        RegParser(ret,operation,4,1);
    }
    else if(temp=="iret " || temp=="iret")
    {
        ret="20";
        if(operation.size()>4) return e;
        else return ret;
    }
    else if(temp=="ret ")
    {
        ret="40";
        if(operation.size()>4) return e;
        else return ret;
    }
    else if(temp=="push ")
    {
        ret="B0";
        RegParser(ret,operation,5,1);
        ret[3]='6';
        ret+="22";
    }
    else if(temp=="pop ")
    {
        ret="A0";
        RegParser(ret,operation,4,1);
        ret[3]='6';
        ret+="32";
    }
    else if(temp=="xchg ")
    {
        ret="50";
        RegParser(ret,operation,5,2);
    }
    else if(temp=="add ")
    {
        ret="70";
        RegParser(ret,operation,4,2);
    }
    else if(temp=="sub ")
    {
        ret="71";
        RegParser(ret,operation,4,2);
    }
    else if(temp=="mul ")
    {
        ret="72";
        RegParser(ret,operation,4,2);
    }
    else if(temp=="div ")
    {
        ret="73";
        RegParser(ret,operation,4,2);
    }
    else if(temp=="cmp ")
    {
        ret="74";
        RegParser(ret,operation,4,2);
    }
    else if(temp=="not ")
    {
        ret="80";
        RegParser(ret,operation,4,2);
    }
    else if(temp=="and ")
    {
        ret="81";
        RegParser(ret,operation,4,2);
    }
    else if(temp=="or ")
    {
        ret="82";
        RegParser(ret,operation,3,2);
    }
    else if(temp=="xor ")
    {
        ret="83";
        RegParser(ret,operation,4,2);
    }
    else if(temp=="test ")
    {
        ret="84";
        RegParser(ret,operation,4,2);
    }
    else if(temp=="shl ")
    {
        ret="90";
        RegParser(ret,operation,4,2);
    }
    else if(temp=="shr ")
    {
        ret="91";
        RegParser(ret,operation,4,2);
    }
    else return e;
    return ret;
}
//Skokovi
string jumpCoder(string operation)
{
    string temp="";
    string ret="";
    for(int i=0; i<operation.length(); i++)
    {
        temp+=operation[i];
        if(operation[i]==' ')
            break;
    }
    if(temp=="jmp ")
        ret+="50F";
    else if(temp=="jeq ")
        ret+="51F";
    else if(temp=="jne ")
        ret+="52F";
    else if(temp=="jgt ")
        ret+="53F";
    else return "error";

    if(operation[4]=='%')
    {
        ret+="793";
        temp="";
        for(int i=5; i<operation.length(); i++)
            temp+=operation[i];
        ret+='_';
        ret+=temp;
    }
    else if(operation[4]=='*')
    {
        if(operation[5]=='[')
        {
            char end='!';
            temp="";
            int i=0;
            for(i=6; i<operation.length(); i++)
                if(operation[i]=='+' || operation[i]==']')
                {
                    end=operation[i];
                    break;
                }
                else temp+=operation[i];
            if(end==']')
            {
                string rand="";
                RegParser(rand,temp,0,1);
                if(rand=="error")
                    return "error";
                else
                {
                    ret+=rand[0];
                    ret+="02";
                }
            }
            else if(end=='+')
            {
                string rand="";
                RegParser(rand,temp,0,1);
                if(rand=="error")
                    return "error";
                else
                {
                    ret+=rand[0];
                    ret+="03";
                }
                i++;
                temp="";
                for(; i<operation.length(); i++)
                    if(operation[i]==']')
                        break;
                    else
                        temp+=operation[i];
                if(operation[operation.length()-1]!=']') return "error";
                if(parseHex(temp)=="error")
                    if(parseDecimal(temp)=="error")
                        ret+='_'+temp;
                    else
                        ret+=parseDecimal(temp);
                else
                    ret+=parseHex(temp);
            }
            else return "error";
        }
        else
        {
            string rand;
            temp="";
            for(int i=5; i<operation.length(); i++)
                temp+=operation[i];
            if(parseDecimal(temp)=="error")
                if(parseHex(temp)=="error")
                {
                    RegParser(rand,temp,0,1);
                    if(rand=="error")
                    {
                        temp+="F04_";
                        temp+=ret;
                    }
                    else
                    {
                        ret+=rand[0];
                        ret+="01";
                    }
                }
                else
                {
                    ret+="F04";
                    ret+=parseHex(temp);
                }
            else
            {
                ret+="F04";
                ret+=parseDecimal(temp);
            }
        }
    }
    else
    {
        ret+="F00";
        temp="";
        for(int i=4; i<operation.length(); i++)
            temp+=operation[i];
        if(parseDecimal(temp)=="error")
            if(parseHex(temp)=="error")
            {
                ret+='_';
                ret+=temp;
            }
            else
                ret+=parseHex(temp);
        else
            ret+=parseDecimal(temp);
        
    }
    return ret;
}
//Instrukcije sa operandom
string OperandCoder(string operation)
{
    string temp="";
    string ret="";
    for(int i=0; i<operation.length(); i++)
    {
        temp+=operation[i];
        if(operation[i]==' ')
            break;
    }
    int i;
    if(temp=="call ")
    {
        ret+="30F";
        i=5;
    }
    else
    {
        if(temp=="ldr ")
            ret+="A0";
        else if(temp=="str ")
            ret+="B0";
        else return "error";
        temp="";
        for(i=4; i<operation.length(); i++)
            if(operation[i]==',')
                break;
            else temp+=operation[i];
        string rand="";
        RegParser(rand,temp,0,1);
        if(rand=="error")
            return "error";
        else ret+=rand[0];
        i++;
    }
    temp="";
    if(operation[i]=='$')
    {
        i++;
        for(; i<operation.length(); i++)
            temp+=operation[i];
        if(parseDecimal(temp)=="error")
            if(parseHex(temp)=="error")
            {
                    ret+="F00";
                    ret+='_'+temp;
            }
            else
                ret+="FOO"+parseHex(temp);
        else ret+="F00"+parseDecimal(temp);
    }
    else if(operation[i]=='%')
    {
        i++;
        for(; i<operation.length(); i++)
            temp+=operation[i];
        ret+="705";
        ret+='_'+temp;
    }
    else if(operation[i]=='[')
    {
        i++;
        char end='!';
        for(; i<operation.length(); i++)
            if(operation[i]=='+' || operation[i]==']')
            {
                end=operation[i];
                break;
            }
            else temp+=operation[i];
        string rand="";
        if(end==']')
        {
            RegParser(rand,temp,0,1);
            if(rand=="error")
                return "error";
            else
            {
                ret+=rand[0];
                ret+="02";
            }

        }
        else if(end=='+')
        {
            RegParser(rand,temp,0,1);
            if(rand=="error")
                return "error";
            else
            {
                ret+=rand[0];
                ret+="03";
            }
            i++;
            temp="";
            for(; i<operation.length(); i++)
                if(operation[i]==']')
                    break;
                else
                    temp+=operation[i];
            if(operation[operation.length()-1]!=']') return "error";
            if(parseHex(temp)=="error")
                if(parseDecimal(temp)=="error")
                    ret+='_'+temp;
                else
                    ret+=parseDecimal(temp);
            else
                ret+=parseHex(temp);
        }
        else
            return "error";
    }
    else
    {
        for(; i<operation.length(); i++)
            temp+=operation[i];
        string rand="";
        RegParser(rand,temp,0,1);
        if(rand=="error")
            if(parseDecimal(temp)=="error")
                if(parseHex(temp)=="error")
                {
                    ret+="F04";
                    ret+='_';
                    ret+=temp;
                }
                else
                    ret+="F04"+parseHex(temp);
            else
                ret+="F04"+parseDecimal(temp);
        else
        {
            ret+=rand[0];
            ret+="01";
        }
    }
    return ret;
}
////////Asemblerske direktive
int parseDirective(string directive)
{
    if(directive[0]!='.')
        return 0;
    string temp="";
    int i;
    for(i=1; i<directive.length(); i++)
    {
        temp+=directive[i];
         if(directive[i]==' ')
            break;
    }
    i++;
    if(temp=="global ")
    {
        while(1)
        {
            temp="";
            for(; i<directive.length(); i++)
            {
                if(directive[i]==',' || directive[i]==' ')
                    break;
                else
                    temp+=directive[i];
            }
            int exists=0;
            for(int j=0; j<Simbol::TabelaSimbola.size(); j++)
            {
                if(Simbol::TabelaSimbola.at(j).ime==temp)
                {
                    Simbol::TabelaSimbola.at(j).domen=1;
                    exists=1;
                }
            }
            if(exists==0)
            {
                Simbol::TabelaSimbola.push_back(Simbol(temp,"",-1,1));
            }
            if(i==directive.length())
                break;
            else 
                i++;

        }
        return 1;
    }
    else if(temp=="extern ")
    {
        while(1)
        {
            temp="";
            for(; i<directive.length(); i++)
                if(directive[i]==',' || directive[i]==' ')
                    break;
                else 
                    temp+=directive[i];
            for(int j=0; j<Simbol::TabelaSimbola.size(); j++)
            {
                if(Simbol::TabelaSimbola.at(j).ime==temp && Simbol::TabelaSimbola.at(j).domen==0)
                    return 0;
            }
            Simbol::TabelaSimbola.push_back(Simbol(temp,"undefined",0,0));
            if(i==directive.length())
                break;
            else 
                i++;
        }
        return 1;
    }
    else if(temp=="section ")
    {
        temp="";
        for(; i<directive.length(); i++)
            if(directive[i]==' ')
                break; 
            else
                temp+=directive[i];
        for(int j=0; j<Simbol::TabelaSimbola.size(); j++)
        {
            if(Simbol::TabelaSimbola.at(j).ime==temp)
                return 0;
        }
        Simbol::TabelaSimbola.push_back(Simbol(temp,temp,0,0));
        outputText.push_back(output(temp));
        Simbol::currentSection=temp;
        Simbol::sectionCnt=0;
        return 1;
    }
    else if(temp=="word ")
    {
        while(1)
        {
            temp="";
            for(; i<directive.length(); i++)
                if(directive[i]==',')
                    break;
                else temp+=directive[i];
            if(parseDecimal(temp)=="error")
                if(parseHex(temp)=="error")
                {
                    string rand="_";
                    rand+=temp;
                    outputText.at(outputText.size()-1).sectionData.push_back(rand);
                }
                else
                    outputText.at(outputText.size()-1).sectionData.push_back(parseHex(temp));
            else 
                outputText.at(outputText.size()-1).sectionData.push_back(parseDecimal(temp));
            Simbol::sectionCnt+=2;
            if(i==directive.length())
                break;
            else 
                i++;
        }
        return 1;
    }
    else if(temp=="skip ")
    {
        temp="";
        int br=0;
        for(; i<directive.length(); i++)
        {
            if(directive[i]==' ') break;
            temp+=directive[i];
        }
        if(parseDecimal(temp)=="error")
            if(parseHex(temp)=="error")
                return 0;
            else
                br=HexToInt(parseHex(temp));
        else
            br=HexToInt(parseDecimal(temp));
        string temp="";
        for(int j=0; j<br; j++)
            temp+="00";
        outputText.at(outputText.size()-1).sectionData.push_back(temp);
        Simbol::sectionCnt+=br;
        return 1;
    }
    else if(temp=="end" || temp=="end ")
    {
        if(directive.length()>5 || (directive.length()==5 && directive[4]!=' '))
            return 0;
        Simbol::end=1;
        return 1;
    }
    else if(temp=="equ ")
    {
        temp="";
        for(; i<directive.length(); i++)
            if(directive[i]==',')
                break;
            else
                temp+=directive[i];
        string name=temp;
        for(int j=0; j<Simbol::TabelaSimbola.size(); j++)
            if(Simbol::TabelaSimbola.at(j).ime==name)
                return 0;
        temp="";
        i++;
        for(; i<directive.size(); i++)
            if(directive[i]==' ')
                break;
            else
                temp+=directive[i];
        if(parseDecimal(temp)=="error")
            if(parseHex(temp)=="error")
                return 0;
            else
                Simbol::TabelaSimbola.push_back(Simbol(name,"absolute",HexToInt(parseHex(temp)),0));
        else
            Simbol::TabelaSimbola.push_back(Simbol(name,"absolute",HexToInt(parseDecimal(temp)),0));
        return 1;
    }
    else 
        return 0;
    return 0;
}
///////Uzmi labelu
int takeLabel(string& row)
{
    string temp="";
    int exists=0;
    int i;
    for(i=0; i<row.length(); i++)
    {
        if(row[i]==':')
        {
            exists=1;
            break;
        }
        else temp+=row[i];
    }
    if(exists)
    {
        int found=0;
        for(int j=0; j<Simbol::TabelaSimbola.size(); j++)
        {
            if(Simbol::TabelaSimbola.at(j).ime==temp)
                if(Simbol::TabelaSimbola.at(j).sekcija=="")
                {
                    Simbol::TabelaSimbola.at(j).sekcija=Simbol::currentSection;
                    Simbol::TabelaSimbola.at(j).offset_value=Simbol::sectionCnt;
                    found=1;
                    break;
                }
                else 
                    return -1;
        }
        if(found==0)
            Simbol::TabelaSimbola.push_back(Simbol(temp,Simbol::currentSection,Simbol::sectionCnt,0));
        i++;
        if(i==row.length())
            return 1;
        int ret=1;
        temp="";
        if(row[i]==' ') i++;
        for(; i<row.length(); i++)
        {
            if(row[i]!=' ')
                ret=0;
            temp+=row[i];
        }
        row=temp;
        return ret;
    }
    else 
        return 0;
}
void FirstPass(vector<string> input)
{
    for(int i=0; i<input.size(); i++)
    {
        if(input.at(i)!="")
        {
            int temp=takeLabel(input.at(i));
            if(temp==-1)
            {
                cout<<"Greska na liniji "<<i+1<<",duplirana labela!"<<endl;
                Simbol::fail=1;
                break;
            }
            if(temp==0)
            {
                int direktiva=parseDirective(input.at(i));
                if(direktiva==0)
                {
                    string operand=OperandCoder(input.at(i));
                    if(operand=="error")
                    {
                        string jump=jumpCoder(input.at(i));
                        if(jump=="error")
                        {
                            string regop=OperationCoder(input.at(i));
                            if(regop=="error")
                            {
                                Simbol::fail=1;
                                cout<<input.at(i)<<endl;
                                cout<<"pogresan format instrukcije ili direktive u redu "<<i+1<<"!"<<endl;
                                break;
                            }
                            else
                            {
                                Simbol::sectionCnt+=regop.length()/2;
                                outputText.at(outputText.size()-1).sectionData.push_back(regop);
                            }
                        }
                        else
                        {
                            if(jump.length()==6)
                                Simbol::sectionCnt+=3;
                            else 
                                Simbol::sectionCnt+=5;
                            outputText.at(outputText.size()-1).sectionData.push_back(jump);
                        }
                    }
                    else
                    {
                        if(operand.length()==6)
                            Simbol::sectionCnt+=3;
                        else 
                            Simbol::sectionCnt+=5;
                        outputText.at(outputText.size()-1).sectionData.push_back(operand);
                    }
                }
            }
        }
    }

}

void SecondPass()
{
    for(int i=0; i<outputText.size(); i++)
    {
        int offsetcnt=0;
        for(int j=0; j<outputText.at(i).sectionData.size(); j++)
        {
            if((outputText.at(i).sectionData.at(j).length()>6 && outputText.at(i).sectionData.at(j)[6]=='_') || outputText.at(i).sectionData.at(j)[0]=='_')
            {
                string replace="";
                for(int k=0; k<6; k++)
                    replace+=outputText.at(i).sectionData.at(j)[k];
                int i1;
                if(outputText.at(i).sectionData.at(j)[0]=='_')
                {
                    replace="";
                    i1=1;
                    offsetcnt+=2;
                }
                else 
                {
                    i1=7;
                    offsetcnt+=5;
                }
                string temp="";
                for(; i1<outputText.at(i).sectionData.at(j).length(); i1++)
                    temp+=outputText.at(i).sectionData.at(j)[i1];
                int rb=0;
                string sek;
                string tip="";
                for(int k=0; k<Simbol::TabelaSimbola.size(); k++)
                {
                    if(Simbol::TabelaSimbola.at(k).ime==temp)
                    {
                        if(outputText.at(i).sectionData.at(j)[0]=='_')
                            tip="IMM";
                        else
                        {
                            if(outputText.at(i).sectionData.at(j)[4]=='0' && outputText.at(i).sectionData.at(j)[5]=='5')
                                tip="PC_Rel";
                            if(outputText.at(i).sectionData.at(j)[4]=='0' && outputText.at(i).sectionData.at(j)[5]=='4')
                                tip="MEM_Dir";
                            if(outputText.at(i).sectionData.at(j)[4]=='0' && outputText.at(i).sectionData.at(j)[5]=='3')
                                tip="REG_Ind_Pom";
                            if(outputText.at(i).sectionData.at(j)[4]=='0' && outputText.at(i).sectionData.at(j)[5]=='0')
                                tip="IMM";
                            if(outputText.at(i).sectionData.at(j)[4]=='9' && outputText.at(i).sectionData.at(j)[5]=='3')
                            {
                                tip="PC_Rel";
                                outputText.at(i).sectionData.at(j)[4]='0';
                            }
                        }
                        if(tip=="PC_Rel")
                            if(Simbol::TabelaSimbola.at(k).domen==1)
                                replace+="FFFE";
                            else 
                                replace+=DecimalToHex((Simbol::TabelaSimbola.at(k).offset_value+65534)%65536);
                        else
                            if(Simbol::TabelaSimbola.at(k).domen==1)
                                replace+="0000";
                            else
                                replace+=DecimalToHex(Simbol::TabelaSimbola.at(k).offset_value);
                        sek=Simbol::TabelaSimbola.at(k).sekcija;
                        rb=k;
                        break;
                    }
                }
                outputText.at(i).sectionData.at(j)=replace;
                if((sek!="absolute" || (sek=="absolute" && tip=="PC_Rel")) && tip!="")
                    RelTabela.push_back(relocation(outputText.at(i).sectionName,offsetcnt-2,rb,tip));
            }
            else
            {
                offsetcnt+=outputText.at(i).sectionData.at(j).length()/2;
            }
            if(j==outputText.at(i).sectionData.size()-1)
            {
                outputText.at(i).sectionSize=offsetcnt;
            }
        }
    }
}


