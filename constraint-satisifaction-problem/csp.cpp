#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>

struct Assignment
{
    char variable;
    int value;
};

std::vector<std::string> constraints;
std::map<char, std::vector<char> > constraintMap;
unsigned variablesSize;
int lineNumber = 1;

struct find_id : std::unary_function<Assignment, bool> {
    int id;
    find_id(int variable):id(variable) {}
    bool operator()(Assignment const& m) const {
        return m.variable == id;
    }
};

bool inAssignments(char var, std::vector<Assignment> assignments)
{
    std::vector<Assignment>::iterator it = std::find_if(assignments.begin(),
    assignments.end(), find_id(var));
    return (it != assignments.end());
}

bool inConstraintsMap(char var1, char var2)
{
    for (unsigned i = 0; i < constraintMap[var1].size(); i++)
    {
        if (constraintMap[var1][i] == var2)
        {
            return true;
        }
    }
    return false;
}

void printAssignments(std::vector<Assignment> assignments, char variable, int value)
{
    std::cout << lineNumber << ". ";
    lineNumber++;
    if (!assignments.empty())
    {
        
        for (unsigned i = 0; i < assignments.size(); i++)
        {
            std::cout << assignments[i].variable << "=" << assignments[i].value << ", ";
        }
        
    }
    std::cout << variable << "=" << value;
    std::cout << "\tfailure" << std::endl;
}

void printVariables(std::map< char, std::vector<int> > variables)
{
    for (std::map<char, std::vector<int> >::iterator itr = variables.begin(); 
    itr != variables.end(); ++itr)
    {
        std::cout << itr->first << ":";
        for (unsigned i = 0; i < itr->second.size(); i++)
        {
            std::cout << itr->second[i] << " ";
        }
        std::cout << std::endl;
    }
}

char getMostConstrainingVariable(std::vector<char> mostConstrainedVars, std::vector<Assignment> assignments)
{
    std::map<char, int> remVarConstraints;
    for (unsigned i = 0; i < mostConstrainedVars.size(); i++)
    {
        char var = mostConstrainedVars[i];
        remVarConstraints[var] = 0;
    }

    for (unsigned i = 0; i < mostConstrainedVars.size(); i++)
    {
        char var = mostConstrainedVars[i];
        for (unsigned j = 0; j < constraintMap[var].size(); j++)
        {
            if (!inAssignments(constraintMap[var][j], assignments))
            {
                remVarConstraints[var]++;
            }
        }
    }

    char mostConstrainingVar = ' ';
    int largestConstraint = -1;

    for (std::map<char, int>::iterator itr = remVarConstraints.begin(); 
    itr != remVarConstraints.end(); ++itr)
    {
        char var = itr->first;
        int constraints = itr->second;
        if (constraints > largestConstraint)
        {
            mostConstrainingVar = var;
            largestConstraint = constraints;
        }
        else if (constraints == largestConstraint && int(var) < int(mostConstrainingVar))
        {
            mostConstrainingVar = var;
        }
    }

    remVarConstraints.clear();

    return mostConstrainingVar;
}

char getMostConstrainedVariable(std::vector<Assignment> assignments,
std::map< char, std::vector<int> > variables)
{
    std::vector<char> mostConstrainedVars;

    unsigned smallest = 999999;
    for (std::map< char, std::vector<int> >::iterator itr = variables.begin(); 
    itr != variables.end(); ++itr)
    {
        char var = itr->first;
        std::vector<int> domain = itr->second;

        if (!inAssignments(var, assignments))
        {
            if (domain.size() < smallest)
            {
                mostConstrainedVars.clear();
                mostConstrainedVars.push_back(var);
                smallest = domain.size();
            }
            else if (domain.size() == smallest)
            {
                mostConstrainedVars.push_back(var);
            }
        }
    }

    if (mostConstrainedVars.size() > 1)
    {
        return getMostConstrainingVariable(mostConstrainedVars, assignments);
    }
    
    return mostConstrainedVars[0];
}

char selectVariable(std::vector<Assignment> assignments, std::map< char, std::vector<int> > variables)
{
    return getMostConstrainedVariable(assignments, variables);
}

bool valsConstrained(char var1, char val1, int var2, int val2)
{
    for (unsigned i = 0; i < constraints.size(); i++)
    {
        char v1 = constraints[i][0];
        char v2 = constraints[i][4];
        char op = constraints[i][2];

        if (var1 == v1 && var2 == v2)
        {
            switch (op)
            {
            case '=':
                return (val1 == val2);
                break;
            case '!':
                return (val1 != val2);
                break;
            case '>':
                return (val1 > val2);
                break;
            case '<':
                return (val1 < val2);
                break;
            }
        }
        else if (var1 == v2 && var2 == v1)
        {
            switch (op)
            {
            case '=':
                return (val2 == val1);
                break;
            case '!':
                return (val2 != val1);
                break;
            case '>':
                return (val2 > val1);
                break;
            case '<':
                return (val2 < val1);
                break;
            }
        }
    }
    return false;
}

std::vector<int> getLeastConstrainingValues(char variable, std::vector<Assignment> assignments,
std::map< char, std::vector<int> > variables)
{
    std::vector<int> domain = variables[variable];
    std::map<int, int> valCounts;

    for (unsigned i = 0; i < domain.size(); i++)
    {
        valCounts[domain[i]] = 0;
    }

    for (unsigned i = 0; i < domain.size(); i++)
    {
        int value = domain[i];
        std::vector<char> varsCheck = constraintMap[variable];

        for (unsigned j = 0; j < varsCheck.size(); j++)
        {
            char var = varsCheck[j];

            if (!inAssignments(var, assignments))
            {
                std::vector<int> domainCheck = variables[var];
                for (unsigned k = 0; k < domainCheck.size(); k++)
                {
                    char var1 = variable;
                    int val1 = value;
                    char var2 = var;
                    int val2 = domainCheck[k];

                    if (!valsConstrained(var1, val1, var2, val2))
                    {
                        valCounts[value]++;
                    }
                }
            }
        }
    }

    std::vector<int> orderedValues;
    std::map<int, int>::iterator itr = valCounts.begin();
    orderedValues.push_back(itr->first);
    for (std::advance(itr, 1); itr != valCounts.end(); ++itr)
    {
        for (unsigned i = 0; i < orderedValues.size(); i++)
        {
            if (itr->second < valCounts[orderedValues[i]] || 
            (itr->second == valCounts[orderedValues[i]] && itr->first < orderedValues[i]))
            {
                orderedValues.insert(orderedValues.begin() + i, itr->first);
                break;
            }
            else if (i == orderedValues.size() - 1)
            {
                orderedValues.push_back(itr->first);
                break;
            }
        }
    }

    return orderedValues;
}

std::vector<int> getOrderedValues(char var, std::vector<Assignment> assignments,
std::map< char, std::vector<int> > variables)
{
    return getLeastConstrainingValues(var, assignments, variables);
}

bool constraintSatisfied(std::string constraint, std::vector<Assignment> assignments)
{
    char var1 = constraint[0];
    char var2 = constraint[4];
    char op = constraint[2];

    int comp1;
    int comp2;

    for (unsigned i = 0; i < assignments.size(); i++)
    {
        if (assignments[i].variable == var1)
        {
            comp1 = assignments[i].value;
        }
        if (assignments[i].variable == var2)
        {
            comp2 = assignments[i].value;
        }
    }

    switch (op)
    {
    case '=':
        return (comp1 == comp2);
        break;
    case '!':
        return (comp1 != comp2);
        break;
    case '>':
        return (comp1 > comp2);
        break;
    case '<':
        return (comp1 < comp2);
        break;
    }

    return false;
}

bool assignmentsSatisfied(std::vector<Assignment> assignments, std::map< char, std::vector<int> > variables)
{
    if (variables.size() > assignments.size())
    {
        return false;
    }

    for (unsigned i = 0; i < constraints.size(); i++)
    {
        if (!constraintSatisfied(constraints[i], assignments))
        {
            return false;
        }
    }

    return true;
}

std::map< char, std::vector<int> > applyForwardChecking(char variable, int value,
std::vector<Assignment> assignments, std::map< char, std::vector<int> > variables)
{
    char var1 = variable;
    int val1 = value;
    std::map< char, std::vector<int> > updatedVariables;

    for (std::map<char, std::vector<int> >::iterator itr = variables.begin(); 
    itr != variables.end(); ++itr)
    {
        char var2 = itr->first;
        std::vector<int> domain = itr->second;
        for (unsigned i = 0; i < domain.size(); i++)
        {
            int val2 = domain[i];
            if (inAssignments(var2, assignments) || var2 == var1 || !inConstraintsMap(var2, var1))
            {
                updatedVariables[var2] = variables[var2];
            }
            else
            {
                for (unsigned j = 0; j < constraints.size(); j++)
                {
                    char v1 = constraints[j][0];
                    char v2 = constraints[j][4];
                    char op = constraints[j][2];

                    if (var1 == v1 && var2 == v2)
                    {
                        switch (op)
                        {
                        case '=':
                            if (val1 == val2)
                            {
                                updatedVariables[var2].push_back(variables[var2][i]);
                            }
                            goto label;
                        case '!':
                            if (val1 != val2)
                            {
                                updatedVariables[var2].push_back(variables[var2][i]);
                            }
                            goto label;
                        case '>':
                            if (val1 > val2)
                            {
                                updatedVariables[var2].push_back(variables[var2][i]);
                            }
                            goto label;
                        case '<':
                            if (val1 < val2)
                            {
                                updatedVariables[var2].push_back(variables[var2][i]);
                            }
                            goto label;
                        }
                    }
                    else if (var1 == v2 && var2 == v1)
                    {
                        switch (op)
                        {
                        case '=':
                            if (val2 == val1)
                            {
                                updatedVariables[var2].push_back(variables[var2][i]);
                            }
                            goto label;
                        case '!':
                            if (val2 != val1)
                            {
                                updatedVariables[var2].push_back(variables[var2][i]);
                            }
                            goto label;
                        case '>':
                            if (val2 > val1)
                            {
                                updatedVariables[var2].push_back(variables[var2][i]);
                            }
                            goto label;
                        case '<':
                            if (val2 < val1)
                            {
                                updatedVariables[var2].push_back(variables[var2][i]);
                            }
                            goto label;
                        }
                    }
                }
            }
            label: if (i > domain.size()) break;
        }
    }

    return updatedVariables;
}

bool emptyDomain(std::map< char, std::vector<int> > variables)
{
    if (variables.size() < variablesSize)
    {
        return true;
    }

    return false;
}

bool valueConsistent(int variable, int value, std::vector<Assignment> assignments, 
std::map< char, std::vector<int> > variables, std::string procedure)
{
    if (procedure == "fc" && emptyDomain(variables))
    {
        return false;
    }

    std::vector<char> varsCheck = constraintMap[variable];
    for (unsigned i = 0; i < varsCheck.size(); i++)
    {
        if (inAssignments(varsCheck[i], assignments))
        {
            int var = varsCheck[i];
            int val = 0;
            for (unsigned j = 0; j < assignments.size(); j++)
            {
                if (assignments[j].variable == var)
                {
                    val = assignments[j].value;
                    break;
                }
            }
            if (!valsConstrained(variable, value, var, val))
            {
                return false;
            }
        }
    }
    return true;
}

std::vector<Assignment> backtrackingSearch(std::vector<Assignment> assignments,
std::map< char, std::vector<int> > variables, std::string procedure)
{
    if (!assignments.empty() && assignmentsSatisfied(assignments, variables))
    {
        return assignments;
    }

    char var = selectVariable(assignments, variables);

    std::vector<int> values = getOrderedValues(var, assignments, variables);

    for (unsigned i = 0; i < values.size(); i++)
    {
        int val = values[i];

        std::map< char, std::vector<int> > newVariables = variables;
        if (procedure == "fc")
        {
            newVariables = applyForwardChecking(var, val, assignments, variables);
        }
        if (valueConsistent(var, val, assignments, newVariables, procedure))
        {
            variables = newVariables;
            struct Assignment newAssignment;
            newAssignment.variable = var;
            newAssignment.value = val;
            assignments.push_back(newAssignment);

            std::vector<Assignment> result = backtrackingSearch(assignments, variables, procedure);
            
            if (!result.empty())
            {
                return result;
            }

            for (unsigned i = 0; i < assignments.size(); i++)
            {
                if (assignments[i].variable == var && assignments[i].value == val)
                {
                    assignments.erase(assignments.begin() + i);
                }
            }
        }
        else
        {
            printAssignments(assignments, var, val);
        }
    }

    std::vector<Assignment> failure;
    
    return failure;
}

std::vector<Assignment> backtrackingSearch(std::map< char, std::vector<int> > variables, std::string procedure)
{
    std::vector<Assignment> assignments;
    return backtrackingSearch(assignments, variables, procedure);
}

int main(int argc, char **argv)
{
    if (argc < 2 || argc > 3)
    {
        std::cout << "invalid command line arguments, correct form: ./csp <filename> <fc/none> (example: ./csp test1 fc)";
        return 1;
    }
    std::string testFile = argv[1];
    std::string varFile = testFile + ".var";
    std::string conFile = testFile + ".con";
    std::string procedure = "";
    if (argc > 2)
    {
        procedure = argv[2];
    }

    std::map< char, std::vector<int> > variables;

    std::ifstream infile;
    std::string readline;

    // Parse .var file into DOMAINS and VARIABLES
    infile.open(varFile);
    if (infile.is_open())
    {
        while (getline(infile, readline))
        {
            std::istringstream iss(readline);
            std::string split = "";
            std::vector<int> domain;

            iss >> split;
            char letter = split[0];

            while (iss >> split)
            {
                domain.push_back(std::stoi(split));
            }
            variables[letter] = domain;
        }
        infile.close();
    }
    else
    {
        std::cout << "error: file " << argv[1] << ".var not found" << std::endl;
        return 1;
    }
    variablesSize = variables.size();
    
    // Parse .con file into CONSTRAINTS and CONSTRAINT MAP
    infile.open(conFile);
    if (infile.is_open())
    {
        while (getline(infile, readline))
        {
            char var1 = readline[0];
            char var2 = readline[4];
            constraintMap[var1].push_back(var2);
            constraintMap[var2].push_back(var1);
            constraints.push_back(readline);
        }
        infile.close();
    }
    else
    {
        std::cout << "error: file " << argv[1] << ".con not found" << std::endl;
        return 1;
    }

    // Backtrack search and print values
    std::vector<Assignment> solution;
    solution = backtrackingSearch(variables, procedure);

    std::cout << lineNumber << ". ";
    for (unsigned i = 0; i < solution.size(); i++)
    {
        std::cout << solution[i].variable << "=" << solution[i].value;;
        if (i < solution.size()-1) 
        {
            std::cout << ", ";
        }
    }
    std::cout << "\tsolution" << std::endl;

    return 0;
}