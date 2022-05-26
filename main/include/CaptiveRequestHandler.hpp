#include "ESPAsyncWebServer.h"

extern const char *index_html = asm("_binary_getpassword_html_start");

// This code is based on the tutorial at https://iotespresso.com/create-captive-portal-using-esp32/
class CaptiveRequestHandler : public AsyncWebHandler
{
public:
    CaptiveRequestHandler() {}
    virtual ~CaptiveRequestHandler() {}

    bool canHandle(AsyncWebServerRequest *request)
    {
        return true;
    }

    void handleRequest(AsyncWebServerRequest *request)
    {
        request->send_P(200, "text/html", index_html);
    }
};