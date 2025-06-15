#include "resolve.h"
#include <string>
#include <unordered_map>
#include <iostream>

#define FOUND_ALL_DEF 0
#define MULTI_DEF 1
#define NO_DEF 2

std::string errSymName;

int callResolveSymbols(std::vector<ObjectFile> &allObjects);

void resolveSymbols(std::vector<ObjectFile> &allObjects) {
    int ret = callResolveSymbols(allObjects);
    if (ret == MULTI_DEF) {
        std::cerr << "multiple definition for symbol " 
            << errSymName << std::endl;
        abort();
    } else if (ret == NO_DEF) {
        std::cerr << "undefined reference for symbol " 
            << errSymName << std::endl;
        abort();
    }
}

int callResolveSymbols(std::vector<ObjectFile> &allObjects)
{
    using namespace std;
    unordered_map<string,pair<Symbol*,bool> >known_sym;
    known_sym.clear();
    for(auto &obj : allObjects)
    {
        for(auto &sym : obj.symbolTable)
        {
            if(sym.bind==0) continue;
            if(sym.type!=1 && sym.type!=2) continue;
            bool isStrong;
            if(sym.index==1 || sym.index==3) isStrong=true;
            else isStrong=false;
            if(known_sym.find(sym.name)!=known_sym.end()) 
            {
                if(isStrong==1)
                    if(known_sym[sym.name].second==true)
                    {
                        errSymName=sym.name;
                        return MULTI_DEF;
                    }
                    else known_sym[sym.name]=make_pair(&sym,1);
                else continue;
            }
            else known_sym[sym.name]=make_pair(&sym,isStrong);
        }
    }

    for(auto &obj : allObjects)
    {
        for(auto &re : obj.relocTable)
        {
            if(known_sym.find(re.name)==known_sym.end())
            {
                errSymName=re.name;
                return NO_DEF;
            }
            re.sym=known_sym[re.name].first;
        }
    }
    return FOUND_ALL_DEF;
}
