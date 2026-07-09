#pragma once

#include <stdexcept>
#include <string>

namespace BooleanEngine
{

class BooleanEngineException : public std::runtime_error
{
public:
    explicit BooleanEngineException(const std::string& message)
        : std::runtime_error(message)
    {
    }
};

class ParserException : public BooleanEngineException
{
public:
    explicit ParserException(const std::string& message)
        : BooleanEngineException(message)
    {
    }
};

class EvaluationException : public BooleanEngineException
{
public:
    explicit EvaluationException(const std::string& message)
        : BooleanEngineException(message)
    {
    }
};

class HypercubeException : public BooleanEngineException
{
public:
    explicit HypercubeException(const std::string& message)
        : BooleanEngineException(message)
    {
    }
};

class KarnaughMapException : public BooleanEngineException
{
public:
    explicit KarnaughMapException(const std::string& message)
        : BooleanEngineException(message)
    {
    }
};

class GroupDetectionException : public BooleanEngineException
{
public:
    explicit GroupDetectionException(const std::string& message)
        : BooleanEngineException(message)
    {
    }
};

class SimplificationException : public BooleanEngineException
{
public:
    explicit SimplificationException(const std::string& message)
        : BooleanEngineException(message)
    {
    }
};

}
