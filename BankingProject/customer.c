struct customer{
    char username[1024];
    char password[1024];
    char accountNumber[1024];
    int type; //0-admin/1-normal/2-joint
    bool status; //active/inactive

};