#include <cstdio>
#include <iostream>
#include <memory>

using std::printf;

// ---------------------------------------------------------------------------
// External C API

struct external_api { int data; };

external_api * open_my_api()
{
    external_api * api = (external_api *)malloc(sizeof(external_api));
    api->data = 42;
    printf("External C api %p opened\n", api);
    return api;
}

void close_my_api(external_api * api)
{
    free(api);
    printf("External C api %p closed\n", api);
}

// ---------------------------------------------------------------------------
// C++ use

int main()
{
    auto api = std::unique_ptr<external_api, void(*)(external_api *)>(
        open_my_api(),
        close_my_api
    );
    std::cout <<"My api is at " <<api.get() <<std::endl;
    std::cout <<"    sizeof(api) is " <<sizeof api <<std::endl;
    return 0;
}
