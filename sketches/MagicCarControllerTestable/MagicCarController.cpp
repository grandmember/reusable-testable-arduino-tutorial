#include "MagicCarController.h"

#include <cstdio>
#include <cstring>

namespace
{
const auto kPlainText = "text/plain";
const auto kSuccess   = 200;
const auto kError     = 404;

bool areEqual(const std::vector<char>& v, const char* c)
{
    return strcmp(v.data(), c) == 0;
}
} // namespace

namespace magic_car
{
MagicCarController::MagicCarController(Car& car, RestServer& restServer)
    : mCar{car}
    , mRestServer{restServer}
{
}

void MagicCarController::registerDriveEndpoint()
{
    mRestServer.on("/drive", [this]() {
        const auto arguments = mRestServer.args();

        for (auto i = 0; i < arguments; i++)
        {
            const auto command = mRestServer.argName(i);
            if (areEqual(command, "speed"))
            {
                mCar.setSpeed(static_cast<float>(mRestServer.argToInt(i)));
            }
            else if (areEqual(command, "angle"))
            {
                mCar.setAngle(mRestServer.argToInt(i));
            }
        }
        mRestServer.send(kSuccess, kPlainText, "ok");
    });
}

void MagicCarController::registerGyroscopeEndpoint()
{
    mRestServer.on("/gyro", [this]() {
        char heading[4];
        sprintf(heading, "%d", mCar.getHeading());
        mRestServer.send(kSuccess, kPlainText, heading);
    });
}

void MagicCarController::registerErrorHandler()
{
    mRestServer.onNotFound(
        [this]() { mRestServer.send(kError, kPlainText, "Unknown command"); });
}

void MagicCarController::beginServer()
{
    mRestServer.begin();
}

void MagicCarController::update()
{
    mRestServer.handleClient();
    mCar.update();
}
} // namespace magic_car