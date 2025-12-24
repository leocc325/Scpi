#include "ScpiParseTree.h"

ScpiParseTree::ScpiParseTree()
{
    root = new ScpiParseNode("Root",nullptr,nullptr);
}

ScpiParseTree::~ScpiParseTree()
{

}
