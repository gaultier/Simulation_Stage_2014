#include "Oculus.h"

NullOculus::NullOculus()
{
}

NullOculus::~NullOculus()
{
}

bool GenericOculus::isMoving() const
{
    spdlog::get("console")->debug() << "Generic Oculus is not moving";
    return false;
}

bool GenericOculus::isUsingDebugHmd()
{
    return true;
}

glm::vec3 GenericOculus::dAngles() const
{
    return glm::vec3(0, 0, 0);
}
