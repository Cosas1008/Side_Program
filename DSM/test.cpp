#include <hiredis/hiredis.h>
#include <iostream>
#include <string>
using namespace std;
 
int main(void)
{
    redisContext *redis = (redisContext *) redisConnect("127.0.0.1", 6379);
    if ((redis == NULL) || (redis->err))
    {
        cout << "Fail to connect..." << endl;
        return -1;
    }
    redisReply *r = (redisReply *) redisCommand(redis, "set a 10");
    r = (redisReply *) redisCommand(redis, "get a");
    cout << "The value of key 'a' is " << endl;
    cout << r->str << endl;
    freeReplyObject(r);
    redisFree(redis);
}