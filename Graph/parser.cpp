bool isFileExist(const char* p_caFile)
{
    ifstream input;
    input.open(p_caFile, ios::in);
    if(!input){
        printf("Warning: can not find file: %s !\n", p_caFile);
        input.close();
        return false;
    }
    string s;
    printf("File %s Open Success\n", p_caFile);
    input.getline(s);
    cout << s << endl;


    input.close();
    return true;
}