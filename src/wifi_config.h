

#define BASE_URL "http://192.168.1.125/"


#ifndef USER_NAME
#define USER_NAME "ed"
#endif

const char * time_server_url=BASE_URL "gettime?from=bedsidescreen";

const char * get_alarm_time_url=BASE_URL "getalarmtime/" USER_NAME;

const char * set_alarm_url=BASE_URL "setalarmtime/" USER_NAME "/";

const char * hostname="EdBedside";                                                                  