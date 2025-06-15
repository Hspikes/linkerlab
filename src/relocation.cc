#include "relocation.h"
#include<iostream>
#include <sys/mman.h>

void handleRela(std::vector<ObjectFile> &allObject, ObjectFile &mergedObject, bool isPIE)
{
    using std::cout;
    using std::endl;
    uint64_t userCodeStart = isPIE ? 0xe9 : 0xe6;
    uint64_t textOff = mergedObject.sections[".text"].off + userCodeStart;
    uint64_t textAddr = mergedObject.sections[".text"].addr + userCodeStart;

    uint64_t preText=0;
    for(auto &obj : allObject)
    {
        for(auto &re : obj.relocTable)
            re.offset+=preText;
        preText+=obj.sections[".text"].size;
    }

    for(auto &obj : allObject)
    {
        for(auto &re : obj.relocTable)
        {
            uint64_t addr=re.offset+textOff;
            int valueToFill;
            if(re.type==10)
            {
                valueToFill=re.sym->value+re.addend;    
            }
            else if(re.type==2 || re.type==4)
            {
                valueToFill=re.sym->value-(re.offset+textAddr)+re.addend;
            }
            // cout<<"Name: "<<re.name<<" type: "<<re.type<<endl
                // <<"Offset: "<<re.offset<<" addr:"<<addr<<" value: "<<valueToFill<<endl;
            addr+=(uint64_t)mergedObject.baseAddr;
            *reinterpret_cast<int *>(addr) = valueToFill;
        }
    }
}