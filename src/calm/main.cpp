#include "util/calmwin.h"

#include "core/app.h"

// TODO: Significant error handling

int main()
{
    calm::App app(512, 512, "calm engine");
    app.start();

    return 0;
}
