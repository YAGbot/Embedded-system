#include <iostream>
#include <string>
#include <jsoncpp/json/json.h>
 
using namespace std;
 
int main()
{
    string strJsonContent = "{\"role_id\": 1,\"occupation\": \"paladin\",\"camp\": \"alliance\"}";
 
    int nRoleDd = 0;
    string strOccupation = "";
    string strCamp = "";
    
    Json::Reader reader;
    Json::Value root;
 
    if (reader.parse(strJsonContent, root))
    {
        nRoleDd = root["role_id"].asInt();
        strOccupation = root["occupation"].asString();
        strCamp = root["camp"].asString();
    }
 
    cout << "role_id is: " << nRoleDd << endl;
    cout << "occupation is: " << strOccupation << endl;
    cout << "camp is: " << strCamp << endl;
 
    return 0;
}
