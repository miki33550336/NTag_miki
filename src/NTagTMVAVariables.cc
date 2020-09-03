#include <algorithm>

#include <TTree.h>

#include <TMVA/Reader.h>

#include "NTagTMVAVariables.hh"

NTagTMVAVariables::NTagTMVAVariables(unsigned int verbose)
{
    msg = NTagMessage("TMVAVariables", verbose);
    Clear();
}

NTagTMVAVariables::~NTagTMVAVariables() {}

void NTagTMVAVariables::Clear()
{
    iVariableMap["N10"] = 0;
    iVariableMap["N200"] = 0;
    iVariableMap["N50"] = 0;

    for (const auto& pair: iVariableMap) {
        iEventVectorMap[pair.first] = new std::vector<int>();
        fVariableMap[pair.first] = 0.;
    }

    fVariableMap["dt"] = 0.;
    fVariableMap["sumQ"] = 0.;
    fVariableMap["spread"] = 0.;
    fVariableMap["trmsold"] = 0.;
    fVariableMap["beta1"] = 0.;
    fVariableMap["beta2"] = 0.;
    fVariableMap["beta3"] = 0.;
    fVariableMap["beta4"] = 0.;
    fVariableMap["beta5"] = 0.;
    fVariableMap["prompt_nfit"] = 0.;
    fVariableMap["tbsenergy"] = 0.;
    fVariableMap["tbswall"] = 0.;
    fVariableMap["tbsgood"] = 0.;
    fVariableMap["tbsdirks"] = 0.;
    fVariableMap["tbspatlik"] = 0.;
    fVariableMap["tbsovaq"] = 0.;
    fVariableMap["prompt_bonsai"] = 0.;
    fVariableMap["nwall"] = 0.;
    fVariableMap["trms40"] = 0.;
    fVariableMap["bonsai_nfit"] = 0.;

    for (const auto& pair: fVariableMap) {
        fEventVectorMap[pair.first] = new std::vector<float>();
    }
}

std::vector<const char*> NTagTMVAVariables::Keys()
{
    std::vector<const char*> keys;

    for (const auto& pair: fVariableMap) {
            keys.push_back(pair.first);
    }

    return keys;
}

void NTagTMVAVariables::AddVariablesToReader(TMVA::Reader* reader)
{
    for (auto& pair: fVariableMap) {
        msg.Print(Form("Adding variable %s...", pair.first), pDEBUG);
        reader->AddVariable(pair.first, &pair.second);
    }
}

void NTagTMVAVariables::SetBranchAddressToTree(TTree* tree)
{
    for (auto& pair: iEventVectorMap) {
        tree->SetBranchAddress(pair.first, &(pair.second));
    }

    for (auto& pair: fEventVectorMap) {
        if (!iVariableMap.count(pair.first))
            tree->SetBranchAddress(pair.first, &(pair.second));
    }
}

void NTagTMVAVariables::MakeBranchesToTree(TTree* tree)
{
    for (auto& pair: iEventVectorMap) {
        tree->Branch(pair.first, &(pair.second));
    }

    for (auto& pair: fEventVectorMap) {
        if (!iVariableMap.count(pair.first))
            tree->Branch(pair.first, &(pair.second));
    }
}

void NTagTMVAVariables::SetVariablesForCaptureCandidate(int iCandidate)
{
    for (const auto& pair: iEventVectorMap) {
        iVariableMap[pair.first] = pair.second->at(iCandidate);
    }

    for (const auto& pair: fEventVectorMap) {
        if (iVariableMap.count(pair.first))
            fVariableMap[pair.first] = iVariableMap[pair.first];
        else
            fVariableMap[pair.first] = pair.second->at(iCandidate);
    }
}

void NTagTMVAVariables::DumpCurrentVariables()
{
    for (const auto& pair: fVariableMap) {
        msg.Print(Form("%s: %f", pair.first, pair.second));
    }
}

int NTagTMVAVariables::GetNumberOfCandidates()
{
    return static_cast<int>(iEventVectorMap["N10"]->size());
}