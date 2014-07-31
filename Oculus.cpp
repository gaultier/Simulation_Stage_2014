#include "Oculus.h"

NullOculus::NullOculus()
{
    logger->trace(logger->get() << "Null oculus constructor");
}

NullOculus::~NullOculus()
{
    logger->trace(logger->get() << "Null oculus destructor");
}

bool GenericOculus::isMoving()
{
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

